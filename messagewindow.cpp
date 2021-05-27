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
	//����ʾ��Ӱ
	BaseWindow::paintEvent(event);

	//����ʾ����
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
	//���������ק�ı��С
	return QFrame::nativeEvent(eventType, message, result);
}

void MessageWindow::InitControl()
{
	//����������
	m_pTitleBar = new TitleBar(this);
	//������������
	m_pContent = new Content(this);
	//����״̬��
	m_pStatusBar = new StatusBar(this);
	m_pStatusBar->setVisible(false);

	//��������
	QVBoxLayout *layout = new QVBoxLayout();
	//���ü�����Ե�հ�
	layout->setSpacing(0);
	layout->setContentsMargins(SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH);
	//���������뵽������
	layout->addWidget(m_pTitleBar);
	layout->addWidget(m_pContent);
	layout->addWidget(m_pStatusBar);
	setLayout(layout);

	//���ذ�ť
	m_pTitleBar->m_pSkinButton->setVisible(false);
	m_pTitleBar->m_pCloudPlayButton->setVisible(false);
	m_pTitleBar->m_pMiniPortalButton->setVisible(false);
	m_pTitleBar->m_pMenuButton->setVisible(false);
	m_pTitleBar->m_pMinButton->setVisible(false);
	m_pTitleBar->m_pMaxButton->setVisible(false);
	m_pTitleBar->installEventFilter(this);
	connect(m_pTitleBar->m_pCloseButton, SIGNAL(clicked()), this, SLOT(reject()));

	//������ʾͼƬ
	m_pImageLabel = new QLabel();
	m_pImageLabel->setFixedSize(45, 45);
	m_pImageLabel->setScaledContents(true);

	//������ʾ��Ϣ����QLabel�ܹ��Զ��жϲ�������ʾ
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

	//�Ƿ�����ȡ����ť
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