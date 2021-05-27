#include "stdafx.h"
#include "skinwindow.h"
#include "titlebar.h"
#include "content.h"
#include "statusbar.h"
#include "changeskinwidget.h"

#include <QFileIconProvider>

SkinWindow::SkinWindow(QFrame *parent)
    : BaseWindow(parent)
{
	setObjectName("skinWindow");

	setFixedSize(SKIN_WINDOW_WIDTH, SKIN_WINDOW_HEIGHT);

	InitControl();

	m_currentPage = 1;
	showSkin(QString::number(m_currentPage));
}

SkinWindow::~SkinWindow()
{

}

void SkinWindow::paintEvent(QPaintEvent *event)
{
	//先显示阴影
	BaseWindow::paintEvent(event);

	//再显示背景
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawPixmap(QRect(SHADOW_WIDTH, SHADOW_WIDTH, width()-2*SHADOW_WIDTH, height()-2*SHADOW_WIDTH), QPixmap(g_skinName));
}

bool SkinWindow::eventFilter(QObject *obj, QEvent *event)
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

bool SkinWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	//禁用鼠标拖拽改变大小
	return QFrame::nativeEvent(eventType, message, result);
}

void SkinWindow::InitControl()
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

	//初始化皮肤列表
	for(int i=0; i<SKIN_COUNT; i++)
	{
		m_skinNameList << QString(":/Skin/%1").arg(i);

		m_skinTipList << tr("Skin%1").arg(i+1);
	}

	QGridLayout *skinLayout = new QGridLayout();
	skinLayout->setSpacing(5);
	skinLayout->setContentsMargins(5, 35, 5, 0);

	for(int i=0; i<8; i++)
	{
		ChangeSkinWidget *changeSkinWidget = new ChangeSkinWidget();
		m_changeSkinWidgetList.append(changeSkinWidget);
		connect(changeSkinWidget, SIGNAL(useSkin(QString)), this, SLOT(useSkin(QString)));
		skinLayout->addWidget(changeSkinWidget, i/4, i%4);
	}
	
	m_pageCount = m_skinNameList.size() / 8;
	m_lastPageCount = m_skinNameList.size() % 8;
	if(m_lastPageCount > 0)
	{
		m_pageCount = m_pageCount + 1;
	}

	QSignalMapper *signalMapper = new QSignalMapper(this);
	QList<QPushButton *> pageButtonList;
	for(int i=0; i<m_pageCount; i++)
	{
		QPushButton *pagePushButton = new QPushButton();
		pagePushButton->setFixedWidth(20);
		pagePushButton->setText(QString::number(i+1, 10));
		pagePushButton->setObjectName("blueButton");
		pagePushButton->setCursor(Qt::PointingHandCursor);
		connect(pagePushButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
		signalMapper->setMapping(pagePushButton, pagePushButton->text());
		pageButtonList.push_back(pagePushButton);
	}

	//m_pFirstPagePushButton = new QPushButton();
	//m_pFirstPagePushButton->setObjectName("blueButton");
	//m_pFirstPagePushButton->setText(tr("First"));
	//m_pFirstPagePushButton->setFixedWidth(50);
	//m_pFirstPagePushButton->setCursor(Qt::PointingHandCursor);
	//connect(m_pFirstPagePushButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

	//m_pPreviousPagePushButton = new QPushButton();
	//m_pPreviousPagePushButton->setObjectName("blueButton");
	//m_pPreviousPagePushButton->setText(tr("Previous"));
	//m_pPreviousPagePushButton->setFixedWidth(50);
	//m_pPreviousPagePushButton->setCursor(Qt::PointingHandCursor);
	//connect(m_pPreviousPagePushButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

	//m_pNextPagePushButton = new QPushButton();
	//m_pNextPagePushButton->setObjectName("blueButton");
	//m_pNextPagePushButton->setText(tr("Next"));
	//m_pNextPagePushButton->setFixedWidth(50);
	//m_pNextPagePushButton->setCursor(Qt::PointingHandCursor);
	//connect(m_pNextPagePushButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

	//m_pLastPagePushButton = new QPushButton();
	//m_pLastPagePushButton->setObjectName("blueButton");
	//m_pLastPagePushButton->setText(tr("End"));
	//m_pLastPagePushButton->setFixedWidth(50);
	//m_pLastPagePushButton->setCursor(Qt::PointingHandCursor);
	//connect(m_pLastPagePushButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
	//
	//signalMapper->setMapping(m_pFirstPagePushButton, tr("First"));
	//signalMapper->setMapping(m_pPreviousPagePushButton, tr("Previous"));
	//signalMapper->setMapping(m_pNextPagePushButton, tr("Next"));
	//signalMapper->setMapping(m_pLastPagePushButton, tr("End"));
	connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(showSkin(QString)));

	QHBoxLayout *pageLayout = new QHBoxLayout();
	pageLayout->addStretch();
	//pageLayout->addWidget(m_pFirstPagePushButton, 0, Qt::AlignVCenter);
	//pageLayout->addWidget(m_pPreviousPagePushButton, 0, Qt::AlignVCenter);
	for(int i=0; i<pageButtonList.count(); i++)
	{
		QPushButton *pagePushButton = pageButtonList[i];
		pageLayout->addWidget(pagePushButton, 0, Qt::AlignVCenter);
	}
	//pageLayout->addWidget(m_pNextPagePushButton, 0, Qt::AlignVCenter);
	//pageLayout->addWidget(m_pLastPagePushButton, 0, Qt::AlignVCenter);
	pageLayout->addStretch();
	pageLayout->setSpacing(2);
	pageLayout->setContentsMargins(0, 10, 0, 0);

	QVBoxLayout *contentLayout = new QVBoxLayout();
	contentLayout->addLayout(skinLayout);
	contentLayout->addLayout(pageLayout);
	contentLayout->addStretch();
	m_pContent->setLayout(contentLayout);

}

void SkinWindow::showSkin(QString currentPage)
{
	//计算当前页
	if(currentPage == tr("First"))
	{
		m_currentPage = 1;
	}
	else if(currentPage == tr("Previous"))
	{
		if(m_currentPage > 1)
		{
			m_currentPage = m_currentPage - 1;
		}
	}
	else if(currentPage == tr("Next"))
	{
		if(m_currentPage < m_pageCount)
		{
			m_currentPage = m_currentPage + 1;
		}
	}
	else if(currentPage == tr("End"))
	{
		m_currentPage = m_pageCount;
	}
	else
	{
		bool ok;  
		m_currentPage = currentPage.toInt(&ok, 10);
	}

	//按钮显示状态
	//if(m_currentPage == 1)
	//{
	//	m_pFirstPagePushButton->show();
	//	m_pLastPagePushButton->show();
	//	m_pFirstPagePushButton->hide();
	//	m_pPreviousPagePushButton->hide();
	//}
	//else if(m_currentPage == m_pageCount)
	//{
	//	m_pFirstPagePushButton->show();
	//	m_pPreviousPagePushButton->show();
	//	m_pNextPagePushButton->hide();
	//	m_pLastPagePushButton->hide();
	//}
	//else
	//{
	//	m_pFirstPagePushButton->hide();
	//	m_pPreviousPagePushButton->show();
	//	m_pNextPagePushButton->show();
	//	m_pLastPagePushButton->hide();
	//}

	//显示当前页皮肤
	int startSkinIndex = (m_currentPage - 1) * 8;
	int startTipIndex = startSkinIndex;
	for(int i=0; i<m_changeSkinWidgetList.count(); i++)
	{
		if(m_currentPage == m_pageCount)
		{
			if(i >= m_lastPageCount)
			{
				m_changeSkinWidgetList[i]->setVisible(false);
				continue;
			}
		}
		else
		{
			m_changeSkinWidgetList[i]->setVisible(true);
		}

		m_changeSkinWidgetList[i]->showSkin(m_skinNameList.at(startSkinIndex++), m_skinTipList.at(startTipIndex++));
	}
}

void SkinWindow::useSkin(QString skinName)
{
	g_skinName = skinName;
	LoadStyle(QString("%1.qss").arg(skinName));

	update();

	emit changeSkin();
}
