#include "navdata.h"


int nav_socket;
struct sockaddr_in nav_dst_addr, nav_src_addr;
pthread_t nav_pid = NULL;
int nav_stop = 0;
navdata_t *navdata;
navdata_unpacked_t navdata_unpacked;

static int nav_socket_init()
{
    int res;
    char *ardrone_ip;
    struct timeval tv_out;
    tv_out.tv_sec = 3;
    tv_out.tv_usec = 0;

    ardrone_ip = get_ardrone_ip();

    bzero(&nav_src_addr, sizeof(nav_src_addr));
    nav_src_addr.sin_family = AF_INET;
    nav_src_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    nav_src_addr.sin_port = htons(NAV_DATA_PORT);

    bzero(&nav_dst_addr, sizeof(nav_dst_addr));
    nav_dst_addr.sin_family = AF_INET;
    nav_dst_addr.sin_addr.s_addr = inet_addr(ardrone_ip);
    nav_dst_addr.sin_port = htons(NAV_DATA_PORT);
    
    nav_socket = socket(AF_INET, SOCK_DGRAM, 0);
    res = bind(nav_socket, (struct sockaddr*)&nav_src_addr, sizeof(nav_src_addr));
    if (res != 0)
        printf(">>>>>>binding nav socket to NAV_DATA_PORT FAILED!\n");
    else
        printf(">>>>>>binding nav socket to NAV_DATA_PORT OK!\n");

    setsockopt(nav_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv_out, sizeof(tv_out));
    

    return res;

}

static inline void send_datagram(char *data, int len)
{
    int res;
    res = sendto(nav_socket, data, len, 0, (struct sockaddr *)&nav_dst_addr, sizeof(nav_dst_addr));
}

static inline int recv_datagram(char *msg)
{
    int len = sizeof(nav_dst_addr);
    return recvfrom(nav_socket, (void*)msg, MAX_NAVDATA_SIZE, 0, (struct sockaddr*)&nav_dst_addr, &len);
}

static int is_nav_pkt(char *msg)
{
   if (*((uint32_t*)msg) == NAVDATA_HEADER)
       return 1;
   else
       return 0;
}

static uint32_t get_state(char *msg)
{
    msg += NAVDATA_STATE_OFFSET;
    return *((uint32_t*)msg);
}


static uint8_t * navdata_unpack_option(uint8_t *navdata_ptr, uint32_t ptrsize, uint8_t *data, uint32_t datasize)
{
    uint32_t minSize = (ptrsize < datasize) ? ptrsize : datasize;
    memcpy(data, navdata_ptr, minSize);
    return (navdata_ptr + ptrsize);
}



static int ardrone_navdata_unpack_all(navdata_unpacked_t *navdata_unpacked, navdata_t *navdata, uint32_t *cks)
{

    int res;
    navdata_cks_t navdata_cks;
    navdata_option_t* navdata_option_ptr;

    navdata_option_ptr = (navdata_option_t*) &(navdata->options[0]);
    memset( &navdata_cks, 0, sizeof(navdata_cks));
    memset( navdata_unpacked, 0, sizeof(navdata_unpacked_t) );

    navdata_unpacked->nd_seq          = navdata->sequence;
    navdata_unpacked->ardrone_state   = navdata->ardrone_state;
    navdata_unpacked->vision_defined  = navdata->vision_defined;

    while( navdata_option_ptr != NULL )
    {
      // Check if we have a valid option
      if( 0 == navdata_option_ptr->size )
      {
          printf("One option %d is not a valid option because its size is zero", navdata_option_ptr->tag);
          navdata_option_ptr = NULL;
          res = -1;
      }
      else
      {
          if( navdata_option_ptr->tag <= NAVDATA_NUM_TAGS)
          {
               //#ifdef DEBUG_NAVDATA_C
               //if (navdata_unpacked->ardrone_state & ARDRONE_COMMAND_MASK)
               //{
               //    ack = "[ACK]";
               //}
               //this->print(ack + "Received navdatas tags :" + itos(navdata_option_ptr->tag));
               //ack = "";
               //#endif

               navdata_unpacked->last_navdata_refresh |= NAVDATA_OPTION_MASK(navdata_option_ptr->tag);
          }


         switch( navdata_option_ptr->tag )
         {

              #define NAVDATA_OPTION(STRUCTURE,NAME,TAG) \
              case TAG: \
              navdata_option_ptr = ardrone_navdata_unpack( navdata_option_ptr, navdata_unpacked->NAME); \
              break;

              #define NAVDATA_OPTION_DEMO(STRUCTURE,NAME,TAG)  NAVDATA_OPTION(STRUCTURE,NAME,TAG)
              #define NAVDATA_OPTION_CKS(STRUCTURE,NAME,TAG) {}

              #include "navdata_keys.h"

          case NAVDATA_CKS_TAG:
              navdata_option_ptr = ardrone_navdata_unpack( navdata_option_ptr, navdata_cks );
              *cks = navdata_cks.cks;
              navdata_option_ptr = NULL; // End of structure
              break;


          default:
              //printf("Tag %d is an unknown navdata option tag", navdata_option_ptr->tag);
              navdata_option_ptr = (navdata_option_t *)(((uint64_t)navdata_option_ptr) + navdata_option_ptr->size);
              break;
          }
       }
    }

    return res;

}



void *nav_worker(void *arg)
{
    char msg[MAX_NAVDATA_SIZE];
    int res;
	uint32_t csk;

    memset(msg, '\0', sizeof(msg));

    while (!nav_stop) {
        res = recv_datagram(msg);    
        if (-1 == res) {
            printf(">>>>>>read navdata TIMEOUT: %s\n", strerror(errno));
            continue;
        }
        if (!(get_state(msg) & ARDRONE_NAVDATA_BOOTSTRAP) && is_nav_pkt(msg)) {    
            navdata = (navdata_t*)msg;
			ardrone_navdata_unpack_all(&navdata_unpacked, navdata, &csk);
            printf("Accs[X:%011.6fmg\tY:%011.6fmg\tZ:%11.6fmg]; Bat[%d\%]\r", 
                    navdata_unpacked.navdata_phys_measures.phys_accs[ACC_X],
                    navdata_unpacked.navdata_phys_measures.phys_accs[ACC_Y],
                    navdata_unpacked.navdata_phys_measures.phys_accs[ACC_Z],
                    navdata_unpacked.navdata_demo.vbat_flying_percentage);
        }
    }

    printf("nav worker exit.\n");
}

static void nav_thread_init()
{
    int err;
    err = pthread_create(&nav_pid, NULL, nav_worker, NULL);
    if (err != 0)
        printf("create nav thread error: %s\n", strerror(err));
}


/* 
 * value == 1, demo mode
 * value == 0, debug mode
 */
void config_navdata(int value)
{
    uint32_t option;
    char cmd[1024], tmp[128];
    memset(cmd, '\0', sizeof(cmd));
    memset(tmp, '\0', sizeof(tmp));

    option = NAVDATA_OPTION_MASK (NAVDATA_DEMO_TAG) | 
        NAVDATA_OPTION_MASK (NAVDATA_PHYS_MEASURES_TAG) |
        NAVDATA_OPTION_MASK (NAVDATA_RAW_MEASURES_TAG);

    if (0 == value) {
        sprintf(cmd, "AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", get_seq_num(), 
                    ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID);
        sprintf(tmp, "AT*CONFIG=%d,\"general:navdata_demo\",\"FALSE\"\r", get_seq_num());
        strcat(cmd, tmp);
        send_AT_cmd(cmd);
    }
    else {
        sprintf(cmd, "AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", get_seq_num(), 
                    ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID);
        sprintf(tmp, "AT*CONFIG=%d,\"general:navdata_demo\",\"TRUE\"\r", get_seq_num());
        strcat(cmd, tmp);
        send_AT_cmd(cmd);
        sprintf(cmd, "AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", get_seq_num(), 
                    ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID);
        sprintf(tmp, "AT*CONFIG=%d,\"general:navdata_options\",\"%d\"\r", get_seq_num(), option);
        strcat(cmd, tmp);
        send_AT_cmd(cmd);
    }
}


void navdata_init()
{
    char data[5] = {0x01, 0x01, 0x01, 0x01, 0x00};
    char msg[MAX_NAVDATA_SIZE], cmd[1024], tmp[64];
    int res = 0;
    uint32_t state;

    memset(msg, '\0', sizeof(msg));
    memset(cmd, '\0', sizeof(cmd));
    memset(tmp, '\0', sizeof(tmp));

    printf("\033[1;33m""Initing navdata...\n""\033[0m");

    nav_socket_init();

    while (1) {
try_again:
        send_datagram(data, strlen(data));
        res = recv_datagram(msg);
        if (res == -1) {
            printf(">>>>>>receive ardrone status TIMEOUT: %s\n", strerror(errno));
            goto try_again;
        } else {
            if (!is_nav_pkt(msg))
                continue;
            
            state = get_state(msg);

            if (state & ARDRONE_COMMAND_MASK) {
                sprintf(cmd, "AT*CTRL=%d,5,0\r", get_seq_num());
                send_AT_cmd(cmd);
            }
            
            config_navdata(0);
            break;
        }
    }

    while (1) {
        res = recv_datagram(msg);
        if (-1 == res) {
            printf(">>>>>>receive ardrone status TIMEOUT: %s\n", strerror(errno));
            goto try_again;
        } else {
            if (!is_nav_pkt(msg))
                continue;
            state = get_state(msg);

            if (state & ARDRONE_COMMAND_MASK) {
                sprintf(cmd, "AT*CTRL=%d,5,0\r", get_seq_num());
                send_AT_cmd(cmd);
            }

            if (!(state & ARDRONE_NAVDATA_DEMO_MASK) && !(state & ARDRONE_NAVDATA_BOOTSTRAP))
                break;
            else {
                ardrone_init();
                goto try_again;
                /*
                if (try <= 5) {
                    printf(">>>>>>navdata init failed, try = %d\n", try);
                    try++;
                    ardrone_init(); 
                    goto try_again;
                }
                else {
                    printf("Initing navdata failed after try %d times\n", try);
                    cmd_thread_exit();
                    return;           
                }
                */
            }
        }
    }

    printf("\033[1;33m""Init navdata OK.\n""\033[0m");
    if (!nav_pid)
        nav_thread_init();
}

void nav_thread_exit()
{
    nav_stop = 1;
}
