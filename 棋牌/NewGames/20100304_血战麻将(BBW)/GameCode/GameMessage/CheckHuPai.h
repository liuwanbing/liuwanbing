#pragma once
#include "..\ChangeFile\GameDataEx.h"
#include "..\ChangeFile\UserDataEx.h"

class CServerGameDesk;

///糊牌检测类
class CheckHuPai
{
public:
	CheckHuPai(void);
	~CheckHuPai(void);
	
public:	
	CServerGameDesk  *pDesk;//父类（桌子）

	///当前检测玩家的位置
	BYTE            m_byStation;
	///玩家是否自摸胡牌
	bool            m_bZimo;
	///糊牌数结构数组
	PingHuStruct	hupaistruct;
	PingHuStruct	TempHupaistruct;///糊牌数结构数组
	///糊牌检测临时数据
	CheckHuStruct	HuTempData;		
	///糊牌检测临时数据(无财神)
	CheckHuStruct	NoJingHuTempData;		
	///精牌总数
	BYTE			m_byJingNum;
	///糊牌类型
	BYTE			m_byHuType[MAX_HUPAI_TYPE];
	//当前最大番数值
	int				m_iMaxFan;
	//最后自摸或点炮的牌
	BYTE			m_byLastPai;
	//手牌不超过20张，0为结束标志
	BYTE			m_byArHandPai[HAND_CARD_NUM];


	///组牌数据
	BYTE		   m_byShunNum;			//顺子的个数
	BYTE           m_byShunData[4];		//顺子的数据
	BYTE		   m_byKeziNum;			//刻子的个数
	BYTE           m_byKeziData[4];		//刻子的数据
	BYTE		   m_byGangNum;			//杠牌的个数
	BYTE           m_byGangData[4][2];	//杠牌的数据
	BYTE		   m_byJiang;			//将牌数据

	BYTE		   m_byChiNum;			//吃牌的个数
	BYTE		   m_byPengNum;			//碰牌的个数


public:
	//拷贝父类指针
	void SetDeskPoint(CServerGameDesk *desk);

	///检测能否听牌
	virtual bool CanTing(BYTE station,BYTE CanOut[]);

	///检测能否吃听牌：吃后听牌
	virtual bool ChiTing(BYTE station,BYTE CanOut[],BYTE pai[][3]);

	///检测杠后能否保存听牌状态
	virtual bool GangTing(BYTE station,tagCPGNotifyEx &cpg ,BYTE ps = 255,bool CanTing[] = NULL);

	//只检测能否糊牌
	virtual bool CanHu(BYTE station,BYTE lastpai,bool zimo);

	//详细检测糊牌类型
	virtual bool CanHu(BYTE station,BYTE lastpai,BYTE hutype[],bool zimo);

	///将吃碰杠数据加入到糊牌结构中
	virtual void AddGCPData(GCPStructEx gcpData[],PingHuStruct * tempstruct);

	///转换手牌数据的存储方式，为了方便糊牌检测
	virtual void ChangeHandPaiData(BYTE handpai[],BYTE pai);

	///添加一张牌到临时数组中
	virtual void AddAPaiToArr(BYTE pai);

	///平糊检测
	virtual bool CheckPingHu(BYTE lastzhuapai=255,bool zimo=false,bool jing=false,bool CheckTing=false);

	///设置精牌
	virtual void SetJingPai();

	///恢复精牌
	virtual void ReSetJingPai();

	///获取最大的组合牌型
	virtual bool  GetMaxCombination(bool zimo,bool jing,BYTE lastpai = 255);

	///平糊组牌
	virtual bool MakePingHu(CheckHuStruct &PaiData,PingHuStruct &hupaistruct,int csnum=0);

	///恢复牌数,把减掉的牌还原
	virtual void ReSetPaiNum(BYTE num[],CheckHuStruct &TempPai);

	///检测连牌是否同一花色
	virtual bool CheckTongSe(BYTE pai1,BYTE pai2);

	///检测连牌是否连张
	virtual bool CheckLianPai(BYTE pai1,BYTE pai2,BYTE num=1);

	///描述：清除组牌数据
	virtual void CheckHuPai::ClearPingHuData();

	///检测平糊数据组合：将牌，吃，碰杠，顺子，暗刻等
	virtual void CheckPingHuData();

	///添加一种糊牌类型
	virtual void SetAHupaiType(BYTE type,BYTE hupai[]);

	///撤销一种糊牌类型
	virtual void ReSetAHupaiType(BYTE type,BYTE hupai[]);

	///描述：清除所有糊牌类型
	virtual void ClearHupaiType(BYTE hupai[]);

	///检测是否存在某种糊牌类型
	virtual bool CheckHupaiType(BYTE type,BYTE hupai[]);

	///初始化数据
	virtual void InitData();



/////////////////详细糊牌类型检测///////////////////////////////////////////////////////////////////////////////	
	//平糊
	virtual bool CheckIsPingHu();	
	//天糊
	virtual bool CheckTianHu();	
	//地糊
	virtual bool CheckDiHu();	
	//杠上花
	virtual bool CheckGangKai();	
	//杠后炮
	virtual bool CheckGangHouPao();	
	//一般高
	virtual bool CheckYiBanGao();	
	//喜相逢
	virtual bool CheckXiXiangFeng();	
	//连六
	virtual bool CheckLianLiu();	
	//老少副
	virtual bool CheckLaoShaoFu();	
	//幺九刻
	virtual bool CheckYaoJiuKe();	
	//明杠
	virtual bool CheckMingGang();	
	//缺一门
	virtual bool CheckQueYiMen();	
	//无字
	virtual bool CheckWuZi();	
	//边张
	virtual bool CheckBianZhang();	
	//坎张
	virtual bool CheckKanZhang();	
	//单钓将
	virtual bool CheckDanDiaoJiang();	
	//箭刻
	virtual bool CheckJianKe();	
	//圈风刻
	virtual bool CheckQuanFengKe();	
	//门风刻
	virtual bool CheckMenFengKe();	
	//门前清
	virtual bool CheckMenQianQing();	
	//四归一
	virtual bool CheckSiGuiYi();	
	//双同刻
	virtual bool CheckShuangTongKe();	
	//双暗刻
	virtual bool CheckShuangAnGang();	
	//暗杠
	virtual bool CheckAnGang();	
	//断幺
	virtual bool CheckDuanYao();	
	//全带幺
	virtual bool CheckQuanDaiYao();	
	//不求人
	virtual bool CheckBuQiuRen();	
	//双明杠
	virtual bool CheckShuangMingGang();	
	//和绝张
	virtual bool CheckHuJueZhang();	
	//碰碰和
	virtual bool CheckPengPengHu();	
	//一色牌检测（检测手牌和吃碰杠牌，哪里都适用）：0不是一色牌，1混一色，2清一色，
	virtual int CheckYiSe();
	//混一色
	virtual bool CheckHunYiSe();	
	//三色三步高
	virtual bool CheckSanSeSanBuGao();	
	//五门齐
	virtual bool CheckWuMenQi();	
	//全求人
	virtual bool CheckQuanQiuRen();	
	//双暗杠
	virtual bool CheckShuangAnKe();	
	//双箭刻
	virtual bool CheckShuangJianKe();	
	//花龙
	virtual bool CheckHuaLong();	
	//推不倒
	virtual bool CheckTuiBuDao();	
	//三色三同顺
	virtual bool CheckSanSeSanTongShun();	
	//三色三节高
	virtual bool CheckSanSeSanJieGao();	
	//无番和
	virtual bool CheckWuFanHe();	
	//妙手回春
	virtual bool CheckMiaoShouHuiChun();	
	//海底捞月
	virtual bool CheckHaiDiLaoYue();	
	//抢杠和
	virtual bool CheckQiangGangHe();	
	//大于五
	virtual bool CheckDaYuWu();	
	//小于五
	virtual bool CheckXiaoYuWu();	
	//三风刻
	virtual bool CheckSanFengKe();	
	//清龙
	virtual bool CheckQingLong();	
	//三色双龙会
	virtual bool CheckSanSeShuangLongHui();	
	//一色三步高
	virtual bool CheckYiSeSanBuGao();	
	//全带五
	virtual bool CheckQuanDaiWu();	
	//三同刻
	virtual bool CheckSanTongKe();	
	//三暗刻
	virtual bool CheckSanAnKe();	
	//全双刻
	virtual bool CheckQuanShuangKe();	
	//清一色
	virtual bool CheckQingYiSe();	
	//一色三同顺
	virtual bool CheckYiSeSanTongShun();	
	//一色三节高
	virtual bool CheckYiSeSanJieGao();	
	//全大
	virtual bool CheckQuanDa();	
	//全中
	virtual bool CheckQuanZhong();	
	//全小
	virtual bool CheckQuanXiao();	
	//一色四步高
	virtual bool CheckYiSeSiBuGao();	
	//三杠
	virtual bool CheckSanGang();	
	//混幺九
	virtual bool CheckHunYaoJiu();	
	//一色四同顺
	virtual bool CheckYiSeSiTongShun();	
	//一色四节高
	virtual bool CheckYiSeSiJieGao();	
	//清幺九
	virtual bool CheckQingYaoJiu();	
	//小四喜
	virtual bool CheckXiaoSiXi();	
	//小三元
	virtual bool CheckXiaoSanYuan();	
	//字一色
	virtual bool CheckZiYiSe();	
	//四暗刻
	virtual bool CheckSiAnKe();	
	//一色双龙会
	virtual bool CheckYiSeShuangLongHui();	
	//大四喜
	virtual bool CheckDaSiXi();	
	//大三元
	virtual bool CheckDaSanYuan();	
	//绿一色
	virtual bool CheckLvYiSe();	
	//四杠（十八罗汉）
	virtual bool CheckSiGang();	
	//九莲宝灯
	virtual bool CheckJiuLianBaoDeng();	

	/////////////////////////特殊牌型///////////////////////////////////////////////////////////////////////////////////////////////

	//全不靠（即十三烂）
	virtual bool CheckQuanBuKao();	
	//组合龙
	virtual bool CheckZuHeLong();	
	//七对
	virtual int CheckQiDui();	
	//七星不靠（七星十三烂）
	virtual bool CheckQiXingBuKao();	

	//连七对（七对的特殊）
	virtual bool CheckLianQiDui();	
	//十三幺
	virtual bool CheckShiSanYao();	



};