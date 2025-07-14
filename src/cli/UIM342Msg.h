#ifndef _UIM342_MSG_H_
#define _UIM342_MSG_H_

#if 0

#include "CANRR.h"

typedef enum UIM342MessageTypeEnum
{
    UIM342_MSG_8C_GET_SERIAL_NUMBER,
    UIM342_MSG_NOP
} UIM342_MSG_TYPE_T;

//CANReqRsp *createUIM342Msg( UIM342_MSG_TYPE_T type, uint producerID, uint consumerID );

class UIM342MsgGetSerialNumber : public CANReqRsp
{
    public:
        UIM342MsgGetSerialNumber();
       ~UIM342MsgGetSerialNumber();

        virtual uint getReqDataLength();
        virtual void getReqData( uint8_t *bufPtr );

        virtual void parseResponse();

        std::string getSerialNumberAsStr();

    private:

        uint m_serialNumber;
};

class UIM342MsgGetModel : public CANReqRsp
{
    public:
        UIM342MsgGetModel();
       ~UIM342MsgGetModel();

        virtual uint getReqDataLength();
        virtual void getReqData( uint8_t *bufPtr );

        virtual void parseResponse();

        std::string getModelAsStr();
        std::string getFWVersionAsStr();

    private:

        uint m_modelNumber;

        std::string m_model;
        std::string m_firmware;
};

class UIM342MsgGetCANBitrate : public CANReqRsp
{
    public:
        UIM342MsgGetCANBitrate();
       ~UIM342MsgGetCANBitrate();

        virtual uint getReqDataLength();
        virtual void getReqData( uint8_t *bufPtr );

        virtual void parseResponse();

        std::string getCANBitrateAsStr();

    private:

        uint m_bitrate;
};

class UIM342MsgGetCANNodeID : public CANReqRsp
{
    public:
        UIM342MsgGetCANNodeID();
       ~UIM342MsgGetCANNodeID();

        virtual uint getReqDataLength();
        virtual void getReqData( uint8_t *bufPtr );

        virtual void parseResponse();

        std::string getCANNodeIDAsStr();

    private:

        uint m_nodeID;
};

class UIM342MsgGetCANGroupID : public CANReqRsp
{
    public:
        UIM342MsgGetCANGroupID();
       ~UIM342MsgGetCANGroupID();

        virtual uint getReqDataLength();
        virtual void getReqData( uint8_t *bufPtr );

        virtual void parseResponse();

        std::string getCANGroupIDAsStr();

    private:

        uint m_groupID;
};

class UIM342MsgNop : public CANReqRsp
{
    public:
        UIM342MsgNop();
       ~UIM342MsgNop();

    private:

};

#endif

#endif // _UIM342_MSG_H_
