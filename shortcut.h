#ifndef SHORTCUT_H_
#define SHORTCUT_H_

namespace Shortcut
{
	//******************************************************************************
	// �� �� ��  : GetDesktopPath
	// ��������  : �õ���ǰ������·��
	// �������  : desktopPath - ���ص�ǰ������·��
	// �� �� ֵ  : true - �ɹ�
	//             false - ʧ��
	//******************************************************************************
	BOOL GetDesktopPath(char *desktopPath);

	//******************************************************************************
	// �� �� ��  : GetIEQuickLaunchPath
	// ��������  : �õ�������������·��
	// �������  : iEQueickLaunchPath - �õ�������������·��
	// �� �� ֵ  : true - �ɹ�
	//             false - ʧ��
	//******************************************************************************
	BOOL GetIEQuickLaunchPath(char *iEQueickLaunchPath);

	//******************************************************************************
	// �� �� ��  : GetProgramsPath
	// ��������  : �õ� ��ʼ->������ ��·��
	// �������  : programsPath - �õ� ��ʼ->������ ��·��
	// �� �� ֵ  : true - �ɹ�
	//             false - ʧ��
	//******************************************************************************
	BOOL GetProgramsPath(char *programsPath);

	//******************************************************************************
	// ��������  : ��ָ���ļ���ָ����Ŀ¼�´������ݷ�ʽ
	// �������  : fileName - ָ���ļ���ΪNULL��ʾ��ǰ���̵�EXE�ļ���
	//             lnkFileDir - ָ��Ŀ¼������ΪNULL��
	//             lnkFileName - ��ݷ�ʽ���ƣ�ΪNULL��ʾEXE�ļ�����
	//             wHotkey - Ϊ0��ʾ�����ÿ�ݼ�
	//             description - ��ע
	//             iShowCmd - ���з�ʽ��Ĭ��Ϊ���洰��
	// ��    ע : ʹ�ô˺���ǰ�������ȳ�ʼ��com
	//******************************************************************************
	BOOL CreateFileShortcut(char *fileName, 
		                    char *lnkFileDir, 
		                    char *lnkFileName,	
		                    char *workDir, 
		                    WORD wHotkey, 
		                    char *description, 
		                    int iShowCmd = SW_SHOWNORMAL);
	//******************************************************************************
	// �� �� ��  : AutoCreateCurrentFileShortcut
	// ��������  : �Զ�ע�᱾����Ŀ�ݷ�ʽ
	// �� �� ֵ  : true - �ɹ�
	//             false - ʧ��
	//******************************************************************************
	BOOL AutoCreateCurrentFileShortcut();
}

#endif//SHORTCUT_H_