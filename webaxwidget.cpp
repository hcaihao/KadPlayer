#include "stdafx.h"
#include "webaxwidget.h"
#include "windows.h"

WebAxWidget::WebAxWidget(QWidget* parent, Qt::WindowFlags f) 
    : QAxWidget(parent, f)
{
	setControl("{8856F961-340A-11D0-A96B-00C04FD705A2}");
	//setFocusPolicy(Qt::StrongFocus);
	dynamicCall("SetSilent(bool)", true);
}

WebAxWidget::~WebAxWidget()
{

}

bool WebAxWidget::translateKeyEvent(int message, int keycode) const
{
	if (message >= WM_KEYFIRST && message <= WM_KEYLAST)
	{
		return true;
	}
	else
	{
		return QAxWidget::translateKeyEvent(message, keycode);
	}
}

bool WebAxWidget::eventFilter(QObject *obj, QEvent *event)
{
	return QAxWidget::eventFilter(obj, event);
}

bool WebAxWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	//MSG* msg = reinterpret_cast<MSG*>(message);

	//qDebug() << msg->message;

	//switch(msg->message)
	//{
	//case WM_NCACTIVATE:

	//case WM_SETFOCUS:

	//case WM_KILLFOCUS:

	//}

	return QAxWidget::nativeEvent(eventType, message, result);
}

void WebAxWidget::cppCall(const QString& key, const QString& value)
{
	qDebug() << key << value;

	if(key == "SearchKey")
	{
		m_signaler.EmitSearchKey(value);
	}
	else if(key == "openVodClientWindow")
	{
		dynamicCall("Navigate(const QString&)", value);
	}
}
