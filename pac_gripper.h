#ifndef PAC_GRIPPER_H
#define	PAC_GRIPPER_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include "pac_state.h"
        
    #define DCM_KP          3.5
    #define DCM_KD          0.5

    #define DEADZONE_CW     54
    #define DEADZONE_CCW    56
    
    
    #define CMD_SLOPE       3.0   

    #define CAL_SLOPE       40
    #define CAL_COUNT       25
    #define CAL_CLOSE       2550
    
    #define TICK_PER_RES    12    
    #define GEAR_RATIO      298

    return_value_t pac_grip_init(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* PAC_GRIPPER_H */

