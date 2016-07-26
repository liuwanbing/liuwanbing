
//声音类型设置
#define SOUND_OUT_CARD				0x10				//出牌声音
#define SOUND_WARNING				0x20				//警告
#define SOUND_SEND_CARD				0x30				//发牌声音
#define SOUND_OUT_CARD_DEF			0x40				//默认出牌声音
#define SOUND_OUT_CARD_BOMB			0x50				//出炸彈
#define SOUND_OUT_CARD_PASS			0x60				//不出
#define SOUND_INVALIDATE			0x70				//提示失败
#define SOUND_START_DEF				0x80				//四个人开始游戏后
#define SOUND_GAME_OVER				0x90				//游戏结束
#define SOUND_ADD_DOUBLE			0xA0				//加棒
#define SOUND_ROB_NT               0xB0                //抢地主

#define SOUND_WIN					0x01				//胜利
#define SOUND_LOSE					0x02				//输
#define SOUND_WAITING				0x03				//等待
#define SOUND_ERROR					0x04				//出牌出错了
#define SOUND_READY					0x05				//游戏开始
#define SOUND_START					0x06				//准备出第一手牌
#define SOUND_START_OUT_CARD		0x07				//出第一手牌
#define SOUND_CALL_SCORE			0x08				//叫分
#define SOUND_OUT_CARD_FINISH		0x09				//某玩家出牌結束
#define SOUND_GAME_PASS				0x0A				//不出
#define SOUND_FRIEND_APPEAR			0x0B				//盟友出現
#define SOUND_USER_RUNAWARY			0x0C				//有人逃跑
#define SOUND_USER_LEFT				0x0D				//有人离开
#define SOUND_GIVE_BACK				0x0E				//埋底

//按注类型(梭哈备用)
#define TYPE_GIVE_UP					0x00			//放弃
#define TYPE_NOTE						0x01			//下注
#define TYPE_ADD						0x02			//加注
#define TYPE_FOLLOW						0x03			//跟注
#define TYPE_SHUTTLE					0x04			//梭哈

//设置下注声音(专为梭哈准备)
BOOL SetNoteSound(BYTE bNoteStyle,BOOL bIsBoy);
//声音通用设置
BOOL SetPlaySound(BYTE bSoundType);
//出牌牌型声音設置
HWND SetOutCardTypeSound(BYTE iCardList[],int iCardCount,HWND hParent,BOOL IsBoy = true,BOOL IsFirest = TRUE);
//播放mp3声音
HWND PlayCardMusic(TCHAR FileName[],HWND hParent);
//播入场景声音
HWND SetSceneSound(BYTE iSceneSound,HWND hParent,BOOL IsBoy = TRUE,int iVal = -1);
