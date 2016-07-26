#pragma once

#include "GameEngineHead.h"
#include "DirectShowBasic.h"


// 单个播放
typedef map< UINT_PTR ,CDXGraph* >				mapDXGraph;
class CMapDXGraph
{
protected:
	mapDXGraph									m_mapDXGraph;

public:
	CMapDXGraph(){}
	~CMapDXGraph(){}

public:
	mapDXGraph*	GetObject() { return &m_mapDXGraph; }
};

typedef map< UINT_PTR ,CGraphBuilderCB* >		mapGraphBuilderCB;
class CMapGraphBuilderCB
{
protected:
	mapGraphBuilderCB							m_mapGraphBuilderCB;

public:
	CMapGraphBuilderCB(){}
	~CMapGraphBuilderCB(){}

public:
	mapGraphBuilderCB*	GetObject() { return &m_mapGraphBuilderCB; }
};

typedef map< UINT_PTR ,CString >				mapKeyName;
class CMapKeyName
{
protected:
	mapKeyName									m_mapKeyName;

public:
	CMapKeyName(){}
	~CMapKeyName(){}

public:
	mapKeyName*	GetObject() { return &m_mapKeyName; }
};

// 播放列表
typedef vector<CDXGraph*>						verGraph;
class CVerGraph
{
protected:
	verGraph									m_verGraph;

public:
	CVerGraph(){}
	~CVerGraph(){}

public:
	verGraph*	GetObject() { return &m_verGraph; }
};

typedef vector<CGraphBuilderCB*>				verGraphBuilderCB;
class CVerGraphBuilderCB
{
protected:
	verGraphBuilderCB							m_verGraphBuilderCB;

public:
	CVerGraphBuilderCB(){}
	~CVerGraphBuilderCB(){}

public:
	verGraphBuilderCB*	GetObject() { return &m_verGraphBuilderCB; }
};

//播放类
class GAME_ENGINE_CLASS CD3DMusic
{
	//全局
private:
	static CD3DMusic*		pObject;						//对象指针

	// 变量定义
private:
	bool						m_bBackTime;					//背景播放
	
	//------------------------------------------
	// 单个播放
private:
	CMapDXGraph*				m_mapDXGraph;
	CMapGraphBuilderCB*			m_mapGraphBuilderCB;
	CMapKeyName*				m_mapKeyName;


	//----------------------------------------------------------
	//播放列表
private:
	CVerGraph*					m_verBackGraph;
	CVerGraphBuilderCB*			m_verBackBuilder;
	bool						m_bPause;
	UINT_PTR					m_idEvent;
	UINT_PTR					m_iBackID;
	UINT_PTR					m_iBackMaxID;

private:
	CD3DMusic(void);
	~CD3DMusic(void);

	// 对象函数
public:
	// 返回对象
	inline static CD3DMusic* _Object()
	{
		if ( pObject == NULL )
		{
			pObject = new CD3DMusic;
		}
		return pObject;
	}
	//删除对象
	static void Delete()
	{
		if ( pObject != NULL )
		{
			delete pObject;
			pObject = NULL;
		}
	}

	// 内部函数
private:
	// 获取路径
	CString			GetAppPath();
	// 注册文件
	BOOL			RegisterFilter(LPCTSTR  inFilterAx);
	// 删除声音
	void			mapErase(UINT idEvent);
	// 删除声音
	void			mapErase();
	// 单个播放回调
	static void		CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime );
	// 背景播放回调
	static void		CALLBACK ListTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime );

	// 播放函数
public:
	//------------------------------------------
	// 单个播放

	//************************************
	// Method:    播放声音
	// FullName:  CD3DMusic::Start
	// Access:    public 
	// Returns:   bool
	// Parameter: LPCTSTR  filename	文件名 (创建声音以及播放)
	//************************************
	bool Start(LPCTSTR filename);

	//************************************
	// Method:    暂停声音
	// FullName:  CD3DMusic::Pause
	// Access:    public 
	// Returns:   bool
	// Parameter: LPCTSTR  filename	文件名 (如果为NULL 则全部暂停)
	//************************************
	bool Pause(LPCTSTR filename  = NULL);

	//************************************
	// Method:    播放声音(需要先调用Start载入以后才能使用 用于暂停或停止后启动)
	// FullName:  CD3DMusic::Run
	// Access:    public 
	// Returns:   bool
	// Parameter: LPCTSTR  filename	文件名 (如果为NULL 则全部停止或暂停的声音播放)
	//************************************
	bool Run(LPCTSTR filename  = NULL);

	//************************************
	// Method:    停止声音(停止以及把此声音彻底删除,如要再播放则需要重新Start载入播放)
	// FullName:  CD3DMusic::Stop
	// Access:    public 
	// Returns:   bool
	// Parameter: LPCTSTR  filename 文件名 (如果为NULL 则全部停止)
	//************************************
	bool Stop(LPCTSTR filename  = NULL);

	//************************************
	// Method:    控制音量大小
	// FullName:  CD3DMusic::ControlVolume
	// Access:    public 
	// Returns:   void
	// Parameter: int ibulk			音量大小(-10000 ~ 0)0最大
	//************************************
	void ControlVolume(int ibulk);



	//----------------------------------------------------------
	//播放列表

	//************************************
	// Method:    添加曲目
	// FullName:  CD3DMusic::ListAdd
	// Access:    public 
	// Returns:   void
	// Parameter: LPCTSTR  filename  文件名
	//************************************
	void ListAdd(LPCTSTR filename);


	//************************************
	// Method:    开始背景声音
	// FullName:  CD3DMusic::Back_Start
	// Access:    public 
	// Returns:   void
	//************************************
	void ListStart();


	//************************************
	// Method:    清除所有背景声音
	// FullName:  CD3DMusic::ListClear
	// Access:    public 
	// Returns:   void
	//************************************
	void ListClear();

	//************************************
	// Method:    播放背景声音(在被停止或暂停的时候调用播放)
	// FullName:  CD3DMusic::ListRun
	// Access:    public 
	// Returns:   void
	//************************************
	void ListRun();


	//************************************
	// Method:    暂停背景声音(只能被Back_Run()启动播放)
	// FullName:  CD3DMusic::ListPause
	// Access:    public 
	// Returns:   void
	//************************************
	void ListPause();

	//************************************
	// Method:    停止背景声音(只是把声音归0,可以用Back_Run()再次播放)
	// FullName:  CD3DMusic::ListStop
	// Access:    public 
	// Returns:   void
	//************************************
	void ListStop();


	//************************************
	// Method:    控制背景声音大小
	// FullName:  CD3DMusic::ListVolume
	// Access:    public 
	// Returns:   void
	// Parameter: int ibulk			音量大小(-10000 ~ 0)0最大
	//************************************
	void ListVolume(int ibulk);

};
