#include "pac_state.h"

void reset_board() {
    asm ("RESET");
    while(1);
}
