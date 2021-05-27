#include "stdafx.h"
#include "cloudwindow.h"
#include "titlebar.h"
#include "content.h"
#include "statusbar.h"
#include "webaxwidget.h"

CloudWindow::CloudWindow(QFrame *parent)
    : BaseWindow(parent)
{
	setObjectName("cloudWindow");

    resize(CLOUD_WINDOW_WIDTH, CLOUD_WINDOW_HEIGHT);

	InitControl();
}

CloudWindow::~CloudWindow()
{

}

void CloudWindow::paintEvent(QPaintEvent *event)
{
	//先显示阴影
	BaseWindow::paintEvent(event);

	//再显示背景
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawPixmap(QRect(SHADOW_WIDTH, SHADOW_WIDTH, width()-2*SHADOW_WIDTH, height()-2*SHADOW_WIDTH), QPixmap(g_skinName));
}

bool CloudWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	return BaseWindow::nativeEvent(eventType, message, result);
}

void CloudWindow::moveEvent(QMoveEvent * event)
{
	Q_UNUSED(event);

	QPoint point = pos();
	point.setY(point.y() + m_pTitleBar->height() + SHADOW_WIDTH);
	point.setX(point.x() + SHADOW_WIDTH);

	m_pWebAxWidget->move(point);
}

void CloudWindow::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);

	QSize size = this->size();
	size.setHeight(height() - m_pTitleBar->height() - SHADOW_WIDTH*2);
	size.setWidth(width() - SHADOW_WIDTH*2);

	m_pWebAxWidget->resize(size);
}

void CloudWindow::showEvent(QShowEvent *event)
{
	Q_UNUSED(event);

	m_pWebAxWidget->show();
}

void CloudWindow::hideEvent(QHideEvent *event)
{
	Q_UNUSED(event);

	m_pWebAxWidget->hide();

	stop();
}

void CloudWindow::stop()
{
	if(g_playChannel == CHANNEL_THUNDER)
	{
		m_pWebAxWidget->dynamicCall("Navigate(const QString&)", THUNDER_CLOUD_PLAY_URL);
	}
	else if(g_playChannel == CHANNEL_QQ)
	{

	}
	else if(g_playChannel == CHANNEL_OTHER)
	{
		m_pWebAxWidget->dynamicCall("Navigate(const QString&)", HUOYAN_CLOUD_PLAY_URL);
	}
}

void CloudWindow::InitControl()
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

	m_pWebAxWidget = new WebAxWidget(this);
	m_pWebAxWidget->setObjectName("webAxWidget");
	m_pWebAxWidget->setProtocolName("openVodClientWindow");
	
	if(g_playChannel == CHANNEL_THUNDER)
	{
		m_pWebAxWidget->dynamicCall("Navigate(const QString&)", THUNDER_CLOUD_PLAY_URL);
	}
	else if(g_playChannel == CHANNEL_QQ)
	{

	}
	else if(g_playChannel == CHANNEL_OTHER)
	{
		m_pWebAxWidget->dynamicCall("Navigate(const QString&)", HUOYAN_CLOUD_PLAY_URL);
	}

	m_pWebAxWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::Popup);	//Qt::Tool不显示任务栏图标，需设置CloudWindow的parent

	QVBoxLayout *contentLayout = new QVBoxLayout();
	contentLayout->setSpacing(0);
	contentLayout->setContentsMargins(0, 0, 0, 0);
	//contentLayout->addWidget(m_pWebAxWidget);
	m_pContent->setLayout(contentLayout);
}
