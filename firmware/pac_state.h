#ifndef PAC_STATE_H
#define	PAC_STATE_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdint.h>
    #include <stdbool.h>
 
    #define     JOINT_NUM                   2
    #define     ARM_BUF_SIZE                20
    #define     GRIPPER_BUF_SIZE            20
    #define     GRIPPER_STATUS_SIZE         2 
    
    typedef enum {
        RET_OK = 0,
        RET_ERROR = 1,
        RET_UNKNOWN = 127
    } return_value_t;
    
    typedef struct {
        return_value_t      init_return;
        uint32_t            fcy;
    } clock_data;
    
    typedef struct {
        return_value_t      init_return;
        int8_t              flag;
    } timer_data;
    
    typedef struct {
        return_value_t      init_return;
        uint16_t            time;
        uint16_t            max_time;
        int16_t             temperature;
    } board_data;
    
    typedef struct {
        return_value_t      init_return;
        
        int16_t             goal_angle[JOINT_NUM];
        int16_t             goal_speed[JOINT_NUM];
        uint8_t             fedb_packet[ARM_BUF_SIZE];
    } arm_motor_data;

    typedef struct {
        return_value_t      init_return;

        int8_t              adhesive_engage; 
        int8_t              wrist_lock;
        int8_t              automatic_mode_enable;
        int8_t              experiment_in_progress; 

        int16_t             current[GRIPPER_BUF_SIZE]; 
    } grip_motor_data;
    
    typedef struct {
        return_value_t      init_return;
        
        uint8_t             target;
        int16_t             address;
        int16_t             data;
    } uart1_rx_data;
    
    typedef struct {
        int16_t             V06_current;
        int16_t             V11_current;
    } current_data;
    
    void reset_board(void);
        
#ifdef	__cplusplus
}
#endif

#endif	/* PAC_STATE_H */

