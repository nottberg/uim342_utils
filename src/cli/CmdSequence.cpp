#include <unistd.h>
#include <sys/eventfd.h>

#include "CmdSequence.h"

CNCAxis::CNCAxis()
{

}

CNCAxis::~CNCAxis()
{

}

CNCMotorAxis::CNCMotorAxis()
{

}

CNCMotorAxis::~CNCMotorAxis()
{

}

CNCMachine::CNCMachine()
{

}

CNCMachine::~CNCMachine()
{

}

void
CNCMachine::setCanBus( std::string id, CANBus *bus )
{
    m_canBuses.insert( std::pair<std::string, CANBus*>( id, bus ) );
}

void
CNCMachine::setAxis( std::string id, CNCAxis *axisObj )
{
    m_axes.insert( std::pair<std::string, CNCAxis*>( id, axisObj ) );
}

CANBus*
CNCMachine::getCANBus()
{
    std::map<std::string, CANBus*>::iterator it = m_canBuses.begin();
    
    return it->second; 
}

CmdStep::CmdStep()
{

}

CmdStep::~CmdStep()
{

}

CmdStepExecuteCANRR::CmdStepExecuteCANRR()
{
    m_RR     = NULL;
    m_tgtBus = NULL;
}

CmdStepExecuteCANRR::~CmdStepExecuteCANRR()
{

}

void
CmdStepExecuteCANRR::setTargetBus( CANBus *busPtr )
{
    m_tgtBus = busPtr;
}

void
CmdStepExecuteCANRR::setRR( CANReqRsp *rrObj )
{
    m_RR = rrObj;
}

CmdSequence::CmdSequence()
{
    m_pendingFD =  eventfd(0, EFD_SEMAPHORE);
}

CmdSequence::~CmdSequence()
{
}

void
CmdSequence::calculateTimeout( uint curTime )
{

}

uint
CmdSequence::getTimeout()
{
    return -1;
}

uint
CmdSequence::getPendingFD()
{
    return m_pendingFD;
}

CS_RESULT_T
CmdSequence::setTargetMachine( CNCMachine *tgtMachine )
{
    m_tgtMachine = tgtMachine;

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
CmdSequence::appendStep( CmdStep *stepObj )
{
    m_stepList.push_back( stepObj );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
CmdSequence::startExecution()
{
    uint64_t u = 1;
    write( m_pendingFD, &u, sizeof(u) );

    return CS_RESULT_SUCCESS;
}

CS_ACTION_T
CmdSequence::getNextAction()
{
    return CS_ACTION_DONE;
}
