
#if !defined(_GVSERVER_FILTER_SDK__INCLUDED_)
#define _GVSERVER_FILTER_SDK__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 *	获取SDK插件版本信息
 *	@param lpVersion 返回插件的版本信息
 *	@param dwVSLen 版本信息缓冲区长度
 */
#define BRGS_FUNCNAME_GETVERSION	"BRGS_GetVersion"
typedef void (_cdecl * BRGS_GetVersion_PROC)(OUT LPTSTR lpVersion, 
											 IN DWORD dwVSLen);
/**
 *	初始化SDK Filter
 *	可以在该方法中读取相关的配置参数，建立数据库连接等初始化工作
 *	@return 初始化成功返回TRUE，否则返回FALSE
 */
#define BRGS_FUNCNAME_INITFILTER	"BRGS_InitFilter"
typedef BOOL (_cdecl * BRGS_InitFilter_PROC)(void);

/**
 *	验证用户身份
 *	@param lpUserName 输入用户名
 *	@param lpPassword 输入用户密码
 *	@param lpUserID 输出该用户的ID号（注意：该ID号必须唯一，不能重复）
 *	@param lpUserLevel 输出该用户的级别
 *	@param lpNickName 输出该用户的昵称
 *	@param dwNCLen 保存用户昵称缓冲区的长度
 *	@return 返回出错码，成功返回 GV_ERR_SUCCESS，否则返回对应的错误代码
 */
#define BRGS_FUNCNAME_VERIFYUSER	"BRGS_VerifyUser"
typedef DWORD (_cdecl * BRGS_VerifyUser_PROC)(IN LPCTSTR lpUserName, 
											  IN LPCTSTR lpPassword, 
											  OUT LPDWORD lpUserID, 
											  OUT LPDWORD lpUserLevel, 
											  OUT LPTSTR lpNickName,
											  IN DWORD dwNCLen);


/**
 *	释放SDK Filter资源
 *	在服务器退出时调用
 */
#define BRGS_FUNCNAME_RELEASE		"BRGS_ReleaseFilter"
typedef BOOL (_cdecl * BRGS_ReleaseFilter_PROC)(void);

#endif //_GVSERVER_FILTER_SDK__INCLUDED_