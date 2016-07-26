
#if !defined(COMMON_METAINFO_H)
#define COMMON_METAINFO_H

#ifdef _WIN32
#include "wtypes.h"
#include "tchar.h"
#endif


#include "gameobject_i.h"

namespace MyGame
{
	//////////////////////////////////////////////////////////////////////////
	// e.g:DEFINE_MODULE_ID(0x00000001, IUserManager);
	// usage:
	//  int nID = ModuleInfo<IUserManager>::ID();
	//  
	//	ModuleID<0x00000001>::InterfaceType
	//	ModuleID<0x00000001>::Name()
	//
	

#define	DEFINE_MODULE_ID(moduleid, interface)\
	class interface;\
	template<>\
	class ModuleInfo<interface>\
	{\
	public:\
		enum{id = moduleid};\
		static const int ID()\
		{\
			return id;\
		}\
	};\
	\
	template<>\
	class ModuleID<moduleid>\
	{\
		typedef interface InterfaceType;\
	public:\
		static const LPTSTR Name() \
		{\
			static const LPTSTR szName = _T(#interface);\
			return szName;\
		}\
};
	

	template<typename Interface>
	class ModuleInfo;

	template<int id>
	class ModuleID;

	//////////////////////////////////////////////////////////////////////////
	// e.g:DEFINE_EVENT_ID(0x00000001, IUserManagerEvent);
	// usage:
	//	EventInfo<IUserManagerEvent>::ID()
	//

	template<typename event>
	class EventInfo;
	
#define	DEFINE_EVENT_ID(eventid, event)\
	class event;\
	template<>\
	class EventInfo<event>\
	{\
		enum{id = eventid};\
	public:\
		static const int ID()\
		{\
			return id;\
		}\
	};\

	
	
	//////////////////////////////////////////////////////////////////////////
	// INTERFACE_PARAM

	typedef struct	tagINTERFACE_PARAM
	{
		int		nInterfaceID;
		IGameObject* pModule;
	} INTERFACE_PARAM;
	
	template<typename Interface>
	INTERFACE_PARAM  __I(Interface*& pInterface)
	{
		INTERFACE_PARAM stInterface = {ModuleInfo<Interface>::ID(),  pInterface};
		return  stInterface;
	}

	//////////////////////////////////////////////////////////////////////////
	// EVENT_PARAM

	class IEvent;
	typedef struct  tagEVENT_PARAM
	{
		int		nEventID;
		IEvent* pEventHandler;
	} EVENT_PARAM;
	
	template<typename Event>
	class _Event
	{
	public:
		EVENT_PARAM  operator ()(Event* pEventHandler)
		{
			EVENT_PARAM stEvent = {EventInfo<Event>::ID(),  pEventHandler};
			return stEvent;
		}
	};

	#define __E(Event, pEventHandler)\
		_Event<Event>()(pEventHandler)

	//////////////////////////////////////////////////////////////////////////
	// PRODUCER_PARAM

	class IEventProducer;
	typedef  struct  tagPRODUCER_PARAM
	{
		int				nEventID;
		IEventProducer* pEventProducer;
	} PRODUCER_PARAM;
	
	template<typename Event>
	class _Producer
	{
	public:
		PRODUCER_PARAM  operator ()(IEventProducer* pEventProducer)
		{
			PRODUCER_PARAM stProducer = {EventInfo<Event>::ID(),  pEventProducer};
			return stProducer;
		}
	};

	#define __Pr(Event, pEventProducer)\
		_Producer<Event>()( pEventProducer)
	
	
	
}; // end namespace MyGame
#endif
//////////////////////////////////////////////////////////////////////////

