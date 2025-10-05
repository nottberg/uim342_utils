#include <unistd.h>
#include <sys/eventfd.h>

#include "CNCAxis.h"

CNCAxisComponent::CNCAxisComponent()
{
    m_type = CNCA_AXISCOMP_TYPE_NOTSET;
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
CNCAxisComponent::setType( CNCA_AXISCOMP_TYPE_T type )
{
    m_type = type;
}

CNCA_AXISCOMP_TYPE_T
CNCAxisComponent::getType()
{
    return m_type;
}

void
CNCAxisComponent::eventFD( int fd )
{
    printf( "CNCAxisComponent::eventFD - default\n" );
}

CNCA_RESULT_T
CNCAxisComponent::registerWithEventLoop( EventLoop *loop )
{
    printf( "CNCAxisComponent::registerWithEventLoop - default\n" );

    return CNCA_RESULT_SUCCESS;
}

CNCAxis::CNCAxis()
{

}

CNCAxis::~CNCAxis()
{

}

void
CNCAxis::setID( std::string value )
{
    m_id = value;
}

std::string
CNCAxis::getID()
{
    return m_id;
}

CNCA_RESULT_T
CNCAxis::addComponent( std::string compID, CNCA_AXISCOMP_TYPE_T compType, CNCAxisComponent *component )
{
    if( component == NULL )
        return CNCA_RESULT_FAILURE;

    component->setType( compType );

    std::map< std::string, CNCAxisComponent* >::iterator it = m_components.find( compID );

    if( it != m_components.end() )
    {
        it->second = component;
        return CNCA_RESULT_SUCCESS;
    }

    m_components.insert( std::pair< std::string, CNCAxisComponent* >( compID, component ) );

    return CNCA_RESULT_SUCCESS;
}

CNCA_RESULT_T
CNCAxis::removeComponent( std::string compID )
{
    std::map< std::string, CNCAxisComponent* >::iterator it = m_components.find( compID );

    if( it == m_components.end() )
        return CNCA_RESULT_FAILURE;

    m_components.erase( it );

    return CNCA_RESULT_SUCCESS;
}

CNCA_RESULT_T
CNCAxis::getComponentByID( std::string compID, CNCAxisComponent **component )
{
    if( component == NULL )
        return CNCA_RESULT_FAILURE;

    std::map< std::string, CNCAxisComponent* >::iterator it = m_components.find( compID );

    if( it != m_components.end() )
    {
        *component = it->second;
        return CNCA_RESULT_SUCCESS;
    }

    *component = NULL;

    return CNCA_RESULT_FAILURE;
}

//CNCM_RESULT_T
//CNCAxis::getBusID( std::string &id )
//{
//    id = "cbus0";
//    return CNCM_RESULT_SUCCESS;
//}

void
CNCAxis::updateParameter( std::string name, std::string value )
{
    m_parameters.insert( std::pair<std::string, std::string>(name, value));
}

CNCA_RESULT_T
CNCAxis::getParameter( std::string name, std::string &value )
{
    std::map< std::string, std::string >::iterator it = m_parameters.find( name );

    if( it == m_parameters.end() )
        return CNCA_RESULT_FAILURE;

    value = it->second;
    return CNCA_RESULT_SUCCESS;
}

CNCA_RESULT_T
CNCAxis::registerWithEventLoop( EventLoop *loop )
{
    printf( "CNCAxis::registerWithEventLoop - default\n" );

    for( std::map< std::string, CNCAxisComponent* >::iterator it = m_components.begin(); it != m_components.end(); it++ )
    {
        it->second->registerWithEventLoop( loop );
    }

    return CNCA_RESULT_SUCCESS;
}

void
CNCAxis::debugPrint()
{
    printf( "  === Axis: %s ===\n", m_id.c_str() );

    for( std::map< std::string, std::string >::iterator it = m_parameters.begin(); it != m_parameters.end(); it++ )
    {
        printf( "    %s: %s\n", it->first.c_str(), it->second.c_str() );
    }
}
