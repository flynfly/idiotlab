#include "BWindows.h"

bool MciSend(tstring &sCmd, tstring &sError); //&��ʾ���ô���

bool MciSend(LPCTSTR szCmd);//�޷�������

tstring MciSendRetStr(tstring &sCmd, tstring &sError);//������Ϣ�ַ���

tstring MciGetErrorStr(DWORD fdwError);