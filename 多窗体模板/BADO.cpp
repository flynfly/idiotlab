//////////////////////////////////////////////////////////////////////////
// ADO ���ݿ����ͨ��ģ�� BADO ��ʵ��
//
//////////////////////////////////////////////////////////////////////////


#include "BADO.h"
#include <string.h>
#include <tchar.h>

#pragma warning(disable:4996) // �ر� warning C4996: �� 'wcscpy': This function or variable may be unsafe. Consider using wcscpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.

TCHAR m_EmptyStr[2]={TEXT('\0')};

// ����ȫ�ֶ������һ��Ҫ�У�����ռ��һ�� CBAdoConnection ��Ķ���ĸ���
//   ��ʹ CBAdoConnection::ms_ConnObjCounts>0��
//   �� CBAdoConnection::ms_ConnObjCounts=0 ʱ����ִ�� CoUninitialize();
CBAdoConnection ADOConn;

// GenComError ���ڱ�cpp�ļ���ʹ�ã�.h�ļ���Ҳû��������
//   �Ҹú������ܱ������ļ�ʹ��
static void GenComError(_com_error &e, LPTSTR szErrBuff)
{
	_stprintf(szErrBuff, TEXT("Error:\n"));
	_stprintf(szErrBuff, TEXT("%sCode = %08lx\n") ,szErrBuff ,e.Error());
	_stprintf(szErrBuff, TEXT("%sCode meaning = %s\n") , szErrBuff, (TCHAR *) e.ErrorMessage());
	_stprintf(szErrBuff, TEXT("%sSource = %s\n") , szErrBuff, (TCHAR *) e.Source());
	_stprintf(szErrBuff, TEXT("%sDescription = %s") ,szErrBuff, (TCHAR *) e.Description());
}

//////////////////////////////////////////////////////////////////////////
// CBAdoConnection ���ʵ��
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// �������е� static ��Ա�� static ����

long CBAdoConnection::ms_ConnObjCounts = 0;

CBAdoConnection::CBAdoConnection()
{
	ms_ConnObjCounts++;			// ����Ķ������ +1
	if (ms_ConnObjCounts==1)	// �� +1 �� ==1����ִ�� CoInitialize(NULL);��ʼ�� com ����
		CoInitialize(NULL);

	try
	{
		m_pConn.CreateInstance(__uuidof(Connection));
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
	}
	catch(_com_error &e)
	{
		m_pConn = NULL;
		GenComError(e, m_ErrStr);
	}
}

CBAdoConnection::~CBAdoConnection()
{
	try
	{
		Close();
		m_pConn.Release();
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
	}

	ms_ConnObjCounts--;				// ����Ķ������ -1
	if (ms_ConnObjCounts<=0)		// �� -1 �� ==0����ִ�� CoUninitialize();
		CoUninitialize();
}

void CBAdoConnection::Close()
{
	try
	{
		if ( m_pConn->State == adStateOpen ) m_pConn->Close();
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
	}
}

bool CBAdoConnection::Open( LPCTSTR szDabaBaseName, LPCTSTR szDBPassword /*= NULL*/, bool blShowMsgBoxIfFail /*= true*/, bool blSQLServer /*= false*/, LPCTSTR szSQLServerUID /*= NULL*/, LPCTSTR szSQLServerName /*= NULL*/, long iTimeOut/*=20*/ )
{
	TCHAR szBuff[1024]={TEXT('\0')};

	Close();

	if (blSQLServer)
	{
		// �� SQL Server ���ݿ�ĸ�ʽ�������ݿ�
		// Provider=SQLOLEDB.1;Persist Security Info=False;User ID=sa;Password=;Initial Catalog=pubs
		_tcscpy(szBuff, TEXT("Provider=SQLOLEDB.1;Persist Security Info=False;Initial Catalog="));
		_tcscat(szBuff, szDabaBaseName);

		if (szSQLServerUID)
		{
			_tcscat(szBuff, TEXT(";uid="));
			_tcscat(szBuff, szSQLServerUID);
		}
		else
		{
			_tcscat(szBuff, TEXT(";Integrated Security=SSPI;"));
		}

		if (szDBPassword)
		{
			_tcscat(szBuff, TEXT(";pwd="));
			_tcscat(szBuff, szDBPassword);
		}

		if (szSQLServerName)
		{
			_tcscat(szBuff, TEXT(";Data Source="));
			_tcscat(szBuff, szSQLServerName);
		}
	}
	else
	{
		// �� Access ���ݿ�ĸ�ʽ�������ݿ�
		_tcscpy(szBuff, TEXT("Provider=Microsoft.ACE.OLEDB.12.0;Data Source="));
		// mdb �ļ��� Provider=Microsoft.Jet.OLEDB.4.0;
		_tcscat(szBuff, szDabaBaseName);
		_tcscat(szBuff, TEXT(";Persist Security Info=False"));

		if (szDBPassword)
		{
			_tcscat(szBuff, TEXT(";Jet OLEDB:Database Password="));
			_tcscat(szBuff, szDBPassword);
		}
	}

	try
	{
		HRESULT hr;
		m_pConn->ConnectionString = (_bstr_t)szBuff;
		hr = m_pConn->Open("", "", "", adModeUnknown);
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
		return true;
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
		if (blShowMsgBoxIfFail)	// szDabaBaseName
		{
			TCHAR szErrorBuff[1024]={ TEXT('\0') };
			_tcscpy(szErrorBuff, TEXT("BADO: Fail to connect database: \r\n"));
			_tcscat(szErrorBuff , szDabaBaseName);
			GenComError(e, m_ErrStr);

			MessageBox(0, szErrorBuff, TEXT("BADO Error"), 0);
		}
		return false;
	}
}

bool CBAdoConnection::Execute( LPCTSTR szSQL )
{
	try
	{
		m_pConn->Execute(szSQL, NULL, adCmdText);
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
		return true;
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
		return false;
	}
}

_ConnectionPtr & CBAdoConnection::ConnPtr()
{
	return m_pConn;
}

long CBAdoConnection::State()
{
	return m_pConn->State;
}

LPCTSTR CBAdoConnection::ErrorLastStr()
{
	return m_ErrStr;
}


//////////////////////////////////////////////////////////////////////////
// CBAdoRecordset ���ʵ��
//////////////////////////////////////////////////////////////////////////

CBAdoRecordset::CBAdoRecordset()
{
	try
	{
		m_pRst.CreateInstance(__uuidof(Recordset));
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
	}
	catch(_com_error &e)
	{
		m_pRst = NULL;
		GenComError(e, m_ErrStr);
	}
}

CBAdoRecordset::~CBAdoRecordset()
{
	Close();
	m_pRst.Release();
}

bool CBAdoRecordset::Open( LPCTSTR szSQL, bool blShowMsgBoxIfFail/*=true*/ )
{
	// ʹ��ȫ�� ADOConn �� RecordSet
	return Open(szSQL, ADOConn, blShowMsgBoxIfFail);
}

bool CBAdoRecordset::Open( LPCTSTR szSQL, CBAdoConnection &conn, bool blShowMsgBoxIfFail/*=true*/ )
{
	try
	{	
		_ConnectionPtr cp = conn.ConnPtr();
		if(cp == NULL)
		{
			_tcscpy(m_ErrStr, TEXT("Invalid Connection: the parameter 'conn' is NULL in function CBAdoRecordset::Open(). "));
			return false;
		}

		Close();

		//cp.CreateInstance(__uuidof(Connection));
		//cp->ConnectionString = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=PPI.accdb;Persist Security Info=False";
		//cp->Open("","","",adModeUnknown);

		int i=m_pRst -> Open(szSQL, _variant_t((IDispatch*)cp), adOpenDynamic, adLockOptimistic, adCmdText);
		// m_pRst = conn->Execute("select * from ProtLinks", NULL, adCmdText);
		
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
		return true;
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
		if (blShowMsgBoxIfFail)	// szDabaBaseName
		{
			TCHAR szErrorBuff[1024]={ TEXT('\0') };
			_tcscpy(szErrorBuff, TEXT("BADO: Fail to open recordset: \r\n"));
			_tcscat(szErrorBuff , szSQL);
			MessageBox(0, szErrorBuff, TEXT("BADO Error"), 0);
		}
		return false;
	}
}

bool CBAdoRecordset::Open( tstring &sSQL, CBAdoConnection &conn, bool blShowMsgBoxIfFail/*=true*/ )
{
	return Open(sSQL.c_str(), conn, blShowMsgBoxIfFail);
}

void CBAdoRecordset::Close()
{
	if ( m_pRst->State == adStateOpen) m_pRst->Close();
}

_RecordsetPtr & CBAdoRecordset::RsPtr()
{
	return m_pRst;
}

bool CBAdoRecordset::EOFRs()
{
	return m_pRst->EOFado != 0;
}

bool CBAdoRecordset::MoveNext()
{
	try
	{
		m_pRst -> MoveNext();
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
		return true;
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
		return false;
	}
}

bool CBAdoRecordset::MovePrevious()
{
	try
	{
		m_pRst -> MovePrevious();
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
		return true;
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
		return false;
	}
}

bool CBAdoRecordset::MoveFirst()
{
	try
	{
		m_pRst -> MoveFirst();
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
		return true;
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
		return false;
	}
}

bool CBAdoRecordset::MoveLast()
{
	try
	{
		m_pRst -> MoveLast();
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
		return true;
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
		return false;
	}
}

LPCTSTR CBAdoRecordset::ErrorLastStr()
{
	return m_ErrStr;
}

LPTSTR CBAdoRecordset::GetField( LPCTSTR szFieldName )
{
	try
	{
		return (TCHAR *)(_bstr_t)m_pRst -> GetFields() -> GetItem(szFieldName) -> GetValue();
//		return (TCHAR *)(_bstr_t)m_pRst->GetCollect(szFieldName);
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
		m_EmptyStr[0] = TEXT('\0');
		return m_EmptyStr;
	}
}

LPTSTR CBAdoRecordset::GetField( long index )
{
	try
	{
		return (TCHAR *)(_bstr_t)m_pRst -> GetFields() -> GetItem(index) -> GetValue();
		//return (TCHAR *)(_bstr_t)m_pRst->GetCollect(index);
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
		m_EmptyStr[0] = TEXT('\0');
		return m_EmptyStr;
	}
}

bool CBAdoRecordset::SetField( LPCTSTR szFieldName, LPCTSTR szNewValue )
{
	try
	{
		m_pRst -> Fields -> Item[szFieldName] -> Value = szNewValue;  
		//m_pRst -> PutCollect(szFieldName, szNewValue);
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
		return true;
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
		return false;
	}
}

bool CBAdoRecordset::SetField( long index, LPCTSTR szNewValue )
{
	try
	{
		m_pRst -> Fields -> Item[index] -> Value = szNewValue; 
		// m_pRst -> PutCollect(index, szNewValue);
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
		return true;
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
		return false;
	}
}

bool CBAdoRecordset::SetField( LPCTSTR szFieldName, int iNewValue )
{
	LPTSTR szValue = new TCHAR [128];
	bool ret;
	_stprintf(szValue, TEXT("%d"), iNewValue);
	ret = SetField(szFieldName, szValue);
	delete []szValue;
	return ret;
}

bool CBAdoRecordset::SetField( LPCTSTR szFieldName, double dNewValue )
{
	LPTSTR szValue = new TCHAR [128];
	bool ret;
	_stprintf(szValue, TEXT("%.15g"), dNewValue);
	ret = SetField(szFieldName, szValue);
	delete []szValue;
	return ret;
}

bool CBAdoRecordset::SetField( long index, double dNewValue )
{
	LPTSTR szValue = new TCHAR [128];
	bool ret;
	_stprintf(szValue, TEXT("%.15g"), dNewValue);
	ret = SetField(index, szValue);
	delete []szValue;
	return ret;
}

bool CBAdoRecordset::SetField( LPCTSTR szFieldName, float fNewValue )
{
	LPTSTR szValue = new TCHAR [128];
	bool ret;
	_stprintf(szValue, TEXT("%.7g"), fNewValue);
	ret = SetField(szFieldName, szValue);
	delete []szValue;
	return ret;
}

bool CBAdoRecordset::SetField( long index, float fNewValue )
{
	LPTSTR szValue = new TCHAR [128];
	bool ret;
	_stprintf(szValue, TEXT("%.7g"), fNewValue);
	ret = SetField(index, szValue);
	delete []szValue;
	return ret;
}

bool CBAdoRecordset::SetField( long index, int iNewValue )
{
	LPTSTR szValue = new TCHAR [128];
	bool ret;
	_stprintf(szValue, TEXT("%d"), iNewValue);
	ret = SetField(index, szValue);
	delete []szValue;
	return ret;
}

bool CBAdoRecordset::AddNew()
{
	try
	{
		m_pRst -> AddNew();
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
		return true;
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
		return false;
	}
}

bool CBAdoRecordset::Update()
{
	try
	{
		m_pRst -> Update();
		m_ErrStr[0] = TEXT('\0');		// ��մ�����Ϣ�ַ���Ϊ ""
		return true;
	}
	catch(_com_error &e)
	{
		GenComError(e, m_ErrStr);
		return false;
	}
}

long CBAdoRecordset::State()
{
	return m_pRst->State;
}
