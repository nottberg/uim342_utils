#ifndef  __CNC_AXIS_H__
#define __CNC_AXIS_H__

#include <string>
#include <map>

#include "EventLoop.h"

typedef enum CNCAxisResultTypes
{
    CNCA_RESULT_SUCCESS,
    CNCA_RESULT_FAILURE
}CNCA_RESULT_T;

typedef enum CNCAxisComponentType
{
    CNCA_AXISCOMP_TYPE_NOTSET,
    CNCA_AXISCOMP_TYPE_CANBUS,
    CNCA_AXISCOMP_TYPE_MOTOR
}CNCA_AXISCOMP_TYPE_T;

class CNCAxisComponent : public ELEventCB 
{
    public:
        CNCAxisComponent();
       ~CNCAxisComponent();

        void setID( std::string id );
        std::string getID();

        void setType( CNCA_AXISCOMP_TYPE_T type );
        CNCA_AXISCOMP_TYPE_T getType();

        virtual void eventFD( int fd );

        virtual CNCA_RESULT_T registerWithEventLoop( EventLoop *loop );

    private:

        std::string          m_id;
        CNCA_AXISCOMP_TYPE_T m_type;
};

class CNCAxis
{
    public:
        CNCAxis();
       ~CNCAxis();

        void setID( std::string value );
        std::string getID();

        CNCA_RESULT_T addComponent( std::string compID, CNCA_AXISCOMP_TYPE_T compType, CNCAxisComponent *component );
        CNCA_RESULT_T removeComponent( std::string compID );

        CNCA_RESULT_T getComponentByID( std::string compID, CNCAxisComponent **component );

        //CNCM_RESULT_T getBusID( std::string &id );

        CNCA_RESULT_T getParameter( std::string name, std::string &value );

        void updateParameter( std::string name, std::string value );

        CNCA_RESULT_T registerWithEventLoop( EventLoop *loop );

        virtual void debugPrint();

    private:

        std::string m_id;

        std::map< std::string, CNCAxisComponent* > m_components;

        std::map< std::string, std::string > m_parameters;
};

#endif // __CNC_AXIS_H__