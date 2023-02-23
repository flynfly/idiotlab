#include "mdlSvmRun.h"
#include "mdlFileSys.h"
#include "mdlShellExec.h"//֧�������ⲿ����

LPCTSTR mc_szScaleExeFile = TEXT("svm-scale.exe");//ģ�鼶����
LPCTSTR mc_szTrainExeFile = TEXT("svm-train.exe");//libSvm�Ŀ�ִ���ļ���
LPCTSTR mc_szTestExeFile = TEXT("svm-predict.exe");

// ����һ���ļ��Ƿ���ڣ������ڷ���true���粻���ڷ���false��������ʾszPromptIfNotExist
bool TestFileExist(tstring &sFile, TCHAR * szPromptIfNotExist)
{
	if (FMFileExist(sFile.c_str())==1)//��.c_str()����ַ����׵�ַ
		return true;
	else
	{
		tstring sPrompt = sFile + TEXT("\r\n") + szPromptIfNotExist; 
		MsgBox(sPrompt, TEXT("�ļ�������"),mb_OK,mb_IconExclamation);
		return false;
	}
}

//У��LibSVM��ִ���ļ��Ƿ����,���ڷ���true,�����ڵ�����ʾ��Ϣ������false
bool VerifyLibSVMFiles()
{
	//��pApp->Path��ñ��������������ļ���
	tstring sAppPath = FMAddBackSlash(pApp->Path());
	if (!TestFileExist(sAppPath + mc_szScaleExeFile,TEXT("ȱ�ٹ�һ����ִ���ļ����뽫svm-scale.exe���������������ļ�����!")))
		return false;
	if (!TestFileExist(sAppPath + mc_szTrainExeFile,TEXT("ȱ��SVMѵ����ִ���ļ����뽫svm-train.exe���������������ļ�����!")))
		return false;
	if (!TestFileExist(sAppPath + mc_szTestExeFile,TEXT("ȱ��SVM���Կ�ִ���ļ����뽫svm-predict.exe���������������ļ�����!")))
		return false;

	return true;
}

//��szPathĿ¼�´���һ���������ļ����ļ���ΪszBatFileName��������д��������sCmdLine
//�ɹ��������������귵��true,�޷������������ļ�����false
bool RunBat (TCHAR * szPath, TCHAR * szBatFileName, tstring &sCmdLine)
{
	//�������ļ�����ȫ·��
	tstring sBatFile;
	sBatFile = FMAddBackSlash(szPath) ;
	sBatFile = sBatFile + szBatFileName;
	
	HANDLE hFileBat = EFOpen(sBatFile.c_str(),EF_OpStyle_Output);
	if (hFileBat==INVALID_HANDLE_VALUE) return false;
	EFPrint(hFileBat,sCmdLine.c_str());
	EFClose(hFileBat);

	SERunWaitTerminate(sBatFile.c_str(),true);
	//ɾ���������ļ�
	FMDeleteFiles(sBatFile.c_str(),true,true,false);
	return true;
}

bool SVMScaleData(  TCHAR * szFileData,
					TCHAR * szFileDataScaleParam,
					bool fParamReadOrWrite,
					tstring &sRetString)
{
	//׼��SVM��ִ���ļ���(��ȫ·��)
	tstring sAppPath = FMAddBackSlash(pApp->Path());
	tstring sSVMExeScale = sAppPath + mc_szScaleExeFile;

	TCHAR * szPath = NULL,*szExp = NULL; // szDataFile�е�·�����֡���չ������
	TCHAR * szFileNameOnly = FMTrimFileName(szFileData,true,true, &szPath, &szExp);
	tstring sFileDataScale;
	tstring sCmdLine;

	if (szFileDataScaleParam) // szFileDataScaleParam!=NULL
	{
		sFileDataScale = szPath;
		sFileDataScale += TEXT("\\");
		sFileDataScale += szFileNameOnly;
		sFileDataScale += TEXT(".scale.");
		sFileDataScale += szExp;

		//�����һ�������ļ�szFileDataScaleParam����ļ�����ͬ������(��.data)
		if (_tcscmp (sFileDataScale.c_str(),szFileDataScaleParam)==0)
		{
			sFileDataScale = szPath;
			sFileDataScale += TEXT("\\");
			sFileDataScale += szFileNameOnly;
			sFileDataScale += TEXT(".scale.data.");
			sFileDataScale += szExp;
		}
		//���sFileDataScale�ļ��Ѵ��ڣ�ɾ�����ļ�
		FMDeleteFiles (sFileDataScale.c_str(), true, true, false);
		//ɾ�����飬����ļ���Ȼ���ڣ�˵��δɾ���ɹ�
		if (FMFileExist (sFileDataScale.c_str())==1)
		{
			sRetString = TEXT("����ɾ��֮ǰ�����Ĺ�һ�����ݽ���ļ�:")	+ sFileDataScale;//����˵��
			return false;
		}
		//׼����һ���������浽���ļ�
		if (! fParamReadOrWrite)
		{
			//����д�µĲ����ļ�
			//���szFileDataScaleParam�Ѵ��ڣ�ɾ�����ļ�
			FMDeleteFiles(szFileDataScaleParam, true, true,false);
		
			//ɾ�����飬����ļ���Ȼ���ڣ�˵��δɾ���ɹ�
			if (FMFileExist(szFileDataScaleParam)==1)
			{
				sRetString = TEXT("����ɾ����ǰ�����Ĺ�һ�������ļ�: ");
				sRetString = sRetString + szFileDataScaleParam; // ����˵��
				return false;
			}

			//���ɹ�һ����������
			sCmdLine = TEXT("\"") + sSVMExeScale + TEXT("\"  -s  ");
		}
		else
		{
			//�����еĲ����ļ�
			sCmdLine = TEXT("\"") + sSVMExeScale + TEXT("\"  -r  ");
		}

		//�������ɹ�һ����������
		sCmdLine = sCmdLine + TEXT("\"") + szFileDataScaleParam + TEXT("\"  ");
		sCmdLine = sCmdLine + TEXT("\"") + szFileData + TEXT("\"  ");
		sCmdLine = sCmdLine + TEXT("> \"") + sFileDataScale + TEXT("\"");

		//ͨ���������ļ��ķ�ʽ���д������У����ȴ����н���
		if ( ! RunBat (szPath, TEXT("svmscale.bat"),sCmdLine))
		{
			sRetString = TEXT("���ݹ�һ��ʧ��!�޷����ļ���");
			sRetString = sRetString + szPath + TEXT("�д����������������ļ�: svmscale.bat��");
			return false;
		}

		//������ļ�sFileDataScale �Ƿ�������
		if ( FMFileExist( sFileDataScale.c_str()) != 1 )
		{
			sRetString = TEXT("���ݹ�һ��ʧ��!δ�����ɹ�һ����Ľ�������ļ�:") + sFileDataScale;
			return false; 
		}
	}
	else
	{
		//����һ����ֱ������sFileDataScaleΪԭ�����ļ�
		sFileDataScale = szFileData;
	}
	sRetString = sFileDataScale; 
	return true;
}


bool SVMTrain ( TCHAR * szFileData, 
				TCHAR * szFileModel, 
				TCHAR * szFileDataScaleParam,
				int iKernelFunc,
				int iFold,
				TCHAR * szOtherParams,
				tstring &sRetString)
{
	//׼��SVM��ִ���ļ���(��ȫ·��)
	tstring sAppPath = FMAddBackSlash (pApp->Path());
	tstring sSVMExeTrain = sAppPath + mc_szTrainExeFile;

	//�����ļ�������ͬ
	if (szFileData && szFileModel && _tcscmp(szFileData,szFileModel)==0)
	{
		sRetString = TEXT("���������ļ���Ҫ���ɵ�model�ļ�������ͬһ�ļ���");
		return false;
	}
	if (szFileData && szFileDataScaleParam && _tcscmp(szFileData, szFileDataScaleParam)==0)
	{
		sRetString = TEXT("���������ļ��͹�һ�������ļ�������ͬһ�ļ���");
		return false;
	}
	if (szFileModel && szFileDataScaleParam && _tcscmp(szFileModel, szFileDataScaleParam)==0)
	{
		sRetString = TEXT("Ҫ���ɵ�model�ļ��͹�һ�������ļ�������ͬһ�ļ���");
		return false;
	}

	//׼���ļ���
	TCHAR * szPath = NULL, *szExp = NULL; //szDataFile�е�·�����֡���չ������
	TCHAR * szFileNameOnly = FMTrimFileName (szFileData, true,	true, &szPath, &szExp);
	tstring sFileDataScale, sCmdLine;
	//��һ�����ݣ���ɹ���sFileDataScale �б����˹�һ����Ľ�������ļ���
	if ( ! SVMScaleData (szFileData, szFileDataScaleParam, false, sFileDataScale))
	{
		sRetString = sFileDataScale;	//��ʧ�ܣ�sFileDataScaleΪ������Ϣ
		return false;
	}

	//����SVMѵ������ѵ��ģ��
	sCmdLine = TEXT("\"") + sSVMExeTrain + TEXT("\"  ");
	sCmdLine = sCmdLine + TEXT(" -t ") + Str(iKernelFunc);

	if (szFileModel==NULL)
		sCmdLine = sCmdLine + TEXT(" -v ") + Str(iFold);
	//���������м���������
	sCmdLine = sCmdLine + TEXT(" ") + szOtherParams + TEXT("  ");
	//���������м������ļ��ļ���: sFileDataScale��
	sCmdLine = sCmdLine + TEXT("  \"") + sFileDataScale + TEXT("\"  ");
	//������model�ļ������������м�model�ļ����ļ���
	if (szFileModel!= NULL)
		sCmdLine = sCmdLine + TEXT("  \"") + szFileModel + TEXT("\"");

	//ͨ���������ļ��ķ�ʽ�����д������У����ȴ����н���
	if ( ! RunBat (szPath, TEXT("svntrain.bat"),sCmdLine))
	{
		sRetString = TEXT("SVMѵ��ʧ��! �޷����ļ��� ");
		sRetString = sRetString + szPath + TEXT("�д����������������ļ�: svmtrain.bat��");
		return false;
	}

	//��ʾ���
	tstring sFileResult;
	if (szFileModel==NULL)
	{
		//������model�ļ�
		sFileResult = sFileDataScale + TEXT("(t=") + Str (iKernelFunc) + TEXT( ").training_result" );
		if ( FMFileExist( sFileResult.c_str()) !=1 )
			{ 
				sRetString = TEXT("SVMѵ��ʧ��! δ������ѵ������ļ�:")	+ sFileResult;
				return false;
			}
			//�ɹ�:����ļ������ɣ���sRetString���ؽ���ļ���
			sRetString = sFileResult;
	}
	else
	{
		//���model�ļ��Ƿ�������
		if ( FMFileExist( szFileModel ) != 1 )
		{ 
			sRetString = TEXT(" model�ļ�����ʧ��!δ������model�ļ�: ");
			sRetString = sRetString + szFileModel;
			return false;
		}
		//�ɹ�: model �ļ������ɣ���sRetString ����model�ļ���
		sRetString = szFileModel;
	}
	return true;	
}


bool SVMPredict (   TCHAR * szFileData,
					TCHAR * szFileModel,
					TCHAR * szFileDataScaleParam,
					TCHAR * szFileResult,
					int iParamB,
					tstring &sRetString)
{
	//׼��SVM��ִ���ļ���
	tstring sAppPath = FMAddBackSlash (pApp->Path());
	tstring sSVMExePredict = sAppPath + mc_szTestExeFile;

	//�����ļ�������ͬ
	if (szFileData && szFileModel && _tcscmp(szFileData, szFileModel)==0)
	{ 
		sRetString = TEXT("���������ļ���model�ļ�������ͬһ�ļ���");
		return false;
	}
	if (szFileData && szFileDataScaleParam && _tcscmp(szFileData,szFileDataScaleParam)==0)
	{ 
		sRetString = TEXT("���������ļ��͹�һ�������ļ�������ͬһ�ļ���");
		return false;
	}
	if (szFileData && szFileResult && _tcscmp(szFileData,szFileResult)==0)
	{ 
		sRetString = TEXT("���������ļ��ͽ���ļ�������ͬһ�ļ���");
		return false;
	}
	if (szFileModel && szFileDataScaleParam && _tcscmp(szFileModel,szFileDataScaleParam)==0)
	{ 
		sRetString = TEXT("model�ļ��͹�һ�������ļ�������ͬһ�ļ���");
		return false;
	}
	if (szFileModel && szFileResult && _tcscmp(szFileModel,szFileResult)==0)
	{ 
		sRetString = TEXT("model�ļ��ͽ���ļ�������ͬһ�ļ���");
		return false;
	}
	if (szFileDataScaleParam && szFileResult && _tcscmp(szFileDataScaleParam,szFileResult)==0)
	{ 
		sRetString = TEXT("��һ�������ļ��ͽ���ļ�������ͬһ�ļ���");
		return false;
	}

	//׼���ļ���
	TCHAR *szPath = NULL, *szExp = NULL; //szDataFile�е�·�����֡���չ������
	TCHAR *szFileNameOnly = FMTrimFileName (szFileData,true,true, &szPath, &szExp);

	tstring sFileDataScale, sCmdLine;
	//��һ�����ݣ���ɹ���sFileDataScale�б����˹�һ����Ľ�������ļ���
	if ( ! SVMScaleData(szFileData, szFileDataScaleParam, true, sFileDataScale) )
	{
		sRetString = sFileDataScale;		//ʧ��ʱsFileDataScale��Ϊ������Ϣ
		return false;
	}

	//����SVM���Գ������ģ��
	sCmdLine = TEXT("\"") + sSVMExePredict + TEXT("\"  ");
	if (iParamB>0)
	{
		sCmdLine = sCmdLine + TEXT(" -b ") + Str (iParamB);
	}

	//���������м������ļ��ļ���:sFileDataScale
	sCmdLine = sCmdLine + TEXT("  \"") + sFileDataScale + TEXT("\"  ");		//���������м�model�ļ����ļ���
	sCmdLine = sCmdLine + TEXT("  \"") + szFileModel + TEXT("\"");			//���������мӽ���ļ����ļ���
	sCmdLine = sCmdLine + TEXT("  \"") + szFileResult + TEXT("\"");

	//ͨ���������ļ��ķ�ʽ���д������У����ȴ����н���
	if ( ! RunBat (szPath,TEXT("svmpredict.bat"),sCmdLine))
		{ 
			sRetString = TEXT("SVM����ʧ��!�޷����ļ���");
			sRetString = sRetString + szPath + TEXT(" �д����������������ļ�: svmpredict.bat��");
			return false;
		}
	//��ʾ���
	//������ļ��Ƿ�������
	if ( FMFileExist( szFileResult ) != 1 )
	{ 
		sRetString = TEXT ("SVM����ʧ��!δ�����ɽ���ļ�: ");
		sRetString = sRetString + szFileResult;
		return false;
	}

	//�ɹ�:����ļ������ɣ���sRetString���ؽ���ļ���
	sRetString = szFileResult;
	return true;

}