//�Զ�����svm����
#include"BWindows.h"

//ͨ��ģ�麯������

// ����һ���ļ��Ƿ���ڣ������ڷ���true���粻���ڷ���false��������ʾszPromptIfNotExist
bool TestFileExist(tstring &sFile, TCHAR * szPromptIfNotExist);


//У��LibSVM��ִ���ļ��Ƿ����,���ڷ���true,�����ڵ�����ʾ��Ϣ������false
bool VerifyLibSVMFiles();

//������
bool RunBat(TCHAR * szPath,TCHAR * szBatFileName, tstring &sCmdLine);

//��һ�������ļ�
bool SVMScaleData(  TCHAR * szFileData,
					TCHAR * szFileDataScaleParam,
					bool fParamReadOrWrite,
					tstring &sRetString);
//����Svmѵ��
bool SVMTrain ( TCHAR * szFileData, 
				TCHAR * szFileModel, 
				TCHAR * szFileDataScaleParam,
				int iKernelFunc,
				int iFold,
				TCHAR * szOtherParams,
				tstring &sRetString);
//����Svm����
bool SVMPredict (   TCHAR * szFileData,
					TCHAR * szFileModel,
					TCHAR * szFileDataScaleParam,
					TCHAR * szFileResult, 
					int iParamB,
					tstring &sRetString);