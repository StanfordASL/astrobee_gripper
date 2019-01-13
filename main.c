#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <p33Exxxx.h>
#include <qei32.h>

#include "main.h"
#include "clock.h"
#include "libpic30.h"
#include "drv_uart1.h"
#include "drv_uart2.h"
#include "pac_arm.h"
#include "pac_gripper.h"

clock_data          clock_state;
timer_data          timer_state;
board_data          board_state;
arm_motor_data      arm_motor_state;
grip_motor_data     grip_motor_state;
uart1_rx_data       uart1_rx_state;
current_data        current_state;

int main(int argc, char** argv) {
  uint8_t rxBuffer[50];
  int16_t i = 0, fdbIdx = 0;
  int16_t loop_count = 0;
 
  clock_init();
  timer_init();
  counter_init();

  // Initialize variables
  board_state.temperature = 0;
  
  uart1_rx_state.init_return = RET_OK;
  uart1_rx_state.target  = 255;
  uart1_rx_state.address = 255;
  uart1_rx_state.data    = 0; 

  current_state.V06_current  = 0;
  current_state.V11_current = 0;

  DRV_UART1_InitializerDefault();
  DRV_UART2_InitializerDefault();
  uart1_init();

  pac_arm_init();
  pac_grip_init();
  
  while(1) {
    if(!timer_state.flag) {
        continue;
    }
      
    TMR2 = 0;
    loop_count++;
    
    // ADC measurement
    AD1CON1bits.SAMP = 0;
    while(!_AD1IF);
    _AD1IF = 0;

    if(loop_count == 250) {
      board_state.temperature = -1;
    }
    else if(loop_count == 500) {
      for(i=0; i<10; i++) {
        arm_motor_state.fedb_packet[fdbIdx++] = rxBuffer[i+9];
      }
    }
    else if(loop_count == 750) {
      for(i=0; i<10; i++) {
        arm_motor_state.fedb_packet[fdbIdx++] = rxBuffer[i+9];
      }
    } 
    else if(loop_count == 1000) {
      if(uart2_read_register(rxBuffer, GRIPPER_ID, GRIPPER_STATUS, GRIPPER_STATUS_SIZE) == 0) {
        // TODO(acauligi): How are STATUS_L and STATUS_H returned from Teensy?
        int8_t STATUS_H = rxBuffer[9];
        int8_t STATUS_L = rxBuffer[10];

        grip_motor_data.adhesive_engage         = STATUS_L & 1;
        grip_motor_data.wrist_lock              = STATUS_L & 2;
        grip_motor_data.automatic_mode_enable   = STATUS_L & 8;
        grip_motor_data.experiment_in_progress  = (STATUS_L >> 7) & 1;
      }
    } 
    else if(loop_count == 1250) {
      board_state.max_time = 0;
      board_state.temperature = 0;
      fdbIdx = 0;
      loop_count = 0; 
    }

    board_state.time    = TMR2;
    board_state.max_time = max(TMR2,board_state.max_time);
          
    timer_state.flag = false;
  }

  uart1_delete();
  fatal();
  
  return (EXIT_SUCCESS);
}

void fatal() {
  return NULL;
}
