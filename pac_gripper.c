#include <p33Exxxx.h>
#include <stdlib.h>
#include <qei32.h>

#include "pac_gripper.h"
#include "main.h"

return_value_t pac_grip_init() {
  grip_motor_state.init_return = RET_OK;

  for(int i=0; i<GRIPPER_BUF_SIZE; i++) {
    grip_motor_state.current[i] = 0.;
  }
        
  grip_motor_state.adhesive_engage        = 0; 
  grip_motor_state.wrist_lock             = 0;
  grip_motor_state.automatic_mode_enable  = 0;
  grip_motor_state.experiment_in_progress = 0; 

  grip_motor_state.srv_l1_curr            = 0;
  grip_motor_state.srv_l2_curr            = 0;
  grip_motor_state.srv_r_curr             = 0;
  grip_motor_state.srv_w_curr             = 0;
  grip_motor_state.tof                    = 0;
  grip_motor_state.temp                   = -1;
    
  return grip_motor_state.init_return;
}
