#include "BWindows.h"

bool MciSend(tstring &sCmd, tstring &sError); //&表示引用传递

bool MciSend(LPCTSTR szCmd);//无返回重载

tstring MciSendRetStr(tstring &sCmd, tstring &sError);//返回信息字符串

tstring MciGetErrorStr(DWORD fdwError);