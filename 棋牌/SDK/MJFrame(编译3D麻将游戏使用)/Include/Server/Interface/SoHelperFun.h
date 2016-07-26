
#if !defined(SO_HELPFUN_H)
#define SO_HELPFUN_H

#ifdef _WIN32
#include <tchar.h>
#endif
#include <stdio.h>

#include "typeDef.h"

#include "SoGameFactory_i.h"
#include "SoModuleimpl_t.h"


namespace MyGame
{

	//////////////////////////////////////////////////////////////////////////
	template<typename type>
	class DummyInstance
	{
	public:
		type* operator() ()
		{
			type* pDummy = NULL;
			return pDummy;
		}
	};

	template<typename EventProducer, typename Event>
	void __AddSoEventHandler(ISoGameFactory *pGameFactory, EventProducer* pDummy, Event* pHandler)
	{
		QueryInterface(pGameFactory, pDummy);
//		printf("__AddSoEventHandler::QueryInterface%d\n", pDummy);
		if (pDummy != NULL)
		{
			EVENT_PARAM stEventParam = __E(Event,pHandler);
			pDummy->AddEventHandler(stEventParam);
		}
	}

#define MyGame_AddSoEventHandler(ProducerType, Event, pHandler)\
	__AddSoEventHandler(\
		pHandler->GetGameFactory(),		\
		DummyInstance<ProducerType>()(), \
        static_cast<Event*>(pHandler)\
        );\


    template<typename EventProducer, typename Event>
        void __RemoveSoEventHandler(ISoGameFactory *pGameFactory, EventProducer* pDummy, Event* pHandler)
    {
        QueryInterface(pGameFactory, pDummy);
        if (pDummy != NULL)
        {
            EVENT_PARAM stEventParam = __E(Event,pHandler);
            pDummy->RemoveEventHandler(stEventParam);
        }
    }

#define MyGame_RemoveSoEventHandler(ProducerType, Event, pHandler)\
    __RemoveSoEventHandler(\
    pHandler->GetGameFactory(),		\
    DummyInstance<ProducerType>()(), \
    static_cast<Event*>(pHandler)\
    );\


    template<typename Interface>
        BOOL QueryInterface(ISoGameFactory *pSoGameFactory, Interface*& pInstance)
    {
		BOOL bQuery = FALSE;
		if (pSoGameFactory != NULL)
		{
			INTERFACE_PARAM param = __I(pInstance);
			
			param.pModule = pSoGameFactory->QueryInterface(param.nInterfaceID);
			

			pInstance = static_cast<Interface*>(param.pModule);
			bQuery = TRUE;
		}

		return bQuery;
	}

	//////////////////////////////////////////////////////////////////////////

#define IsGamePlaying(pProcess, bGameStart)\
	bGameStart = FALSE;			\
	if(pProcess != NULL)		\
	{							\
		if( gsPlaying == pProcess->GetGameState())	\
		{						\
			bGameStart = TRUE;	\
		}						\
	}							\

};// end namespace MiniGame
#endif

