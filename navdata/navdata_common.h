/*********************************************************************
 * @file    navdata_commom.h
 * @author  kzhong1991<kzhong1991@gmail.com>
 * @date    2015-11-02
 * @brief   Common navdata configuration, the struct of navdata
 **********************************************************************/

#ifndef NAVDATA_COMMOM_H
#define NAVDATA_COMMOM_H
#include <stdint.h>

#define NAVDATA_HEADER                  0x55667788
#define NAVDATA_MAX_SIZE                2048
#define NAVDATA_MAX_CUSTOM_TIME_SAVE    20
#define NB_NAVDATA_DETECTION_RESULTS    4
#define NAVDATA_STATE_OFFSET            4
#define NAVDATA_SEQUENCE_NUM_OFFSET     8
#define NAVDATA_VERSION_OFFSET          12
#define NAVDATA_OPTION_TAG_OFFSET       16
#define NAVDATA_OPTION_SIZE_OFFSET      18
#define NAVDATA_OPTION_DATA_OFFSET      20

#define NB_CORNER_TRACKERS_WIDTH    5      /* number of trackers in width of current picture */
#define NB_CORNER_TRACKERS_HEIGHT   4      /* number of trackers in height of current picture */

#define DEFAULT_NB_TRACKERS_WIDTH    (NB_CORNER_TRACKERS_WIDTH+1)// + NB_BLOCK_TRACKERS_WIDTH)
#define DEFAULT_NB_TRACKERS_HEIGHT   (NB_CORNER_TRACKERS_HEIGHT+1)// + NB_BLOCK_TRACKERS_HEIGHT)

#define NAVDATA_OPTION_MASK(option) ( 1 << (option) )
#define NAVDATA_OPTION_FULL_MASK    ((1<<NAVDATA_NUM_TAGS)-1)


typedef int32_t    bool_t;

typedef struct _matrix33_t
{
    float m11;
    float m12;
    float m13;
    float m21;
    float m22;
    float m23;
    float m31;
    float m32;
    float m33;
} matrix33_t;

typedef struct _vector31_t {
    union {
        float v[3];
        struct
        {
            float x;
            float y;
            float z;
        };
    };
} vector31_t;

typedef union _vector21_t {
    float  v[2];
    struct
    {
        float x;
        float y;
    };
} vector21_t;

typedef struct _screen_point_t {
    int32_t x;
    int32_t y;
} screen_point_t;

typedef union _float_to_int_u {
    float f_value;
    int   i_value;
} float_to_int_u;


#define _ATTRIBUTE_PACKED_  __attribute__ ((packed))



// Define constants for gyrometers handling
typedef enum {
  GYRO_X    = 0,
  GYRO_Y    = 1,
  GYRO_Z    = 2,
  NB_GYROS  = 3
} def_gyro_t;

// Define constants for accelerometers handling
typedef enum {
  ACC_X   = 0,
  ACC_Y   = 1,
  ACC_Z   = 2,
  NB_ACCS = 3
} def_acc_t;

/**
 * @struct _velocities_t
 * @brief Velocities in float format
 */
typedef struct _velocities_t {
  float x;
  float y;
  float z;
} velocities_t;

// Define navdata option tag
#define NAVDATA_OPTION_DEMO(STRUCTURE,NAME,TAG)  TAG = 0,
#define NAVDATA_OPTION(STRUCTURE,NAME,TAG)       TAG ,
#define NAVDATA_OPTION_CKS(STRUCTURE,NAME,TAG)   NAVDATA_NUM_TAGS, TAG = 0xFFFF

typedef enum _navdata_tag_t {
    #include "navdata_keys.h"
} navdata_tag_t;


/*===============================================Navigation Data Stream==================================================
*  +------------+-------------+-----------------+--------------++------------------------------------++----++----------+
*  |   Header   | Drone state | Sequence Number | Version flag ||               Option1              ||... || Checksum |
*  +--------------------+-----------------------+--------------++------------------------------------++----++----------+
*  | 32-bit int |    32-bit   |   32-bit int    | 32-bit int   || id:16-bit | size:16-bit | data ....||... ||          |
*  +------------+-------------+-----------------+--------------++------------------------------------++----++----------+
************************************************************************************************************************/

typedef struct _navdata_option_t
{
    uint16_t tag;      /* Tag for a specific option */
    uint16_t size;     /* Length of the struct */
    #if defined(_MSC_VER)
        uint8_t  data[1];   /* Structure  complete with the specific tag */
    #else
        uint8_t  data[];
    #endif
}  _ATTRIBUTE_PACKED_  navdata_option_t;

/**
 * @brief Navdata structure sent over the network.
 */
typedef struct _navdata_t {
    uint32_t    header;			 /*!< Always set to NAVDATA_HEADER */
    uint32_t    ardrone_state;    /*!< Bit mask built from def_ardrone_state_mask_t */
    uint32_t    sequence;         /*!< Sequence number, incremented for each sent packet */
    bool_t     vision_defined;
    navdata_option_t  options[1];
} _ATTRIBUTE_PACKED_ navdata_t;


/*----------------------------------------------------------------------------*/
/**
 * @brief Minimal navigation data for all flights.
 */
typedef struct _navdata_demo_t {
    uint16_t     tag;					/*!< Navdata block ('option') identifier */
    uint16_t     size;					/*!< set this to the size of this structure */

    uint32_t     ctrl_state;             /*!< Flying state (landed, flying, hovering, etc.) defined in CTRL_STATES enum. */
    uint32_t     vbat_flying_percentage; /*!< battery voltage filtered (mV) */

    float       theta;                  /*!< UAV's pitch in milli-degrees */
    float       phi;                    /*!< UAV's roll  in milli-degrees */
    float       psi;                    /*!< UAV's yaw   in milli-degrees */

    int32_t      altitude;               /*!< UAV's altitude in centimeters */

    float       vx;                     /*!< UAV's estimated linear velocity */
    float       vy;                     /*!< UAV's estimated linear velocity */
    float       vz;                     /*!< UAV's estimated linear velocity */
    uint32_t     num_frames;			    /*!< streamed frame index */ // Not used -> To integrate in video stage.

    // Camera parameters compute by detection
    matrix33_t  detection_camera_rot;   /*!<  Deprecated ! Don't use ! */
    vector31_t  detection_camera_trans; /*!<  Deprecated ! Don't use ! */
    uint32_t	    detection_tag_index;    /*!<  Deprecated ! Don't use ! */

    uint32_t	    detection_camera_type;  /*!<  Type of tag searched in detection */

    // Camera parameters compute by drone
    matrix33_t  drone_camera_rot;		/*!<  Deprecated ! Don't use ! */
    vector31_t  drone_camera_trans;	    /*!<  Deprecated ! Don't use ! */

} _ATTRIBUTE_PACKED_ navdata_demo_t;

/*----------------------------------------------------------------------------*/
/**
 * @brief Last navdata option that *must* be included at the end of all navdata packets
 * + 6 bytes
 */
typedef struct _navdata_cks_t {
    uint16_t    tag;
    uint16_t    size;

    // Checksum for all navdatas (including options)
    uint32_t    cks;
} _ATTRIBUTE_PACKED_ navdata_cks_t;

/*----------------------------------------------------------------------------*/
/**
 * @brief Timestamp
 * + 6 bytes
 */
typedef struct _navdata_time_t {
    int16_t   tag;
    int16_t   size;

    uint32_t  time;  /*!< 32 bit value where the 11 most significant bits represents the seconds, */
                    /*  and the 21 least significant bits are the microseconds. */
} _ATTRIBUTE_PACKED_ navdata_time_t;


/*----------------------------------------------------------------------------*/
/**
 * @brief Raw sensors measurements
 */
typedef struct _navdata_raw_measures_t {
    uint16_t   tag;
    uint16_t   size;

  // +12 bytes
    uint16_t   raw_accs[NB_ACCS];    // filtered accelerometers
    int16_t    raw_gyros[NB_GYROS];  // filtered gyrometers
    int16_t    raw_gyros_110[2];     // gyrometers  x/y 110 deg/s
    uint32_t   vbat_raw;             // battery voltage raw (mV)
    uint16_t   us_debut_echo;
    uint16_t   us_fin_echo;
    uint16_t   us_association_echo;
    uint16_t   us_distance_echo;
    uint16_t   us_courbe_temps;
    uint16_t   us_courbe_valeur;
    uint16_t   us_courbe_ref;
    uint16_t   flag_echo_ini;
    // TODO:   uint16_t  frame_number; // from ARDrone_Magneto
    uint16_t   nb_echo;
    uint32_t   sum_echo;
    int32_t    alt_temp_raw;

    int16_t    gradient;

} _ATTRIBUTE_PACKED_ navdata_raw_measures_t;


typedef struct _navdata_pressure_raw_t {
    uint16_t   tag;
    uint16_t   size;

    int32_t    up;
    int16_t    ut;
    int32_t    Temperature_meas;
    int32_t    Pression_meas;

}_ATTRIBUTE_PACKED_ navdata_pressure_raw_t;

typedef struct _navdata_magneto_t {
    uint16_t           tag;
    uint16_t           size;

    int16_t            mx;
    int16_t            my;
    int16_t            mz;
    vector31_t        magneto_raw;       // magneto in the body frame, in mG
    vector31_t        magneto_rectified;
    vector31_t        magneto_offset;
    float             heading_unwrapped;
    float             heading_gyro_unwrapped;
    float             heading_fusion_unwrapped;
    char              magneto_calibration_ok;
    uint32_t           magneto_state;
    float             magneto_radius;
    float             error_mean;
    float             error_var;

} _ATTRIBUTE_PACKED_ navdata_magneto_t;

typedef struct _navdata_wind_speed_t {
    uint16_t   tag;
    uint16_t   size;

    float     wind_speed;
    float     wind_angle;
    float     wind_compensation_theta;
    float     wind_compensation_phi;
    float     state_x1;
    float     state_x2;
    float     state_x3;
    float     state_x4;
    float     state_x5;
    float     state_x6;
    float     magneto_debug1;
    float     magneto_debug2;
    float     magneto_debug3;

} _ATTRIBUTE_PACKED_ navdata_wind_speed_t;

typedef struct _navdata_kalman_pressure_t{
    uint16_t   tag;
    uint16_t   size;

    float     offset_pressure;
    float     est_z;
    float     est_zdot;
    float     est_bias_PWM;
    float     est_biais_pression;
    float     offset_US;
    float     prediction_US;
    float     cov_alt;
    float     cov_PWM;
    float     cov_vitesse;
    bool_t    bool_effet_sol;
    float     somme_inno;
    bool_t    flag_rejet_US;
    float     u_multisinus;
    float     gaz_altitude;
    bool_t    Flag_multisinus;
    bool_t    Flag_multisinus_debut;

} _ATTRIBUTE_PACKED_ navdata_kalman_pressure_t;



typedef struct _navdata_phys_measures_t {
    uint16_t   tag;
    uint16_t   size;

    float     accs_temp;
    uint16_t   gyro_temp;
    float     phys_accs[NB_ACCS];
    float     phys_gyros[NB_GYROS];
    uint32_t   alim3V3;              // 3.3volt alim [LSB]
    uint32_t   vrefEpson;            // ref volt Epson gyro [LSB]
    uint32_t   vrefIDG;              // ref volt IDG gyro [LSB]

} _ATTRIBUTE_PACKED_ navdata_phys_measures_t;


typedef struct _navdata_gyros_offsets_t {
    uint16_t   tag;
    uint16_t   size;

    float     offset_g[NB_GYROS];

} _ATTRIBUTE_PACKED_ navdata_gyros_offsets_t;


typedef struct _navdata_euler_angles_t {
    uint16_t   tag;
    uint16_t   size;

    float     theta_a;
    float     phi_a;

} _ATTRIBUTE_PACKED_ navdata_euler_angles_t;


typedef struct _navdata_references_t {
    uint16_t   tag;
    uint16_t   size;

    int32_t    ref_theta;
    int32_t    ref_phi;
    int32_t    ref_theta_I;
    int32_t    ref_phi_I;
    int32_t    ref_pitch;
    int32_t    ref_roll;
    int32_t    ref_yaw;
    int32_t    ref_psi;

    float     vx_ref;
    float     vy_ref;
    float     theta_mod;
    float     phi_mod;

    float     k_v_x;
    float     k_v_y;
    uint32_t   k_mode;

    float     ui_time;
    float     ui_theta;
    float     ui_phi;
    float     ui_psi;
    float     ui_psi_accuracy;
    int32_t    ui_seq;

} _ATTRIBUTE_PACKED_ navdata_references_t;


typedef struct _navdata_trims_t {
    uint16_t   tag;
    uint16_t   size;

    float     angular_rates_trim_r;
    float     euler_angles_trim_theta;
    float     euler_angles_trim_phi;

} _ATTRIBUTE_PACKED_ navdata_trims_t;

typedef struct _navdata_rc_references_t {
    uint16_t   tag;
    uint16_t   size;

    int32_t    rc_ref_pitch;
    int32_t    rc_ref_roll;
    int32_t    rc_ref_yaw;
    int32_t    rc_ref_gaz;
    int32_t    rc_ref_ag;

} _ATTRIBUTE_PACKED_ navdata_rc_references_t;


typedef struct _navdata_pwm_t {
    uint16_t       tag;
    uint16_t       size;

    uint8_t        motor1;
    uint8_t        motor2;
    uint8_t        motor3;
    uint8_t        motor4;
    uint8_t        sat_motor1;
    uint8_t        sat_motor2;
    uint8_t        sat_motor3;
    uint8_t        sat_motor4;
    float         gaz_feed_forward;
    float         gaz_altitude;
    float         altitude_integral;
    float         vz_ref;
    int32_t        u_pitch;
    int32_t        u_roll;
    int32_t        u_yaw;
    float         yaw_u_I;
    int32_t        u_pitch_planif;
    int32_t        u_roll_planif;
    int32_t        u_yaw_planif;
    float         u_gaz_planif;
    uint16_t       current_motor1;
    uint16_t       current_motor2;
    uint16_t       current_motor3;
    uint16_t       current_motor4;

  //WARNING: new navdata (FC 26/07/2011)
    float         altitude_prop;
    float         altitude_der;

} _ATTRIBUTE_PACKED_ navdata_pwm_t;


typedef struct _navdata_altitude_t {
    uint16_t     tag;
    uint16_t     size;

    int32_t      altitude_vision;
    float       altitude_vz;
    int32_t      altitude_ref;
    int32_t      altitude_raw;

    float       obs_accZ;
    float       obs_alt;
    vector31_t 	obs_x;
    uint32_t     obs_state;
    vector21_t	est_vb;
    uint32_t 	est_state ;

} _ATTRIBUTE_PACKED_ navdata_altitude_t;


typedef struct _navdata_vision_raw_t {
    uint16_t   tag;
    uint16_t   size;

    float     vision_tx_raw;
    float     vision_ty_raw;
    float     vision_tz_raw;

} _ATTRIBUTE_PACKED_ navdata_vision_raw_t;


typedef struct _navdata_vision_t {
    uint16_t         tag;
    uint16_t         size;

    uint32_t         vision_state;
    int32_t          vision_misc;
    float           vision_phi_trim;
    float           vision_phi_ref_prop;
    float           vision_theta_trim;
    float           vision_theta_ref_prop;

    int32_t          new_raw_picture;
    float           theta_capture;
    float           phi_capture;
    float           psi_capture;
    int32_t          altitude_capture;
    uint32_t         time_capture;     // time in TSECDEC format (see config.h)
    velocities_t    body_v;

    float           delta_phi;
    float           delta_theta;
    float           delta_psi;

    uint32_t         gold_defined;
    uint32_t         gold_reset;
    float           gold_x;
    float           gold_y;

} _ATTRIBUTE_PACKED_ navdata_vision_t;


typedef struct _navdata_vision_perf_t {
    uint16_t     tag;
    uint16_t     size;

    // +44 bytes
    float       time_szo;
    float       time_corners;
    float       time_compute;
    float       time_tracking;
    float       time_trans;
    float       time_update;
    float       time_custom[NAVDATA_MAX_CUSTOM_TIME_SAVE];
} _ATTRIBUTE_PACKED_ navdata_vision_perf_t;


typedef struct _navdata_trackers_send_t {
    uint16_t         tag;
    uint16_t         size;

    int32_t          locked[DEFAULT_NB_TRACKERS_WIDTH * DEFAULT_NB_TRACKERS_HEIGHT];
    screen_point_t  point[DEFAULT_NB_TRACKERS_WIDTH * DEFAULT_NB_TRACKERS_HEIGHT];

} _ATTRIBUTE_PACKED_ navdata_trackers_send_t;


typedef struct _navdata_vision_detect_t {
    uint16_t     tag;
    uint16_t     size;

    uint32_t     nb_detected;
    uint32_t     type[NB_NAVDATA_DETECTION_RESULTS];
    uint32_t     xc[NB_NAVDATA_DETECTION_RESULTS];
    uint32_t     yc[NB_NAVDATA_DETECTION_RESULTS];
    uint32_t     width[NB_NAVDATA_DETECTION_RESULTS];
    uint32_t     height[NB_NAVDATA_DETECTION_RESULTS];
    uint32_t     dist[NB_NAVDATA_DETECTION_RESULTS];
    float       orientation_angle[NB_NAVDATA_DETECTION_RESULTS];
    matrix33_t  rotation[NB_NAVDATA_DETECTION_RESULTS];
    vector31_t  translation[NB_NAVDATA_DETECTION_RESULTS];
    uint32_t     camera_source[NB_NAVDATA_DETECTION_RESULTS];

} _ATTRIBUTE_PACKED_ navdata_vision_detect_t;

typedef struct _navdata_vision_of_t {
    uint16_t   tag;
    uint16_t   size;

    float     of_dx[5];
    float     of_dy[5];

} _ATTRIBUTE_PACKED_ navdata_vision_of_t;


typedef struct _navdata_watchdog_t {
    uint16_t   tag;
    uint16_t   size;

    // +4 bytes
    int32_t    watchdog;

} _ATTRIBUTE_PACKED_ navdata_watchdog_t;

typedef struct _navdata_adc_data_frame_t {
    uint16_t  tag;
    uint16_t  size;

    uint32_t  version;
    uint8_t   data_frame[32];

} _ATTRIBUTE_PACKED_ navdata_adc_data_frame_t;

typedef struct _navdata_video_stream_t {
    uint16_t     tag;
    uint16_t     size;

    uint8_t      quant;			// quantizer reference used to encode frame [1:31]
    uint32_t     frame_size;		// frame size (bytes)
    uint32_t     frame_number;	// frame index
    uint32_t     atcmd_ref_seq;  // atmcd ref sequence number
    uint32_t     atcmd_mean_ref_gap;	// mean time between two consecutive atcmd_ref (ms)
    float       atcmd_var_ref_gap;
    uint32_t     atcmd_ref_quality; // estimator of atcmd link quality

    // drone2
    uint32_t     out_bitrate;     // measured out throughput from the video tcp socket
    uint32_t     desired_bitrate; // last frame size generated by the video encoder

    // misc temporary data
    int32_t      data1;
    int32_t      data2;
    int32_t      data3;
    int32_t      data4;
    int32_t      data5;

    // queue usage
    uint32_t     tcp_queue_level;
    uint32_t     fifo_queue_level;

} _ATTRIBUTE_PACKED_ navdata_video_stream_t;

typedef enum
{
    NAVDATA_HDVIDEO_STORAGE_FIFO_IS_FULL = (1<<0),
    NAVDATA_HDVIDEO_USBKEY_IS_PRESENT    = (1<<8),
    NAVDATA_HDVIDEO_USBKEY_IS_RECORDING  = (1<<9),
    NAVDATA_HDVIDEO_USBKEY_IS_FULL       = (1<<10)

}_navdata_hdvideo_states_t;


typedef struct _navdata_hdvideo_stream_t {
    uint16_t     tag;
    uint16_t     size;

    uint32_t     hdvideo_state;
    uint32_t     storage_fifo_nb_packets;
    uint32_t     storage_fifo_size;
    uint32_t     usbkey_size;           /*! USB key in kbytes - 0 if no key present */
    uint32_t     usbkey_freespace;      /*! USB key free space in kbytes - 0 if no key present */
    uint32_t     frame_number;          /*! 'frame_number' PaVE field of the frame starting to be encoded for the HD stream */
    uint32_t     usbkey_remaining_time; /*! time in seconds */

} _ATTRIBUTE_PACKED_ navdata_hdvideo_stream_t;


typedef struct _navdata_games_t {
    uint16_t     tag;
    uint16_t     size;
    uint32_t     double_tap_counter;
    uint32_t     finish_line_counter;

} _ATTRIBUTE_PACKED_ navdata_games_t;

typedef struct _navdata_wifi_t {
    uint16_t     tag;
    uint16_t     size;
    uint32_t     link_quality;

} _ATTRIBUTE_PACKED_ navdata_wifi_t;

typedef struct navdata_zimmu_3000_t {
    uint16_t    tag;
    uint16_t    size;

    int32_t     vzimmuLSB;
    float      vzfind;

}_ATTRIBUTE_PACKED_ navdata_zimmu_3000_t;


/**
 * @struct _navdata_unpacked_t
 * @brief Decoded navigation data.
*/
typedef struct _navdata_unpacked_t
{
    uint32_t         nd_seq;
    uint32_t         ardrone_state;
    bool_t          vision_defined;
    uint32_t         last_navdata_refresh;  /*! mask showing which block was refreshed when receiving navdata */

    #define NAVDATA_OPTION_DEMO(STRUCTURE,NAME,TAG)  STRUCTURE NAME ;
    #define NAVDATA_OPTION(STRUCTURE,NAME,TAG)       STRUCTURE NAME ;
    #define NAVDATA_OPTION_CKS(STRUCTURE,NAME,TAG)
    #include "navdata_keys.h"

}  _ATTRIBUTE_PACKED_  navdata_unpacked_t;

#if defined(_MSC_VER)
    /* Go back to default packing policy */
    #pragma pack()
#endif

#endif // NAVDATA_COMMOM_H
