#ifndef WEBAXWIDGET_H
#define WEBAXWIDGET_H

#include <ActiveQt\QAxWidget>
//#include <../../qtactiveqt/include/ActiveQt/qaxwidget.h>

class Signaler : public QObject
{
	Q_OBJECT
public:
	void EmitSearchKey(QString key) { emit searchKey(key); };
signals:
	void searchKey(QString key);
};


// You can subclass QAxWidget, but you cannot use the Q_OBJECT macro in the subclass (the generated moc-file will not compile), so you cannot add further signals, slots or properties.
class WebAxWidget : public QAxWidget
{
public:
	WebAxWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
	~WebAxWidget();

	Signaler m_signaler;
protected:
    bool translateKeyEvent(int message, int keycode) const;
	bool eventFilter(QObject *obj, QEvent *event);
	bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private:
	void cppCall(const QString& key, const QString& value);


};

#endif // WEBAXWIDGET_H
