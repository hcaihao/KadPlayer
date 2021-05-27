#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QtGui>
#include <QFrame>

class BaseWindow : public QFrame
{
	Q_OBJECT

public:
	BaseWindow(QFrame *parent);
	~BaseWindow();

protected:
    void paintEvent(QPaintEvent *event);
	bool nativeEvent(const QByteArray &eventType, void *message, long *result);

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
private:
	QPoint m_movePos; //移动的距离
	bool m_pressMouse; //按下鼠标左键
};

#endif // BASEWINDOW_H
