#include "mdlSvmRun.h"
#include "mdlFileSys.h"
#include "mdlShellExec.h"//支持运行外部程序

LPCTSTR mc_szScaleExeFile = TEXT("svm-scale.exe");//模块级常量
LPCTSTR mc_szTrainExeFile = TEXT("svm-train.exe");//libSvm的可执行文件名
LPCTSTR mc_szTestExeFile = TEXT("svm-predict.exe");

// 测试一个文件是否存在，若存在返回true，如不存在返回false并给出提示szPromptIfNotExist
bool TestFileExist(tstring &sFile, TCHAR * szPromptIfNotExist)
{
	if (FMFileExist(sFile.c_str())==1)//用.c_str()获得字符串首地址
		return true;
	else
	{
		tstring sPrompt = sFile + TEXT("\r\n") + szPromptIfNotExist; 
		MsgBox(sPrompt, TEXT("文件不存在"),mb_OK,mb_IconExclamation);
		return false;
	}
}

//校验LibSVM可执行文件是否存在,存在返回true,不存在弹出提示信息并返回false
bool VerifyLibSVMFiles()
{
	//用pApp->Path获得本程序运行所在文件夹
	tstring sAppPath = FMAddBackSlash(pApp->Path());
	if (!TestFileExist(sAppPath + mc_szScaleExeFile,TEXT("缺少归一化可执行文件，请将svm-scale.exe拷到本程序运行文件夹中!")))
		return false;
	if (!TestFileExist(sAppPath + mc_szTrainExeFile,TEXT("缺少SVM训练可执行文件，请将svm-train.exe拷到本程序运行文件夹中!")))
		return false;
	if (!TestFileExist(sAppPath + mc_szTestExeFile,TEXT("缺少SVM测试可执行文件，请将svm-predict.exe拷到本程序运行文件夹中!")))
		return false;

	return true;
}

//在szPath目录下创建一个批处理文件，文件名为szBatFileName，向其中写入命令行sCmdLine
//成功且批处理运行完返回true,无法创建批处理文件返回false
bool RunBat (TCHAR * szPath, TCHAR * szBatFileName, tstring &sCmdLine)
{
	//批处理文件名含全路径
	tstring sBatFile;
	sBatFile = FMAddBackSlash(szPath) ;
	sBatFile = sBatFile + szBatFileName;
	
	HANDLE hFileBat = EFOpen(sBatFile.c_str(),EF_OpStyle_Output);
	if (hFileBat==INVALID_HANDLE_VALUE) return false;
	EFPrint(hFileBat,sCmdLine.c_str());
	EFClose(hFileBat);

	SERunWaitTerminate(sBatFile.c_str(),true);
	//删除批处理文件
	FMDeleteFiles(sBatFile.c_str(),true,true,false);
	return true;
}

bool SVMScaleData(  TCHAR * szFileData,
					TCHAR * szFileDataScaleParam,
					bool fParamReadOrWrite,
					tstring &sRetString)
{
	//准备SVM可执行文件名(含全路径)
	tstring sAppPath = FMAddBackSlash(pApp->Path());
	tstring sSVMExeScale = sAppPath + mc_szScaleExeFile;

	TCHAR * szPath = NULL,*szExp = NULL; // szDataFile中的路径部分、扩展名部分
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

		//如果归一化参数文件szFileDataScaleParam与此文件名相同，改名(加.data)
		if (_tcscmp (sFileDataScale.c_str(),szFileDataScaleParam)==0)
		{
			sFileDataScale = szPath;
			sFileDataScale += TEXT("\\");
			sFileDataScale += szFileNameOnly;
			sFileDataScale += TEXT(".scale.data.");
			sFileDataScale += szExp;
		}
		//如果sFileDataScale文件已存在，删除此文件
		FMDeleteFiles (sFileDataScale.c_str(), true, true, false);
		//删除后检查，如果文件仍然存在，说明未删除成功
		if (FMFileExist (sFileDataScale.c_str())==1)
		{
			sRetString = TEXT("不能删除之前残留的归一化数据结果文件:")	+ sFileDataScale;//错误说明
			return false;
		}
		//准备归一化参数保存到的文件
		if (! fParamReadOrWrite)
		{
			//重新写新的参数文件
			//如果szFileDataScaleParam已存在，删除此文件
			FMDeleteFiles(szFileDataScaleParam, true, true,false);
		
			//删除后检查，如果文件仍然存在，说明未删除成功
			if (FMFileExist(szFileDataScaleParam)==1)
			{
				sRetString = TEXT("不能删除以前残留的归一化参数文件: ");
				sRetString = sRetString + szFileDataScaleParam; // 错误说明
				return false;
			}

			//生成归一化的命令行
			sCmdLine = TEXT("\"") + sSVMExeScale + TEXT("\"  -s  ");
		}
		else
		{
			//读已有的参数文件
			sCmdLine = TEXT("\"") + sSVMExeScale + TEXT("\"  -r  ");
		}

		//继续生成归一化的命令行
		sCmdLine = sCmdLine + TEXT("\"") + szFileDataScaleParam + TEXT("\"  ");
		sCmdLine = sCmdLine + TEXT("\"") + szFileData + TEXT("\"  ");
		sCmdLine = sCmdLine + TEXT("> \"") + sFileDataScale + TEXT("\"");

		//通过批处理文件的方式运行此命令行，并等待运行结束
		if ( ! RunBat (szPath, TEXT("svmscale.bat"),sCmdLine))
		{
			sRetString = TEXT("数据归一化失败!无法在文件夹");
			sRetString = sRetString + szPath + TEXT("中创建或运行批处理文件: svmscale.bat。");
			return false;
		}

		//检查结果文件sFileDataScale 是否已生成
		if ( FMFileExist( sFileDataScale.c_str()) != 1 )
		{
			sRetString = TEXT("数据归一化失败!未能生成归一化后的结果数据文件:") + sFileDataScale;
			return false; 
		}
	}
	else
	{
		//不归一化，直接设置sFileDataScale为原数据文件
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
	//准备SVM可执行文件名(含全路径)
	tstring sAppPath = FMAddBackSlash (pApp->Path());
	tstring sSVMExeTrain = sAppPath + mc_szTrainExeFile;

	//检查各文件不能相同
	if (szFileData && szFileModel && _tcscmp(szFileData,szFileModel)==0)
	{
		sRetString = TEXT("特征数据文件和要生成的model文件不能是同一文件。");
		return false;
	}
	if (szFileData && szFileDataScaleParam && _tcscmp(szFileData, szFileDataScaleParam)==0)
	{
		sRetString = TEXT("特征数据文件和归一化参数文件不能是同一文件。");
		return false;
	}
	if (szFileModel && szFileDataScaleParam && _tcscmp(szFileModel, szFileDataScaleParam)==0)
	{
		sRetString = TEXT("要生成的model文件和归一化参数文件不能是同一文件。");
		return false;
	}

	//准备文件名
	TCHAR * szPath = NULL, *szExp = NULL; //szDataFile中的路径部分、扩展名部分
	TCHAR * szFileNameOnly = FMTrimFileName (szFileData, true,	true, &szPath, &szExp);
	tstring sFileDataScale, sCmdLine;
	//归一化数据，如成功，sFileDataScale 中保存了归一化后的结果数据文件名
	if ( ! SVMScaleData (szFileData, szFileDataScaleParam, false, sFileDataScale))
	{
		sRetString = sFileDataScale;	//如失败，sFileDataScale为出错信息
		return false;
	}

	//调用SVM训练程序训练模型
	sCmdLine = TEXT("\"") + sSVMExeTrain + TEXT("\"  ");
	sCmdLine = sCmdLine + TEXT(" -t ") + Str(iKernelFunc);

	if (szFileModel==NULL)
		sCmdLine = sCmdLine + TEXT(" -v ") + Str(iFold);
	//在命令行中加其他参数
	sCmdLine = sCmdLine + TEXT(" ") + szOtherParams + TEXT("  ");
	//在命令行中加数据文件文件名: sFileDataScale，
	sCmdLine = sCmdLine + TEXT("  \"") + sFileDataScale + TEXT("\"  ");
	//若生成model文件，在命令行中加model文件的文件名
	if (szFileModel!= NULL)
		sCmdLine = sCmdLine + TEXT("  \"") + szFileModel + TEXT("\"");

	//通过批处理文件的方式，运行此命令行，并等待运行结束
	if ( ! RunBat (szPath, TEXT("svntrain.bat"),sCmdLine))
	{
		sRetString = TEXT("SVM训练失败! 无法在文件夹 ");
		sRetString = sRetString + szPath + TEXT("中创建或运行批处理文件: svmtrain.bat。");
		return false;
	}

	//显示结果
	tstring sFileResult;
	if (szFileModel==NULL)
	{
		//不生成model文件
		sFileResult = sFileDataScale + TEXT("(t=") + Str (iKernelFunc) + TEXT( ").training_result" );
		if ( FMFileExist( sFileResult.c_str()) !=1 )
			{ 
				sRetString = TEXT("SVM训练失败! 未能生成训练结果文件:")	+ sFileResult;
				return false;
			}
			//成功:结果文件已生成，由sRetString返回结果文件名
			sRetString = sFileResult;
	}
	else
	{
		//检查model文件是否已生成
		if ( FMFileExist( szFileModel ) != 1 )
		{ 
			sRetString = TEXT(" model文件生成失败!未能生成model文件: ");
			sRetString = sRetString + szFileModel;
			return false;
		}
		//成功: model 文件已生成，由sRetString 返回model文件名
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
	//准备SVM可执行文件名
	tstring sAppPath = FMAddBackSlash (pApp->Path());
	tstring sSVMExePredict = sAppPath + mc_szTestExeFile;

	//检查各文件不能相同
	if (szFileData && szFileModel && _tcscmp(szFileData, szFileModel)==0)
	{ 
		sRetString = TEXT("特征数据文件和model文件不能是同一文件。");
		return false;
	}
	if (szFileData && szFileDataScaleParam && _tcscmp(szFileData,szFileDataScaleParam)==0)
	{ 
		sRetString = TEXT("特征数据文件和归一化参数文件不能是同一文件。");
		return false;
	}
	if (szFileData && szFileResult && _tcscmp(szFileData,szFileResult)==0)
	{ 
		sRetString = TEXT("特征数据文件和结果文件不能是同一文件。");
		return false;
	}
	if (szFileModel && szFileDataScaleParam && _tcscmp(szFileModel,szFileDataScaleParam)==0)
	{ 
		sRetString = TEXT("model文件和归一化参数文件不能是同一文件。");
		return false;
	}
	if (szFileModel && szFileResult && _tcscmp(szFileModel,szFileResult)==0)
	{ 
		sRetString = TEXT("model文件和结果文件不能是同一文件。");
		return false;
	}
	if (szFileDataScaleParam && szFileResult && _tcscmp(szFileDataScaleParam,szFileResult)==0)
	{ 
		sRetString = TEXT("归一化参数文件和结果文件不能是同一文件。");
		return false;
	}

	//准备文件名
	TCHAR *szPath = NULL, *szExp = NULL; //szDataFile中的路径部分、扩展名部分
	TCHAR *szFileNameOnly = FMTrimFileName (szFileData,true,true, &szPath, &szExp);

	tstring sFileDataScale, sCmdLine;
	//归一化数据，如成功，sFileDataScale中保存了归一化后的结果数据文件名
	if ( ! SVMScaleData(szFileData, szFileDataScaleParam, true, sFileDataScale) )
	{
		sRetString = sFileDataScale;		//失败时sFileDataScale中为出错信息
		return false;
	}

	//调用SVM测试程序测试模型
	sCmdLine = TEXT("\"") + sSVMExePredict + TEXT("\"  ");
	if (iParamB>0)
	{
		sCmdLine = sCmdLine + TEXT(" -b ") + Str (iParamB);
	}

	//在命令行中加数据文件文件名:sFileDataScale
	sCmdLine = sCmdLine + TEXT("  \"") + sFileDataScale + TEXT("\"  ");		//在命令行中加model文件的文件名
	sCmdLine = sCmdLine + TEXT("  \"") + szFileModel + TEXT("\"");			//在命令行中加结果文件的文件名
	sCmdLine = sCmdLine + TEXT("  \"") + szFileResult + TEXT("\"");

	//通过批处理文件的方式运行此命令行，并等待运行结束
	if ( ! RunBat (szPath,TEXT("svmpredict.bat"),sCmdLine))
		{ 
			sRetString = TEXT("SVM测试失败!无法在文件夹");
			sRetString = sRetString + szPath + TEXT(" 中创建或运行批处理文件: svmpredict.bat。");
			return false;
		}
	//显示结果
	//检查结果文件是否已生成
	if ( FMFileExist( szFileResult ) != 1 )
	{ 
		sRetString = TEXT ("SVM测试失败!未能生成结果文件: ");
		sRetString = sRetString + szFileResult;
		return false;
	}

	//成功:结果文件已生成，由sRetString返回结果文件名
	sRetString = szFileResult;
	return true;

}