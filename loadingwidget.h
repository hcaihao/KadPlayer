#ifndef LOADINGWIDGET_H
#define LOADINGWIDGET_H

#include <QWidget>
#include <QLabel>  
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>

class LoadingWidget : public QWidget
{
	Q_OBJECT
public:

	explicit LoadingWidget(QWidget *parent = 0);
	~LoadingWidget();

	void Start();
	void Stop();
private:
	int m_index;
	QLabel *m_pLoadingLabel;
	QTimer *m_pTimer;

	void InitControl();
private slots:
	void updatePixmap();
};

#endif //LOADINGWIDGET_H