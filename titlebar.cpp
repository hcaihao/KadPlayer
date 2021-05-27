#include "stdafx.h"
#include "titlebar.h"
#include "statusbar.h"
#include "content.h"

TitleBar *g_pTitleBar = NULL;

TitleBar::TitleBar(QFrame * parent)
	: m_bMaxWin(false), m_bTopWin(false), QFrame(parent)
{
	setObjectName("titleBar");

	InitControl();
	AddContextMenu();

	g_pTitleBar = this;
}

TitleBar::~TitleBar()
{

}

void TitleBar::InitControl()
{
	m_pTitleLabel = new QLabel(this);
	m_pTitleLabel->setObjectName("titleLabel");
	m_pTitleLabel->setToolTip(tr("Welcome"));
	m_pTitleLabel->setText(QString("%1 %2").arg(APP_TITLE).arg(APP_VERSION_STR));

	m_pCloudPlayButton = new QToolButton(this);
	m_pCloudPlayButton->setObjectName("cloudPlayButton");
	m_pCloudPlayButton->setToolTip(tr("Cloud play"));
	m_pCloudPlayButton->setCursor(Qt::PointingHandCursor);
	connect(m_pCloudPlayButton, SIGNAL(clicked()), this, SLOT(showCloud()));
#ifdef KAD_SEARCHER
	m_pCloudPlayButton->hide();
#endif

	m_pSkinButton = new QToolButton(this);
	m_pSkinButton->setObjectName("skinButton");
	m_pSkinButton->setToolTip(tr("Change skin"));
	m_pSkinButton->setCursor(Qt::PointingHandCursor);
	connect(m_pSkinButton, SIGNAL(clicked()), this, SLOT(showSkin()));

	m_pMiniPortalButton = new QToolButton(this);
	m_pMiniPortalButton->setObjectName("miniPortalButton");
	m_pMiniPortalButton->setToolTip(tr("Today news"));
	m_pMiniPortalButton->setCursor(Qt::PointingHandCursor);
	connect(m_pMiniPortalButton, SIGNAL(clicked()), this, SLOT(showMiniPortal()));
	m_pMiniPortalButton->setVisible(false);

	m_pMenuButton = new QToolButton(this);
	m_pMenuButton->setObjectName("menuButton");
	m_pMenuButton->setToolTip(tr("Short menu"));
	m_pMenuButton->setCursor(Qt::PointingHandCursor);
	connect(m_pMenuButton, SIGNAL(clicked()), this, SLOT(showMenu()));

	m_pMinButton = new QToolButton(this);
	m_pMinButton->setObjectName("minButton");
	m_pMinButton->setToolTip(tr("Minimize"));
	m_pMinButton->setCursor(Qt::PointingHandCursor);
	connect(m_pMinButton, SIGNAL(clicked()), this, SLOT(showMin()));

	m_pMaxButton = new QToolButton(this);
	m_pMaxButton->setObjectName("maxButton");
	m_pMaxButton->setToolTip(tr("Maximize"));
	m_pMaxButton->setCursor(Qt::PointingHandCursor);
	m_pMaxButton->setProperty("MaxState", m_bMaxWin);
	connect(m_pMaxButton, SIGNAL(clicked()), this, SLOT(showMaxRestore()));

	m_pCloseButton = new QToolButton(this);
	m_pCloseButton->setObjectName("closeButton");
	m_pCloseButton->setToolTip(tr("Close"));
	m_pCloseButton->setCursor(Qt::PointingHandCursor);
	connect(m_pCloseButton, SIGNAL(clicked()), parent(), SLOT(hide()));


	QHBoxLayout *layout = new QHBoxLayout();
	layout->setAlignment(Qt::AlignTop);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(m_pTitleLabel);
	layout->addStretch();
	layout->addWidget(m_pCloudPlayButton);
	layout->addWidget(m_pSkinButton);
	layout->addWidget(m_pMiniPortalButton);
	layout->addWidget(m_pMenuButton);
	layout->addWidget(m_pMinButton);
	layout->addWidget(m_pMaxButton);
	layout->addWidget(m_pCloseButton);

	setLayout(layout);
}

void TitleBar::AddContextMenu()
{
	m_pTitleMenu = new QMenu(this);
	m_pTitleMenu->setObjectName("titleMenu");

	QAction *settingAction = new QAction(QIcon(":/Image/menu_setting.png"), tr("System setting"), this);
	m_pTitleMenu->addAction(settingAction);
	connect(settingAction, SIGNAL(triggered()), this, SIGNAL(showSettingWindow()));

	m_pTitleMenu->addSeparator();

	QAction *updateAction = new QAction(QIcon(":/Image/menu_update.png"), tr("Check update"), this);
	m_pTitleMenu->addAction(updateAction);
	connect(updateAction, SIGNAL(triggered()), this, SIGNAL(showUpdate()));

	QAction *helpAction = new QAction(QIcon(":/Image/menu_help.png"), tr("Online help"), this);
	m_pTitleMenu->addAction(helpAction);
	connect(helpAction, SIGNAL(triggered()), this, SIGNAL(showHelp()));

	QAction *feedbackAction = new QAction(QIcon(":/Image/menu_feedback.png"), tr("Feedback"), this);
	m_pTitleMenu->addAction(feedbackAction);
	connect(feedbackAction, SIGNAL(triggered()), this, SLOT(showFeedback()));

	QAction *aboutAction = new QAction(QIcon(":/Image/menu_about.png"), tr("About"), this);
	m_pTitleMenu->addAction(aboutAction);
	connect(aboutAction, SIGNAL(triggered()), this, SIGNAL(showAbout()));
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		showMaxRestore();
	}
}

void TitleBar::showTop()
{
	if(m_bTopWin)
	{
		SetWindowPos((HWND)parentWidget()->winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	}
	else 
	{
		SetWindowPos((HWND)parentWidget()->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	}

	m_bTopWin = !m_bTopWin;
}

void TitleBar::showSkin()
{
	emit showSkinWindow();
}

void TitleBar::showFeedback()
{
	emit showFeedbackWindow(FEEDBACK_BUG, "");
}

void TitleBar::showMiniPortal()
{
	emit showMiniPortalWindow();
}

void TitleBar::showCloud()
{
	emit showCloudWindow();
}

void TitleBar::showMenu()
{
	//设置主菜单出现的位置
	QPoint point = m_pMenuButton->pos();
	point.setX(point.x() - 25);
	point.setY(point.y() + 25);
	m_pTitleMenu->exec(mapToGlobal(point));

	//修复MenuButton样式不还原的Bug
	//m_pMenuButton->update();
}

void TitleBar::showMin()
{
	parentWidget()->showMinimized();
}

void TitleBar::showMaxRestore()
{
	if(m_bMaxWin)
	{
		parentWidget()->setGeometry(m_windowRect);
	}
	else 
	{
		m_windowRect = parentWidget()->geometry();

		QRect rect = QApplication::desktop()->availableGeometry();
		rect.setLeft(rect.left() - SHADOW_WIDTH);
		rect.setTop(rect.top() - SHADOW_WIDTH);
		rect.setRight(rect.right() + SHADOW_WIDTH);
		rect.setBottom(rect.bottom() + SHADOW_WIDTH);
		parentWidget()->setGeometry(rect);


		//int frameCX = GetSystemMetrics(SM_CXSIZEFRAME); 
		//int frameCY = GetSystemMetrics(SM_CYSIZEFRAME);
		//int menuCY = GetSystemMetrics(SM_CYMENU);

		////多显示器处理
		//RECT rc;
		//GetWindowRect((HWND)parentWidget()->winId(), &rc);
		//HMONITOR hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);

		//MONITORINFO mi; 
		//mi.cbSize = sizeof(mi); 
		//GetMonitorInfo(hMonitor, &mi); 

		//SetWindowPos((HWND)parentWidget()->winId(),
		//	NULL,
		//	mi.rcWork.left - SHADOW_WIDTH,
		//	mi.rcWork.top - SHADOW_WIDTH,
		//	mi.rcWork.right - mi.rcWork.left + SHADOW_WIDTH * 2,
		//	mi.rcWork.bottom - mi.rcWork.top + SHADOW_WIDTH * 2,
		//	SWP_SHOWWINDOW);
	}

	m_bMaxWin = !m_bMaxWin;

	RefreshStyle(m_pMaxButton, "MaxState", m_bMaxWin);
}
