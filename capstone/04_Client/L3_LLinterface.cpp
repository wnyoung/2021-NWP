#include "mbed.h"
#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "protocol_parameters.h"
#include "time.h"
#include <string>

static uint8_t rcvdMsg[L3_MAXDATASIZE];
static uint8_t rcvdSize;
static int16_t rcvdRssi;
static int8_t rcvdSnr;

static string acceptWord = "[Accepted]";
static string declineWord = "[Declined]";
// test
static string testWord = "vr";

const char * cAcc = acceptWord.c_str();
const char * cDec = declineWord.c_str();
// test
const char * ctest = testWord.c_str();

//TX function
void (*L3_LLI_dataReqFunc)(uint8_t* msg, uint8_t size);
int (*L3_LLI_configReqFunc)(uint8_t type, uint8_t value);

//interface event : DATA_IND, RX data has arrived
void L3_LLI_dataInd(uint8_t* dataPtr, uint8_t size, int8_t snr, int16_t rssi)
{
    debug_if(DBGMSG_L3, "\n --> DATA IND : size:%i, %s\n", size, dataPtr);
    debug_if(DBGMSG_L3, "\n --> Rcvd DATA : size: %s\n", dataPtr);

    memcpy(rcvdMsg, dataPtr, size*sizeof(uint8_t));
    rcvdSize = size;
    rcvdSnr = snr;
    rcvdRssi = rssi;
    
    if(strncmp((const char*)rcvdMsg, cAcc, 10)==0) L3_event_setEventFlag(L3_event_allowPDU);
    else if(strncmp((const char*)rcvdMsg, cDec, 10)==0) L3_event_setEventFlag(L3_event_deniedPDU);
    else L3_event_setEventFlag(L3_event_dataPDU);

}

uint8_t* L3_LLI_getMsgPtr()
{
    return rcvdMsg;
}
uint8_t L3_LLI_getSize()
{
    return rcvdSize;
}

void L3_LLI_setDataReqFunc(void (*funcPtr)(uint8_t*, uint8_t))
{
    L3_LLI_dataReqFunc = funcPtr;
}

void L3_LLI_setConfigReqFunc(int (*funcPtr)(uint8_t, uint8_t))
{
    L3_LLI_configReqFunc = funcPtr;
}