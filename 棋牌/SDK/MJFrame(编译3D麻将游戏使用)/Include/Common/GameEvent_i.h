#ifndef _GAMEEVENT_I_H
#define _GAMEEVENT_I_H

#include "metainfo.h"

namespace MyGame
{
	//////////////////////////////////////////////////////////////////////////
	// usage:
	// IUserManager * pUserManager = GetUserManager();
	// pUserManager->AddEventHandler(_E<IUserManagerEvent>(this));
	// 
	// pUserManager->RemoveEventHandler(_E<IUserManagerEvent>(this));
	//

	class IEventProducer
	{
	public:
		virtual void AddEventHandler(EVENT_PARAM& stEventHandler) = 0;
		virtual void RemoveEventHandler(EVENT_PARAM& stEventHandler) = 0;
	};


	//////////////////////////////////////////////////////////////////////////
	// usage:
	// class IUserManagerEvent : public IEvent
	// IUserManagerEvent* pEventHandler;
	//
	//	if (pEventHandler->GetVersion() == 1)
	//	{
	//		FireEvent(&IUserManagerEvent::OnXXX);
	//	}
	//	else if (pEventHandler->GetVersion() == 2)
	//	{
	//		FireEvent(&IUserManagerEvent::OnXXX, 1, _T("string")); // support entry param
	//	}
	//	

	class IEvent
	{
	public:
		virtual int GetVersion() = 0;
	};

};

#endif

