#include "stdafx.h"
#include "changeskinwidget.h"

ChangeSkinWidget::ChangeSkinWidget(QWidget *parent)
	: QWidget(parent)
{
	setFixedSize(CHANGE_SKIN_WIDGET_WIDTH, CHANGE_SKIN_WIDGET_HEIGHT);

	m_mousePress = false;
	m_mouseEnter = false;

	InitControl();
}

ChangeSkinWidget::~ChangeSkinWidget()
{

}

void ChangeSkinWidget::InitControl()
{
	setCursor(Qt::PointingHandCursor);

	m_pSkinImageLabel = new QLabel();
	m_pSkinImageLabel->setScaledContents(true); 
	m_pSkinImageLabel->setFixedSize(100, 65);
	
	m_pSkinNameLabel = new QLabel();
	m_pSkinNameLabel->setObjectName("blackLabel");

	m_pUseSkinPushButton = new QPushButton();
	m_pUseSkinPushButton->setObjectName("useSkinButton");
	m_pUseSkinPushButton->setText(tr("Use skin"));
	m_pUseSkinPushButton->setFixedSize(85, 25);
	connect(m_pUseSkinPushButton, SIGNAL(clicked()), this, SLOT(on_useSkinPushButton_clicked()));

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(m_pSkinImageLabel, 0, Qt::AlignCenter);
	layout->addWidget(m_pSkinNameLabel, 0, Qt::AlignCenter);
	layout->addWidget(m_pUseSkinPushButton, 0, Qt::AlignCenter);
	layout->setSpacing(5);
	layout->setContentsMargins(0, 10, 0, 10);
	setLayout(layout);
}

void ChangeSkinWidget::showSkin(QString pixmapName, QString skinName)
{
	m_pixmapName = pixmapName;

	//����Ƥ������
	QPixmap pixmap(m_pixmapName);
	m_pSkinImageLabel->setPixmap(pixmap);

	//����Ƥ������
	m_pSkinNameLabel->setText(skinName);
}

void ChangeSkinWidget::on_useSkinPushButton_clicked()
{
	emit useSkin(m_pixmapName);
}

void ChangeSkinWidget::paintEvent(QPaintEvent *)
{
	if(m_mouseEnter)
	{
		//���Ʊ߿� 
		QPainter painter(this);
		QPen pen(QColor(210, 225, 230)); 
		painter.setPen(pen); 
		painter.drawRoundRect(0,0,width()-1, height()-1, 5, 5); 
	}
}

void ChangeSkinWidget::mousePressEvent(QMouseEvent * event)
{
	//ֻ����������
	if(event->button() == Qt::LeftButton) 
	{
		m_mousePress = true;
	}
}

void ChangeSkinWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(m_mousePress && (rect()).contains(event->pos()))
	{
		emit useSkin(m_pixmapName);
	}
	m_mousePress = false;
}

void ChangeSkinWidget::enterEvent(QEvent *)
{
	m_mouseEnter = true;
	update();
}

void ChangeSkinWidget::leaveEvent(QEvent *)
{
	m_mouseEnter = false;
	update();
}