#ifndef CHANGESKINWIDGET_H
#define CHANGESKINWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QPainter>
#include <QPen>

class ChangeSkinWidget : public QWidget
{
	Q_OBJECT

public:
	ChangeSkinWidget(QWidget *parent = 0);
	~ChangeSkinWidget();
protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);
	void paintEvent(QPaintEvent *event);
private:
	void InitControl();

	QLabel *m_pSkinImageLabel; //显示皮肤
	QLabel *m_pSkinNameLabel; //显示皮肤名称
	QPushButton *m_pUseSkinPushButton; //使用此皮肤按钮

	QString m_pixmapName;
	bool m_mousePress;
	bool m_mouseEnter;
public slots:
	void showSkin(QString pixmapName, QString skinName);
private slots:
	void on_useSkinPushButton_clicked();
signals:
	void useSkin(QString skinName);
};

#endif  //CHANGESKINWIDGET_H