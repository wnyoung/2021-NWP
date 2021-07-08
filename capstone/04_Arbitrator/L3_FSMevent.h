typedef enum L3_event
{
    L3_event_VRmsgRcvd = 0,
    L3_event_TextmsgRcvd = 1,
    L3_event_VDmsgRcvd = 2,
    L3_event_Timeout = 3,
    L3_event_msgRcvd = 4,
    // L3_event_dataToSend = 4,
} L3_event_e;


void L3_event_setEventFlag(L3_event_e event);
void L3_event_clearEventFlag(L3_event_e event);
void L3_event_clearAllEventFlag(void);
int L3_event_checkEventFlag(L3_event_e event);