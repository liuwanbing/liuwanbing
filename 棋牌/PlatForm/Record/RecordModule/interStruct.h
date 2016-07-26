

// ***************************************************************
//  interStruct   version:  1.0   ·  date: 08/21/2008
//  -------------------------------------------------------------
//  系统内部用到的类
//  -------------------------------------------------------------
//  Copyright (C) 2008 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************
//为解析消息做准备，但如果要解析，就不能保证通用性。
#pragma  once
#define  MAX_MSG_HEAD_LEN   50
#define  MAX_MSG_DATA_LEN   2000   //最大一个消息数据
#define  MAX_PLAYER_COUNT   180   //最多有八个玩家

#define  FILE_TYPE_NONE        1
#define  FILE_TYPE_ZIP         2
#define  FILE_TYPE_PASS_ZIP    3
struct RecordNetMessageHead
{
	DWORD						uMessageSize;						//数据包大小
	DWORD						bMainID;							//处理主类型
	DWORD						bAssistantID;						//辅助处理类型 ID
	DWORD						bHandleCode;						//数据包处理代码
	DWORD						bReserve;							//保留字段
};

struct RecordNetMsgStruct
{
	BYTE NetMsgHead[MAX_MSG_HEAD_LEN]; //头消息   //最长50个字节
	BYTE NetMsgData[MAX_MSG_DATA_LEN];   //网络消息
	UINT uHeadSize;      //头的大小
	UINT uDataSize;      //网络消息大小	
	UINT uTimeSlot;      //与上一消息的时间间隔
};
struct FileHeadBase 
{
	RecordGameInfoStruct RoomGameInfo;
	RecordUserItemStruct UsrInfoStruct[MAX_PLAYER_COUNT]; //玩家信息
	BYTE bMyDeskStation;	
	UINT uPlayerCount;
	UINT uFileType;            //保存的文件类型  1明文 2压缩，3加密压缩	
	UINT uTotalMsgCount;       //总共消息的数量
};
struct FileHeadStruct : public FileHeadBase
{
	unsigned char arrcFileHeadMd5[16];
	unsigned char arrcFileMsgMd5[16];
};
