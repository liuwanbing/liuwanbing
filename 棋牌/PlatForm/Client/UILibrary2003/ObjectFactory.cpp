#include "StdAfx.h"
#include ".\objectfactory.h"

#include "Gif.h"

CObjectFactory::CObjectFactory(void)
{
	static	int iInitGdiPlusCount = 0;
}

CObjectFactory::~CObjectFactory(void)
{
}

HRESULT CObjectFactory::InitRes()
{
	if (FAILED(CGif::Initialize()))
	{
		return E_FAIL;
	}

	return NOERROR;
}

HRESULT CObjectFactory::UninitRes()
{
	if (FAILED(CGif::Uninitialize()))
	{
		return E_FAIL;
	}

	return NOERROR;
}

HRESULT CObjectFactory::CreateInstance(IUnknown * pUnkOuter, REFIID riid, void ** ppvObject)
{
	if (NULL == ppvObject)
	{
		return E_POINTER;
	}

	* ppvObject = NULL;

	if (CLSID_IGif == riid)
	{
		*ppvObject = new CGif;
		return NOERROR;
	}
	else
	{
		return E_NOINTERFACE ;
	}

	return NOERROR;
}