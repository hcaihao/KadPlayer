#include "stdafx.h"
#include "basewindow.h"

BaseWindow::BaseWindow(QFrame *parent)
	: QFrame(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	setAttribute(Qt::WA_TranslucentBackground);	//http://blog.sina.com.cn/s/blog_ec6748cf0101hg2t.html

	//初始化为未按下鼠标左键
	m_pressMouse = false;
}

BaseWindow::~BaseWindow()
{

}

void BaseWindow::paintEvent(QPaintEvent *event)
{
	//绘制10个像素的阴影，内部填充白色
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);
	path.addRect(SHADOW_WIDTH, SHADOW_WIDTH, width()-SHADOW_WIDTH*2, height()-SHADOW_WIDTH*2);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.fillPath(path, QBrush(Qt::white));

	QColor color(0, 0, 0);
	for(int i=0; i<SHADOW_WIDTH; i++)
	{
		QPainterPath path;
		path.setFillRule(Qt::WindingFill);
		path.addRect(SHADOW_WIDTH-i, SHADOW_WIDTH-i, width()-(SHADOW_WIDTH-i)*2, height()-(SHADOW_WIDTH-i)*2);
		color.setAlpha(100 - qSqrt(i)*50);	//根据SHADOW_WIDTH调整
		painter.setPen(color);
		painter.drawPath(path);
	}

	QFrame::paintEvent(event);
}

bool BaseWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	MSG* msg = reinterpret_cast<MSG*>(message);

	switch(msg->message)
	{
	case WM_NCHITTEST:
		{
			int xPos = GET_X_LPARAM(msg->lParam) - frameGeometry().x();
			int yPos = GET_Y_LPARAM(msg->lParam) - frameGeometry().y();

			if(childAt(xPos, yPos) == NULL)
			{
				*result = HTCAPTION;	// 伪装成点击标题栏
			}
			else
			{
				return false;
			}

			if(xPos > BORDER_MIN_WIDTH && xPos < BORDER_MAX_WIDTH)
			{
				*result = HTLEFT;
			}
			if(xPos > (width() - BORDER_MAX_WIDTH) && xPos < (width() - BORDER_MIN_WIDTH))
			{
				*result = HTRIGHT;
			}
			if(yPos > BORDER_MIN_WIDTH && yPos < BORDER_MAX_WIDTH)
			{
				*result = HTTOP;
			}
			if(yPos > (height() - BORDER_MAX_WIDTH) && yPos < (height() - BORDER_MIN_WIDTH))
			{
				*result = HTBOTTOM;
			}
			if(xPos > BORDER_MIN_WIDTH && xPos < BORDER_MAX_WIDTH && yPos > BORDER_MIN_WIDTH && yPos < BORDER_MAX_WIDTH)
			{
				*result = HTTOPLEFT;
			}
			if(xPos > (width() - BORDER_MAX_WIDTH) && xPos < (width() - BORDER_MIN_WIDTH) && yPos > BORDER_MIN_WIDTH && yPos < BORDER_MAX_WIDTH)
			{
				*result = HTTOPRIGHT;
			}
			if(xPos > BORDER_MIN_WIDTH && xPos < BORDER_MAX_WIDTH && yPos > (height() - BORDER_MAX_WIDTH) && yPos < (height() - BORDER_MIN_WIDTH))
			{
				*result = HTBOTTOMLEFT;
			}
			if(xPos > (width() - BORDER_MAX_WIDTH) && xPos < (width() - BORDER_MIN_WIDTH) && yPos > (height() - BORDER_MAX_WIDTH) && yPos < (height() - BORDER_MIN_WIDTH))
			{
				*result = HTBOTTOMRIGHT;
			}

			return true;
		}
	}

	return QFrame::nativeEvent(eventType, message, result);
}

void BaseWindow::mousePressEvent(QMouseEvent *event)
{
	//只能是鼠标左键移动和改变大小
	if(event->button() == Qt::LeftButton) 
	{
		m_pressMouse = true;
	}

	//窗口移动距离
	m_movePos = event->globalPos() - pos(); 
}

void BaseWindow::mouseReleaseEvent(QMouseEvent *)
{
	m_pressMouse = false;
}

void BaseWindow::mouseMoveEvent(QMouseEvent *event)
{
	//移动窗口
	if(m_pressMouse)   
	{
		QPoint movePos = event->globalPos();
		move(movePos - m_movePos);
	}
}