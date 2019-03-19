#ifndef CLOCK_H
#define	CLOCK_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    #include "pac_state.h"

    #define FCY 70000000UL

    return_value_t clock_init(void);
    return_value_t timer_init(void);
    return_value_t counter_init(void);
    
    void __attribute__((__interrupt__, __auto_psv__)) _T1Interrupt(void);

#ifdef	__cplusplus
}
#endif

#endif	/* CLOCK_H */

