#ifndef __SOFIREEVENT_T_H__
#define __SOFIREEVENT_T_H__

#include "Svrdef.h"
#include "metainfo.h"

namespace MyGame
{
#pragma warning(push)
#pragma warning(disable:4311)
#pragma warning(disable:4003)
#pragma warning(disable:4786)
#define MAX_HANDLER_COUNT		5           //最多在一个回调方法上，有几个回调入口

//#ifndef UINT
//#define UINT unsigned long
//#endif	

	template<typename EventProducer = IEventProducer, int nEventCount = 5>           //这里的5表示只面会有几个大类的回调，如只有一个，可在设置为1
	class TSoEventProducerImpl : public EventProducer
	{
		enum {eventcount = nEventCount};
	public:
		TSoEventProducerImpl()
		{
			memset(m_arrEventType, 0, sizeof(m_arrEventType));
			memset(m_arrEventList, 0, sizeof(m_arrEventList));
		}
		virtual ~TSoEventProducerImpl() {}

	private:
		IEvent* m_arrEventList[eventcount*MAX_HANDLER_COUNT];
		UINT	m_arrEventType[eventcount];

		// IEventProducer
	public:
		virtual void AddEventHandler(EVENT_PARAM& stEventHandler)
		{			
			UINT dwEventID = stEventHandler.nEventID;
			int nEventSlotIndex = -1;
	
			if (GetSlotIndex(dwEventID, nEventSlotIndex, stEventHandler.pEventHandler)) 
			{

				m_arrEventList[nEventSlotIndex] = stEventHandler.pEventHandler;
			}			
		}

		virtual void RemoveEventHandler(EVENT_PARAM& stEventHandler)
		{
			UINT dwEventID = stEventHandler.nEventID;
			int nEventSlotIndex = -1;
			
			if (GetSlotIndex(dwEventID, nEventSlotIndex, stEventHandler.pEventHandler)) 
			{
				m_arrEventList[nEventSlotIndex] = NULL;
			}
		}

	protected:
		template<typename ElementType>
		BOOL GetEqualOrFreeIndexInRange(ElementType arrElment[], 
										int nStart,
										int nEnd,
										ElementType EqualValue, 
										int& nReturnIndex										
										)
		{
			BOOL bGet = FALSE;
			int nIndex = nStart;
			int nFreeIndex = -1;
						
			for(; nIndex < nEnd; nIndex++)
			{	//printf("1: nIndex%d, nEnd%d, %d = %d\n", nIndex, nEnd, arrElment[nIndex], EqualValue);
				if (arrElment[nIndex] == EqualValue)
				{
	
					break;
				}
				
				if (nFreeIndex == -1 &&  arrElment[nIndex] == 0)
				{
					nFreeIndex = nIndex;
				}
			}
			
			if (nIndex < nEnd) 
			{
				nReturnIndex = nIndex;
				bGet = TRUE;
			}
			else if (nFreeIndex != -1) 
			{
				nReturnIndex = nFreeIndex;
				bGet = TRUE;
			}
			else
			{
				bGet = FALSE;
			}
//			printf("nIndex:%d, nEnd:%d, nFreeIndex:%d\n", nIndex, nEnd, nFreeIndex);
			return bGet;
		}
		
		BOOL GetEventTypeIndex(UINT dwEventID, int& nEventTypeIndex)
		{
			BOOL bGet = FALSE;
			
			bGet = GetEqualOrFreeIndexInRange(
				m_arrEventType, 0, eventcount, dwEventID, nEventTypeIndex);

			return bGet;
		}

		BOOL GetSlotIndex(UINT dwEventID, int& nEventSlotIndex, IEvent* pEventHandler)
		{
			BOOL bGet = FALSE;
			int nEventTypeIndex = 0;

			if (GetEventTypeIndex(dwEventID, nEventTypeIndex)) 
			{

				int nStart = nEventTypeIndex*MAX_HANDLER_COUNT;
				int nEnd = nStart + MAX_HANDLER_COUNT;
				
				bGet = GetEqualOrFreeIndexInRange(
					m_arrEventList, nStart, nEnd, pEventHandler, nEventSlotIndex);

				if (bGet) 
				{
					m_arrEventType[nEventTypeIndex] = dwEventID;
				}
			}

			return bGet;
		}
		
		template<typename EventTypeList,typename EventType> inline
		BOOL PrepareEventHandlerList(EventTypeList& arrEventHandler, EventType* pDummy)
		{
			BOOL bPrepare = FALSE;
			UINT dwEventID = EventInfo<EventType>::ID();

			int nEventTypeIndex = 0;
			
			if (GetEventTypeIndex(dwEventID, nEventTypeIndex)) 
			{
				int nStart = nEventTypeIndex*MAX_HANDLER_COUNT;
				
				for(int i = 0; i < MAX_HANDLER_COUNT; i++)
				{
					arrEventHandler[i] = static_cast<EventType*>(m_arrEventList[nStart + i]);
				}

				bPrepare = TRUE;
			}

			return bPrepare;
		}

#define FIREEVENT_IMPLEMENT(expression)\
	Type* pDummy = NULL;\
	Type* arrEventHandler[MAX_HANDLER_COUNT] = {0};\
	\
	if (PrepareEventHandlerList(arrEventHandler, pDummy)) \
	{\
		for(int i = 0; i < MAX_HANDLER_COUNT; i++)\
		{\
			Type* pEventHandler = static_cast<Type*>(arrEventHandler[i]);\
		\
			if (pEventHandler != NULL)\
			{\
				expression;\
			}\
		}\
	}




	public:
		// 0 argument
		template<class Type> inline
			void FireEvent(int (Type::*EventName)())
		{
			FIREEVENT_IMPLEMENT((pEventHandler->*EventName)());
		}

		// 1 argument
		template<class Type, class ArgT> inline
			void FireEvent(int (Type::*EventName)(ArgT), const ArgT& arg)
		{
			FIREEVENT_IMPLEMENT((pEventHandler->*EventName)(arg));	
		}

		// 2 arguments
		template<class Type, class Arg1T, class Arg2T> inline
			void FireEvent(int (Type::*EventName)(Arg1T, Arg2T),
			const Arg1T& arg1, const Arg2T& arg2)
		{
			FIREEVENT_IMPLEMENT((pEventHandler->*EventName)(arg1, arg2));
		}

		// 3 arguments
		template<class Type, class Arg1T, class Arg2T, class Arg3T> inline
			void FireEvent(int (Type::*EventName)(Arg1T, Arg2T, Arg3T),
			const Arg1T& arg1, const Arg2T& arg2, const Arg3T& arg3)
		{
			FIREEVENT_IMPLEMENT((pEventHandler->*EventName)(arg1, arg2, arg3));
		}

		// 4 arguments
		template<class Type, class Arg1T, class Arg2T, class Arg3T, class Arg4T> inline
			void FireEvent(int (Type::*EventName)(Arg1T, Arg2T, Arg3T, Arg4T),
			const Arg1T& arg1, const Arg2T& arg2, const Arg3T& arg3, const Arg4T& arg4)
		{
			FIREEVENT_IMPLEMENT((pEventHandler->*EventName)(arg1, arg2, arg3, arg4));
		}

		// 5 arguments
		template<class Type, class Arg1T, class Arg2T, class Arg3T, class Arg4T, class Arg5T> inline
			void FireEvent(int (Type::*EventName)(Arg1T, Arg2T, Arg3T, Arg4T, Arg5T),
			const Arg1T& arg1, const Arg2T& arg2, const Arg3T& arg3,
			const Arg4T& arg4, const Arg5T& arg5)
		{
			FIREEVENT_IMPLEMENT((pEventHandler->*EventName)(arg1, arg2, arg3, arg4, arg5));
		}

		// 6 arguments
		template<class Type, class Arg1T, class Arg2T, class Arg3T, class Arg4T, class Arg5T, class Arg6T> inline
			void FireEvent(int (Type::*EventName)(Arg1T, Arg2T, Arg3T, Arg4T, Arg5T, Arg6T),
			const Arg1T& arg1, const Arg2T& arg2, const Arg3T& arg3,
			const Arg4T& arg4, const Arg5T& arg5, const Arg6T& arg6)
		{
			FIREEVENT_IMPLEMENT((pEventHandler->*EventName)(arg1, arg2, arg3, arg4, arg5, arg6));
		}
	};

		
#pragma warning(pop)

};

#endif
