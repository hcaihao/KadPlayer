#ifndef SHORTCUT_H_
#define SHORTCUT_H_

namespace Shortcut
{
	//******************************************************************************
	// 函 数 名  : GetDesktopPath
	// 功能描述  : 得到当前的桌面路径
	// 输出参数  : desktopPath - 返回当前的桌面路径
	// 返 回 值  : true - 成功
	//             false - 失败
	//******************************************************************************
	BOOL GetDesktopPath(char *desktopPath);

	//******************************************************************************
	// 函 数 名  : GetIEQuickLaunchPath
	// 功能描述  : 得到快速启动栏的路径
	// 输出参数  : iEQueickLaunchPath - 得到快速启动栏的路径
	// 返 回 值  : true - 成功
	//             false - 失败
	//******************************************************************************
	BOOL GetIEQuickLaunchPath(char *iEQueickLaunchPath);

	//******************************************************************************
	// 函 数 名  : GetProgramsPath
	// 功能描述  : 得到 开始->程序组 的路径
	// 输出参数  : programsPath - 得到 开始->程序组 的路径
	// 返 回 值  : true - 成功
	//             false - 失败
	//******************************************************************************
	BOOL GetProgramsPath(char *programsPath);

	//******************************************************************************
	// 函数功能  : 对指定文件在指定的目录下创建其快捷方式
	// 输入参数  : fileName - 指定文件，为NULL表示当前进程的EXE文件。
	//             lnkFileDir - 指定目录，不能为NULL。
	//             lnkFileName - 快捷方式名称，为NULL表示EXE文件名。
	//             wHotkey - 为0表示不设置快捷键
	//             description - 备注
	//             iShowCmd - 运行方式，默认为常规窗口
	// 备    注 : 使用此函数前必须首先初始化com
	//******************************************************************************
	BOOL CreateFileShortcut(char *fileName, 
		                    char *lnkFileDir, 
		                    char *lnkFileName,	
		                    char *workDir, 
		                    WORD wHotkey, 
		                    char *description, 
		                    int iShowCmd = SW_SHOWNORMAL);
	//******************************************************************************
	// 函 数 名  : AutoCreateCurrentFileShortcut
	// 功能描述  : 自动注册本程序的快捷方式
	// 返 回 值  : true - 成功
	//             false - 失败
	//******************************************************************************
	BOOL AutoCreateCurrentFileShortcut();
}

#endif//SHORTCUT_H_