#include <winbase.h>
#include "BArithmetic.h"

//构造函数
BaseArithmetic::BaseArithmetic()
{
	return;
}

BaseArithmetic::~BaseArithmetic()
{
	return;
}

/*************************************************
*Function: 生成随机数
*explain:该函数利用一个固定的值来获取随机值，有效避免大量随机运算时出现规律性数据
*writer:
*		帅东坡
*Parameters:
*		void
*Return:
*		返回随机数
*************************************************/
int BaseArithmetic::MyRand()
{
	static int innerSeed = GetTickCount();

	return (((innerSeed = innerSeed * 214013L + 2531011L) >> 16) & 0x7fff );
}

/*************************************************
*Function: 获取权重随机值
*explain:该函数通过一组权重值来增大某一范围内随机值的生成概率；
*数据值不能超过宏【MAX_PATH】，权重值必须大于等于0
*writer:
*		帅东坡
*Parameters:
*		void
*Return:
*		返回随机数
*************************************************/
int BaseArithmetic::GetWeight(unsigned int iPoint[], int iCount)
{
	unsigned int idata[MAX_PATH + 1] = {0};//设置断点 数目比指定数组有效值多一个
	unsigned int imidd = 0;
	if (iCount > MAX_PATH)
	{
		return 0;
	}

	for (int i = 0; i < iCount; ++i)//根据数组有效值，划分断点
	{
		idata[i] = imidd;
		imidd += iPoint[i];
	}

	idata[iCount] = imidd;//将最后一个断点加进来

	unsigned int tag = MyRand() % imidd;//在指定范围内随机生成值

	for (int i = 0; i < iCount; ++i)
	{
		if (tag >= idata[i] && tag <= idata[i+1])//在断点之间，返回该断点值
		{
			return i;//按权重找到了随机值
		}
	}

	return 0;//异常，返回起始值
}

/*************************************************
*Function: 获得输入数字位数
*explain:该函数将输入的数算出其位数
*writer:
*		帅东坡
*Parameters:
*		需要计算的数字
*Return:
*		返回位数
*************************************************/
int BaseArithmetic::GetNumPlace(int inum)
{
	int iPlace = 0;
	inum = abs(inum);//取绝对值
	while(inum > 0)
	{
		inum = inum / 10;
		iPlace++;
	}

	return iPlace;
}