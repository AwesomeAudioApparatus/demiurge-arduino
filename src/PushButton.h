#ifndef _DEMIURGE_PUSHBUTTON_H_
#define _DEMIURGE_PUSHBUTTON_H_


#include "Signal.h"

class PushButton : Signal {

public:
   PushButton(int position);
   ~PushButton();
};


#endif
