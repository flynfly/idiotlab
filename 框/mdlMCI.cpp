#include "mdlMCI.h"


bool MciSend( tstring &sCmd, tstring &sError )
{
	DWORD ret=mciSendString(sCmd.c_str(),NULL,0,NULL);
	if (0==ret)
	{
		//命令发送成功，返回true
		return true;
	}
	else
	{
		sError=MciGetErrorStr(ret);//命令发送失败，由sError返回错误信息
		return false;
	}
}

bool MciSend( LPCTSTR szCmd )
{
	DWORD ret=mciSendString(szCmd,NULL,0,NULL);//无返回信息的重载函数
	return (0==ret);
}

tstring MciSendRetStr( tstring &sCmd, tstring &sError )
{
	tstring sResult(TEXT(""));//结果字符串
	TCHAR szBuff[1024]={0};//第一个元素0，其余未给出：全部初始化为0,临时接收字符串，函数结束后数组被释放
	DWORD ret=mciSendString(sCmd.c_str(),szBuff,sizeof(szBuff)/sizeof(TCHAR),NULL);
	//sizeof(szBuff)/sizeof(TCHAR)表示m_szBuff[]中能保存的字符数

	if (0==ret)
	{
		sResult=szBuff;//发送成功，将szBuff[]中的字符串赋值给sResult并返回
	}
	else
	{
		sResult=TEXT("");//命令
		sError=MciGetErrorStr(ret);
	}
	return sResult;//return 临时对象，不能return临时指针变量
}

tstring MciGetErrorStr( DWORD fdwError )
{
	tstring sError(TEXT(""));
	TCHAR szBuff[1024]={0};//用于临时接收错误信息，初始0

	if (mciGetErrorString(fdwError,szBuff,1024))
		sError=szBuff;//获得错误信息
	else
		sError=TEXT("");//获取失败
	return sError;	
}


