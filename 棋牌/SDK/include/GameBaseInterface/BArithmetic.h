#pragma once

//本类专门存入游戏开发者在开发过程中积累的大量有价值且重复使用率极高的算法
class BaseArithmetic
{
public:
	//构造函数
	BaseArithmetic();
	virtual ~BaseArithmetic();

public:
	//生成随机数
	int MyRand();

	//获取权重随机值
	int GetWeight(unsigned int iPoint[], int iCount);

	//获得输入数字位数
	int GetNumPlace(int inum);
};

#include "BArithmetic.inl"