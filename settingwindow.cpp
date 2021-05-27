#include "stdafx.h"
#include "settingwindow.h"
#include "titlebar.h"
#include "content.h"
#include "statusbar.h"
#include "utilitymanager.h"

SettingWindow::SettingWindow(QFrame *parent)
    : BaseWindow(parent)
{
	setObjectName("settingWindow");

    setFixedSize(SETTING_WINDOW_WIDTH, SETTING_WINDOW_HEIGHT);

	InitControl();
}

SettingWindow::~SettingWindow()
{

}

void SettingWindow::paintEvent(QPaintEvent *event)
{
	//先显示阴影
	BaseWindow::paintEvent(event);

	//再显示背景
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawPixmap(QRect(SHADOW_WIDTH, SHADOW_WIDTH, width()-2*SHADOW_WIDTH, height()-2*SHADOW_WIDTH), QPixmap(g_skinName));
}

bool SettingWindow::eventFilter(QObject *obj, QEvent *event)
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

bool SettingWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	//禁用鼠标拖拽改变大小
	return QFrame::nativeEvent(eventType, message, result);
}

void SettingWindow::InitControl()
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
	connect(m_pTitleBar->m_pCloseButton, SIGNAL(clicked()), this, SLOT(on_cancelButton_cliecked()));


	QGroupBox *playSettingGroupBox = new QGroupBox(tr("Play channel"), this);
#ifdef KAD_SEARCHER
	playSettingGroupBox->hide();
#endif

	QRadioButton *thunderRadioButton = new QRadioButton();
	thunderRadioButton->setText(tr("Thunder channel(Need platinum VIP, common user can preview)"));

	QRadioButton *qqRadioButton = new QRadioButton();
	qqRadioButton->setText(tr("QQ channel(Opening soon)"));

	QRadioButton *otherRadioButton = new QRadioButton();
	otherRadioButton->setText(tr("Free channel(free, but not stable)"));

	m_pChannelButtonGroup = new QButtonGroup(this);
	m_pChannelButtonGroup->addButton(thunderRadioButton, CHANNEL_THUNDER);
	m_pChannelButtonGroup->addButton(qqRadioButton, CHANNEL_QQ);
	m_pChannelButtonGroup->addButton(otherRadioButton, CHANNEL_OTHER);
	m_pChannelButtonGroup->button(g_playChannel)->setChecked(true);


	QVBoxLayout *playSettingLayout = new QVBoxLayout();
	playSettingLayout->setSpacing(20);
	playSettingLayout->setContentsMargins(40, 20, 40, 20);
	playSettingLayout->addWidget(thunderRadioButton);
	playSettingLayout->addWidget(qqRadioButton);
	playSettingLayout->addWidget(otherRadioButton);
	playSettingGroupBox->setLayout(playSettingLayout);


	QGroupBox *systemSettingGroupBox = new QGroupBox(tr("System Setting"), this);

	m_pAutoStartCheckBox = new QCheckBox();
	m_pAutoStartCheckBox->setObjectName("autoStartCheckBox");
	m_pAutoStartCheckBox->setChecked(g_isAutoStart);
	m_pAutoStartCheckBox->setText(tr("Auto startup"));
	connect(m_pAutoStartCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_autoStartCheckBox_clicked(bool)));

	m_pSaveHistoryCheckBox = new QCheckBox();
	m_pSaveHistoryCheckBox->setObjectName("saveHistoryCheckBox");
	m_pSaveHistoryCheckBox->setChecked(g_isSaveHistory);
	m_pSaveHistoryCheckBox->setText(tr("Save search keywords"));
	connect(m_pSaveHistoryCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_saveHistoryCheckBox_clicked(bool)));

	QLabel *clickSettingLabel = new QLabel();
	clickSettingLabel->setObjectName("blackLabel");
	clickSettingLabel->setText(tr("Double click:"));

	QRadioButton *clickPlayRadioButton = new QRadioButton();
	clickPlayRadioButton->setText(tr("Play"));
	clickPlayRadioButton->setChecked(g_doubleClick == CLICK_PLAY);
#ifdef KAD_SEARCHER
	clickPlayRadioButton->hide();
#endif

	QRadioButton *clickDownloadRadioButton = new QRadioButton();
	clickDownloadRadioButton->setText(tr("Download"));
	clickDownloadRadioButton->setChecked(g_doubleClick == CLICK_DOWNLOAD);

	QRadioButton *clickCopyRadioButton = new QRadioButton();
	clickCopyRadioButton->setText(tr("Copy"));
	clickCopyRadioButton->setChecked(g_doubleClick == CLICK_COPY);

	m_pClickButtonGroup = new QButtonGroup(this);
	m_pClickButtonGroup->addButton(clickPlayRadioButton, CLICK_PLAY);
	m_pClickButtonGroup->addButton(clickDownloadRadioButton, CLICK_DOWNLOAD);
	m_pClickButtonGroup->addButton(clickCopyRadioButton, CLICK_COPY);
	m_pClickButtonGroup->button(g_doubleClick)->setChecked(true);

	QHBoxLayout *clickSettingLayout = new QHBoxLayout();
	clickSettingLayout->setSpacing(10);
	clickSettingLayout->setContentsMargins(0, 0, 0, 0);
	clickSettingLayout->addWidget(clickSettingLabel);
	clickSettingLayout->addWidget(clickPlayRadioButton);
	clickSettingLayout->addWidget(clickDownloadRadioButton);
	clickSettingLayout->addWidget(clickCopyRadioButton);
	clickSettingLayout->addStretch();

	QVBoxLayout *systemSettingLayout = new QVBoxLayout();
	systemSettingLayout->setSpacing(20);
	systemSettingLayout->setContentsMargins(40, 20, 40, 20);
	systemSettingLayout->addWidget(m_pAutoStartCheckBox);
	systemSettingLayout->addWidget(m_pSaveHistoryCheckBox);
	systemSettingLayout->addLayout(clickSettingLayout);
	systemSettingGroupBox->setLayout(systemSettingLayout);


	QVBoxLayout *centerLayout = new QVBoxLayout();
	centerLayout->setSpacing(10);
	centerLayout->setContentsMargins(40, 30, 40, 30);
	centerLayout->addWidget(playSettingGroupBox);
	centerLayout->addWidget(systemSettingGroupBox);



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
	contentLayout->setSpacing(10);
	contentLayout->setContentsMargins(0, 0, 0, 0);
	contentLayout->addLayout(centerLayout);
	contentLayout->addLayout(bottomLayout);
	contentLayout->addStretch();
	m_pContent->setLayout(contentLayout);
}

void SettingWindow::on_okButton_cliecked()
{
	g_isAutoStart = m_pAutoStartCheckBox->isChecked();
	g_isSaveHistory = m_pSaveHistoryCheckBox->isChecked();
	g_playChannel = m_pChannelButtonGroup->checkedId();
	g_doubleClick = m_pClickButtonGroup->checkedId();

	UtilityManager::SetAsAutoRun(APP_TITLE, g_isAutoStart);

	close();
}

void SettingWindow::on_cancelButton_cliecked()
{
	m_pAutoStartCheckBox->setChecked(g_isAutoStart);
	m_pSaveHistoryCheckBox->setChecked(g_isSaveHistory);
	m_pChannelButtonGroup->button(g_playChannel)->setChecked(true);

	close();
}