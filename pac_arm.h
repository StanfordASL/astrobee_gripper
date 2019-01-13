#ifndef PAC_ARM_H
#define	PAC_ARM_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include "pac_state.h"

    #define DYNAMIXEL_XM430_W210
    
    #define PING_DATA               0x01   
    #define READ_DATA               0x02
    #define WRITE_DATA              0x03
    #define REG_WRITE               0x04
    #define ACTION                  0x05
    #define FACTORY_RESET           0x06
    #define REBOOT                  0x08
    #define STATUS                  0x55
    #define SYNC_READ               0x82
    #define SYNC_WRITE              0x83
    #define BULK_READ               0x92
    #define BULK_WRITE              0x93

    #define BROADCASTING_ID         0xfe    
    
    #define PWM_LIMIT               36
    #define CURRENT_LIMIT           38
    #define ACC_LIMIT               40
    #define VEL_LIMIT               44
    #define MAX_POS_LIMIT           48
    #define MIN_POS_LIMIT           52

    #define TORQUE_EN               64
    #define ARM_LED                 65
    #define VEL_I_GAIN              76
    #define VEL_P_GAIN              78
    #define POS_D_GAIN              80
    #define POS_I_GAIN              82
    #define POS_P_GAIN              84
    #define FDK_2ND_GAIN            88
    #define FDK_1ST_GAIN            90

    #define GOAL_PWM                100
    #define GOAL_CURRENT            102
    #define GOAL_VEL                104
    #define PROFILE_ACC             108
    #define PROFILE_VEL             112
    #define GOAL_POS                116

    #define PRESENT_CURRENT         126
    #define PRESENT_VEL             128
    #define PRESENT_POS             132

    #define GRIPPER_STATUS          48 
    #define GRIPPER_RECORD          122 
    #define GRIPPER_EXPERIMENT      123

    #define MID_POSITION            2048
    #define MIN_ANGLE               -180
    #define MAX_ANGLE               180

    return_value_t pac_arm_init(void);

    int16_t positionTodeg (uint32_t position);
    uint32_t degToposition (int16_t degree);

    void uart2_write_register(uint8_t id, uint16_t addr, uint8_t size, int32_t value);
    void uart2_sync_write_register(uint16_t addr, uint8_t size, int32_t value);

    int8_t uart2_read_register(uint8_t *rxBuffer, uint8_t id, uint16_t addr, uint8_t size);
    int8_t uart2_sync_read_register(uint8_t *rxBuffer, uint16_t addr, uint8_t size);

    int8_t uart2_read_status_packet(uint8_t *buf, uint8_t *size);
    
#ifdef	__cplusplus
}
#endif

#endif	/* PAC_ARM_H */

