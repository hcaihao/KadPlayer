#include "stdafx.h"
#include "feedbackwindow.h"
#include "titlebar.h"
#include "content.h"
#include "statusbar.h"
#include "changeskinwidget.h"
#include "servicemanager.h"
#include "messagewindow.h"

extern ServiceManager *g_pServiceManager;

FeedbackWindow::FeedbackWindow(QFrame *parent)
    : BaseWindow(parent)
{
	setObjectName("feedbackWindow");

	setFixedSize(FEEDBACK_WINDOW_WIDTH, FEEDBACK_WINDOW_HEIGHT);

	InitControl();
}

FeedbackWindow::~FeedbackWindow()
{

}

void FeedbackWindow::paintEvent(QPaintEvent *event)
{
	//先显示阴影
	BaseWindow::paintEvent(event);

	//再显示背景
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawPixmap(QRect(SHADOW_WIDTH, SHADOW_WIDTH, width()-2*SHADOW_WIDTH, height()-2*SHADOW_WIDTH), QPixmap(g_skinName));
}

bool FeedbackWindow::eventFilter(QObject *obj, QEvent *event)
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

bool FeedbackWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	//禁用鼠标拖拽改变大小
	return QFrame::nativeEvent(eventType, message, result);
}

void FeedbackWindow::InitControl()
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

	QGroupBox *feedbackGroupBox = new QGroupBox(tr("Feedback"), this);

	m_pFeedbackContentPlainTextEdit = new QPlainTextEdit();
	m_pFeedbackContentPlainTextEdit->setObjectName("feedbackContentPlainTextEdit");
	m_pFeedbackContentPlainTextEdit->setFixedHeight(120);

	QRadioButton *bugRadioButton = new QRadioButton();
	bugRadioButton->setText(tr("Bug report"));

	QRadioButton *sexRadioButton = new QRadioButton();
	sexRadioButton->setText(tr("Pornographic resource"));

	QRadioButton *pirateRadioButton = new QRadioButton();
	pirateRadioButton->setText(tr("Piratical resource"));

	m_pFeedbackTypeButtonGroup = new QButtonGroup(this);
	m_pFeedbackTypeButtonGroup->addButton(bugRadioButton, FEEDBACK_BUG);
	m_pFeedbackTypeButtonGroup->addButton(sexRadioButton, FEEDBACK_SEX);
	m_pFeedbackTypeButtonGroup->addButton(pirateRadioButton, FEEDBACK_PIRATE);
	m_pFeedbackTypeButtonGroup->button(FEEDBACK_BUG)->setChecked(true);
	connect(m_pFeedbackTypeButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(on_feedbackTypeButtonGroup_buttonClicked(int)));

	QHBoxLayout *feedbackTypeLayout = new QHBoxLayout();
	feedbackTypeLayout->setSpacing(10);
	feedbackTypeLayout->setContentsMargins(0, 0, 0, 0);
	feedbackTypeLayout->addWidget(bugRadioButton);
	feedbackTypeLayout->addWidget(sexRadioButton);
	feedbackTypeLayout->addWidget(pirateRadioButton);
	feedbackTypeLayout->addStretch();


	QFormLayout *feedbackLayout = new QFormLayout();
	feedbackLayout->setSpacing(10);
	feedbackLayout->setContentsMargins(40, 30, 40, 30);
	feedbackLayout->addRow(feedbackTypeLayout);
	feedbackLayout->addRow(m_pFeedbackContentPlainTextEdit);
	feedbackGroupBox->setLayout(feedbackLayout);


	QVBoxLayout *centerLayout = new QVBoxLayout();
	centerLayout->setSpacing(10);
	centerLayout->setContentsMargins(40, 30, 40, 30);
	centerLayout->addWidget(feedbackGroupBox);

	m_pOKButton = new QPushButton();
	m_pOKButton->setObjectName("okButton");
	m_pOKButton->setText(tr("OK"));
	m_pOKButton->setFixedSize(60, 25);
	connect(m_pOKButton, SIGNAL(clicked()), this, SLOT(on_okButton_cliecked()));

	m_pCancelBbutton = new QPushButton();
	m_pCancelBbutton->setObjectName("cancelButton");
	m_pCancelBbutton->setText(tr("Cancel"));
	m_pCancelBbutton->setFixedSize(60, 25);
	connect(m_pCancelBbutton, SIGNAL(clicked()), this, SLOT(on_cancelButton_cliecked()));

	QHBoxLayout *bottomLayout = new QHBoxLayout();
	bottomLayout->setSpacing(10);
	bottomLayout->setContentsMargins(20, 0, 20, 0);
	bottomLayout->addStretch();
	bottomLayout->addWidget(m_pOKButton);
	bottomLayout->addWidget(m_pCancelBbutton);

	QVBoxLayout *contentLayout = new QVBoxLayout();
	contentLayout->setSpacing(20);
	contentLayout->setContentsMargins(0, 0, 0, 0);
	contentLayout->addLayout(centerLayout);
	contentLayout->addLayout(bottomLayout);
	contentLayout->addStretch();
	m_pContent->setLayout(contentLayout);
}

void FeedbackWindow::SetInfo(int feedbackType, QString content)
{
	QList<QAbstractButton*> list = m_pFeedbackTypeButtonGroup->buttons();
	foreach(QAbstractButton* button, list)
	{
		if(m_pFeedbackTypeButtonGroup->id(button) == feedbackType)
		{
			button->setVisible(true);
			button->setChecked(true);
		}
		else
		{
			button->setVisible(false);
			button->setChecked(false);
		}
	}

	m_pFeedbackContentPlainTextEdit->setPlainText(content);
}

void FeedbackWindow::on_feedbackTypeButtonGroup_buttonClicked(int id)
{
	Q_UNUSED(id);
}

void FeedbackWindow::on_okButton_cliecked()
{
	int type = m_pFeedbackTypeButtonGroup->checkedId();
	QString content = m_pFeedbackContentPlainTextEdit->toPlainText().trimmed();

	if(content.isEmpty())
	{
		MessageWindow::GetInstance().ShowMessage(tr("Notice"), tr("Please fill out the feedback."));
	}
	else
	{
		QMetaObject::invokeMethod(g_pServiceManager, "SetFeedback", Qt::QueuedConnection, Q_ARG(QString, APP_VERSION_STR), Q_ARG(int, type), Q_ARG(QString, content));
		close();
	}
}

void FeedbackWindow::on_cancelButton_cliecked()
{
	close();
}
