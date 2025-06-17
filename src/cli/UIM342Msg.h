#ifndef _UIM342_MSG_H_
#define _UIM342_MSG_H_

#include "CANRR.h"

class U342MsgSerialNumber : public CANReqRsp
{
    public:
        U342MsgSerialNumber( uint producerID, uint consumerID );
       ~U342MsgSerialNumber();

    private:

};

#endif // _UIM342_MSG_H_
