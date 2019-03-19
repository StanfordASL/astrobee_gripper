#include <p33Exxxx.h>
#include <stdint.h>

#include "pac_arm.h"
#include "main.h"
#include "clock.h"
#include "libpic30.h"
#include "drv_uart2.h"

void uart2_write(uint8_t *buf, unsigned int txIdx) {
    unsigned int sent = 0, numBytes = 0;

    while (numBytes < txIdx) {
        if (DRV_UART2_TXBufferIsEmpty()) {
            sent = DRV_UART2_Write (buf, txIdx - numBytes);
            buf += sent;
            numBytes += sent;
        }
    }

    while(U2STAbits.TRMT == 0);
}

int8_t uart2_read(uint8_t *buf, unsigned int rxIdx) {
    unsigned int recv = 0, numBytes = 0;

    while(numBytes < rxIdx) {
        if (!DRV_UART2_RXBufferIsEmpty()) {
            recv = DRV_UART2_Read (buf, rxIdx - numBytes);
            buf += recv;
            numBytes += recv;
        }
    }

    return numBytes;
}

int16_t positionTodeg (uint32_t position) {
    return (int16_t)(0.088 * (position - MID_POSITION));
}

uint32_t degToposition (int16_t degree) {
    int16_t tmp;
    
    if(degree >= MAX_ANGLE)
        tmp = MAX_ANGLE;
    else if(degree <= MIN_ANGLE)
        tmp = MIN_ANGLE;
    else
        tmp = degree;
    
    return (uint32_t)((tmp / 0.088) + MID_POSITION);
}

return_value_t pac_arm_init() {
    int i;
    
    arm_motor_state.init_return = RET_OK;
    
    arm_motor_state.goal_angle[0] = 90;
    arm_motor_state.goal_angle[1] =  0;
    
    // velocity unit is 0.229 RPM
    for(i=0; i<JOINT_NUM; i++)
        arm_motor_state.goal_speed[i] = 10;
    
    // initialize feedback packet
    for(i=0; i<ARM_BUF_SIZE; i++)
        arm_motor_state.fedb_packet[i] = 0;
    
    uart2_sync_write_register(TORQUE_EN, 1, 1);
    
    // set maximum goal PWM
    uart2_write_register(0, GOAL_PWM, 2, 275);
    uart2_write_register(1, GOAL_PWM, 2,  75);
    
    // set acceleration and velocity profile
    uart2_sync_write_register(PROFILE_ACC, 4, 10);
    uart2_sync_write_register(PROFILE_VEL, 4, 10);

    for(i=0; i<JOINT_NUM; i++) {
        uart2_write_register(i, GOAL_POS, 4, degToposition(arm_motor_state.goal_angle[i]));
    }
    
    return arm_motor_state.init_return;
}

unsigned short update_crc(unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size) {
    unsigned short i, j;
    unsigned short crc_table[256] = {
        0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
        0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
        0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
        0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
        0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
        0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
        0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
        0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
        0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
        0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
        0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
        0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
        0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
        0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
        0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
        0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
        0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
        0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
        0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
        0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
        0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
        0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
        0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
        0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
        0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
        0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
        0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
        0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
        0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
        0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
        0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
        0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
    };
 
    for(j = 0; j < data_blk_size; j++)
    {
        i = ((unsigned short)(crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
        crc_accum = (crc_accum << 8) ^ crc_table[i];
    }
 
    return crc_accum;
}

void uart2_write_register(uint8_t id, uint16_t addr, uint8_t size, int32_t value) {
    uint8_t txBuffer[30] = {}, rxBuffer[30] = {};
    uint8_t txIdx = 10, rxSize, i;
    unsigned short crc_value = 0;

    txBuffer[0] = 0xff;             // header
    txBuffer[1] = 0xff;             // header
    txBuffer[2] = 0xfd;             // header
    txBuffer[3] = 0x00;             // reserved
    txBuffer[4] = id;               // ID
    txBuffer[5] =  lowbyte(size+5);
    txBuffer[6] = highbyte(size+5);
    txBuffer[7] = WRITE_DATA;
    txBuffer[8] =  lowbyte(addr);
    txBuffer[9] = highbyte(addr);
    for(i=0; i < size; i++) {
        txBuffer[txIdx++] = lowbyte(value);
        value = value >> 8;
    }
    crc_value = update_crc(crc_value, txBuffer, txIdx);
    txBuffer[txIdx++] =  lowbyte(crc_value);
    txBuffer[txIdx++] = highbyte(crc_value);
    uart2_write(txBuffer, txIdx);  
    
    if (uart2_read_status_packet(rxBuffer, &rxSize) < 0)
        fatal();
}

void uart2_sync_write_register(uint16_t addr, uint8_t size, int32_t value) {
    uint8_t txBuffer[50] = {};
    uint8_t txIdx = 12, i, j;
    int32_t tmp_value = value;
    unsigned short crc_value = 0;

    txBuffer[0]  = 0xff;             // header
    txBuffer[1]  = 0xff;             // header
    txBuffer[2]  = 0xfd;             // header
    txBuffer[3]  = 0x00;             // reserved
    txBuffer[4]  = BROADCASTING_ID;  // ID
    txBuffer[5]  =  lowbyte((size+1)*JOINT_NUM+7);
    txBuffer[6]  = highbyte((size+1)*JOINT_NUM+7); 
    txBuffer[7]  = SYNC_WRITE;
    txBuffer[8]  =  lowbyte(addr);
    txBuffer[9]  = highbyte(addr);
    txBuffer[10] =  lowbyte(size);
    txBuffer[11] = highbyte(size);
    for(i=0; i<JOINT_NUM; i++) {
        txBuffer[txIdx++] = i;
        value = tmp_value;
        for(j=0; j< size; j++) {
            txBuffer[txIdx++] = lowbyte(value);
            value = value >> 8;
        }
    }
    crc_value = update_crc(crc_value, txBuffer, txIdx);
    txBuffer[txIdx++] =  lowbyte(crc_value);
    txBuffer[txIdx++] = highbyte(crc_value);
    uart2_write(txBuffer, txIdx);  
}

int8_t uart2_read_register(uint8_t *rxBuffer, uint8_t id, uint16_t addr, uint8_t size) {
    uint8_t txBuffer[30] = {};
    uint8_t rxSize;
    unsigned short crc_value = 0;

    txBuffer[0]  = 0xff;             // header
    txBuffer[1]  = 0xff;             // header
    txBuffer[2]  = 0xfd;             // header
    txBuffer[3]  = 0x00;             // reserved
    txBuffer[4]  = id;               // ID
    txBuffer[5]  =  lowbyte(7);
    txBuffer[6]  = highbyte(7);
    txBuffer[7]  = READ_DATA;
    txBuffer[8]  =  lowbyte(addr);
    txBuffer[9]  = highbyte(addr);
    txBuffer[10] =  lowbyte(size);
    txBuffer[11] = highbyte(size);
    crc_value = update_crc(crc_value, txBuffer, 12);
    txBuffer[12] =  lowbyte(crc_value);
    txBuffer[13] = highbyte(crc_value);
    uart2_write(txBuffer, 14);
    
    if (uart2_read_status_packet(rxBuffer, &rxSize) < 0)
        return -1;
    
    return 0;
}

int8_t uart2_sync_read_register(uint8_t *rxBuffer, uint16_t addr, uint8_t size) {
    uint8_t txBuffer[30] = {};
    uint8_t txIdx = 12, rxSize, i;
    unsigned short crc_value = 0;
    
    txBuffer[0]  = 0xff;             // header
    txBuffer[1]  = 0xff;             // header
    txBuffer[2]  = 0xfd;             // header
    txBuffer[3]  = 0x00;             // reserved
    txBuffer[4]  = BROADCASTING_ID;  // ID
    txBuffer[5]  =  lowbyte(JOINT_NUM+7);
    txBuffer[6]  = highbyte(JOINT_NUM+7);
    txBuffer[7]  = SYNC_READ;
    txBuffer[8]  =  lowbyte(addr);
    txBuffer[9]  = highbyte(addr);
    txBuffer[10] =  lowbyte(size);
    txBuffer[11] = highbyte(size);
    //txBuffer[txIdx++] = JOINT_NUM;
    for(i=0; i < JOINT_NUM; i++)
        txBuffer[txIdx++] = i;
    crc_value = update_crc(crc_value, txBuffer, txIdx);
    txBuffer[txIdx++] =  lowbyte(crc_value);
    txBuffer[txIdx++] = highbyte(crc_value);
    uart2_write(txBuffer, txIdx);
    
    if (uart2_read_status_packet(rxBuffer, &rxSize) < 0)
        return -1;
    
    return 0;
}

int8_t uart2_read_status_packet(uint8_t *buf, uint8_t *size) {    
    uart2_read(buf, 7);

    if (buf[0] != 0xff || buf[1] != 0xff || buf[2] != 0xfd)
        return -1;
    
    uart2_read(buf+7, (buf[5] | (buf[6] << 8)));
    *size = (buf[5] | (buf[6] << 8)) + 7;

    return 0;
}
