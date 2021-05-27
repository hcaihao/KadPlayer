#include "StdAfx.h"
#include "loadingwidget.h"

LoadingWidget::LoadingWidget(QWidget *parent)
	: QWidget(parent)
{
	m_index = 1;

	InitControl();
}

LoadingWidget::~LoadingWidget()
{

}

void LoadingWidget::InitControl()
{
	m_pTimer = new QTimer(this);
	m_pTimer->setInterval(100);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updatePixmap()));

	m_pLoadingLabel = new QLabel(this);

	QPixmap pixmap(QString(":/Image/loading_%1.png").arg(m_index));
	m_pLoadingLabel->setFixedSize(pixmap.size());
	m_pLoadingLabel->setPixmap(pixmap);
	m_pLoadingLabel->setScaledContents(true);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addStretch();
	layout->addWidget(m_pLoadingLabel);
	layout->addStretch();
	setLayout(layout);
}

void LoadingWidget::Start()
{
	m_index = 1;

	m_pTimer->start();
}

void LoadingWidget::Stop()
{
	m_pTimer->stop();

	m_pLoadingLabel->setPixmap(QPixmap());
}

void LoadingWidget::updatePixmap()
{
	QPixmap pixmap(QString(":/Image/loading_%1.png").arg(m_index));
	m_pLoadingLabel->setPixmap(pixmap);

	m_index++;
	if(m_index > 8)
	{
		m_index = 1;
	}
}