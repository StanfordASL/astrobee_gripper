#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

    #define max(a,b)        (((a) > (b)) ? (a) : (b))
    #define min(a,b)        (((a) < (b)) ? (a) : (b))

    #define lowbyte(v)      ((unsigned char) (v))
    #define highbyte(v)     ((unsigned char) (((unsigned int) (v)) >> 8))
    
    #define PROXIMAL_ID               0x01   
    #define DISTAL_ID                 0x02
    #define GRIPPER_ID                0x04
    
    extern clock_data       clock_state;
    extern timer_data       timer_state;
    extern board_data       board_state;
    extern arm_motor_data   arm_motor_state;
    extern grip_motor_data  grip_motor_state;
    extern uart1_rx_data    uart1_rx_state;
    extern current_data     current_state;
    
    void fatal(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

