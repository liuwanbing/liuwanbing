#if !defined(BR_ANYCHAT_CORE_SDK_H__INCLUDED_)
#define BR_ANYCHAT_CORE_SDK_H__INCLUDED_

#include <mmsystem.h>
#include <wingdi.h>


/**
 *	AnyChat Core SDK Include File
 */


#pragma once

#include "GVSDK.h"

#define BRAC_API extern "C" __declspec(dllexport)

// 视频图像格式定义
enum BRAC_PixelFormat{
	BRAC_PIX_FMT_RGB24 = 0,						///< Packed RGB 8:8:8, 24bpp, RGBRGB...（MEDIASUBTYPE_RGB24）
	BRAC_PIX_FMT_RGB32,							///< 对应于：MEDIASUBTYPE_RGB32，Packed RGB 8:8:8, 32bpp, (msb)8A 8R 8G 8B(lsb), in cpu endianness
	BRAC_PIX_FMT_YV12,							///< 对应于：MEDIASUBTYPE_YV12，Planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
	BRAC_PIX_FMT_YUY2,							///< 对应于：MEDIASUBTYPE_YUY2，Packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
};

// 音频设备定义
enum BRAC_AudioDevice{
	BRAC_AD_WAVEIN = 0,							///< 输入设备：Mic
	BRAC_AD_WAVEOUT,							///< 输出设备：Wave
};

// 功能模式定义
#define BRAC_FUNC_VIDEO_CBDATA		0x00000001L	///< 通过回调函数输出视频数据
#define BRAC_FUNC_VIDEO_AUTODISP	0x00000002L	///< 由SDK包处理视频，将视频显示在指定的窗口上
#define BRAC_FUNC_AUDIO_CBDATA		0x00000004L	///< 通过回调函数输出音频数据
#define BRAC_FUNC_AUDIO_AUTOPLAY	0x00000008L	///< 由SDK包处理音频，直接播放
#define BRAC_FUNC_CONFIG_LOCALINI	0x00000010L	///< 生成本地配置文件（AnyChatSDK.ini）
#define BRAC_FUNC_FIREWALL_OPEN		0x00000020L	///< 允许SDK操作Windows防火墙，将当前应用程序加入防火墙访问列表（避免Windows提示用户是否阻止当前应用程序）
#define BRAC_FUNC_CHKDEPENDMODULE	0x00000040L	///< 自动检查SDK所依赖的组件，并自动注册
#define BRAC_FUNC_AUDIO_VOLUMECALC	0x00000080L	///< 由SDK自动计算语音的音量
#define BRAC_FUNC_AUDIO_AUTOVOLUME	0x00000100L	///< 允许SDK自动控制Mic录音音量
#define BRAC_FUNC_NET_SUPPORTUPNP	0x00000200L	///< 允许SDK打开用户网络中的UPNP设备，如果用户的路由器或是防火墙支持UPNP协议，则可提高P2P打洞的成功率


// 内核参数定义
#define BRAC_SO_AUDIO_VADCTRL				1	///< 音频静音检测控制（参数为：int型：1打开，0关闭）
#define BRAC_SO_AUDIO_NSCTRL				2	///< 音频噪音抑制控制（参数为：int型：1打开，0关闭）
#define BRAC_SO_AUDIO_ECHOCTRL				3	///< 音频回音消除控制（参数为：int型：1打开，0关闭）
#define BRAC_SO_AUDIO_AGCCTRL				4	///< 音频自动增益控制（参数为：int型：1打开，0关闭）

#define BRAC_SO_RECORD_VIDEOBR				10	///< 录像视频码率设置（参数为：int型，单位：bps）
#define BRAC_SO_RECORD_AUDIOBR				11	///< 录像音频码率设置（参数为：int型，单位：bps）
#define BRAC_SO_RECORD_TMPDIR				12	///< 录像文件临时目录设置（参数为字符串PCHAR类型）
#define BRAC_SO_SNAPSHOT_TMPDIR				13	///< 快照文件临时目录设置（参数为字符串PCHAR类型）

#define BRAC_SO_CORESDK_PATH				20	///< 设置AnyChat Core SDK相关组件路径（参数为字符串PCHAR类型）

// 传输任务信息参数定义
#define BRAC_TRANSTASK_PROGRESS				1	///< 传输任务进度查询（参数为：DOUBLE型（0.0 ~ 100.0））
#define BRAC_TRANSTASK_BITRATE				2	///< 传输任务当前传输码率（参数为：int型，单位：bps）
#define BRAC_TRANSTASK_STATUS				3	///< 传输任务当前状态（参数为：int型）


// 录像功能标志定义
#define BRAC_RECORD_FLAGS_VIDEO		0x00000001L	///< 录制视频
#define BRAC_RECORD_FLAGS_AUDIO		0x00000002L	///< 录制音频


// 用户状态标志定义
#define BRAC_USERSTATE_CAMERA				1	///< 用户摄像头状态（参数为int型）
#define BRAC_USERSTATE_HOLDMIC				2	///< 用户持有Mic状态（参数为int型）
#define BRAC_USERSTATE_SPEAKVOLUME			3	///< 用户当前说话音量（参数为DOUBLE类型（0.0 ~ 100.0））
#define BRAC_USERSTATE_RECORDING			4	///< 用户录像（音）状态（参数为int型）
#define	BRAC_USERSTATE_LEVEL				5	///< 用户级别（参数为int型）
#define BRAC_USERSTATE_NICKNAME				6	///< 用户昵称（参数为字符串PCHAR类型）
#define BRAC_USERSTATE_LOCALIP				7	///< 用户本地IP地址（内网，参数为字符串PCHAR类型）
#define BRAC_USERSTATE_INTERNETIP			8	///< 用户互联网IP地址（参数为字符串PCHAR类型）


// 视频数据回调函数定义
typedef void (CALLBACK * BRAC_VideoData_CallBack)(DWORD dwUserid, LPVOID lpBuf, DWORD dwLen, BITMAPINFOHEADER bmiHeader, LPVOID lpUserValue);
// 音频数据回调函数定义
typedef void (CALLBACK * BRAC_AudioData_CallBack)(DWORD dwUserid, LPVOID lpBuf, DWORD dwLen, WAVEFORMATEX waveFormatEx, LPVOID lpUserValue);
// 文字消息回调函数定义
typedef void (CALLBACK * BRAC_TextMessage_CallBack)(DWORD dwFromUserid, DWORD dwToUserid, BOOL bSecret, LPCTSTR lpMsgBuf, DWORD dwLen, LPVOID lpUserValue);
// 透明通道数据回调函数定义
typedef void (CALLBACK * BRAC_TransBuffer_CallBack)(DWORD dwUserid, LPBYTE lpBuf, DWORD dwLen, LPVOID lpUserValue);
// 透明通道数据扩展回调函数定义
typedef void (CALLBACK * BRAC_TransBufferEx_CallBack)(DWORD dwUserid, LPBYTE lpBuf, DWORD dwLen, DWORD wParam, DWORD lParam, DWORD dwTaskId, LPVOID lpUserValue);
// 文件传输回调函数定义
typedef void (CALLBACK * BRAC_TransFile_CallBack)(DWORD dwUserid, LPCTSTR lpFileName, LPCTSTR lpTempFilePath, DWORD dwFileLength, DWORD wParam, DWORD lParam, DWORD dwTaskId, LPVOID lpUserValue);
// 音量变化回调函数定义
typedef void (CALLBACK * BRAC_VolumeChange_CallBack)(BRAC_AudioDevice device, DWORD dwCurrentVolume, LPVOID lpUserValue);
// SDK Filter 通信数据回调函数定义
typedef void (CALLBACK * BRAC_SDKFilterData_CallBack)(LPBYTE lpBuf, DWORD dwLen, LPVOID lpUserValue);
// 录像、快照任务完成回调函数定义
typedef void (CALLBACK * BRAC_RecordSnapShot_CallBack)(DWORD dwUserid, LPCTSTR lpFileName, DWORD dwParam, BOOL bRecordType, LPVOID lpUserValue);
// 异步消息通知回调函数定义
typedef void (CALLBACK* BRAC_NotifyMessage_CallBack)(DWORD dwNotifyMsg, DWORD wParam, DWORD lParam, LPVOID lpUserValue);


/**
 *	API方法定义
 */

// 获取SDK版本信息
BRAC_API DWORD BRAC_GetSDKVersion(DWORD& dwMainVer, DWORD& dwSubVer, LPSTR lpCompileTime, DWORD dwBufLen);
// 激活（关闭）SDK调用日志
BRAC_API DWORD BRAC_ActiveCallLog(BOOL bActive);

// 设置服务器验证密码（可用于阻止非法用户用SDK连接服务器，合法用户可正常连接）
BRAC_API DWORD BRAC_SetServerAuthPass(LPCTSTR lpPassword);
// 初始化系统
BRAC_API DWORD BRAC_InitSDK(HWND hWnd, DWORD dwFuncMode);

// 设置视频数据回调函数
BRAC_API DWORD BRAC_SetVideoDataCallBack(BRAC_PixelFormat pixFmt, BRAC_VideoData_CallBack lpFunction, LPVOID lpUserValue=NULL);
// 设置音频数据回调函数
BRAC_API DWORD BRAC_SetAudioDataCallBack(BRAC_AudioData_CallBack lpFunction, LPVOID lpUserValue=NULL);
// 设置文字消息回调函数
BRAC_API DWORD BRAC_SetTextMessageCallBack(BRAC_TextMessage_CallBack lpFunction, LPVOID lpUserValue=NULL);
// 设置透明通道数据回调函数
BRAC_API DWORD BRAC_SetTransBufferCallBack(BRAC_TransBuffer_CallBack lpFunction, LPVOID lpUserValue=NULL);
// 设置透明通道数据扩展回调函数
BRAC_API DWORD BRAC_SetTransBufferExCallBack(BRAC_TransBufferEx_CallBack lpFunction, LPVOID lpUserValue=NULL);
// 设置文件传输回调函数
BRAC_API DWORD BRAC_SetTransFileCallBack(BRAC_TransFile_CallBack lpFunction, LPVOID lpUserValue=NULL);
// 设置音量变化回调函数
BRAC_API DWORD BRAC_SetVolumeChangeCallBack(BRAC_VolumeChange_CallBack lpFunction, LPVOID lpUserValue=NULL);
// 设置SDK Filter通信数据回调函数定义
BRAC_API DWORD BRAC_SetSDKFilterDataCallBack(BRAC_SDKFilterData_CallBack lpFunction, LPVOID lpUserValue=NULL);
// 设置录像快照任务完成通知回调函数
BRAC_API DWORD BRAC_SetRecordSnapShotCallBack(BRAC_RecordSnapShot_CallBack lpFunction, LPVOID lpUserValue=NULL);
// 设置异步消息通知回调函数
BRAC_API DWORD BRAC_SetNotifyMessageCallBack(BRAC_NotifyMessage_CallBack lpFunction, LPVOID lpUserValue=NULL);


// 连接服务器
BRAC_API DWORD BRAC_Connect(LPCTSTR lpServerAddr, DWORD dwPort);
// 登录系统
BRAC_API DWORD BRAC_Login(LPCTSTR lpUserName, LPCTSTR lpPassword, DWORD dwPassEncType); 
// 进入房间
BRAC_API DWORD BRAC_EnterRoom(DWORD dwRoomid, LPCTSTR lpRoomPass, DWORD dwPassEncType);
// 进入房间
BRAC_API DWORD BRAC_EnterRoomEx(LPCTSTR lpRoomName, LPCTSTR lpRoomPass);
// 离开房间
BRAC_API DWORD BRAC_LeaveRoom(DWORD dwRoomid);
// 注销系统
BRAC_API DWORD BRAC_Logout(VOID);

// 释放所有资源
BRAC_API DWORD BRAC_Release(VOID);

// 获取当前房间在线用户列表
BRAC_API DWORD BRAC_GetOnlineUser(LPDWORD lpUserIDArray, DWORD& dwUserNum);
// 查询用户摄像头的状态
BRAC_API DWORD BRAC_GetCameraState(DWORD dwUserid, DWORD& dwState);
// 查询用户发言状态
BRAC_API DWORD BRAC_GetSpeakState(DWORD dwUserid, DWORD& dwState);
// 查询用户级别
BRAC_API DWORD BRAC_GetUserLevel(DWORD dwUserid, DWORD& dwLevel);
// 查询用户名称
BRAC_API DWORD BRAC_GetUserName(DWORD dwUserid, CHAR* lpUserName, DWORD dwLen);
// 查询房间名称
BRAC_API DWORD BRAC_GetRoomName(DWORD dwRoomId, CHAR* lpRoomName, DWORD dwLen);
// 显示本地视频画面调节对话框
BRAC_API DWORD BRAC_ShowLVProperty(HWND hParent, LPCTSTR szCaption=NULL, DWORD dwX=0, DWORD dwY=0);

// 查询指定用户相关状态
BRAC_API DWORD BRAC_QueryUserState(DWORD dwUserId, int infoname, char FAR* infoval, int infolen);

// 枚举本地视频采集设备
BRAC_API DWORD BRAC_EnumVideoCapture(CHAR** lpDeviceName, DWORD& dwDeviceNum);
// 选择指定的视频采集设备
BRAC_API DWORD BRAC_SelectVideoCapture(LPCTSTR szCaptureName);
// 获取当前使用的视频采集设备
BRAC_API DWORD BRAC_GetCurVideoCapture(CHAR* lpDeviceName, DWORD dwLen);
// 枚举本地音频采集设备
BRAC_API DWORD BRAC_EnumAudioCapture(CHAR** lpDeviceName, DWORD& dwDeviceNum);
// 选择指定的音频采集设备
BRAC_API DWORD BRAC_SelectAudioCapture(LPCTSTR szCaptureName);
// 获取当前使用的音频采集设备
BRAC_API DWORD BRAC_GetCurAudioCapture(CHAR* lpDeviceName, DWORD dwLen);


// 操作用户视频
BRAC_API DWORD BRAC_UserCameraControl(DWORD dwUserid, BOOL bOpen);
// 操作用户语音
BRAC_API DWORD BRAC_UserSpeakControl(DWORD dwUserid, BOOL bOpen);
// 设置视频显示位置
BRAC_API DWORD BRAC_SetVideoPos(DWORD dwUserid, HWND hWnd, DWORD dwLeft, DWORD dwTop, DWORD dwRight, DWORD dwBottom);

// 获取指定音频设备的当前音量
BRAC_API DWORD BRAC_AudioGetVolume(BRAC_AudioDevice device, DWORD& dwVolume);
// 设置指定音频设备的音量
BRAC_API DWORD BRAC_AudioSetVolume(BRAC_AudioDevice device, DWORD dwVolume);

// 用户音、视频录制
BRAC_API DWORD BRAC_StreamRecordCtrl(DWORD dwUserId, BOOL bStartRecord, DWORD dwFlags, DWORD dwParam);
// 对用户的视频进行抓拍（快照）
BRAC_API DWORD BRAC_SnapShot(DWORD dwUserId, DWORD dwFlags, DWORD dwParam);

// 透明通道传送缓冲区
BRAC_API DWORD BRAC_TransBuffer(DWORD dwUserid, LPBYTE lpBuf, DWORD dwLen);
// 透明通道传送缓冲区扩展
BRAC_API DWORD BRAC_TransBufferEx(DWORD dwUserid, LPBYTE lpBuf, DWORD dwLen, DWORD wParam, DWORD lParam, DWORD dwFlags, DWORD& dwTaskId);
// 传送文件
BRAC_API DWORD BRAC_TransFile(DWORD dwUserid, LPCTSTR lpLocalPathName, DWORD wParam, DWORD lParam, DWORD dwFlags, DWORD& dwTaskId);
// 查询传输任务相关信息
BRAC_API DWORD BRAC_QueryTransTaskInfo(DWORD dwUserid, DWORD dwTaskId, int infoname, char FAR* infoval, int infolen);
// 取消传输任务
BRAC_API DWORD BRAC_CancelTransTask(DWORD dwUserid, DWORD dwTaskId);
// 传送文本消息
BRAC_API DWORD BRAC_SendTextMessage(DWORD dwUserid, BOOL bSecret, LPCTSTR lpMsgBuf, DWORD dwLen);
// 发送SDK Filter 通信数据
BRAC_API DWORD BRAC_SendSDKFilterData(LPBYTE lpBuf, DWORD dwLen);

// 更改当前的聊天模式
BRAC_API DWORD BRAC_ChangeChatMode(DWORD dwChatMode);
// 获取指定用户当前的聊天模式
BRAC_API DWORD BRAC_GetUserChatMode(DWORD dwUserid, DWORD& dwChatMode);
// 请求与对方私聊，向对方发起私聊请求
BRAC_API DWORD BRAC_PrivateChatRequest(DWORD dwUserid);
// 回复对方的私聊请求
BRAC_API DWORD BRAC_PrivateChatEcho(DWORD dwUserid, DWORD dwRequestid,BOOL bAccept);
// 回复对方的私聊请求（扩展，可以附带出错代码）
BRAC_API DWORD BRAC_PrivateChatEchoEx(DWORD dwUserid, DWORD dwRequestid,DWORD dwErrorCode);
// 退出与某用户的私聊，或者将某用户从自己的私聊列表中清除
BRAC_API DWORD BRAC_PrivateChatExit(DWORD dwUserid);

// SDK内核参数设置
BRAC_API DWORD BRAC_SetSDKOption(int optname, const char FAR* optval, int optlen);
// SDK内核参数状态查询
BRAC_API DWORD BRAC_GetSDKOption(int optname, char FAR* optval, int optlen);



#endif //BR_ANYCHAT_CORE_SDK_H__INCLUDED_
