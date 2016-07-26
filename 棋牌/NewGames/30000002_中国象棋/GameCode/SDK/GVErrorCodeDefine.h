#if !defined(ANYCHAT_ERROR_CODE_DEFINE_)
#define ANYCHAT_ERROR_CODE_DEFINE_

#pragma once

#define GV_ERR_SUCCESS			0		///< 成功

#define GV_ERR_DB_ERROR			1		///< 数据库错误
#define GV_ERR_NOTINIT			2		///< 系统没有初始化
#define	GV_ERR_NOTINROOM		3		///< 还未进入房间
#define GV_ERR_MEMORYFAIL		4		///< 内存不足

#define GV_ERR_FUNCNOTALLOW		20		///< 函数功能不允许
#define GV_ERR_FUNCOPTERROR		21		///< 函数参数错误

//连接部分
#define GV_ERR_CONNECT_TIMEOUT	100		///< 连接服务器超时
#define GV_ERR_CONNECT_ABORT	101		///< 与服务器的连接中断
#define GV_ERR_CONNECT_AUTHFAIL	102		///< 未能通过服务器的认证，属于非法连接
#define GV_ERR_CONNECT_DNSERROR	103		///< 域名解析失败

//登录部分
#define GV_ERR_CERTIFY_FAIL		200		///< 认证失败，用户名或密码有误
#define GV_ERR_ALREADY_LOGIN	201		///< 该用户已登录
#define GV_ERR_ACCOUNT_LOCK		202		///< 帐户已被暂时锁定
#define GV_ERR_IPADDR_LOCK		203		///< IP地址已被暂时锁定
#define GV_ERR_VISITOR_DENY		204		///< 游客登录被禁止（登录时没有输入密码）
#define GV_ERR_INVALID_USERID	205		///< 无效的用户ID（用户不存在）

//进入房间
#define GV_ERR_ROOM_LOCK		300		///< 房间已被锁住，禁止进入
#define GV_ERR_ROOM_PASSERR		301		///< 房间密码错误，禁止进入
#define GV_ERR_ROOM_FULLUSER	302		///< 房间已满员，不能进入
#define GV_ERR_ROOM_INVALID		303		///< 房间不存在
#define GV_ERR_ROOM_EXPIRE		304		///< 房间服务时间已到期
#define GV_ERR_ROOM_REJECT		305		///< 房主拒绝进入
#define GV_ERR_ROOM_OWNERBEOUT	306		///< 房主不在，不能进入房间
#define GV_ERR_ROOM_ENTERFAIL	307		///< 不能进入房间
#define GV_ERR_ROOM_ALREADIN	308		///< 已经在房间里面了，本次进入房间请求忽略


//私聊
#define GV_ERR_ROOM_PRINULL		401		///< 用户已经离开房间
#define GV_ERR_ROOM_REJECTPRI	402		///< 用户拒绝了私聊邀请
#define GV_ERR_ROOM_PRIDENY		403		///< 不允许与该用户私聊，或是用户禁止私聊

#define GV_ERR_ROOM_PRIREQIDERR	420		///< 私聊请求ID号错误，或请求不存在
#define GV_ERR_ROOM_PRIALRCHAT	421		///< 已经在私聊列表中

#define GV_ERR_ROOM_PRITIMEOUT	431		///< 私聊请求超时
#define GV_ERR_ROOM_PRICHATBUSY	432		///< 对方正在私聊中，繁忙状态
#define GV_ERR_ROOM_PRIUSERCLOSE 433	///< 对方用户关闭私聊
#define GV_ERR_ROOM_PRISELFCLOSE 434	///< 用户自己关闭私聊

//Mic控制权
#define	GV_ERR_MICLOSE_TIMEOUT	500		///< 说话时间太长，请休息一下
#define GV_ERR_MICLOSE_HIGHUSER	501		///< 有高级别用户需要发言，请休息一下


//修改昵称
#define GV_ERR_NAMECHG_ONUSE	600		///< 该昵称已被使用，请换用其它的昵称

// 传输部分
#define GV_ERR_TRANSBUF_CREATEFAIL	700	///< 创建任务失败
#define GV_ERR_TRANSBUF_NOTASK		701	///< 没有该任务，或是任务已完成

#define GV_ERR_TRANSFILE_OPENFAIL	710	///< 打开文件出错
#define GV_ERR_TRANSFILE_ZEROLEN	711	///< 文件长度为0
#define GV_ERR_TRANSFILE_TOOLARGE	712	///< 文件长度太大
#define GV_ERR_TRANSFILE_READFAIL	713	///< 读文件出错

// 录像部分
#define GV_ERR_RECORD_NOTASK		720	///< 没有录像任务
#define GV_ERR_RECORD_CREATEFAIL	721	///< 创建录像任务失败


#endif