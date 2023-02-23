//////////////////////////////////////////////////////////////////////////
// ADO ���ݿ����ͨ��ģ��
//
// ���������У�����Ҫ��������ͷ�ļ���Ӧ�Ȱ�������ͷ�ļ��� "BWindows.h"��
//   ����ٰ�����ͷ�ļ�
//////////////////////////////////////////////////////////////////////////

#pragma once

#import "msado15.dll" no_namespace rename("EOF","EOFado") 

#include <string>
typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstring;
using std::string;
using std::wstring;

class CBAdoConnection	// ��װ _ConnectionPtr �� ADO Connection �Ķ���
{
private:
	// ��̬��Ա����������Ѵ������������б�������ã���
	//   ���캯���У���ֵ+1���� +1 �� ==1����ִ�� CoInitialize(NULL);
	//   ���������У���ֵ-1���� -1 �� ==0����ִ�� CoUninitialize();
	static long ms_ConnObjCounts;

public:
	CBAdoConnection();
	~CBAdoConnection();

	// ����ADO���ӣ��ɹ�����true��ʧ�ܷ���false
	// �� szDabaBaseName ���ݿ��ļ�
	// ������ݿ����������룬��ָ�� szDBPassword�������������Ϊ NULL ��ʾû������
	// SQLServerUID ��ֻ�� SQLServer ���� uid=
	bool Open(	LPCTSTR szDabaBaseName,
				LPCTSTR szDBPassword = NULL,
				bool	blShowMsgBoxIfFail = true, 
				bool	blSQLServer = false, 
				LPCTSTR szSQLServerUID = NULL, 
				LPCTSTR szSQLServerName = NULL,
				long	iTimeOut = 20);

	bool Execute(LPCTSTR szSQL);
	long State();
	void Close();				// �رն���
	_ConnectionPtr &ConnPtr();	// ���� m_pConn
	LPCTSTR ErrorLastStr();		// �����ϴδ�����Ϣ�ַ���
private:
	_ConnectionPtr m_pConn;
	TCHAR m_ErrStr[1024];		// ������Ϣ�Ļ�����
};

class CBAdoRecordset
{
public:
	CBAdoRecordset();
	~CBAdoRecordset();

	// ʹ��ȫ�� ADOConn �� RecordSet
	bool Open(LPCTSTR szSQL, bool blShowMsgBoxIfFail=true);

	// ʹ�ò���ָ���� conn �� RecordSet
	bool Open(LPCTSTR szSQL, CBAdoConnection &conn, bool blShowMsgBoxIfFail=true);
	bool Open(tstring &sSQL, CBAdoConnection &conn, bool blShowMsgBoxIfFail=true);

	long State();

	bool EOFRs();
	bool MoveNext();
	bool MovePrevious();
	bool MoveFirst();
	bool MoveLast();

	// ��õ�ǰ��¼��һ���ֶ�ֵ
	LPTSTR GetField(LPCTSTR szFieldName);
	LPTSTR GetField(long index);

	// ���õ�ǰ��¼��һ���ֶ�ֵ
	bool SetField(LPCTSTR szFieldName, LPCTSTR szNewValue);
	bool SetField(long index, LPCTSTR szNewValue);

	bool SetField(LPCTSTR szFieldName, int iNewValue);
	bool SetField(long index, int iNewValue);

	bool SetField(LPCTSTR szFieldName, float fNewValue);
	bool SetField(long index, float fNewValue);

	bool SetField(LPCTSTR szFieldName, double dNewValue);
	bool SetField(long index, double dNewValue);


	// ����һ����¼
	bool AddNew();

	// ���¼�¼��
	bool Update();

	void Close();
	_RecordsetPtr & RsPtr();	// ���� m_pRst
	LPCTSTR ErrorLastStr();		// �����ϴδ�����Ϣ�ַ���

private:
	_RecordsetPtr m_pRst;	
	TCHAR m_ErrStr[1024];		// ������Ϣ�Ļ�����
};

// ����ȫ�ֶ������һ��Ҫ�У�����ռ��һ�� CBADOConnection ��Ķ���ĸ���
//   ��ʹ CBADOConnection::ms_ConnObjCounts>0��
//   �� CBADOConnection::ms_ConnObjCounts=0 ʱ����ִ�� CoUninitialize();
extern CBAdoConnection ADOConn;

