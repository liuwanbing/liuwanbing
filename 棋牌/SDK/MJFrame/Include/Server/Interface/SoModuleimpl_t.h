//lightchen模拟客户端的工厂方法

#if !defined(SO_MODULEIMPL_H)
#define SO_MODULEIMPL_H

#include "Svrdef.h"
#include "metainfo.h"
#include "gameobject_i.h"
#include "SoGameFactory_i.h"

namespace MyGame
{
	template<typename Interface>
	class TModuleImpl : public Interface
	{
	public:
		TModuleImpl()
		{
			m_pSoGameFactory = NULL;			
		}
		virtual ~TModuleImpl()
		{
			UnRegisterInterface();
		}

		void RegisterInterface(ISoGameFactory *pSoGameFactory)
		{
			m_pSoGameFactory = pSoGameFactory;
			if(m_pSoGameFactory != NULL)
			{
				m_pSoGameFactory->RegisterInterface(ModuleInfo<Interface>::ID(), this);
			}
		}

		void UnRegisterInterface()
		{
			if(m_pSoGameFactory  != NULL)
			{
				m_pSoGameFactory->UnRegisterInterface(ModuleInfo<Interface>::ID());
			}
			m_pSoGameFactory = NULL;
		}


		ISoGameFactory* GetGameFactory()
		{
			return m_pSoGameFactory;
		}
		

		
	protected:
		ISoGameFactory *m_pSoGameFactory;
	};

}
#endif

