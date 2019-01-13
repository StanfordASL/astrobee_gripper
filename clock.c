#include "p33Exxxx.h"
#include "stdbool.h"

#include "clock.h"
#include "main.h"

// Select Internal FRC at POR
_FOSCSEL(FNOSC_FRC & IESO_OFF);

// Enable Clock Switching and Configure Primary Oscillator in XT mode
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_NONE);      

// Watchdog Timer Enabled/Disabled
_FWDT(FWDTEN_OFF);

// Disable JTAG
_FICD(JTAGEN_OFF & ICS_PGD2); 

return_value_t clock_init() {
    /*
     * 70 MIPS:
     * FOSC = Fin*(PLLDIV+2)/((PLLPRE+2)*2(PLLPOST+1))
     * FOSC = 7.37 MHz(74+2)/((0+2)*2(0+1)) = 140.03 Mhz
     * Fcy = FOSC/2
     */
    
    PLLFBD = 74;
    CLKDIVbits.PLLPRE  = 0;
    CLKDIVbits.PLLPOST = 0;
        
    // Initiate Clock Switch to FRC oscillator with PLL (NOSC=0b001)
    __builtin_write_OSCCONH(0x01);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC!= 0b001);
    
    // Wait for PLL to lock
    while (OSCCONbits.LOCK!= 1);
    
    clock_state.fcy = 70000000;
    clock_state.init_return = RET_OK;
    
    return clock_state.init_return;
}

return_value_t timer_init() {
    T1CONbits.TON    = 0;
    T1CONbits.TCS    = 0;
    T1CONbits.TGATE  = 0;
    T1CONbits.TCKPS1 = 1;
    T1CONbits.TCKPS0 = 0;
    
    TMR1 = 0x00;
    // period = PR1 * pre-scale * Tcy
    // PR1 = 1 milli-second
    PR1  = 1094;
    
    IPC0bits.T1IP = 0x01;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
        
    T1CONbits.TON = 1;
    
    timer_state.flag        = false;
    timer_state.init_return = RET_OK;
    
    return timer_state.init_return;
}

return_value_t counter_init() {
    T2CONbits.TON    = 0;
    T2CONbits.TCS    = 0;
    T2CONbits.TGATE  = 0;
    T2CONbits.TCKPS1 = 1;
    T2CONbits.TCKPS0 = 1;
    
    TMR2 = 0x00;
    // PR2 = 100 milli-second
    PR2  = 27344;
    
    T2CONbits.TON = 1;
    
    board_state.time    = 0;
    board_state.max_time = 0;
    
    board_state.init_return = RET_OK;
    
    return board_state.init_return;
}

void __attribute__((__interrupt__, __auto_psv__)) _T1Interrupt(void) {
    timer_state.flag = true;
    IFS0bits.T1IF    = 0;
}
