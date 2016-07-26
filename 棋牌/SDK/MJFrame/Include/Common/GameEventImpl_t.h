
#ifndef _GAMEEVENTIMPL_T_H
#define _GAMEEVENTIMPL_T_H

namespace MyGame
{
	template<typename event>
	class CDefaultEventImpl : public event
	{
	};

	template<typename event, int nVersion = 1>
	class TEventImpl : public CDefaultEventImpl<event>
	{
	public:
		virtual int GetVersion()
		{
			return nVersion;
		}
	};
};

#endif

