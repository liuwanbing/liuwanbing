#pragma once

//#include "mj.h"
//#include "../ChangeFile/UpgradeMessageEx.h"

//基础逻辑类逻辑类 
//处理：数据基本操作，插入删除，查询等
class CLogicBase
{
public:
	CLogicBase(void);
	~CLogicBase(void);

public:

	///向数组添加一个数据
	///@param  arr[] 数组, data 要添加的数据, ArrLen 数组的长度
	///@return true 添加成功，false 数组满了，添加不成功
	static bool AddOneDataToArr(BYTE arr[],BYTE data,BYTE ArrLen)
	{
		for(int i=0;i<ArrLen;i++)
		{
			if(arr[i]==255)
			{
				arr[i] = data;
				return true;
			}
		}
		return false;
	}

	///从数组添删除一个数据
	///@param  arr[] 数组, data 要删除的数据, ArrLen 数组的长度
	///@return true 添加成功，false 数组满了，添加不成功
	static bool DeleteOneDataFromArr(BYTE arr[],BYTE data,BYTE ArrLen)
	{
		for(int i=0;i<ArrLen;i++)
		{
			if(arr[i]==data)
			{
				arr[i] = 255;
				return true;
			}
		}
		return false;
	}

	///获取掷色子的随机数据
	///@param  data0 色子1, data1 色子2, data2 色子3
	static void GetSeziData(BYTE *data0 = NULL, BYTE *data1 = NULL, BYTE *data2 = NULL)
	{
		srand((unsigned)time(NULL));
		if(data0 != NULL)
		{
			*data0 = rand()%6+1;
		}
		if(data1 != NULL)
		{
			Sleep(50);
			*data1 = rand()%6+1;
		}
		if(data2 != NULL)
		{
			Sleep(50);
			*data2 = rand()%6+1;
		}
	}
	
	///获取数组的有效元素个数
	///@param  arr[] 目标数组,data 无效值, ArrLen 数组长度
	///@return 有效元素个数
	static int GetArrInvalidNum(BYTE arr[],BYTE data,BYTE ArrLen)
	{
		int num = 0;
		for(int i=0;i<ArrLen;i++)
		{
			if(arr[i] != data)
			{
				num++;
			}
		}
		return num;
	}

	///判断数组中是否存在某个元素
	///@param  arr[] 目标数组,data 无效值, ArrLen 数组长度
	///@return 有效元素个数
	static bool IsHaveInArr(BYTE arr[],BYTE data,BYTE ArrLen)
	{
		for(int i=0;i<ArrLen;i++)
		{
			if(arr[i] == data)
			{
				return true;
			}
		}
		return false;
	}
	///判断数组中是否存在某个元素
	///@param  arr[] 目标数组,data 要查找的元素, ArrLen 数组长度
	///@return 有效元素个数
	static int GetNumInArr(BYTE arr[],BYTE data,BYTE ArrLen)
	{
		int num =0;
		for(int i=0;i<ArrLen;i++)
		{
			if(arr[i] == data)
			{
				num++ ;
			}
		}
		return num;
	}


};
