#ifndef RESULTTABWIDGET_H
#define RESULTTABWIDGET_H

#include <QTabWidget>

class ResultTreeWidget;
class WebAxWidget;

class ResultTabWidget : public QTabWidget
{
	Q_OBJECT

public:
	ResultTabWidget(QWidget *parent);
	~ResultTabWidget();

	void AddTab(QString key);
	void SetFilter(QString nameFilter, QStringList typeFilter);

	QStringList m_keyList;
protected:
	bool eventFilter(QObject *obj, QEvent *event);
	bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private:
	void InitControl();
	WebAxWidget *m_pWebAxWidget;
	
	QStringList m_typeFilter;
	QString m_nameFilter;
private slots:
	void on_resultTabWidget_tabCloseRequested(int index);
	void on_resultTabWidget_currentChanged(int index);
	void updateTabTitle(QString title, QWidget *widget);
};

#endif // RESULTTABWIDGET_H
