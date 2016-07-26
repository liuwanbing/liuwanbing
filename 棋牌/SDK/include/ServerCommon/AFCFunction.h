#pragma once

#ifndef AFCFUNCTION_H_2003_7
#define AFCFUNCTION_H_2003_7

/**************************************************************************************************************/

// AFC 全局函数

/**************************************************************************************************************/

//计算数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//安全删除指针
#define  SafeDelete(pData)	{ try { delete pData; } catch (...) { TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);} pData=NULL; } 

//安全删除指针
#define  SafeDeleteArray(pData)	{ try { delete [] pData; } catch (...) { TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);} pData=NULL; } 

/**************************************************************************************************************/

#endif