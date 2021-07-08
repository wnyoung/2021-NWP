#include "mbed.h"
#include "L3_msg.h"

const char* M_Allow = "[Accepted] Send Message.";
const char* M_Decline = "[Declined] duplicate";
const char* M_Timer = "[Timer] expired";


uint8_t L3Msg_encodeAllow(uint8_t* msg_data) {
    uint8_t len = strlen(M_Allow);

    msg_data[L3MSG_OFFSET_TYPE] = L3MSG_TYPE_ALLOW;
    memcpy(&msg_data[L3MSG_OFFSET_DATA], M_Allow, strlen(M_Allow) * sizeof(uint8_t));

    return len + L3MSG_OFFSET_DATA;
}
uint8_t L3Msg_encodeDecline(uint8_t* msg_data) {
    uint8_t len = strlen(M_Decline);

    msg_data[L3MSG_OFFSET_TYPE] = L3MSG_TYPE_DECLINE;
    memcpy(&msg_data[L3MSG_OFFSET_DATA], M_Decline, strlen(M_Decline) * sizeof(uint8_t));

    return len + L3MSG_OFFSET_DATA;
}

uint8_t L3Msg_encodeData(uint8_t* msg_data, uint8_t* data, int len) {
    msg_data[L3MSG_OFFSET_TYPE] = L3MSG_TYPE_DATA;
    memcpy(&msg_data[L3MSG_OFFSET_DATA], data, len * sizeof(uint8_t));

    return len + L3MSG_OFFSET_DATA;
}

uint8_t L3Msg_encodeTIMER(uint8_t* msg_data) {
    uint8_t len = strlen(M_Timer);

    msg_data[L3MSG_OFFSET_TYPE] = L3MSG_TYPE_TIMER;
    memcpy(&msg_data[L3MSG_OFFSET_DATA], M_Timer, strlen(M_Timer) * sizeof(uint8_t));

    return len + L3MSG_OFFSET_DATA;
}

uint8_t* L3Msg_getWord(uint8_t* msg)
{
    return &msg[L3MSG_OFFSET_DATA];
}