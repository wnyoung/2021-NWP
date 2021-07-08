#include "mbed.h"

// PDU
#define L3MSG_TYPE_DATA     0
/*#define L3MSG_TYPE_ALLOW    1
#define L3MSG_TYPE_DECLINE  2
#define L3MSG_TYPE_TIMER  3*/
#define L3MSG_TYPE_VR       4
#define L3MSG_TYPE_VD       5

// SDU -> 포장
#define L3MSG_OFFSET_TYPE   0
#define L3MSG_OFFSET_DATA   1

#define L3MSG_MAXDATASIZE   26

/*int L3Msg_checkIfData(uint8_t* msg);
int L3Msg_checkIfAllow(uint8_t* msg);
int L3Msg_checkIfDecline(uint8_t* msg);
int L3Msg_checkIfTimer(uint8_t* msg);*/
int L3Msg_checkIfData(uint8_t* msg);
uint8_t L3Msg_encodeData(uint8_t* msg_data, uint8_t* data, int len);
uint8_t L3Msg_encodeVR(uint8_t* msg_data, uint8_t* data, int len);
uint8_t L3Msg_encodeVD(uint8_t* msg_data, uint8_t* data, int len);

uint8_t* L3Msg_getWord(uint8_t* msg);