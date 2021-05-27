#include "stdafx.h"
#include "miniportalwindow.h"
#include "titlebar.h"
#include "content.h"
#include "statusbar.h"
#include "webaxwidget.h"

MiniPortalWindow::MiniPortalWindow(QFrame *parent)
    : BaseWindow(parent)
{
	setObjectName("miniPortalWindow");

    setFixedSize(MINI_PORTAL_WINDOW_WIDTH, MINI_PORTAL_WINDOW_HEIGHT);

	InitControl();

	//http://www.qtcentre.org/threads/42038-Problems-with-recieving-Windows-messages-using-winEvent
}

MiniPortalWindow::~MiniPortalWindow()
{

}

void MiniPortalWindow::paintEvent(QPaintEvent *event)
{
	//先显示阴影
	BaseWindow::paintEvent(event);

	//再显示背景
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawPixmap(QRect(SHADOW_WIDTH, SHADOW_WIDTH, width()-2*SHADOW_WIDTH, height()-2*SHADOW_WIDTH), QPixmap(g_skinName));
}

bool MiniPortalWindow::eventFilter(QObject *obj, QEvent *event)
{
	if(obj == m_pTitleBar)
	{
		if(event->type() == QEvent::MouseButtonDblClick)
		{
			return true;
		}
	}

	return BaseWindow::eventFilter(obj, event);
}

bool MiniPortalWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	//禁用鼠标拖拽改变大小
	return QFrame::nativeEvent(eventType, message, result);
}

void MiniPortalWindow::moveEvent(QMoveEvent * event)
{
	Q_UNUSED(event);

	QPoint point = pos();
	point.setY(point.y() + m_pTitleBar->height() + SHADOW_WIDTH);
	point.setX(point.x() + SHADOW_WIDTH);

	m_pWebAxWidget->move(point);
}

void MiniPortalWindow::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);

	QSize size = this->size();
	size.setHeight(height() - m_pTitleBar->height() - SHADOW_WIDTH*2);
	size.setWidth(width() - SHADOW_WIDTH*2);

	m_pWebAxWidget->resize(size);
}

void MiniPortalWindow::showEvent(QShowEvent *event)
{
	Q_UNUSED(event);

	m_pWebAxWidget->show();
}

void MiniPortalWindow::hideEvent(QHideEvent *event)
{
	Q_UNUSED(event);

	m_pWebAxWidget->hide();
}

void MiniPortalWindow::InitControl()
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
	m_pTitleBar->m_pMinButton->setVisible(false);
	m_pTitleBar->m_pMaxButton->setVisible(false);
	m_pTitleBar->installEventFilter(this);


	m_pWebAxWidget = new WebAxWidget(this);
	m_pWebAxWidget->setObjectName("webAxWidget");
	m_pWebAxWidget->dynamicCall("Navigate(const QString&)", MINI_PORTAL_URL);
	m_pWebAxWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::Popup);	//Qt::Tool不显示任务栏图标，需设置MiniPortalWindow的parent

	QVBoxLayout *contentLayout = new QVBoxLayout();
	contentLayout->setSpacing(0);
	contentLayout->setContentsMargins(0, 0, 0, 0);
	//contentLayout->addWidget(m_pWebAxWidget);
	m_pContent->setLayout(contentLayout);
}
