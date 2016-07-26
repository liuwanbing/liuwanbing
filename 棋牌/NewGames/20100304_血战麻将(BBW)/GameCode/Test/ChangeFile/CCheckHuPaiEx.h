#pragma once

#include "../GameMessage/CCheckHuPai.h"
///事件处理类（重载）
class CCheckHuPaiEx:public CCheckHuPai
{
public:
	CCheckHuPaiEx(void);
	~CCheckHuPaiEx(void);
	
public:
	//virtual 
	
	///检测能否听牌
	virtual bool CanTing(BYTE station,tagCPGNotifyEx &ResultData);
	//详细检测糊牌类型
	virtual bool CanHu(BYTE station,BYTE lastpai,tagCPGNotifyEx &ResultData,bool zimo,BYTE dianpao);
	///转换手牌数据的存储方式，为了方便糊牌检测
	virtual void ChangeHandPaiData(BYTE handpai[],BYTE pai);
	///平糊检测
	virtual bool CheckPingHu(BYTE lastpai=255,bool zimo=false,bool jing=false,bool CheckTing=false);
	///获取最大的组合牌型
	virtual bool  GetMaxCombination(bool zimo,bool jing,BYTE lastpai = 255);
	///平糊组牌
	virtual bool MakePingHu(CheckHuStruct &PaiData,PingHuStruct &hupaistruct,BYTE csnum=0);
	///初始化数据
	virtual void InitData();
	///////////详细糊牌类型//////////////////////////////////////////////


	//七对
	virtual int CheckQiDui();	
	//碰碰和
	virtual bool CheckPengPengHu();	
	//天糊
	virtual bool CheckTianHu();	
	//地糊
	virtual bool CheckDiHu();	
	//杠上花
	virtual bool CheckGangKai();	
	//杠后炮
	virtual bool CheckGangHouPao();	
	//抢杠和
	virtual bool CheckQiangGangHe();	


};

