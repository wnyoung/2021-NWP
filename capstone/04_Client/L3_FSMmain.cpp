#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "L2_FSMmain.h"
#include "protocol_parameters.h"
#include "mbed.h"
#include <string>


//FSM state -------------------------------------------------
#define L3STATE_IDLE                0
#define L3STATE_VR                  1
#define L3STATE_VOICE               2


//state variables
static uint8_t main_state = L3STATE_IDLE; //protocol state
static uint8_t prev_state = main_state;

// ID variables
uint8_t defaultID = 1;                    
uint8_t midID = 2;
uint8_t voiceID = 0;
uint8_t dest_ID = 10;

//SDU (input)
static uint8_t originalWord[200];
static string vrWord = "vr";
static string vdWord = "vd";

const char * cVR = vrWord.c_str();
const char * cVD = vdWord.c_str();
static uint8_t wordLen=0;

// SDU
uint8_t sduSize;
uint8_t sdu[200];

// PDU
uint8_t pduSize;
uint8_t pdu[200];

//serial port interface
static Serial pc(USBTX, USBRX);


//application event handler : generating SDU from keyboard input
static void L3service_processInputWord(void)
{
    char c = pc.getc();
    if (!L3_event_checkEventFlag(L3_event_textSDU))
    {
        if (c == '\n' || c == '\r')
        {
            originalWord[wordLen++] = '\0';
            
            if(strncmp((const char*)originalWord, cVD, 2)==0) L3_event_setEventFlag(L3_event_VdSDU);
            else if(strncmp((const char*)originalWord, cVR, 2)==0) L3_event_setEventFlag(L3_event_VrSDU);
            else L3_event_setEventFlag(L3_event_textSDU);
            debug_if(DBGMSG_L3,"word is ready! ::: %s\n", originalWord);
        }
        else
        {
            originalWord[wordLen++] = c;
            if (wordLen >= L3_MAXDATASIZE-1)
            {
                originalWord[wordLen++] = '\0';
                L3_event_setEventFlag(L3_event_textSDU);
                pc.printf("\n max reached! word forced to be ready :::: %s\n", originalWord);
            }
        }
    }
}



void L3_initFSM()
{
    //initialize service layer
    pc.attach(&L3service_processInputWord, Serial::RxIrq);

    pc.printf("Give a word to send : ");
}

void L3_FSMrun(void)
{   
    if (prev_state != main_state)
    {
        debug_if(DBGMSG_L3, "[L3] State transition from %i to %i\n", prev_state, main_state);
        prev_state = main_state;
    }

    //FSM should be implemented here! ---->>>>
    switch (main_state)
    {
        // State : IDLE
        case L3STATE_IDLE: //IDLE state description
            
            // Event A
            if (L3_event_checkEventFlag(L3_event_VrSDU)) //if data reception event happens
            {
                //msg header setting
                //strcpy((char*)sdu, (char*)originalWord);
                sduSize = L3Msg_encodeVR(sdu, originalWord, wordLen);
                L3_LLI_dataReqFunc(sdu, sduSize);

                debug_if(DBGMSG_L3, "[L3] sending Voice Request..\n");

                /*pduSize = L3Msg_encodeData(pdu, originalWord, wordLen);
                L3_LLI_sendDataReqFunc(pdu, pduSize);

                pc.printf("[MAIN] sending to %i\n", dest_ID);*/
                //debug_if(DBGMSG_L3, "[L3] sending msg....0\n");
                L3_LLI_configReqFunc(L2L3_CFGTYPE_SRCID, midID);
                //debug_if(DBGMSG_L3, "[L3] sending msg....1\n");
                main_state = L3STATE_VR;
                //debug_if(DBGMSG_L3, "[L3] sending msg....2\n");
                memset(sdu, 0, 200);
                wordLen = 0;
                
                L3_event_clearEventFlag(L3_event_VrSDU);
            }
            // Event B
            else if (L3_event_checkEventFlag(L3_event_allowPDU)) //if data needs to be sent (keyboard input)
            {
                //msg header setting
                /*strcpy((char*)sdu, (char*)originalWord);
                L3_LLI_dataReqFunc(sdu, wordLen);

                debug_if(DBGMSG_L3, "[L3] sending msg....\n");

                wordLen = 0;

                pc.printf("Give a word to send : ");*/
                pc.printf("\n[Warning] Cannot Happen\n");
                L3_event_clearEventFlag(L3_event_allowPDU);
            }
            // Event C
            if (L3_event_checkEventFlag(L3_event_deniedPDU)) //if data reception event happens
            {
                pc.printf("\n[Warning] Cannot Happen\n");
                L3_event_clearEventFlag(L3_event_deniedPDU);
            }
            // Event D
            if (L3_event_checkEventFlag(L3_event_textSDU)) //if data reception event happens
            {
                pc.printf("\n[Warning] Cannot Happen\n");
                L3_event_clearEventFlag(L3_event_textSDU);
            }
            // Event E
            if (L3_event_checkEventFlag(L3_event_VdSDU)) //if data reception event happens
            {
                pc.printf("\n[Warning] Cannot Happen\n");
                L3_event_clearEventFlag(L3_event_VdSDU);
            }
            // Event F
            if (L3_event_checkEventFlag(L3_event_dataPDU)) //if data reception event happens
            {
                //Retrieving data info.
                uint8_t* dataPtr = L3_LLI_getMsgPtr();
                uint8_t size = L3_LLI_getSize();

                if(L3Msg_checkIfData(dataPtr)){
                    pc.printf("\n -------------------------------------------------\nRCVD from Arbitrator : %s (length:%i)\n -------------------------------------------------\n", 
                    L3Msg_getWord(dataPtr), size);
                }
                
                L3_event_clearEventFlag(L3_event_dataPDU);
            }
            // Event G
            if (L3_event_checkEventFlag(L3_event_timerPDU)) //if data reception event happens
            {
                pc.printf("\n[Warning] Cannot Happen\n");   
                L3_event_clearEventFlag(L3_event_timerPDU);
            }

            break;

        // State : Voice Request
        case L3STATE_VR:
            // Event A : VR SDU in
            if(L3_event_checkEventFlag(L3_event_VrSDU)){
                pc.printf("\n[Warning] Cannot Happen\n");

                L3_event_clearEventFlag(L3_event_VrSDU);
            }
            // Event B : 중재자로부터 [Accepted] PDU 받은 경우
            else if (L3_event_checkEventFlag(L3_event_allowPDU)) //if data reception event happens
            {
                //Retrieving data info.
                uint8_t* dataPtr = L3_LLI_getMsgPtr();
                uint8_t size = L3_LLI_getSize();

                pc.printf("\n -------------------------------------------------\nRCVD from Arbitrator : %s (length:%i)\n -------------------------------------------------\n", 
                L3Msg_getWord(dataPtr), size);

                // Id를 voice ID로 변경
                L3_LLI_configReqFunc(L2L3_CFGTYPE_SRCID, voiceID);

                // state 변경 (VR->VOICE)
                main_state = L3STATE_VOICE;

                L3_event_clearEventFlag(L3_event_allowPDU);
        
            }
            // Event C : 중재자로부터 [Declined] PDU 받은 경우
            else if (L3_event_checkEventFlag(L3_event_deniedPDU)) //if data reception event happens
            {
                //Retrieving data info.
                uint8_t* dataPtr = L3_LLI_getMsgPtr();
                uint8_t size = L3_LLI_getSize();

                pc.printf("\n -------------------------------------------------\nRCVD from Arbitrator : %s (length:%i)\n -------------------------------------------------\n", 
                            L3Msg_getWord(dataPtr), size);
                // Id를 default ID로 변경
                L3_LLI_configReqFunc(L2L3_CFGTYPE_SRCID, defaultID);

                // state 변경 (VR->IDLE)
                main_state = L3STATE_IDLE;
                
                L3_event_clearEventFlag(L3_event_deniedPDU);
            }
            // Event D : Text SDU in
            else if(L3_event_checkEventFlag(L3_event_textSDU)){
                
                pc.printf("\n[Warning] Cannot Happen\n");
                L3_event_clearEventFlag(L3_event_textSDU);
            }
            // Event E : Vd SDU in
            else if(L3_event_checkEventFlag(L3_event_VdSDU)){

                pc.printf("\n[Warning] Cannot Happen\n");
                L3_event_clearEventFlag(L3_event_VdSDU);
            }
            // Event F : 다른 클라이언트로부터 PDU 받은 경우
            else if (L3_event_checkEventFlag(L3_event_dataPDU)) {

                //Retrieving data info.
                uint8_t* dataPtr = L3_LLI_getMsgPtr();
                uint8_t size = L3_LLI_getSize();

                if(L3Msg_checkIfData(dataPtr)){
                    pc.printf("\n -------------------------------------------------\nRCVD from Arbitrator : %s (length:%i)\n -------------------------------------------------\n", 
                                L3Msg_getWord(dataPtr), size);
                }

                L3_event_clearEventFlag(L3_event_dataPDU);
            }
            // Event G : Timer PDU 받는 경우 (Cannot Happen)
            else if (L3_event_checkEventFlag(L3_event_timerPDU)) {

                pc.printf("\n[Warning] Cannot Happen\n");
                L3_event_clearEventFlag(L3_event_timerPDU);

            }

            break;

        // State : Voice
        case L3STATE_VOICE:
            // Event A : VR SDU in
            if(L3_event_checkEventFlag(L3_event_VrSDU)){

                pc.printf("\n[Warning] Cannot Happen\n");

                L3_event_clearEventFlag(L3_event_VrSDU);
            }
            // Event B : 중재자로부터 [Accepted] PDU 받은 경우
            if (L3_event_checkEventFlag(L3_event_allowPDU)){
               
                pc.printf("\n[Warning] Cannot Happen\n");

                L3_event_clearEventFlag(L3_event_allowPDU);
            }
            // Event C : 중재자로부터 [Declined] PDU 받은 경우
            else if (L3_event_checkEventFlag(L3_event_deniedPDU)){
                
                pc.printf("\n[Warning] Cannot Happen\n");

                L3_event_clearEventFlag(L3_event_deniedPDU);
            }
            // Event D : Text SDU in
            else if(L3_event_checkEventFlag(L3_event_textSDU)){
                //msg header setting
                //strcpy((char*)sdu, (char*)originalWord);
                sduSize = L3Msg_encodeData(sdu, originalWord, wordLen);
                L3_LLI_dataReqFunc(sdu, sduSize);

                debug_if(DBGMSG_L3, "[L3] sending Text..\n");
                
                memset(sdu, 0, 200);
                wordLen = 0;

                /*pduSize = L3Msg_encodeData(pdu, originalWord, wordLen);
                L3_LLI_sendDataReqFunc(pdu, pduSize);

                pc.printf("[MAIN] sending to %i\n", dest_ID);*/

                main_state = L3STATE_VOICE;

                wordLen = 0;
                L3_event_clearEventFlag(L3_event_textSDU);
            }
            // Event E : Vd SDU in
            else if(L3_event_checkEventFlag(L3_event_VdSDU)){
                //msg header setting
                //strcpy((char*)sdu, (char*)originalWord);
                sduSize = L3Msg_encodeVD(sdu, originalWord, wordLen);
                L3_LLI_dataReqFunc(sdu, sduSize);

                debug_if(DBGMSG_L3, "[L3] sending VD..\n");

                /*pduSize = L3Msg_encodeData(pdu, originalWord, wordLen);
                L3_LLI_sendDataReqFunc(pdu, pduSize);

                pc.printf("[MAIN] sending to %i\n", dest_ID);*/

                main_state = L3STATE_IDLE;
                L3_LLI_configReqFunc(L2L3_CFGTYPE_SRCID, defaultID);

                memset(sdu, 0, 200);
                wordLen = 0;
                L3_event_clearEventFlag(L3_event_VdSDU);
            }
            // Event F : 다른 클라이언트로부터 PDU 받은 경우
            else if (L3_event_checkEventFlag(L3_event_dataPDU)){

                pc.printf("\n[Warning] Cannot Happen\n");

                L3_event_clearEventFlag(L3_event_dataPDU);
            }
            // Event G : Timer PDU 받는 경우
            else if (L3_event_checkEventFlag(L3_event_timerPDU)){

                //Retrieving data info.
                uint8_t* dataPtr = L3_LLI_getMsgPtr();
                uint8_t size = L3_LLI_getSize();

                pc.printf("\n -------------------------------------------------\nRCVD from Arbitrator : %s (length:%i)\n -------------------------------------------------\n", 
                            L3Msg_getWord(dataPtr), size);
                    // state 변경 (VOICE -> IDLE)
                main_state = L3STATE_IDLE;

                // Id를 발언중 ID에서 1로 변경
                L3_LLI_configReqFunc(L2L3_CFGTYPE_SRCID, defaultID);

                L3_event_clearEventFlag(L3_event_timerPDU);
            }
            break;

        default :
            break;
    }
}