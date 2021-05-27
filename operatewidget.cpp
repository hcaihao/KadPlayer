#include "StdAfx.h"
#include "operatewidget.h"
#include "playwindow.h"
#include "messagewindow.h"

OperateWidget::OperateWidget(QWidget *parent)
	: QWidget(parent)
{
	InitControl();
}

OperateWidget::~OperateWidget()
{

}

void OperateWidget::InitControl()
{
	m_pPlayPushButton = new QPushButton();
	m_pPlayPushButton->setObjectName("blueButton");
	m_pPlayPushButton->setText(tr("Play"));
	m_pPlayPushButton->setFixedWidth(50);
	m_pPlayPushButton->setCursor(Qt::PointingHandCursor);
	connect(m_pPlayPushButton, SIGNAL(clicked()), this, SLOT(on_playPushButton_clicked()));

	m_pDownloadPushButton = new QPushButton();
	m_pDownloadPushButton->setObjectName("blueButton");
	m_pDownloadPushButton->setText(tr("Download"));
	m_pDownloadPushButton->setFixedWidth(50);
	m_pDownloadPushButton->setCursor(Qt::PointingHandCursor);
	connect(m_pDownloadPushButton, SIGNAL(clicked()), this, SLOT(on_downloadPushButton_clicked()));

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(10, 0, 10, 0);
	layout->addWidget(m_pPlayPushButton);
	layout->addStretch();
	layout->addWidget(m_pDownloadPushButton);
	setLayout(layout);
}

void OperateWidget::SetInfo(const KadResult& kadResult, bool isVideo)
{
	m_kadResult = kadResult;

	m_pPlayPushButton->setVisible(isVideo);
}

void OperateWidget::on_playPushButton_clicked()
{
	QStringList list;
	list << m_kadResult.Ed2k;

	emit playLink(list);
}

void OperateWidget::on_downloadPushButton_clicked()
{
	QStringList list;
	list << m_kadResult.Ed2k;

	emit downloadLink(list);
}