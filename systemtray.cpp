#include "stdafx.h"
#include "systemtray.h"

SystemTray::SystemTray(QWidget *parent)
	: QSystemTrayIcon(parent)
{
	setToolTip(APP_TITLE);

	QFileInfo fileInfo(QApplication::applicationFilePath());
	QFileIconProvider fileIcon;
	QIcon icon = fileIcon.icon(fileInfo);
	setIcon(icon);

	InitControl();
}

SystemTray::~SystemTray()
{

}

void SystemTray::InitControl()
{
	m_pTrayMenu = new QMenu();
	m_pTrayMenu->setObjectName("trayMenu");
	setContextMenu(m_pTrayMenu);
	connect(this , SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(on_trayMenu_activated(QSystemTrayIcon::ActivationReason)));
	
	//初始化顶部
	m_pTopWidget = new QWidget();
	m_pTopWidgetAction = new QWidgetAction(m_pTrayMenu);
	m_pTitleLabel = new QLabel();
	m_pTitleLabel->setObjectName("whiteLabel");
	m_pTitleLabel->setText(APP_TITLE);

	QVBoxLayout *topLayout = new QVBoxLayout();
	topLayout->addWidget(m_pTitleLabel);
	topLayout->setSpacing(10);
	topLayout->setContentsMargins(10, 7, 10, 7);

	m_pTopWidget->setLayout(topLayout);
	m_pTopWidgetAction->setDefaultWidget(m_pTopWidget);
	m_pTopWidget->installEventFilter(this);

	//初始化菜单
	m_pOpenAction = new QAction(tr("Open"), this);
	connect(m_pOpenAction, SIGNAL(triggered()), this, SIGNAL(showWidget()));

	m_pClosePlayWindowAction = new QAction(tr("Close player window"), this);
	connect(m_pClosePlayWindowAction, SIGNAL(triggered()), this, SIGNAL(closePlayWindow()));

	m_pCloseAction = new QAction(tr("Exit"), this);
	connect(m_pCloseAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	//初始化底部
	m_pBottomWidget = new QWidget();
	m_pBottomWidgetAction = new QWidgetAction(m_pTrayMenu);

    m_pEliteModePushButton = new QPushButton();
	m_pEliteModePushButton->setObjectName("trayButton");
	m_pEliteModePushButton->setText(tr("Lite mode"));
	m_pEliteModePushButton->setCursor(Qt::PointingHandCursor);
	m_pEliteModePushButton->setVisible(false);

    m_pFullModePushButton = new QPushButton();
	m_pFullModePushButton->setObjectName("trayButton");
	m_pFullModePushButton->setText(tr("Full mode"));
	m_pFullModePushButton->setCursor(Qt::PointingHandCursor);
	m_pFullModePushButton->setVisible(false);

	QHBoxLayout *bottomLayout = new QHBoxLayout();
	bottomLayout->addWidget(m_pEliteModePushButton);
	bottomLayout->addStretch();
	bottomLayout->addWidget(m_pFullModePushButton);
	bottomLayout->setSpacing(0);
	bottomLayout->setContentsMargins(10, 7, 10, 7);

	m_pBottomWidget->setLayout(bottomLayout);
	m_pBottomWidgetAction->setDefaultWidget(m_pBottomWidget);
	m_pBottomWidget->installEventFilter(this);

	//添加菜单项
	m_pTrayMenu->addAction(m_pTopWidgetAction);
	m_pTrayMenu->addAction(m_pOpenAction);
	m_pTrayMenu->addSeparator();
	m_pTrayMenu->addAction(m_pClosePlayWindowAction);
	m_pTrayMenu->addSeparator();
	m_pTrayMenu->addAction(m_pCloseAction);
	m_pTrayMenu->addAction(m_pBottomWidgetAction);
}

bool SystemTray::eventFilter(QObject *obj, QEvent *event)
{
	if(obj == m_pTopWidget || obj == m_pBottomWidget)
	{
		if(event->type() == QEvent::Paint)
		{
			QPixmap pixmap = QPixmap(g_skinName);
			QRect pixmapRect = pixmap.rect();

			QPainter topPainter(m_pTopWidget);
			//topPainter.setPen(Qt::NoPen);
			//topPainter.setBrush(QColor(70, 180, 20));
			//topPainter.drawRect(m_pTopWidget->rect());
			QRect topRect = m_pTopWidget->rect();
			topPainter.drawPixmap(m_pTopWidget->rect(), pixmap, topRect);

			QPainter bottomPainter(m_pBottomWidget);
			//bottomPainter.setPen(Qt::NoPen);
			//bottomPainter.setBrush(QColor(70, 180, 20));
			//bottomPainter.drawRect(m_pBottomWidget->rect());
			QRect bottomRect(0, pixmapRect.height() - m_pBottomWidget->height(), m_pBottomWidget->width(), m_pBottomWidget->height());
			bottomPainter.drawPixmap(m_pBottomWidget->rect(), pixmap, bottomRect);
		}
	}

	return QSystemTrayIcon::eventFilter(obj, event);
}

void SystemTray::on_trayMenu_activated(QSystemTrayIcon::ActivationReason reason)
{
	Q_UNUSED(reason)

	QWidget *widget = qobject_cast<QWidget *>(parent());
	if(widget)
	{
		m_pOpenAction->setText(widget->isHidden() ? tr("Show") : tr("Hide"));
	}
}