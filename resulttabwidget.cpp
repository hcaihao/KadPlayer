#include "StdAfx.h"
#include "resulttabwidget.h"
#include "resulttreewidget.h"
#include "webaxwidget.h"

ResultTabWidget::ResultTabWidget(QWidget *parent)
	: QTabWidget(parent)
{
	setTabsClosable(true);

	//InitControl();

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(on_resultTabWidget_currentChanged(int))); 
	connect(this, SIGNAL(currentChanged(int)), parent, SLOT(on_resultTabWidget_currentChanged(int)));
	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(on_resultTabWidget_tabCloseRequested(int)));
}

ResultTabWidget::~ResultTabWidget()
{

}

bool ResultTabWidget::eventFilter(QObject *obj, QEvent *event)
{

	return QTabWidget::eventFilter(obj, event);
}

bool ResultTabWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	return QTabWidget::nativeEvent(eventType, message, result);
}

void ResultTabWidget::InitControl()
{
	m_pWebAxWidget = new WebAxWidget(this);
	m_pWebAxWidget->setObjectName("webAxWidget");
	m_pWebAxWidget->dynamicCall("Navigate(const QString&)", "http://www.baidu.com");
	m_pWebAxWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::Popup);	//Qt::Tool不显示任务栏图标，需设置MiniPortalWindow的parent

	addTab(new QWidget(), tr("Today resources"));
	setCurrentIndex(0);

	tabBar()->tabButton(0, QTabBar::RightSide)->hide();
}

void ResultTabWidget::AddTab(QString key)
{
	ResultTreeWidget *resultTreeWidget = new ResultTreeWidget(key, this);
	connect(resultTreeWidget, SIGNAL(updateTabTitle(QString, QWidget*)), this, SLOT(updateTabTitle(QString, QWidget*)));

	m_keyList.append(key);
	addTab(resultTreeWidget, key);
	setCurrentIndex(indexOf(resultTreeWidget));
}

void ResultTabWidget::on_resultTabWidget_tabCloseRequested(int index)
{
	ResultTreeWidget *resultTreeWidget = qobject_cast<ResultTreeWidget*>(widget(index));
	if(resultTreeWidget != NULL)
	{
		m_keyList.removeAll(resultTreeWidget->m_key);
		removeTab(index); 
		delete resultTreeWidget;
	}
}

void ResultTabWidget::on_resultTabWidget_currentChanged(int index)
{
	Q_UNUSED(index)

	ResultTreeWidget *resultTreeWidget = qobject_cast<ResultTreeWidget *>(currentWidget());
	if(resultTreeWidget != NULL)
	{
		resultTreeWidget->FilterResult(m_nameFilter, m_typeFilter);
	}
}

void ResultTabWidget::SetFilter(QString nameFilter, QStringList typeFilter)
{
	if(m_nameFilter != nameFilter || m_typeFilter != typeFilter)
	{
		m_nameFilter = nameFilter;
		m_typeFilter = typeFilter;

		ResultTreeWidget *resultTreeWidget = qobject_cast<ResultTreeWidget *>(currentWidget());
		if(resultTreeWidget != NULL)
		{
			resultTreeWidget->FilterResult(m_nameFilter, m_typeFilter);
		}
	}

}

void ResultTabWidget::updateTabTitle(QString title, QWidget *widget)
{
	int index = indexOf(widget);
	if(index != -1)
	{
		setTabText(index, title);
	}
}
