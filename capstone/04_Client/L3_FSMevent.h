typedef enum L3_event
{
    L3_event_VrSDU = 1,
    L3_event_allowPDU = 2,
    L3_event_deniedPDU = 3,
    L3_event_textSDU = 4,
    L3_event_VdSDU = 5,
    L3_event_dataPDU = 6,
    L3_event_timerPDU = 7
} L3_event_e;


void L3_event_setEventFlag(L3_event_e event);
void L3_event_clearEventFlag(L3_event_e event);
void L3_event_clearAllEventFlag(void);
int L3_event_checkEventFlag(L3_event_e event);