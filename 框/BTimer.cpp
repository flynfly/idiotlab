#include "BTimer.h"

UINT CBTimer::ms_uIDIncrement=0;

bool CBTimer::Enabled()//��ö�ʱ��״̬
{
	return m_blEnabled;
}

UINT CBTimer::Interval()
{
	return m_uElapse;//���ض�ʱ��ʱ����
}

void CBTimer::IntervalSet( UINT uInterval )
{
	if(m_uElapse==uInterval)return;
	//�����µ�ʱ����
	m_uElapse=uInterval;

	if(m_blEnabled)
	{
		EnabledSet(false);
		EnabledSet(true);
	}
}

TIMERPROC CBTimer::pFuncCallBack()//���ػص�������ַ
{
	return m_pFuncCallBack;
}

void CBTimer::pFuncCallBackSet( TIMERPROC ptrFuncCallBack )
{
	if(m_pFuncCallBack==ptrFuncCallBack) return;

	m_pFuncCallBack=ptrFuncCallBack;

	if(m_blEnabled)
	{
		EnabledSet(false);
		EnabledSet(true);
	}
}

HWND CBTimer::hWndAsso()//���ض�ʱ���������ھ��
{
	return m_hWndAsso;
}

void CBTimer::hWndAssoSet( HWND hWnd )
{
	if(m_hWndAsso==hWnd) return;

	bool blLastEnabled=m_blEnabled;
	if(m_blEnabled)EnabledSet(false);

	m_hWndAsso=hWnd;

	if(blLastEnabled)
	{
		EnabledSet(true);
	}

}

CBTimer::CBTimer( HWND hWndAsso/*=NULL*/,UINT uElapse/*=1000*/,TIMERPROC ptrFuncCallBack/*=NULL*/ )
{
	//�����Ա����
	ms_uIDIncrement++;	m_uTimerID=ms_uIDIncrement;
	m_hWndAsso=NULL;
	m_uElapse=0;
	m_pFuncCallBack=NULL;
	m_blEnabled=false;

	hWndAssoSet(hWndAsso);
	IntervalSet(uElapse);
	pFuncCallBackSet(ptrFuncCallBack);

	EnabledSet(true);
}

CBTimer::~CBTimer()
{
	EnabledSet(false);
}

void CBTimer::EnabledSet( bool value )//
{
	if (value==m_blEnabled) return;//��ʱ��״̬��value��ͬ�����ظ��趨

	if (value)
	{
		if (m_pFuncCallBack!=NULL && m_uElapse!=0)//������Ч������������ʱ��
		{
			UINT ret=SetTimer(m_hWndAsso,m_uTimerID,m_uElapse,m_pFuncCallBack);

			if (ret) m_uTimerID=ret;

			m_blEnabled=(bool)ret;//=(ret!=0);
		}
	}
	else
	{
		KillTimer(m_hWndAsso,m_uTimerID);

		if (NULL==m_hWndAsso)
		{
			ms_uIDIncrement++;
			m_uTimerID=ms_uIDIncrement;
		}

		m_blEnabled=false;
	}
}

UINT CBTimer::Identifier()
{
	if (m_blEnabled) return m_uTimerID; else return 0;
}
