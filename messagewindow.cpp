#include "stdafx.h"
#include "messagewindow.h"  
#include "titlebar.h"
#include "content.h"
#include "statusbar.h"

MessageWindow& MessageWindow::GetInstance()
{
	static MessageWindow instance;
	return instance;
}

MessageWindow::MessageWindow(QFrame *parent)
	: BaseWindow(parent)
{
	setObjectName("messageWindow");

	setFixedSize(MESSAGE_WINDOW_WIDTH, MESSAGE_WINDOW_HEIGHT);

	InitControl();
}

MessageWindow::~MessageWindow()
{

}

void MessageWindow::paintEvent(QPaintEvent *event)
{
	//先显示阴影
	BaseWindow::paintEvent(event);

	//再显示背景
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawPixmap(QRect(SHADOW_WIDTH, SHADOW_WIDTH, width()-2*SHADOW_WIDTH, height()-2*SHADOW_WIDTH), QPixmap(g_skinName));
}

bool MessageWindow::eventFilter(QObject *obj, QEvent *event)
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

bool MessageWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	//禁用鼠标拖拽改变大小
	return QFrame::nativeEvent(eventType, message, result);
}

void MessageWindow::InitControl()
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
	connect(m_pTitleBar->m_pCloseButton, SIGNAL(clicked()), this, SLOT(reject()));

	//设置提示图片
	m_pImageLabel = new QLabel();
	m_pImageLabel->setFixedSize(45, 45);
	m_pImageLabel->setScaledContents(true);

	//设置提示信息，让QLabel能够自动判断并换行显示
	m_pMessageLabel = new QLabel();
	m_pMessageLabel->setObjectName("messageLabel");
	m_pMessageLabel->setWordWrap(true);
	m_pMessageLabel->setOpenExternalLinks(true);

	QHBoxLayout *centerLayout = new QHBoxLayout();
	centerLayout->addWidget(m_pImageLabel);
	centerLayout->addWidget(m_pMessageLabel);
	centerLayout->setSpacing(10);
	centerLayout->setContentsMargins(15, 0, 15, 0);

	m_pOKButton = new QPushButton();
	m_pOKButton->setObjectName("okButton");
	m_pOKButton->setText(tr("OK"));
	m_pOKButton->setFixedSize(60, 25);
	connect(m_pOKButton, SIGNAL(clicked()), this, SLOT(accept()));

	m_pCancelBbutton = new QPushButton();
	m_pCancelBbutton->setObjectName("cancelButton");
	m_pCancelBbutton->setText(tr("Cancel"));
	m_pCancelBbutton->setFixedSize(60, 25);
	connect(m_pCancelBbutton, SIGNAL(clicked()), this, SLOT(reject()));

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(m_pOKButton);
	buttonLayout->addWidget(m_pCancelBbutton);
	buttonLayout->setSpacing(10);
	buttonLayout->setContentsMargins(0, 0, 10, 10);

	QVBoxLayout *contentLayout = new QVBoxLayout();
	contentLayout->setSpacing(0);
	contentLayout->setContentsMargins(0, 20, 0, 20);
	contentLayout->addLayout(centerLayout);
	contentLayout->addStretch();
	contentLayout->addLayout(buttonLayout);
	m_pContent->setLayout(contentLayout);
}

bool MessageWindow::ShowMessage(QString title, QString content, bool isOkCancel)
{
	m_pTitleBar->m_pTitleLabel->setText(title);
	m_pMessageLabel->setText(content);
	m_pImageLabel->setPixmap(QPixmap(":/Image/attention.png"));

	//是否隐藏取消按钮
	m_pCancelBbutton->setVisible(isOkCancel);

	return exec();
}

void MessageWindow::closeEvent(QCloseEvent * event)
{
	Q_UNUSED(event);

	if(m_isAccept)
	{
		accept();
	}
	else
	{
		reject();
	}
}

void MessageWindow::accept()
{
	m_isAccept = true;

	m_loop.exit();
}

void MessageWindow::reject()
{
	m_isAccept = false;

	m_loop.exit();
}

bool MessageWindow::exec()
{
	setWindowModality(Qt::ApplicationModal);
	show();

	m_loop.exec(QEventLoop::DialogExec);

	close();
	return m_isAccept;
}