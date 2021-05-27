#include "stdafx.h"
#include "statusbar.h"
#include "utilitymanager.h"

StatusBar *g_pStatusBar = NULL;

StatusBar::StatusBar(QFrame * parent)
	: QFrame(parent)
{
	setObjectName("statusBar");
	//setMouseTracking(true);

	InitControl();

	g_pStatusBar = this;

	m_pStatusTimer = new QTimer(this);
	connect(m_pStatusTimer, SIGNAL(timeout()), this, SLOT(on_getStatus_timeout()));
	m_pStatusTimer->start(ONE_SECOND);


	m_pWordTimer = new QTimer(this);
	connect(m_pWordTimer, SIGNAL(timeout()), this, SLOT(on_getWord_timeout()));
	m_pWordTimer->start(FIVE_SECONDS);
}

StatusBar::~StatusBar()
{
	m_pStatusTimer->stop();
	m_pWordTimer->stop();
}

void StatusBar::InitControl()
{
	m_pCopyrightLabel = new QLabel(this);
	m_pCopyrightLabel->setObjectName("copyrightLabel");
	m_pCopyrightLabel->setText(tr("Copyright 2013-2015"));
	//m_pCopyrightLabel->setOpenExternalLinks(true);
	connect(m_pCopyrightLabel, SIGNAL(linkActivated(const QString &)), this, SLOT(on_copyrightLabel_linkActivated(const QString &)));

	m_pServerImageLabel = new QLabel(this);
	m_pServerImageLabel->setObjectName("serverImageLabel");
	m_pServerImageLabel->setProperty("Online", false);

	m_pServerStatusLabel = new QLabel(this);
	m_pServerStatusLabel->setObjectName("serverStatusLabel");

	QHBoxLayout *m_pLayout = new QHBoxLayout(this);
	m_pLayout->setSpacing(10);
	m_pLayout->setContentsMargins(0, 0, 0, 0);
	m_pLayout->addWidget(m_pCopyrightLabel);
	m_pLayout->addStretch();
	m_pLayout->addWidget(m_pServerImageLabel);
	m_pLayout->addWidget(m_pServerStatusLabel);
	setLayout(m_pLayout);
}

void StatusBar::SetHotWords(QStringList hotWords)
{
	m_hotWords = hotWords;
}

void StatusBar::on_getStatus_timeout()
{
	DWORD unknow = 0;
	DWORD status = 0;
	PDWORD pStatus = &status;

#ifdef LOCAL_KAD_LIB
	status = g_kadLibrary.Connect() ? CONNECT_CONNECTED : CONNECT_CONNECTING;
#else
	pGetEmuleStatus(&unknow, &status, &pStatus);
#endif

	if(status == CONNECT_UNKNOW)
	{
		m_pServerStatusLabel->setText(tr("Status:Unknow"));
		RefreshStyle(m_pServerImageLabel, "Online", false);
	}
	else if(status == CONNECT_CONNECTING)
	{
		m_pServerStatusLabel->setText(tr("Status:Connecting"));
		RefreshStyle(m_pServerImageLabel, "Online", false);
	}
	else if(status == CONNECT_ERROR)
	{
		m_pServerStatusLabel->setText(tr("Status:Disconnected"));
		RefreshStyle(m_pServerImageLabel, "Online", false);
	}
	else if(status == CONNECT_CONNECTED)
	{
		m_pServerStatusLabel->setText(tr("Status:Connected"));
		RefreshStyle(m_pServerImageLabel, "Online", true);

		g_isKadConnected = true;
	}
}

void StatusBar::on_getWord_timeout()
{
	if(m_hotWords.count() > 0)
	{
		int index = rand() % m_hotWords.count();

		QString text = tr("Hot keywords:<a href=\"%1\" style=\"color:#00A2E8\">%1</a>").arg(m_hotWords[index]);

		m_pCopyrightLabel->setText(text);
	}
}

void StatusBar::on_copyrightLabel_linkActivated(const QString & link)
{
	emit searchKey(link);
}