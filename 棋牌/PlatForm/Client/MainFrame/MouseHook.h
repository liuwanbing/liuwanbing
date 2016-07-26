#define WM_MOUSEPOSITION WM_USER+5000

class AFX_EXT_CLASS CMouseHook:public CObject  
{  
public:  
	CMouseHook();	//钩子类的构造函数  
	~CMouseHook();	//钩子类的析构函数  
	BOOL StartHook(HWND hWnd);	//安装钩子函数  
	BOOL StopHook();	//卸载钩子函数  
};  
