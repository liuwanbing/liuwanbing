// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 SQLENGINE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// SQLENGINE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef SQLENGINE_EXPORTS
#define SQLENGINE_API __declspec(dllexport)
#else
#define SQLENGINE_API __declspec(dllimport)
#endif

//注意：一个进程中，最多只能有15个连接

//0x0**		数据库定义
//0x1**		错误代码
//>0x100	句柄

#define DB_TYPE_SQLSERVER	0x0			//SQL Server 数据库
#define DB_TYPE_MYSQL		0x1			//My SQL 数据库
#define DB_TYPE_ORACLE		0x2			//Oracle 数据库

#define DB_ERR_NORESOURCE		0x400		//没有足够的资源
#define DB_ERR_CONNECT_FAILED	0x401		//连接数据库失败
#define DB_ERR_GETRETURNVALUE	0x402		//返回执行值失败

extern SQLENGINE_API int nSQLEngine;

SQLENGINE_API int fnSQLEngine(void);

/**************************
Function	:sqlAddConnection
Desc		:初始化数据库，直接提供参数方式
Author		:Fred Huang 2008-11-25
Para		:
	szServer	:数据库服务器
	szAccount	:数据库访问帐号
	szPassword	:数据库访问密码
	szDatabase	:数据库名
	nPort		:数据库端口
				在MS SQL Server中，当nPort=0时使用1433端口
				在MySQL中，当nPort时，使用3306端口
	nSQLClass		:使用的数据库
				缺省为使用MS SQL Server数据库
	szDetectTable	:数据库自动保持连接使用的表
				目前版本未实现
return		:返回一个数据库对象句柄
			< 0x1000：连接数据库失败
**************************/
extern SQLENGINE_API HANDLE	sqlAddConnection(LPCTSTR szServer,LPCTSTR szAccount,LPCTSTR szPassword,LPCTSTR szDatabase,unsigned short nPort=0,int nSQLClass=DB_TYPE_SQLSERVER,LPCTSTR szDetectTable=TEXT(""));

/**************************
Function	:sqlAddConnection
Desc		:初始化数据库，使用bcf文件
Author		:Fred Huang 2008-11-25
Para		:
	szBCFFile	:bcf文件名
				内容如下	:
					[DATABASECLASS]
					SQLCLASS=0

					[SQLSERVER]
					DBSERVER=127.0.0.1
					DBACCOUNT=sa
					DBPASSWORD=szbzw
					DBPORT=0
					DETECTTABLE=
					DATABASE=bzwoa

					[MYSQL]
					DBSERVER=localhost
					DBACCOUNT=root
					DBPASSWORD=szbzw
					DATABASE=bzw
					DBPORT=3306
					DETECTTABLE=
return		:返回一个数据库对象句柄
			< 0x1000：连接数据库失败
**************************/
extern SQLENGINE_API HANDLE sqlAddConnection(LPCTSTR szBCFFile);

/**************************
Function	:sqlExec
Desc		:执行普通的T-SQL 或 p/SQL
Author		:Fred Huang 2008-11-25
Para		:
	hHandle		:数据库对象句柄
	szSQL		:具体的 T-SQL 或 p/sql
	bReturnRecordsets	:是否需要返回记录集
						缺省为不需要返回记录集
						只有bReturnRecordsets=true时，才可以通过sqlGetValue取得数据库中的值
return		:返回一个记录对象句柄
				=0：执行失败
				=1：执行成功，不需要返回记录集时
				>1：执行成功，记录对象句柄
**************************/

extern SQLENGINE_API HANDLE sqlExec(HANDLE hHandle,LPCTSTR szSQL,bool bReturnRecordsets=false);

/**************************
Function	:sqlSPSetName
Desc		:执行存储过程前先要设置存储过程名称
Author		:Fred Huang 2008-11-25
Para		:
	hHandle		:数据库对象句柄
	szSPName	:存储过程的名称
	bReturnValue:是否有返回值
				因为存储过程是否有返回值，对操作影响很大，所以必须要提供，缺省为不需要
				注意：不是记录集
return		:返回一个存储过程记录对象句柄
			=0：执行失败
			>0：执行成功，记录对象句柄
**************************/
extern SQLENGINE_API HANDLE	sqlSPSetName(HANDLE hHandle,LPCTSTR szSPName,bool bReturnValue=false);

/**************************
Function	:sqlSPAddPara
Desc		:针对存储过程添加数字参数
Author		:Fred Huang 2008-11-25
Para		:
	hDatabase	:数据库对象句柄
	hRecord		:存储过程记录对象句柄
	szPara		:参数名称，对于SQL Server有效，MySQL只使用 lValue
	lValue		:参数值
return		:是否添加成功
Att			:需要注意的是，对于SQL Server，参数的添加可以没有顺序
			但对于MySQL，参数的添加必须严格的按存储过程的参数顺序添加
**************************/
extern SQLENGINE_API bool	sqlSPAddPara(HANDLE hDatabase,HANDLE hRecord,LPCTSTR szPara,long lValue);						//long 类型 

/**************************
Function	:sqlSPAddPara_i64
Desc		:针对存储过程添加数字参数
Author		:Jian Guankun 2011-11-29
Para		:
	hDatabase	:数据库对象句柄
	hRecord		:存储过程记录对象句柄
	szPara		:参数名称，对于SQL Server有效，MySQL只使用 lValue
	lValue		:参数值
return		:是否添加成功
Att			:需要注意的是，对于SQL Server，参数的添加可以没有顺序
			但对于MySQL，参数的添加必须严格的按存储过程的参数顺序添加
**************************/
extern SQLENGINE_API bool	sqlSPAddPara_i64(HANDLE hDatabase,HANDLE hRecord,LPCTSTR szPara,__int64 lValue);						//64位整数 类型 


/**************************
Function	:sqlSPAddPara
Desc		:针对存储过程添加字符参数
Author		:Fred Huang 2008-11-25
Para		:
	hDatabase	:数据库对象句柄
	hRecord		:存储过程记录对象句柄
	szPara		:参数名称，对于SQL Server有效，MySQL只使用 szValue
	szValue		:参数值
	szSize		:参数的大小，对于SQL Server有效，MySQL只使用 szValue
return		:是否添加成功
Att			:需要注意的是，对于SQL Server，参数的添加可以没有顺序
			但对于MySQL，参数的添加必须严格的按存储过程的参数顺序添加
**************************/
extern SQLENGINE_API bool	sqlSPAddPara(HANDLE h1,HANDLE h2,LPCTSTR szPara,LPCTSTR szValue,int szSize);		//字符串类型
//执行SP，先决条件，设置了名称和参数
//bReturnRecordsets ：是否有返回记录集

/**************************
Function	:sqlSPExec
Desc		:执行存储过程
Author		:Fred Huang 2008-11-25
Para		:
	hDatabase	:数据库对象句柄
	hRecord		:存储过程记录集对象句柄
	bReturnRecordsets	:是否需要返回记录集
						注意，是否有返回记录集具体操作差异较大，所以需要明确
return		:是否执行成功
**************************/
extern SQLENGINE_API bool	sqlSPExec(HANDLE hDatabase,HANDLE hRecord,bool bReturnRecordsets=false);

/**************************
Function	:sqlSPClose
Desc		:关闭存储过程记录对象
Author		:Fred Huang 2008-11-25
Para		:
	hDatabase	:数据库对象句柄
	hRecord		:存储过程记录集对象句柄
**************************/
extern SQLENGINE_API void	sqlSPClose(HANDLE hDatabase,HANDLE hRecord);
//关闭某个Command

/**************************
Function	:sqlCloseRecord
Desc		:关闭某个记录集对象
Author		:Fred Huang 2008-11-25
Para		:
	hDatabase	:数据库对象句柄
	hRecord		:记录集对象句柄
**************************/
extern SQLENGINE_API void	sqlCloseRecord(HANDLE hDatabase,HANDLE hRecord);

/**************************
Function	:sqlCloseRecord
Desc		:关闭数据库对象中所有的记录集对象，包括普通记录集和存储过程对象
Author		:Fred Huang 2008-11-25
Para		:
	hDatabase	:数据库对象句柄
att			:不关闭数据库连接
**************************/
extern SQLENGINE_API void	sqlCloseAllRecord(HANDLE hHandle);

/**************************
Function	:sqlCloseConnection
Desc		:关闭数据库连接，并会关闭数据库中所有的记录集对象，包括普通记录集和存储过程对象
Author		:Fred Huang 2008-11-25
Para		:
	hDatabase	:数据库对象句柄
**************************/
extern SQLENGINE_API void	sqlCloseConnection(HANDLE hHandle);

/**************************
Function	:sqlRelease
Desc		:关闭所有数据库连接，并会关闭数据库中所有的记录集对象，包括普通记录集和存储过程对象
Author		:Fred Huang 2008-11-25
**************************/
extern SQLENGINE_API void	sqlRelease();
//取得SP的执行返回值，人有在bReturcValue=true时有效

/**************************
Function	:sqlSPGetReturnValue
Desc		:取得存储过程中的返回值，只有在sqlSPSetName中 bReturnValue=true,且数据库存储过程中确有返回值时才可以
Author		:Fred Huang 2008-11-25
Para		:
	hDatabase	:数据库对象句柄
	hRecord		:记录集对象句柄
**************************/
extern SQLENGINE_API long	sqlSPGetReturnValue(HANDLE hDatabase,HANDLE hRecord);

/**************************
Function	:sqlEndOfFile
Desc		:记录集是否已到最后一条
			普通记录集和存储过程记录集使用同一函数
Author		:Fred Huang 2008-11-25
Para		:
	hDatabase	:数据库对象句柄
	hRecord		:记录集对象句柄
**************************/
extern SQLENGINE_API bool	sqlEndOfFile(HANDLE hDatabase,HANDLE hRecord);

/**************************
Function	:sqlMoveNext
Desc		:移动到下一条记录
			普通记录集和存储过程记录集使用同一函数
Author		:Fred Huang 2008-11-25
Para		:
	hDatabase	:数据库对象句柄
	hRecord		:记录集对象句柄
**************************/
extern SQLENGINE_API void	sqlMoveNext(HANDLE hDatabase,HANDLE hRecord);

extern SQLENGINE_API long	sqlGetRecordCount(HANDLE hDatabase,HANDLE hRecord);
/**************************
Function	:sqlGetValue
Desc		:取记录集中的数据
Author		:Fred Huang 2008-11-25
Para		:
	hDatabase	:数据库对象句柄
	hRecord		:记录集对象句柄
	szFieldName	:字段名
	value		:取值变量
att			:以下函数均为同一功能
**************************/
extern SQLENGINE_API bool	sqlGetValue(HANDLE h1,HANDLE h2,LPCTSTR szFieldName,int &nValue);
extern SQLENGINE_API bool	sqlGetValue(HANDLE h1,HANDLE h2,LPCTSTR szFieldName,UINT &lValue);
extern SQLENGINE_API bool	sqlGetValue(HANDLE h1,HANDLE h2,LPCTSTR szFieldName,LONG &lValue);
extern SQLENGINE_API bool	sqlGetValue(HANDLE h1,HANDLE h2,LPCTSTR szFieldName,DOUBLE &dValue);
extern SQLENGINE_API bool	sqlGetValue(HANDLE h1,HANDLE h2,LPCTSTR szFieldName,bool &bValue);
extern SQLENGINE_API bool	sqlGetValue(HANDLE h1,HANDLE h2,LPCTSTR szFieldName,BYTE &dValue);
extern SQLENGINE_API bool	sqlGetValue(HANDLE h1,HANDLE h2,LPCTSTR szFieldName,DWORD &dValue);
extern SQLENGINE_API bool	sqlGetValue(HANDLE h1,HANDLE h2,LPCTSTR szFieldName,__int64 &llValue);
extern SQLENGINE_API bool	sqlGetValue(HANDLE h1,HANDLE h2,LPCTSTR szFieldName,CString &szValue);
extern SQLENGINE_API bool	sqlGetValue(HANDLE h1,HANDLE h2,LPCTSTR szFieldName,COleDateTime &tTime);
extern SQLENGINE_API bool	sqlGetValue(HANDLE h1,HANDLE h2,LPCTSTR szFieldName,TCHAR szBuffer[], UINT uSize);

/**************************
Function	:sqlSetCharacter
Desc		:设置数据库字符集，只对MySQL 有效，不使用的话，默认为 gb2312
Author		:Fred Huang 2008-11-25
Para		:
	hHandle		:数据库对象句柄
	character	:字符集名称
**************************/
extern SQLENGINE_API void	sqlSetCharacter(HANDLE hHandle,char *character);