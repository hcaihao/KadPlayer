#include "stdafx.h"
#include "mainwindow.h"

#include <QtWidgets/QApplication>

#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "RpcRT4.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "winspool.lib")
//#pragma comment(lib, "shlwapi.lib")
//#pragma comment(lib, "comdlg32.lib")
//#pragma comment(lib, "oleaut32.lib")
//#pragma comment(lib, "glu32.lib")
//#pragma comment(lib, "kernel32.lib")
//#pragma comment(lib, "shell32.lib")
//#pragma comment(lib, "uuid.lib")
//#pragma comment(lib, "ole32.lib")
//#pragma comment(lib, "user32.lib")
//#pragma comment(lib, "advapi32.lib")
//#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "crypt32.lib")

ThunderAgentLib::IAgent *g_pThunder = NULL;
QString g_skinName;
QHash<QString, QString> g_attributeHash;
QStringList g_searchKeyList;
bool g_isHideWindow = false;
bool g_isManualUpdate = false;
bool g_isKadConnected = false;

int g_playChannel = 0;
bool g_isSaveHistory = false;
bool g_isAutoStart = false;
bool g_isFirstRun = false;
int g_doubleClick = 0;
QStringList g_historyMovieList;
QStringList g_favoriteMovieList;

void ProcessArgs(int argc, char *argv[])
{
	for(int i=0; i<argc; i++)
	{
		QString arg = QString::fromLocal8Bit(argv[i]);

		if(arg == "-auto")
		{
			g_isHideWindow = true;
		}
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ProcessArgs(argc, argv);
	::srand(::GetTickCount());

    int ret = 0;

    QSharedMemory sharedMemory(APP_TITLE); 
    if (sharedMemory.create(1) && sharedMemory.error() != QSharedMemory::AlreadyExists)
    {
		MainWindow w;
		if(!g_isHideWindow)
		{
			w.show();
		}
        ret = a.exec();
    }
    else  
    {
		::MessageBoxA(0, "The program is already running", "Notice", 0);
    }

	return ret;
}
