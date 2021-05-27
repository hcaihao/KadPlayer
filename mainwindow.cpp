#include "stdafx.h"
#include "mainwindow.h"
#include "titlebar.h"
#include "content.h"
#include "statusbar.h"
#include "skinwindow.h"
#include "miniportalwindow.h"
#include "settingwindow.h"
#include "systemtray.h"
#include "resulttabwidget.h"
#include "resulttreewidget.h"
#include "utilitymanager.h"
#include "servicemanager.h"
#include "webaxwidget.h"
#include "playwindow.h"
#include "feedbackwindow.h"
#include "messagewindow.h"
#include "cloudwindow.h"
#include "shortcut.h"
#include "playlistwidget.h"

ServiceManager *g_pServiceManager = NULL;

MainWindow *g_pMainWindow = NULL;

#ifdef LOCAL_KAD_LIB
#ifdef NDEBUG
#pragma comment(lib, "../Release/KadLibrary.lib")
#else
#pragma comment(lib, "../Debug/KadLibrary.lib")
#endif
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "WinInet.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "gdiplus.lib")

KadLibrary g_kadLibrary;
#else
Init pInit = NULL;
Uninit pUninit = NULL;
DeleteFileBasicInfoTable pDeleteFileBasicInfoTable = NULL;
GetEmuleStatus pGetEmuleStatus = NULL;
GenerateEd2kLink pGenerateEd2kLink = NULL;
StopKadSearch pStopKadSearch = NULL;
GetKadSearchResult pGetKadSearchResult = NULL;
StartServerSearch pStartServerSearch = NULL;
StopServerSearch pStopServerSearch = NULL;
GetServerSearchResult pGetServerSearchResult = NULL;
StartKadSearch pStartKadSearch = NULL;
#endif

#include "Proxy/include/SP.h"
#ifdef NDEBUG
#pragma comment(lib, "Proxy/lib/SP_r.lib")
#else
#pragma comment(lib, "Proxy/lib/SP_d.lib")
#endif


void KadSearchResult(unsigned int searchId, const SEARCHRESULT &result)
{
	g_pMainWindow->InsertResult(searchId, result);
}

MainWindow::MainWindow(QFrame *parent)
    : m_pSkinWindow(NULL), m_pMiniPortalWindow(NULL), m_pSettingWindow(NULL), m_pCloudWindow(NULL), m_pFeedbackWindow(NULL), BaseWindow(parent)
{
	g_pMainWindow = this;
	//Qt::WindowSystemMenuHint允许任务栏按钮右键菜单（会导致WM_NCHITTEST事件接收不到），Qt::WindowMinimizeButtonHint允许状态栏最小化与还原
	setWindowFlags(/*Qt::WindowSystemMenuHint | */Qt::WindowMinimizeButtonHint | BaseWindow::windowFlags());

	setObjectName("mainWindow");

    setMinimumSize(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);

	// Icon
	QFileInfo fileInfo(QApplication::applicationFilePath());
	QFileIconProvider fileIcon;
	QIcon icon = fileIcon.icon(fileInfo);
	setWindowIcon(icon);

	// Language
	QLocale locale;
	QTranslator *translator = new QTranslator();
	translator->load(locale.name(), QString("%1/translations").arg(QApplication::applicationDirPath()));
	QApplication::installTranslator(translator);

	// Init
	LoadIniData();
	InitControl();
	InitKadLibrary();
	InitThunderLibrary();

	// Service
	g_pServiceManager = new ServiceManager(NULL);	// The object cannot be moved if it has a parent. Event processing will continue in the targetThread.
	connect(g_pServiceManager, SIGNAL(getSystemMessage(QString)), this, SLOT(on_getSystemMessage_emited(QString)));
	connect(g_pServiceManager, SIGNAL(getSensitiveWords(QStringList)), this, SLOT(on_getSensitiveWords_emited(QStringList)));
	connect(g_pServiceManager, SIGNAL(getHotEd2ks(QStringList)), this, SLOT(on_getHotEd2ks_emited(QStringList)));
	connect(g_pServiceManager, SIGNAL(getHotWords(QStringList)), this, SLOT(on_getHotWords_emited(QStringList)));
	connect(g_pServiceManager, SIGNAL(getRandomWord(QString)), this, SLOT(on_getRandomWord_emited(QString)));
	connect(g_pServiceManager, SIGNAL(setSearchLog(QString)), this, SLOT(on_setSearchLog_emited(QString)));
	connect(g_pServiceManager, SIGNAL(setFeedback(QString, int, QString)), this, SLOT(on_setFeedback_emited(QString, int, QString)));
	connect(g_pServiceManager, SIGNAL(setPlayLog(QString)), this, SLOT(on_setPlayLog_emited(QString)));
	
	
	g_pServiceManager->moveToThread(&m_serviceThread);
	m_serviceThread.start();

	QMetaObject::invokeMethod(g_pServiceManager, "GetSystemMessage", Qt::QueuedConnection, Q_ARG(QString, APP_VERSION_STR));
	QMetaObject::invokeMethod(g_pServiceManager, "GetSensitiveWords", Qt::QueuedConnection);
	QMetaObject::invokeMethod(g_pServiceManager, "GetHotEd2ks", Qt::QueuedConnection);
	QMetaObject::invokeMethod(g_pServiceManager, "GetHotWords", Qt::QueuedConnection);

	m_pTimer = new QTimer();
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(checkUpdate()));
	m_pTimer->start(ONE_HOUR);
	QTimer::singleShot(0, this, SLOT(checkUpdate()));

	// First run
	if (g_isFirstRun)
	{
		Shortcut::AutoCreateCurrentFileShortcut();

		//UtilityManager::ModifyChromeBorwserHome("http://www.sina.com");
	}
	
	// Auto run
	if(UtilityManager::IsAutoRun(APP_TITLE) != g_isAutoStart)
	{
		UtilityManager::SetAsAutoRun(APP_TITLE, g_isAutoStart);
	}

	// Style
	LoadStyle(QString("%1.qss").arg(g_skinName));

	bool result = SP_Start("kad", "p.5hex.com", 10018);
	
#ifdef LOCAL_KAD_LIB
	g_kadLibrary.Init(KadSearchResult);
#else
	pInit(0);
#endif
}

MainWindow::~MainWindow()
{
	SaveIniData();

	SP_Stop();

#ifdef LOCAL_KAD_LIB
	g_kadLibrary.Close();
#else
	delete m_pResultTabWidget;
	pUninit();
#endif
}

void MainWindow::paintEvent(QPaintEvent *event)
{
	//先显示阴影
	BaseWindow::paintEvent(event);

	//再显示背景
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawPixmap(QRect(SHADOW_WIDTH, SHADOW_WIDTH, width()-2*SHADOW_WIDTH, height()-2*SHADOW_WIDTH), QPixmap(g_skinName));
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	MSG* msg = reinterpret_cast<MSG*>(message);

	switch(msg->message)
	{
	case UPDATE_EXIST:
		break;
	case UPDATE_NO_EXIST:
		{
			if(g_isManualUpdate)
			{
				MessageWindow::GetInstance().ShowMessage(tr("Notice"), tr("Version: %1 is up to date.").arg(APP_VERSION_STR));
			}
		}
		break;
	case UPDATE_DOWNLOAD_SUCCESS:
		qApp->exit();
		break;
	case UPDATE_DOWNLOAD_FAIL:
		break;
	}

	return BaseWindow::nativeEvent(eventType, message, result);
}

void MainWindow::moveEvent(QMoveEvent * event)
{
	Q_UNUSED(event);

#ifdef TODAY_RESOURCE
	QPoint point = pos();
	point.setY(point.y() + m_pResultTabWidget->mapToParent(QPoint(0, 0)).y() + m_pTitleBar->height() + SHADOW_WIDTH);
	point.setX(point.x() + SHADOW_WIDTH);

	m_pWebAxWidget->move(point);
#endif
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);

	m_pPlayListToolButton->move(m_pPlayListToolButton->isChecked() ? m_pPlayListWidget->width() : 0, m_pResultTabWidget->height() / 2 - m_pPlayListToolButton->height() / 2);

#ifdef TODAY_RESOURCE
	QSize size = this->size();
	size.setHeight(m_pResultTabWidget->height());
	size.setWidth(width() - SHADOW_WIDTH*2);

	m_pWebAxWidget->resize(size);
#endif
}

void MainWindow::showEvent(QShowEvent *event)
{
	Q_UNUSED(event);

	m_pPlayListToolButton->move(m_pPlayListToolButton->isChecked() ? m_pPlayListWidget->width() : 0, m_pResultTabWidget->height() / 2 - m_pPlayListToolButton->height() / 2);

#ifdef TODAY_RESOURCE
	m_pWebAxWidget->show();
#endif
}

void MainWindow::hideEvent(QHideEvent *event)
{
	Q_UNUSED(event);

#ifdef TODAY_RESOURCE
	m_pWebAxWidget->hide();
#endif
}

void MainWindow::InitControl()
{
	m_pSystemTray = new SystemTray(this);
	m_pSystemTray->setObjectName("systemTray");
	connect(m_pSystemTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(on_systemTray_activated(QSystemTrayIcon::ActivationReason)));
	connect(m_pSystemTray, SIGNAL(showCloudWindow()), this, SLOT(showCloudWindow()));
	connect(m_pSystemTray, SIGNAL(showWidget()), this, SLOT(showWidget()));
	connect(m_pSystemTray, SIGNAL(closePlayWindow()), this, SLOT(closePlayWindow()));
	m_pSystemTray->show();

	//创建标题栏
	m_pTitleBar = new TitleBar(this);
	connect(m_pTitleBar, SIGNAL(showSkinWindow()), this, SLOT(showSkinWindow()));
	connect(m_pTitleBar, SIGNAL(showMiniPortalWindow()), this, SLOT(showMiniPortalWindow()));
	connect(m_pTitleBar, SIGNAL(showSettingWindow()), this, SLOT(showSettingWindow()));
	connect(m_pTitleBar, SIGNAL(showCloudWindow()), this, SLOT(showCloudWindow()));
	connect(m_pTitleBar, SIGNAL(showFeedbackWindow(int, QString)), this, SLOT(showFeedbackWindow(int, QString)));
	connect(m_pTitleBar, SIGNAL(showUpdate()), this, SLOT(showUpdate()));
	connect(m_pTitleBar, SIGNAL(showHelp()), this, SLOT(showHelp()));
	connect(m_pTitleBar, SIGNAL(showAbout()), this, SLOT(showAbout()));
	//创建内容区域
	m_pContent = new Content(this);
	m_pContent->setObjectName("");
	//创建状态栏
	m_pStatusBar = new StatusBar(this);
	connect(m_pStatusBar, SIGNAL(searchKey(QString)), this, SLOT(on_searchKey_emited(QString)));

	//创建布局
	QVBoxLayout *layout = new QVBoxLayout();
	//设置间距与边缘空白
	layout->setSpacing(0);	//设置0界面不显示？
	layout->setContentsMargins(SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH);
	//将部件加入到布局中
	layout->addWidget(m_pTitleBar);
	layout->addWidget(m_pContent);
	layout->addWidget(m_pStatusBar);
	setLayout(layout);

#ifdef TODAY_RESOURCE
	//初始化今日资源
	m_pWebAxWidget = new WebAxWidget(this);
	m_pWebAxWidget->setObjectName("webAxWidget");
	m_pWebAxWidget->setProtocolName("SearchKey");
	m_pWebAxWidget->dynamicCall("Navigate(const QString&)", STARTUP_URL);
	m_pWebAxWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::Popup);	//Qt::Tool不显示任务栏图标，需设置MiniPortalWindow的parent
	connect(&m_pWebAxWidget->m_signaler, SIGNAL(searchKey(QString)), this, SLOT(on_searchKey_emited(QString)));
#endif

	//初始化收藏夹
	m_pPlayListWidget = new PlayListWidget();
	m_pPlayListWidget->setObjectName("playListWidget");
	m_pPlayListWidget->hide();

	m_pPlayListToolButton = new QToolButton();
	m_pPlayListToolButton->setObjectName("playListToolButton");
	m_pPlayListToolButton->setCursor(Qt::PointingHandCursor);
	m_pPlayListToolButton->setCheckable(true);
	m_pPlayListToolButton->setChecked(false);
	connect(m_pPlayListToolButton, SIGNAL(clicked(bool)), this, SLOT(on_playListToolButton_clicked(bool)));
#ifdef KAD_SEARCHER
	m_pPlayListToolButton->hide();
#endif

	//初始化搜索区域
	m_pSearchComboBox = new QComboBox();
	m_pSearchComboBox->setEditable(true);
	m_pSearchComboBox->setObjectName("searchComboBox");
	m_pSearchComboBox->lineEdit()->setPlaceholderText(tr("Input search keyword"));
	connect(m_pSearchComboBox->lineEdit(), SIGNAL(returnPressed()), this, SLOT(on_searchComboBox_returnPressed()));

	foreach(QString key, g_searchKeyList)
	{
		m_pSearchComboBox->addItem(key);
	}

	m_pSearchPushButton = new QPushButton();
	m_pSearchPushButton->setObjectName("searchPushButton");
	m_pSearchPushButton->setText(tr("Search"));
	m_pSearchPushButton->setFocusPolicy(Qt::NoFocus);
	connect(m_pSearchPushButton, SIGNAL(clicked()), this, SLOT(on_searchPushButton_clicked()));

	m_pRandomPushButton = new QPushButton();
	m_pRandomPushButton->setObjectName("randomPushButton");
	m_pRandomPushButton->setText(tr("Lucky"));
	m_pRandomPushButton->setFocusPolicy(Qt::NoFocus);
	connect(m_pRandomPushButton, SIGNAL(clicked()), this, SLOT(on_randomPushButton_clicked()));

	QHBoxLayout *searchLayout = new QHBoxLayout();
	searchLayout->setSpacing(10);
	searchLayout->setContentsMargins(10, 5, 10, 5);
	searchLayout->addWidget(m_pSearchComboBox);
	searchLayout->addWidget(m_pSearchPushButton);
	searchLayout->addWidget(m_pRandomPushButton);

	QRadioButton *allRadioButton = new QRadioButton();
	allRadioButton->setText(tr("All"));
	allRadioButton->setChecked(true);

	QRadioButton *movieRadioButton = new QRadioButton();
	movieRadioButton->setText(tr("Movie"));

	QRadioButton *musicRadioButton = new QRadioButton();
	musicRadioButton->setText(tr("Music"));
	
	QRadioButton *fileRadioButton = new QRadioButton();
	fileRadioButton->setText(tr("File"));
	
	QRadioButton *otherRadioButton = new QRadioButton();
	otherRadioButton->setText(tr("Other"));

	QButtonGroup *typeButtonGroup = new QButtonGroup(this);
	typeButtonGroup->addButton(allRadioButton);
	typeButtonGroup->addButton(movieRadioButton);
	typeButtonGroup->addButton(musicRadioButton);
	typeButtonGroup->addButton(fileRadioButton);
	typeButtonGroup->addButton(otherRadioButton);
	connect(typeButtonGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(on_typeButtonGroup_buttonClicked(QAbstractButton *)));

	m_pFilterLabel = new QLabel();
	m_pFilterLabel->setText(tr("Name filter:"));

	m_pFilterLineEdit = new QLineEdit();
	m_pFilterLineEdit->setObjectName("filterLineEdit");
	connect(m_pFilterLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(on_filterLineEdit_textChanged(const QString&)));

	QHBoxLayout *filterLayout = new QHBoxLayout();
	filterLayout->setSpacing(10);
	filterLayout->setContentsMargins(10, 5, 10, 5);
	filterLayout->addWidget(allRadioButton);
	filterLayout->addWidget(movieRadioButton);
	filterLayout->addWidget(musicRadioButton);
	filterLayout->addWidget(fileRadioButton);
	filterLayout->addWidget(otherRadioButton);
	filterLayout->addStretch();
	filterLayout->addWidget(m_pFilterLabel);
	filterLayout->addWidget(m_pFilterLineEdit);

	m_pResultTabWidget = new ResultTabWidget(this);
	m_pResultTabWidget->setObjectName("resultTabWidget");
	m_pPlayListToolButton->setParent(m_pResultTabWidget);

	QStackedLayout *resultLayout = new QStackedLayout();
	resultLayout->setStackingMode(QStackedLayout::StackAll);
	resultLayout->setSpacing(0);
	resultLayout->setContentsMargins(0, 0, 0, 0);
	resultLayout->addWidget(m_pResultTabWidget);
	resultLayout->addWidget(m_pPlayListWidget);
	//resultLayout->addWidget(m_pPlayListToolButton);

	QVBoxLayout *contentLayout = new QVBoxLayout();
	contentLayout->setSpacing(10);
	contentLayout->setContentsMargins(0, 0, 0, 0);
	contentLayout->addLayout(searchLayout);
	contentLayout->addLayout(filterLayout);
	contentLayout->addLayout(resultLayout);
	m_pContent->setLayout(contentLayout);
}

void MainWindow::InitThunderLibrary()
{
	CLSID CLSID;
	HRESULT hResult = CLSIDFromProgID(L"ThunderAgent.Agent", &CLSID);
	if(hResult == S_OK)
	{
		CComPtr<ThunderAgentLib::IAgent> thunderAgentLib;
		HRESULT hResult = thunderAgentLib.CoCreateInstance(CLSID);
		if(hResult == S_OK)
		{
			hResult = thunderAgentLib.QueryInterface(&g_pThunder);
			if(hResult == S_OK)
			{

			}
		}
	}
}

void MainWindow::InitKadLibrary()
{
#ifdef LOCAL_KAD_LIB

#else
	QString dllPath = QString("%1\\dispatch.dll").arg(QApplication::applicationDirPath());
	HMODULE hModule = ::LoadLibraryW(dllPath.toStdWString().data());
	if(hModule)
	{
		pInit = (Init)GetProcAddress(hModule, "init");
		pUninit = (Uninit)GetProcAddress(hModule, "uninit");
		pDeleteFileBasicInfoTable = (DeleteFileBasicInfoTable)GetProcAddress(hModule, "delete_file_basic_info_table");
		pGetEmuleStatus = (GetEmuleStatus)GetProcAddress(hModule, "get_emule_status");
		pGenerateEd2kLink = (GenerateEd2kLink)GetProcAddress(hModule, "generate_ed2k_link");
		pStopKadSearch = (StopKadSearch)GetProcAddress(hModule, "stop_kad_search");
		pGetKadSearchResult = (GetKadSearchResult)GetProcAddress(hModule, "get_kad_search_result");
		pStartServerSearch = (StartServerSearch)GetProcAddress(hModule, "start_server_search");
		pStopServerSearch = (StopServerSearch)GetProcAddress(hModule, "stop_server_search");
		pGetServerSearchResult = (GetServerSearchResult)GetProcAddress(hModule, "get_server_search_result");
		pStartKadSearch = (StartKadSearch)GetProcAddress(hModule, "start_kad_search");
	}
	else
	{
		::MessageBoxA(0, "Component initialization failed.", "Notice", 0);
		exit(0);
	}
#endif

}

void MainWindow::LoadIniData()
{
	QSettings settings(QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(QString::fromLocal8Bit(SETTING_FILE_NAME)), QSettings::IniFormat);
	settings.setIniCodec(QTextCodec::codecForLocale());

	wchar_t skinName[MAX_PATH] = {0};
	::GetPrivateProfileString(L"Option", L"SkinName", L":/Skin/0", skinName, MAX_PATH, settings.fileName().toStdWString().c_str());
	g_skinName = QString::fromWCharArray(skinName);

	g_playChannel = ::GetPrivateProfileInt(L"Option", L"PlayChannel", 0, settings.fileName().toStdWString().c_str());
	g_isAutoStart = ::GetPrivateProfileInt(L"Option", L"AutoStart", 0, settings.fileName().toStdWString().c_str()) == 1;
	g_isSaveHistory = ::GetPrivateProfileInt(L"Option", L"SaveHistory", 1, settings.fileName().toStdWString().c_str()) == 1;
	g_isFirstRun = ::GetPrivateProfileInt(L"Option", L"FirstRun", 1, settings.fileName().toStdWString().c_str()) == 1;
	g_doubleClick = ::GetPrivateProfileInt(L"Option", L"DoubleClick", 0, settings.fileName().toStdWString().c_str());
#ifdef KAD_SEARCHER
	if(g_doubleClick == CLICK_PLAY)
	{
		g_doubleClick = CLICK_COPY;
	}
#endif

	g_searchKeyList.clear();
	settings.beginGroup("Key");
	QStringList searchKeys = settings.allKeys();
	for(int i=0; i<searchKeys.size(); i++)
	{
		QByteArray data = settings.value(searchKeys[i]).toByteArray();
		QStringList value = QString(data).split("|");

		g_searchKeyList.push_back(QString(data));
	}
	settings.endGroup();

	g_historyMovieList.clear();
	settings.beginGroup("History");
	QStringList historyMovies = settings.allKeys();
	for(int i=0; i<historyMovies.size(); i++)
	{
		QByteArray data = settings.value(historyMovies[i]).toByteArray();
		QStringList value = QString(data).split("|");

		g_historyMovieList.push_back(QString(data));
	}
	settings.endGroup();

	g_favoriteMovieList.clear();
	settings.beginGroup("Favorite");
	QStringList favoriteMovies = settings.allKeys();
	for(int i=0; i<favoriteMovies.size(); i++)
	{
		QByteArray data = settings.value(favoriteMovies[i]).toByteArray();
		QStringList value = QString(data).split("|");

		g_favoriteMovieList.push_back(QString(data));
	}
	settings.endGroup();
}

void MainWindow::SaveIniData()
{
	QString settingPath = QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(QString::fromLocal8Bit(SETTING_FILE_NAME));

	::WritePrivateProfileString(L"Option", L"SkinName", g_skinName.toStdWString().data(), settingPath.toStdWString().data());

	::WritePrivateProfileString(L"Option", L"PlayChannel", QString::number(g_playChannel).toStdWString().c_str(), settingPath.toStdWString().data());
	::WritePrivateProfileString(L"Option", L"AutoStart", g_isAutoStart ? L"1" : L"0", settingPath.toStdWString().data());
	::WritePrivateProfileString(L"Option", L"SaveHistory", g_isSaveHistory ? L"1" : L"0", settingPath.toStdWString().data());
	::WritePrivateProfileString(L"Option", L"FirstRun", L"0", settingPath.toStdWString().data());
	::WritePrivateProfileString(L"Option", L"DoubleClick", QString::number(g_doubleClick).toStdWString().c_str(), settingPath.toStdWString().data());

	::WritePrivateProfileSection(L"Key", NULL, settingPath.toStdWString().data());
	if(g_isSaveHistory)
	{
		for(int i=0; i<g_searchKeyList.size(); i++)
		{
			QString value = g_searchKeyList[i];

			::WritePrivateProfileString(L"Key",
				QString::number(i).toStdWString().data(), 
				value.toStdWString().data(), 
				settingPath.toStdWString().data());
		}
	}

	::WritePrivateProfileSection(L"History", NULL, settingPath.toStdWString().data());
	for(int i=0; i<g_historyMovieList.size(); i++)
	{
		QString value = g_historyMovieList[i];

		::WritePrivateProfileString(L"History",
			QString::number(i).toStdWString().data(), 
			value.toStdWString().data(), 
			settingPath.toStdWString().data());
	}

	::WritePrivateProfileSection(L"Favorite", NULL, settingPath.toStdWString().data());
	for(int i=0; i<g_favoriteMovieList.size(); i++)
	{
		QString value = g_favoriteMovieList[i];

		::WritePrivateProfileString(L"Favorite",
			QString::number(i).toStdWString().data(), 
			value.toStdWString().data(), 
			settingPath.toStdWString().data());
	}
}

TitleBar* MainWindow::GetTitleBar()
{
	return m_pTitleBar;
}

Content* MainWindow::GetContent()
{
	return m_pContent;
}

StatusBar* MainWindow::GetStatusBar()
{
	return m_pStatusBar;
}

PlayListWidget* MainWindow::GetPlayListWidget()
{
	return (PlayListWidget*)m_pPlayListWidget;
}

void MainWindow::InsertResult(unsigned int searchId, const SEARCHRESULT &result)
{
	for(int i = 0; i<m_pResultTabWidget->count(); i++)
	{
		ResultTreeWidget *resultTreeWidget = qobject_cast<ResultTreeWidget *>(m_pResultTabWidget->widget(i));
		if(resultTreeWidget != NULL && resultTreeWidget->m_searchID == searchId)
		{
			KadResult kadResult;
			kadResult.Name = QString::fromStdWString(result.Name);
			kadResult.Size = result.Size;
			kadResult.Type = GetFileType(UtilityManager::ws2s(result.Name).c_str());
			kadResult.Resource = result.Resource;
			kadResult.Ed2k = QString::fromStdWString(result.Ed2k);

			resultTreeWidget->insertResult(kadResult);
		}
	}
	
}

void MainWindow::showSkinWindow()
{
	if(m_pSkinWindow == NULL)
	{
		m_pSkinWindow = new SkinWindow(this);
		m_pSkinWindow->setObjectName("skinWindow");
		connect(m_pSkinWindow, SIGNAL(changeSkin()), this, SLOT(changeSkin()));
	}

	m_pSkinWindow->show();
}

void MainWindow::showMiniPortalWindow()
{
	if(m_pMiniPortalWindow == NULL)
	{
		m_pMiniPortalWindow= new MiniPortalWindow(this);
		m_pMiniPortalWindow->setObjectName("miniPortalWindow");
	}

	m_pMiniPortalWindow->show();
}

void MainWindow::showSettingWindow()
{
	if(m_pSettingWindow == NULL)
	{
		m_pSettingWindow = new SettingWindow(this);
		m_pSettingWindow->setObjectName("settingWindow");
	}

	m_pSettingWindow->show();
}

void MainWindow::showCloudWindow()
{
	if(m_pCloudWindow == NULL)
	{
		m_pCloudWindow = new CloudWindow(this);
		m_pCloudWindow->setObjectName("cloudWindow");
	}

	m_pCloudWindow->show();
}

void MainWindow::showFeedbackWindow(int type, QString content)
{
	if(m_pFeedbackWindow == NULL)
	{
		m_pFeedbackWindow = new FeedbackWindow(this);
		m_pFeedbackWindow->setObjectName("feedbackWindow");
	}

	m_pFeedbackWindow->SetInfo(type, content);
	m_pFeedbackWindow->show();
}

void MainWindow::changeSkin()
{
    update();
}

void MainWindow::checkUpdate()
{
	showUpdate();

	g_isManualUpdate = false;
}

void MainWindow::closePlayWindow()
{
	QObjectList list = g_pMainWindow->children();
	foreach(QObject *obj, list)
	{
		PlayWindow *playWindow = qobject_cast<PlayWindow*>(obj);
		if(playWindow)
		{
			playWindow->close();
		}
	}
}

void MainWindow::showWidget()
{
	if(isHidden())
	{
		show();
		raise();
		activateWindow();
	}
	else
	{
		hide();
	}
}

void MainWindow::showUpdate()
{
	QString updaterPath = QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(UPDATER_FILE_NAME);
	QStringList args;
	args << QString("-pid=%1").arg(::GetCurrentProcessId())
		<< QString("-hwnd=%1").arg(winId())
		<< "-notify=0"
		;

	if(!QProcess::startDetached(updaterPath, args))
	{
		MessageWindow::GetInstance().ShowMessage(tr("Notice"), tr("Updater could not be started, Please download the full version.<br/><a href=\"%1\">%1</a>").arg(WEBSITE_URL));
	}

	g_isManualUpdate = true;
}

void MainWindow::showHelp()
{
	QDesktopServices::openUrl(QString::fromLocal8Bit(HELP_URL));
}

void MainWindow::showAbout()
{
	QString text = tr("Version: %1<br/>Copyright 2013-2015<br/><a href=\"%2\">%2</a>").arg(APP_VERSION_STR).arg(WEBSITE_URL);

	MessageWindow::GetInstance().ShowMessage(tr("About"), text);
}

void MainWindow::on_systemTray_activated(QSystemTrayIcon::ActivationReason reason)
{
	switch(reason)
	{
		//点击托盘图标之后松开
	case QSystemTrayIcon::Trigger:
		{
			showWidget();
			break;
		}    
		//双击托盘图标
	case QSystemTrayIcon::DoubleClick:
		{
			showWidget();
			break; 
		}               
	default:
		break;
	}
}

void MainWindow::on_filterLineEdit_textChanged(const QString &text)
{
	m_nameFilter = text;

	m_pResultTabWidget->SetFilter(m_nameFilter, m_typeFilter);
}

void MainWindow::on_typeButtonGroup_buttonClicked(QAbstractButton *button)
{
	m_typeFilter.clear();

	QString type = button->text();
	if(type == tr("All"))
	{
		m_typeFilter = g_attributeHash.values().toSet().toList();
	}
	else if(type == tr("Movie"))
	{
		m_typeFilter << tr("Video") << tr("Subtitle");
	}
	else if(type == tr("Music"))
	{
		m_typeFilter << tr("Audio") << tr("Lyric");
	}
	else if(type == tr("File"))
	{
		m_typeFilter << tr("Zip") << tr("CD");
	}
	else if(type == tr("Other"))
	{
		m_typeFilter << tr("Program") << tr("Document") << tr("Picture") << tr("Web") << tr("Torrent") << tr("EBook");
	}

	m_pResultTabWidget->SetFilter(m_nameFilter, m_typeFilter);
}

void MainWindow::on_playListToolButton_clicked(bool checked)
{
	m_pPlayListWidget->setVisible(checked);

	m_pPlayListToolButton->setChecked(checked);

	m_pPlayListToolButton->move(m_pPlayListToolButton->isChecked() ? m_pPlayListWidget->width() : 0, m_pResultTabWidget->height() / 2 - m_pPlayListToolButton->height() / 2);
}

void MainWindow::on_searchComboBox_returnPressed()
{
	on_searchPushButton_clicked();
}

void MainWindow::on_searchPushButton_clicked()
{
	QString key = m_pSearchComboBox->currentText().trimmed();
	if(key.isEmpty())
	{
		MessageWindow::GetInstance().ShowMessage(tr("Notice"), tr("Please input search keyword."));
		return;
	}

	if(key.length() > 40)
	{
		MessageWindow::GetInstance().ShowMessage(tr("Notice"), tr("Keyword too long."));
		return;
	}

	if(m_sensitiveWords.contains(key))
	{
		MessageWindow::GetInstance().ShowMessage(tr("Notice"), tr("Keyword limited."));
		return;
	}

	if(!g_isKadConnected)
	{
		MessageWindow::GetInstance().ShowMessage(tr("Notice"), tr("Unconnected to kad server."));
		return;
	}

	if(!m_pResultTabWidget->m_keyList.contains(key))
	{
		m_pResultTabWidget->AddTab(key);

		//添加关键字
		g_searchKeyList.removeAll(key);
		g_searchKeyList.insert(0, key);
		m_pSearchComboBox->clear();
		foreach(QString key, g_searchKeyList)
		{
			m_pSearchComboBox->addItem(key);
		}

		QMetaObject::invokeMethod(g_pServiceManager, "SetSearchLog", Qt::QueuedConnection, Q_ARG(QString, key));
	}
}

void MainWindow::on_randomPushButton_clicked()
{
	QMetaObject::invokeMethod(g_pServiceManager, "GetRandomWord", Qt::QueuedConnection);
}

void MainWindow::on_resultTabWidget_currentChanged(int index)
{
#ifdef TODAY_RESOURCE
	if(index == -1)
	{
		m_pWebAxWidget->show();
	}
	else
	{
		m_pWebAxWidget->hide();
	}
#endif
}

void MainWindow::on_searchKey_emited(QString key)
{
	m_pSearchComboBox->setEditText(key);

	on_searchPushButton_clicked();
}

void MainWindow::on_getSystemMessage_emited(QString result)
{
	if(!result.isEmpty())
	{
		if(result.contains("http://") || result.contains("https://"))
		{
			MessageWindow::GetInstance().ShowMessage(tr("System notice"), result);
		}
		else
		{
			m_pSystemTray->showMessage(tr("System notice"), result, QSystemTrayIcon::Information);
		}

		if(result.contains(tr("Auto close")))
		{
			exit(0);
		}
	}

}

void MainWindow::on_getSensitiveWords_emited(QStringList sensitiveWords)
{
	m_sensitiveWords = sensitiveWords;
}

void MainWindow::on_getHotEd2ks_emited(QStringList hotEd2ks)
{
	foreach(QString ed2k, hotEd2ks)
	{
		m_pPlayListWidget->AddRecommend(ed2k);
	}
}

void MainWindow::on_getHotWords_emited(QStringList hotWords)
{
	m_hotWords = hotWords;

	m_pStatusBar->SetHotWords(m_hotWords);
}

void MainWindow::on_getRandomWord_emited(QString randWord)
{
	m_pSearchComboBox->setEditText(randWord);

	on_searchPushButton_clicked();
}

void MainWindow::on_setSearchLog_emited(QString key)
{
	Q_UNUSED(key);
}

void MainWindow::on_setFeedback_emited(QString version, int type, QString content)
{
	Q_UNUSED(version);
	Q_UNUSED(type);
	Q_UNUSED(content);

	MessageWindow::GetInstance().ShowMessage(tr("Notice"), tr("Thank you for your feedback, we will deal with as soon as possible."));
}

void MainWindow::on_setPlayLog_emited(QString link)
{

}
