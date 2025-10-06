#ifndef  __CNC_AXIS_H__
#define __CNC_AXIS_H__

#include <string>
#include <map>

#include "EventLoop.h"
#include "CNCAxisComponent.h"
#include "CANRR.h"

typedef enum CNCAxisResultTypes
{
    CNCA_RESULT_SUCCESS,
    CNCA_RESULT_FAILURE
}CNCA_RESULT_T;

class CNCAxis
{
    public:
        CNCAxis();
       ~CNCAxis();

        void setID( std::string value );
        std::string getID();

        CNCA_RESULT_T addComponent( std::string compID, std::string function, CNCAxisComponent *component );
        CNCA_RESULT_T removeComponent( std::string compID );

        CNCA_RESULT_T getComponentByID( std::string compID, CNCAxisComponent **component );

        CNCA_RESULT_T lookupCANDeviceByFunction( std::string deviceFunc, CANDevice **device );
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