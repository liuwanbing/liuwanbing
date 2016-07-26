#ifndef __SOGAMELOGGERIMPL_H__
#define __SOGAMELOGGERIMPL_H__
#include "afxwin.h"
#include "gameobject_i.h"
#include "SoGameLogger_i.h"

namespace MyGame
{
class CDeskLogicImpl;
class SoGameLogerImpl : public ISoGameLogger
{
public:
    SoGameLogerImpl(CDeskLogicImpl *pDeskLogicImpl);
    ~SoGameLogerImpl(void);
    virtual void  OutputTraceInfo(const char* msg, ...);     
	virtual void  OutputVerboseInfo(const char* msg, ...);     
	virtual void  OutputErrorInfo(const char* msg, ...); 
private:
    BOOL InitLog();
private:
    CDeskLogicImpl *m_pDeskLogicImpl;
};

};

#endif
