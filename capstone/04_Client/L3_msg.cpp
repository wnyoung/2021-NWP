#include "mbed.h"
#include "L3_msg.h"

/*int L3Msg_checkIfData(uint8_t* msg){

    return (msg[L3MSG_OFFSET_TYPE] == L3MSG_TYPE_DATA);
}

int L3Msg_checkIfAllow(uint8_t* msg){
    
    return (msg[L3MSG_OFFSET_TYPE] == L3MSG_TYPE_ALLOW);
}

int L3Msg_checkIfDecline(uint8_t* msg){
    
    return (msg[L3MSG_OFFSET_TYPE] == L3MSG_TYPE_DECLINE);
}

int L3Msg_checkIfTimer(uint8_t* msg){
    
    return (msg[L3MSG_OFFSET_TYPE] == L3MSG_TYPE_TIMER);
}*/

int L3Msg_checkIfData(uint8_t* msg){
    
    return (msg[L3MSG_OFFSET_TYPE] == L3MSG_TYPE_DATA);
}

uint8_t L3Msg_encodeData(uint8_t* msg_data, uint8_t* data, int len){
    msg_data[L3MSG_OFFSET_TYPE] = L3MSG_TYPE_DATA;
    memcpy(&msg_data[L3MSG_OFFSET_DATA], data, len*sizeof(uint8_t));

    return len+L3MSG_OFFSET_DATA;
}

uint8_t L3Msg_encodeVR(uint8_t* msg_data, uint8_t* data, int len){
    msg_data[L3MSG_OFFSET_TYPE] = L3MSG_TYPE_VR;
    memcpy(&msg_data[L3MSG_OFFSET_DATA], data, len*sizeof(uint8_t));

    return len+L3MSG_OFFSET_DATA;
}

uint8_t L3Msg_encodeVD(uint8_t* msg_data, uint8_t* data, int len){
    msg_data[L3MSG_OFFSET_TYPE] = L3MSG_TYPE_VD;
    memcpy(&msg_data[L3MSG_OFFSET_DATA], data, len*sizeof(uint8_t));

    return len+L3MSG_OFFSET_DATA;
}

uint8_t* L3Msg_getWord(uint8_t* msg)
{
    return &msg[L3MSG_OFFSET_DATA];
}