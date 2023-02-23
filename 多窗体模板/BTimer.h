//////////////////////////////////////////////////////////////////////////
//BTimer.h:CBTimer��Ķ���
//ʵ�ֶ�ʱ��
//////////////////////////////////////////////////////////////////////////
#include <Windows.h>

class CBTimer
{
public:
	//�๫�г�Ա�ͳ�Ա����

	//���캯��
	CBTimer(HWND hWndAsso=NULL,UINT uElapse=1000,TIMERPROC ptrFuncCallBack=NULL);

	//��������
	~CBTimer();

	bool Enabled();
	void EnabledSet(bool value);
	
	//���ػ����ö�ʱ���Ĵ������ʱ�䣨���룩
	UINT Interval();
	void IntervalSet(UINT uInterval);

	//
	TIMERPROC pFuncCallBack();
	void pFuncCallBackSet(TIMERPROC ptrFuncCallBack);

	//���ػ����ù������ھ����Ϊ0��ʾ��ʱ��δ��������
	HWND hWndAsso();
	void hWndAssoSet(HWND hWnd);

	UINT Identifier();

private:						//��˽�г�Ա�ͳ�Ա����

	//��̬��Ա
	static UINT ms_uIDIncrement;//Ϊ��ö�ʱ��ID����������

	UINT m_uTimerID;			//��ʱ����ʶ��δ��������ʱΪAPI�����ID������Ϊ��������ID��
	HWND m_hWndAsso;			//�������Ĵ��ھ������Ϊ0��ʾ��ʱ��δ�������ڣ�
	UINT m_uElapse;				//��ʱ���������ʱ�䣬��λ����,����ʱÿ��m_uElapse����ͻ���ûص�����һ��

	TIMERPROC m_pFuncCallBack;	//��ʱ������ʱ�ص������ĵ�ַ
	bool m_blEnabled;			//��ǰ��ʱ�����á�����״̬

};