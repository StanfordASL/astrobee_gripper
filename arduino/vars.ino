// Command addresses
const char ADDRESS_TOGGLE_AUTO = 0x33;
const char ADDRESS_OPEN = 0x34;
const char ADDRESS_CLOSE = 0x35;
const char ADDRESS_MARK = 0x36;
const char ADDRESS_ENGAGE = 0x40;
const char ADDRESS_DISENGAGE = 0x41;
const char ADDRESS_LOCK = 0x50;
const char ADDRESS_UNLOCK = 0x51;
const char ADDRESS_ENABLE_AUTO = 0x60;
const char ADDRESS_DISABLE_AUTO = 0x61;
const char ADDRESS_OPEN_EXPERIMENT = 0x70;
const char ADDRESS_NEXT_RECORD = 0x71;
const char ADDRESS_SEEK_RECORD = 0x72;
const char ADDRESS_CLOSE_EXPERIMENT = 0x73;

// Read registers
const char STATUS = 0x30;
const char RECORD = 0x7A;
const char EXPERIMENT = 0x7B;

// Protocol target constants
const char TARGET_PROXIMAL      = 0x00;
const char TARGET_DISTAL        = 0x01;
const char TARGET_GRIPPER       = 0x04;

// Instructions
const char INSTR_PING           = 0x01;
const char INSTR_READ           = 0x02;
const char INSTR_WRITE          = 0x03;
const char INSTR_REG_WRITE      = 0x04;
const char INSTR_ACTION         = 0x05;
const char INSTR_FACTORY_RESET  = 0x06;
const char INSTR_STATUS         = 0x55;

// Error number
const char ERR_RESULT           = 0x01;
const char ERR_INSTR            = 0x02;
const char ERR_CRC              = 0x03;
const char ERR_DATA_RANGE       = 0x04;
const char ERR_DATA_LEN         = 0x05;
const char ERR_DATA_LIM         = 0x06;
const char ERR_ACCESS           = 0x07;

// Gripper states
bool adhesive_engage;
bool wrist_lock;
bool automatic_mode_enable;
bool experiment_in_progress;

// Global variables
boolean new_data; 
size_t packet_len; 
size_t ndx;
int toggle;
bool send_acknowledge_packet; 

const size_t fixed_packet_len = 7; 
const int num_chars = 30;
const size_t hdr_size = 5;
unsigned char received_packet[num_chars];
unsigned char hdr_buffer[hdr_size];
