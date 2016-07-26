#pragma once
#ifndef OBJECTFACTORY_HEAD_H
#define OBJECTFACTORY_HEAD_H
#include "IGif.h"

// {29B1F315-ECC3-4535-A898-94DAF7B308AA}
const GUID IID_IGif = {0x29b1f315, 0xecc3, 0x4535, {0xa8, 0x98, 0x94, 0xda, 0xf7, 0xb3, 0x8, 0xaa}};

const GUID CLSID_IGif = {0xb568bd34, 0xcc78, 0x4623, {0xb7, 0x98, 0x10, 0xca, 0x7d, 0x1c, 0x82, 0x88}};


class EXT_CLASS CObjectFactory
{
public:
	CObjectFactory(void);

	virtual ~CObjectFactory(void);

public:
	// 作用		：	在使用CObjectFactory前,初始化所需要资源,只需要调用一次;
	static HRESULT InitRes();

	// 作用		：	释放占用资源,在不使用该库时调用,注意不是在不使用该类时;
	static HRESULT UninitRes();

public:
	// 作用		：	创建GUID为riid的类的对象；
	// 参数1	：	预留；
	// 参数2	：	创建的类的GUID标记；
	// 参数3	：	创建的类的对象GUID标记；
	// 返回		：	见IUnknown::CreateInstance
	virtual HRESULT CreateInstance(IUnknown * pUnkOuter, REFIID riid, void ** ppvObject);
};

#endif