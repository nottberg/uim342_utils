#include <unistd.h>
#include <sys/eventfd.h>

#include "CNCAxisComponent.h"

CNCAxisComponent::CNCAxisComponent()
{

}

CNCAxisComponent::~CNCAxisComponent()
{

}

void
CNCAxisComponent::setID( std::string id )
{
    m_id = id;
}

std::string
CNCAxisComponent::getID()
{
    return m_id;
}

void
CNCAxisComponent::setFunction( std::string function )
{
    m_function = function;
}

std::string
CNCAxisComponent::getFunction()
{
    return m_function;
}

void
CNCAxisComponent::eventFD( int fd )
{
    printf( "CNCAxisComponent::eventFD - default\n" );
}

CNCACOMP_RESULT_T
CNCAxisComponent::registerWithEventLoop( EventLoop *loop )
{
    printf( "CNCAxisComponent::registerWithEventLoop - default\n" );

    return CNCACOMP_RESULT_SUCCESS;
}
