#include "mbed.h"

// PDU
#define L3MSG_TYPE_DATA     0
#define L3MSG_TYPE_ALLOW    1
#define L3MSG_TYPE_DECLINE  2
#define L3MSG_TYPE_TIMER  3

// SDU -> Æ÷Àå
#define L3MSG_OFFSET_TYPE   0
#define L3MSG_OFFSET_DATA   1

#define L3MSG_MAXDATASIZE   26

uint8_t L3Msg_encodeAllow(uint8_t* msg_data);		
uint8_t L3Msg_encodeDecline(uint8_t* msg_data);		
uint8_t L3Msg_encodeData(uint8_t* msg_data, uint8_t* data, int len);
uint8_t L3Msg_encodeTIMER(uint8_t* msg_data);

uint8_t* L3Msg_getWord(uint8_t* msg);