#include "mbed.h"
#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "protocol_parameters.h"
#include "time.h"
#include "string"

static uint8_t rcvdMsg[L3_MAXDATASIZE];
static uint8_t rcvdSize;
static int16_t rcvdRssi;
static int8_t rcvdSnr;

static string voiceRequest = "vr";
const char* cVR = voiceRequest.c_str();


//TX function
// void (*L3_LLI_dataReqFunc)(uint8_t* msg, uint8_t size);
void (*L3_LLI_dataReqFunc)(uint8_t* msg, uint8_t size, uint8_t destId);

//interface event : DATA_IND, RX data has arrived
void L3_LLI_dataInd(uint8_t* dataPtr, uint8_t size, int8_t snr, int16_t rssi)
{
    // debug_if(DBGMSG_L3, "\n --> DATA IND[L3] : size:%i, %s\n", size, dataPtr);
    debug_if(DBGMSG_L3, "\nDATA IND[L3] : dataPtr:%s, size:%i, snr: %i, rssi: %d\n", dataPtr, size, snr, rssi);
    

    memcpy(rcvdMsg, dataPtr, size*sizeof(uint8_t));
    debug_if(DBGMSG_L3, "rcvdMsg: %s\n", &rcvdMsg[L3MSG_OFFSET_DATA]);
    rcvdSize = size;
    rcvdSnr = snr;
    rcvdRssi = rssi;


    if (strcmp((const char*)&rcvdMsg[L3MSG_OFFSET_DATA], "vr") == 0) {       //VR
        L3_event_setEventFlag(L3_event_VRmsgRcvd);
        debug_if(DBGMSG_L3, "\n L3_event_setEventFlag(L3_event_VRmsgRcvd)\n", size, dataPtr);
    }
    else if (strcmp((const char*)&rcvdMsg[L3MSG_OFFSET_DATA], "vd") == 0) { //VD
        L3_event_setEventFlag(L3_event_VDmsgRcvd);
    }
    else {                                              //TEXT
        L3_event_setEventFlag(L3_event_TextmsgRcvd);
    }
    
}

uint8_t* L3_LLI_getMsgPtr()
{
    return rcvdMsg;
}
uint8_t L3_LLI_getSize()
{
    return rcvdSize;
}


void L3_LLI_setDataReqFunc(void (*funcPtr)(uint8_t*, uint8_t, uint8_t))
{
    L3_LLI_dataReqFunc = funcPtr;
}



