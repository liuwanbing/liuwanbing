#include "stdafx.h"
#include "SoGameFactory.h"

using namespace MyGame;

SoGameFactory::SoGameFactory(void)
{
    memset(m_Arrary, 0, sizeof(m_Arrary));
}

SoGameFactory::~SoGameFactory(void)
{
    
}

BOOL SoGameFactory::RegisterInterface(int nModuleID, IGameObject* pModule)
{
    for(int index=0; index < (sizeof(m_Arrary) / sizeof(m_Arrary[0])); index++)
    {
        if (m_Arrary[index].m_itemId == 0)
        {
            m_Arrary[index].m_itemId = nModuleID;
            m_Arrary[index].m_OjectPtr = pModule;
            return TRUE;
        }
    }
	return FALSE;
}

BOOL SoGameFactory::UnRegisterInterface(int nModuleID)
{
    for(int index=0; index < (sizeof(m_Arrary) / sizeof(m_Arrary[0])); index++)
    {
        if (m_Arrary[index].m_itemId == nModuleID)
        {
            m_Arrary[index].m_OjectPtr = NULL;
            return TRUE;
        }
    }
	return FALSE;
}

IGameObject* SoGameFactory::QueryInterface(int nModuleID)
{
    for(int index=0; index < (sizeof(m_Arrary) / sizeof(m_Arrary[0])); index++)
    {
        if (m_Arrary[index].m_itemId == nModuleID)
        {
            return m_Arrary[index].m_OjectPtr;
        }
    }
	return NULL;
}

static SoGameFactory    g_GameFactory;

SoGameFactory* SoGameFactory::GetInstance(void)
{
    return &g_GameFactory;
}




