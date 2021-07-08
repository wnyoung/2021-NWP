#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "protocol_parameters.h"
#include "mbed.h"


//FSM state -------------------------------------------------
#define L3STATE_IDLE                0
#define L3STATE_RX                  1

//state variables
static uint8_t main_state = L3STATE_IDLE; //protocol state
static uint8_t prev_state = main_state;

//PDU context/size
uint8_t pdu[200];
uint8_t pduSize;

//serial port interface
static Serial pc(USBTX, USBRX);

void L3_initFSM()
{
    //initialize service layer
    // pc.attach(&L3service_processInputWord, Serial::RxIrq);  // �̰� ������ �ǳ�?

    // pc.printf("Give a word to send : ");
}

void L3_FSMrun(void)
{   
    // debug_if(DBGMSG_L3, "[L3] L3_FSMrun\n");
    if (prev_state != main_state)
    {
        debug_if(DBGMSG_L3, "[L3] State transition from %i to %i\n\n", prev_state, main_state);
        prev_state = main_state;
    }

    //FSM should be implemented here! ---->>>>
    switch (main_state)
    {
        case L3STATE_IDLE: //IDLE state description
            if (L3_event_checkEventFlag(L3_event_VRmsgRcvd)) //if VR reception event happens
            {
                debug_if(DBGMSG_L3, "[L3] @IDLE. L3_event_VRmsgRcvd\n");
                // 1-1. �ʿ��� ó�� ����: ���濡�� �߾���� �ش�.
                pduSize = L3Msg_encodeAllow(pdu);
                L3_LLI_dataReqFunc(pdu, (uint8_t)pduSize, (uint8_t)2);      
                debug_if(DBGMSG_L3, "[L3] sending Voice Allow msg....\n");

                // 1-2. �ʿ��� ó�� ����:Ÿ�̸� ����(��ŸƮ �Լ� ȣ�� �� �ڵ� ���� �Ǵ���,,,)
                L3_timer_startTimer();  //start timer to manage voice client

                // 2. State transmission
                main_state = L3STATE_RX; //goto RX state

                // 3. �̺�Ʈ Ŭ����
                L3_event_clearEventFlag(L3_event_VRmsgRcvd);
            }
            else if (L3_event_checkEventFlag(L3_event_TextmsgRcvd)) //if Text data reception event happens
            {
                // 1. �ʿ��� ó�� ����
                debug_if(DBGMSG_L3, "[L3][WARNING] cannot happen in IDLE state (event %i)\n", L3_event_VRmsgRcvd);

                // 2. State transmission

                // 3. �̺�Ʈ Ŭ����
                L3_event_clearEventFlag(L3_event_TextmsgRcvd);
            }
            else if (L3_event_checkEventFlag(L3_event_VDmsgRcvd)) //if VD reception event happens
            {
                // 1. �ʿ��� ó�� ����
                debug_if(DBGMSG_L3, "[L3][WARNING] cannot happen in IDLE state (event %i)\n", L3_event_VDmsgRcvd);
                // 2. State transmission
                
                // 3. �̺�Ʈ Ŭ����
                L3_event_clearEventFlag(L3_event_VDmsgRcvd);
            }
            else if (L3_event_checkEventFlag(L3_event_Timeout)) //if Timeout event happens
            {
                debug_if(DBGMSG_L3, "[L3] @IDLE. L3_event_Timeout\n");
                // 1-1. �ʿ��� ó�� ���� : timer stop
                L3_timer_stopTimer();

                // 1-2. �ʿ��� ó�� ����: id�� 0���� client���� ��[timer] expired�� �޽��� ����
                pduSize = L3Msg_encodeTIMER(pdu);
                L3_LLI_dataReqFunc(pdu, (uint8_t)pduSize, (uint8_t)0);
                debug_if(DBGMSG_L3, "[L3] sending Voice Timer msg....\n");

                // 2. State transmission

                // 3. �̺�Ʈ Ŭ����
                L3_event_clearEventFlag(L3_event_Timeout);
            }
            
            break;

        
        case L3STATE_RX: //RX state description
            
            if (L3_event_checkEventFlag(L3_event_VRmsgRcvd)) //if VR reception event happens
            {
                debug_if(DBGMSG_L3, "[L3] @RX. L3_event_VRmsgRcvd\n");
                // 1-1. �ʿ��� ó�� ����: ���濡�� �߾���� �ش�.
                pduSize = L3Msg_encodeDecline(pdu);
                L3_LLI_dataReqFunc(pdu, pduSize, 2);       // ���� : destination ID ���Ǹ� ���⼭ �� �� �� �ִ���,,,(id==2�� Ŭ���̾�Ʈ���Ը� �����ؾ���...)
                debug_if(DBGMSG_L3, "[L3] sending Voice Decline msg....\n");

                // 2. State transmission

                // 3. �̺�Ʈ Ŭ����
                L3_event_clearEventFlag(L3_event_VRmsgRcvd);
            }
            else if (L3_event_checkEventFlag(L3_event_TextmsgRcvd)) //if Texr data reception event happens
            {
                debug_if(DBGMSG_L3, "[L3] @RX. L3_event_TextmsgRcvd\n");
                // 1.1 �ʿ��� ó�� ����: PDU�� ���� text�� ����
                uint8_t* dataPtr = L3_LLI_getMsgPtr();
                uint8_t size = L3_LLI_getSize();

                // 1.2 �ʿ��� ó�� ����: ������ Ŭ���̾�Ʈ���� text����
                pduSize = L3Msg_encodeData(pdu, &dataPtr[L3MSG_OFFSET_DATA], size);
                L3_LLI_dataReqFunc(pdu, (uint8_t)pduSize, (uint8_t)1);

                debug_if(DBGMSG_L3, "[L3] sending Text data....\n"); 

                // 1.3 �ʿ��� ó�� ����: Ÿ�̸� ����
                // Ÿ�̸� ������  stop�� start
                L3_timer_stopTimer();
                L3_timer_startTimer();

                // 2. State transmission

                // 3. �̺�Ʈ Ŭ����
                L3_event_clearEventFlag(L3_event_TextmsgRcvd);
            }
            else if (L3_event_checkEventFlag(L3_event_VDmsgRcvd))       // if VD reception event happens
            {
                debug_if(DBGMSG_L3, "[L3] @RX. L3_event_VDmsgRcvd\n");
                // 1. �ʿ��� ó�� ����: Ÿ�̸� ���߱� 
                L3_timer_stopTimer();
                
                // 2. State transmission
                main_state = L3STATE_IDLE; //goto IDLE state
                // debug_if(DBGMSG_L3, "[L3] VD recived. go to IDLE.\n");

                // 3. �̺�Ʈ Ŭ����
                L3_event_clearEventFlag(L3_event_VDmsgRcvd);
            }
            else if (L3_event_checkEventFlag(L3_event_Timeout))         // if Timeout event happens
            {
                debug_if(DBGMSG_L3, "[L3] @RX. L3_event_Timeout\n");
                // 1-1. �ʿ��� ó�� ���� : timer stop
                L3_timer_stopTimer();

                // 1-2. �ʿ��� ó�� ����: id�� 0���� client���� ��[timer] expired�� �޽��� ����
                pduSize = L3Msg_encodeTIMER(pdu);
                L3_LLI_dataReqFunc(pdu, (uint8_t)pduSize, (uint8_t)0);
                debug_if(DBGMSG_L3, "[L3] sending Voice Timer msg....\n");

                // 2. State transmission
                main_state = L3STATE_IDLE;
                // debug_if(DBGMSG_L3, "[L3] timeout!  go to IDLE.\n");

                // 3. �̺�Ʈ Ŭ����
                L3_event_clearEventFlag(L3_event_Timeout);
            }
            break;
            

        default :
            break;
    }
}