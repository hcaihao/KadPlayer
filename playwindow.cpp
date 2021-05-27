#include "stdafx.h"
#include "playwindow.h"
#include "titlebar.h"
#include "content.h"
#include "statusbar.h"
#include "utilitymanager.h"
#include "webaxwidget.h"
#include "servicemanager.h"

#include <WinInet.h>

extern ServiceManager *g_pServiceManager;

QString GetCookies(const QString& url)
{
	wchar_t cookie[4960] = { 0 };
	DWORD len = 4960;
	InternetGetCookie(url.toStdWString().c_str(), NULL, cookie, &len);
	return QString::fromStdWString(cookie);
}

QString MakePlayUrl(const QString& ed2k)
{
	QString myLink = QString::fromLocal8Bit(QUrl::toPercentEncoding(ed2k));
	QString isvip;
	QString sessionid;
	QString userid;

	if (isvip.isEmpty() || sessionid.isEmpty() || userid.isEmpty())
	{
		QString cookies = GetCookies(THUNDER_INIT_URL);
		QStringList cookielist = cookies.split(';');
		for (int i = 0;  i < cookielist.size(); ++i)
		{
			QString &cookie = cookielist[i];
			QStringList cookename = cookie.split('=');
			bool f = cookename.size() > 0;
			if (f && cookename[0].trimmed() == "isvip")
				isvip = cookename[1].trimmed();
			else if (f && cookename[0].trimmed() == "sessionid")
				sessionid = cookename[1].trimmed();
			else if (f && cookename[0].trimmed() == "userid")
				userid = cookename[1].trimmed();
		}
	}

	if (!isvip.isEmpty() && !userid.isEmpty() &&!sessionid.isEmpty())
	{
		QString url ;
		url.sprintf(THUNDER_PLAY_URL, userid.toLocal8Bit().data(),
			isvip.toLocal8Bit().data(), sessionid.toLocal8Bit().data(),
			myLink.toLocal8Bit().data()
			);

		return url;
	}
	else
	{
		return QString("");
	}
}

PlayWindow::PlayWindow(QFrame *parent)
    : BaseWindow(parent)
{
	//setAttribute(Qt::WA_TranslucentBackground, false);

	setObjectName("playWindow");
	setAttribute(Qt::WA_DeleteOnClose);

	resize(PLAY_WINDOW_WIDTH, PLAY_WINDOW_HEIGHT);

	InitControl();
}

PlayWindow::~PlayWindow()
{

}

void PlayWindow::paintEvent(QPaintEvent *event)
{
	//先显示阴影
	BaseWindow::paintEvent(event);

	//再显示背景
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawPixmap(QRect(SHADOW_WIDTH, SHADOW_WIDTH, width()-2*SHADOW_WIDTH, height()-2*SHADOW_WIDTH), QPixmap(g_skinName));
}

bool PlayWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	return BaseWindow::nativeEvent(eventType, message, result);
}

void PlayWindow::moveEvent(QMoveEvent * event)
{
	Q_UNUSED(event);

	QPoint point = pos();
	point.setY(point.y() + m_pTitleBar->height() + SHADOW_WIDTH);
	point.setX(point.x() + SHADOW_WIDTH);

	m_pWebAxWidget->move(point);
}

void PlayWindow::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);

	QSize size = this->size();
	size.setHeight(height() - m_pTitleBar->height() - SHADOW_WIDTH*2);
	size.setWidth(width() - SHADOW_WIDTH*2);

	m_pWebAxWidget->resize(size);
}

void PlayWindow::showEvent(QShowEvent *event)
{
	Q_UNUSED(event);

	m_pWebAxWidget->show();
}

void PlayWindow::hideEvent(QHideEvent *event)
{
	Q_UNUSED(event);

	m_pWebAxWidget->hide();

	stop();
}

void PlayWindow::stop()
{
	m_pWebAxWidget->dynamicCall("Navigate(const QString&)", "about:blank");
}

void PlayWindow::InitControl()
{
	//创建标题栏
	m_pTitleBar = new TitleBar(this);
	//创建内容区域
	m_pContent = new Content(this);
	//创建状态栏
	m_pStatusBar = new StatusBar(this);
	m_pStatusBar->setVisible(false);

	//创建布局
	QVBoxLayout *layout = new QVBoxLayout();
	//设置间距与边缘空白
	layout->setSpacing(0);
	layout->setContentsMargins(SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH);
	//将部件加入到布局中
	layout->addWidget(m_pTitleBar);
	layout->addWidget(m_pContent);
	layout->addWidget(m_pStatusBar);
	setLayout(layout);

	//隐藏按钮
	m_pTitleBar->m_pSkinButton->setVisible(false);
	m_pTitleBar->m_pCloudPlayButton->setVisible(false);
	m_pTitleBar->m_pMiniPortalButton->setVisible(false);
	m_pTitleBar->m_pMenuButton->setVisible(false);
	connect(m_pTitleBar->m_pCloseButton, SIGNAL(clicked()), this, SLOT(close()));	//触发WA_DeleteOnClose

	m_pWebAxWidget = new WebAxWidget(this);
	m_pWebAxWidget->setObjectName("webAxWidget");
	
	if(g_playChannel == CHANNEL_THUNDER)
	{
		m_pWebAxWidget->dynamicCall("Navigate(const QString&)", THUNDER_INIT_URL);
	}
	else if(g_playChannel == CHANNEL_QQ)
	{

	}
	else if(g_playChannel == CHANNEL_OTHER)
	{

	}

	m_pWebAxWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::Popup);	//Qt::Tool不显示任务栏图标，需设置MiniPortalWindow的parent
	connect(m_pWebAxWidget, SIGNAL(DocumentComplete(IDispatch*, QVariant&)), this, SLOT(DocumentComplete(IDispatch*, QVariant&))); // Can't connect.

	QVBoxLayout *contentLayout = new QVBoxLayout();
	contentLayout->setSpacing(0);
	contentLayout->setContentsMargins(0, 0, 0, 0);
	//contentLayout->addWidget(m_pWebAxWidget);
	m_pContent->setLayout(contentLayout);
}

void PlayWindow::Play(QString title, QString link)
{
	title = UtilityManager::GetTrimString(title, 48);
	m_pTitleBar->m_pTitleLabel->setText(title);
	setWindowTitle(title);
	m_link = link;
	m_isPlaying = false;

	if(g_playChannel == CHANNEL_THUNDER)
	{
		QString playUrl = MakePlayUrl(link);
		if (playUrl.isEmpty())
		{
			m_pWebAxWidget->dynamicCall("Navigate(const QString&)", THUNDER_LOAD_URL);
		}
		else
		{
			m_pWebAxWidget->dynamicCall("Navigate(const QString&)", playUrl);
		}
	}
	else if(g_playChannel == CHANNEL_QQ)
	{

	}
	else if(g_playChannel == CHANNEL_OTHER)
	{
		m_pWebAxWidget->dynamicCall("Navigate(const QString&)", tr("%1%2").arg(HUOYAN_PLAY_URL).arg(m_link));
	}

	QMetaObject::invokeMethod(g_pServiceManager, "SetPlayLog", Qt::QueuedConnection, Q_ARG(QString, link));
}

void PlayWindow::DocumentComplete(IDispatch* pDisp, QVariant& URL)
{
	Q_UNUSED(pDisp);
	Q_UNUSED(URL);

	//qDebug() << URL.toString();

	if(!m_isPlaying)
	{
		if(g_playChannel == CHANNEL_THUNDER)
		{
			QString url = MakePlayUrl(m_link);
			if(!url.isEmpty())
			{
				m_pWebAxWidget->dynamicCall("Navigate(const QString&)", url.toStdString().c_str());
				m_isPlaying = true;
			}
		}
		else if(g_playChannel == CHANNEL_QQ)
		{

		}
		else if(g_playChannel == CHANNEL_OTHER)
		{

		}
	}
}