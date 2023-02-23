//////////////////////////////////////////////////////////////////////////
//BTimer.h:CBTimer类的定义
//实现定时器
//////////////////////////////////////////////////////////////////////////
#include <Windows.h>

class CBTimer
{
public:
	//类公有成员和成员函数

	//构造函数
	CBTimer(HWND hWndAsso=NULL,UINT uElapse=1000,TIMERPROC ptrFuncCallBack=NULL);

	//析构函数
	~CBTimer();

	bool Enabled();
	void EnabledSet(bool value);
	
	//返回或设置定时器的触发间隔时间（毫秒）
	UINT Interval();
	void IntervalSet(UINT uInterval);

	//
	TIMERPROC pFuncCallBack();
	void pFuncCallBackSet(TIMERPROC ptrFuncCallBack);

	//返回或设置关联窗口句柄，为0表示定时器未关联窗口
	HWND hWndAsso();
	void hWndAssoSet(HWND hWnd);

	UINT Identifier();

private:						//类私有成员和成员函数

	//静态成员
	static UINT ms_uIDIncrement;//为获得定时器ID的增量变量

	UINT m_uTimerID;			//定时器标识（未关联窗口时为API分配的ID，否则为本类分配的ID）
	HWND m_hWndAsso;			//所关联的窗口句柄（可为0表示定时器未关联窗口）
	UINT m_uElapse;				//定时器触发间隔时间，单位毫秒,启动时每隔m_uElapse毫秒就会调用回调函数一次

	TIMERPROC m_pFuncCallBack;	//定时器触发时回调函数的地址
	bool m_blEnabled;			//当前定时器启用、禁用状态

};