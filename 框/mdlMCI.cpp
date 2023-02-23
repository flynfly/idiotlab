#include "mdlMCI.h"


bool MciSend( tstring &sCmd, tstring &sError )
{
	DWORD ret=mciSendString(sCmd.c_str(),NULL,0,NULL);
	if (0==ret)
	{
		//����ͳɹ�������true
		return true;
	}
	else
	{
		sError=MciGetErrorStr(ret);//�����ʧ�ܣ���sError���ش�����Ϣ
		return false;
	}
}

bool MciSend( LPCTSTR szCmd )
{
	DWORD ret=mciSendString(szCmd,NULL,0,NULL);//�޷�����Ϣ�����غ���
	return (0==ret);
}

tstring MciSendRetStr( tstring &sCmd, tstring &sError )
{
	tstring sResult(TEXT(""));//����ַ���
	TCHAR szBuff[1024]={0};//��һ��Ԫ��0������δ������ȫ����ʼ��Ϊ0,��ʱ�����ַ������������������鱻�ͷ�
	DWORD ret=mciSendString(sCmd.c_str(),szBuff,sizeof(szBuff)/sizeof(TCHAR),NULL);
	//sizeof(szBuff)/sizeof(TCHAR)��ʾm_szBuff[]���ܱ�����ַ���

	if (0==ret)
	{
		sResult=szBuff;//���ͳɹ�����szBuff[]�е��ַ�����ֵ��sResult������
	}
	else
	{
		sResult=TEXT("");//����
		sError=MciGetErrorStr(ret);
	}
	return sResult;//return ��ʱ���󣬲���return��ʱָ�����
}

tstring MciGetErrorStr( DWORD fdwError )
{
	tstring sError(TEXT(""));
	TCHAR szBuff[1024]={0};//������ʱ���մ�����Ϣ����ʼ0

	if (mciGetErrorString(fdwError,szBuff,1024))
		sError=szBuff;//��ô�����Ϣ
	else
		sError=TEXT("");//��ȡʧ��
	return sError;	
}


