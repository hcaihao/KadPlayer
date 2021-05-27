#pragma once

#define WIN32_LEAN_AND_MEAN

#include <QtWidgets>

#include "common.h"

#include <atlbase.h>
#include <atlcom.h>
#include "thunderagent.tlh"
using namespace ThunderAgentLib;

void RefreshStyle(QWidget *widget, const char* name, QVariant value);
void LoadStyle(QWidget *widget, QString filePath);
void LoadStyle(QString filePath);
QString GetHexString(const BYTE *pBuffer, int len);
QString GetFileSize(unsigned __int64 size);
QString GetFileType(const char *name);
QString GetFileFormat(const char *name);
QString GetFileName(QString link);
QString GetFileSize(QString link);
QString GetHash(QString link);
QString GetEd2k(const SRESULT &sResult);

extern ThunderAgentLib::IAgent *g_pThunder;
extern QString g_skinName;
extern QHash<QString, QString> g_attributeHash;
extern QStringList g_searchKeyList;
extern bool g_isHideWindow;
extern bool g_isManualUpdate;
extern bool g_isKadConnected;

extern int g_playChannel;
extern bool g_isSaveHistory;
extern bool g_isAutoStart;
extern bool g_isFirstRun;
extern int g_doubleClick;
extern QStringList g_historyMovieList;
extern QStringList g_favoriteMovieList;