//自动运行svm程序
#include"BWindows.h"

//通用模块函数声明

// 测试一个文件是否存在，若存在返回true，如不存在返回false并给出提示szPromptIfNotExist
bool TestFileExist(tstring &sFile, TCHAR * szPromptIfNotExist);


//校验LibSVM可执行文件是否存在,存在返回true,不存在弹出提示信息并返回false
bool VerifyLibSVMFiles();

//批处理
bool RunBat(TCHAR * szPath,TCHAR * szBatFileName, tstring &sCmdLine);

//归一化数据文件
bool SVMScaleData(  TCHAR * szFileData,
					TCHAR * szFileDataScaleParam,
					bool fParamReadOrWrite,
					tstring &sRetString);
//运行Svm训练
bool SVMTrain ( TCHAR * szFileData, 
				TCHAR * szFileModel, 
				TCHAR * szFileDataScaleParam,
				int iKernelFunc,
				int iFold,
				TCHAR * szOtherParams,
				tstring &sRetString);
//运行Svm测试
bool SVMPredict (   TCHAR * szFileData,
					TCHAR * szFileModel,
					TCHAR * szFileDataScaleParam,
					TCHAR * szFileResult, 
					int iParamB,
					tstring &sRetString);