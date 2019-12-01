#ifndef _DEMIURGE_PUSHBUTTON_H_
#define _DEMIURGE_PUSHBUTTON_H_


#include "Signal.h"

class PushButton : public Signal {

public:
   PushButton(int position);
   ~PushButton() override;
};


#endif
