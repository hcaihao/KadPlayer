#ifndef OPERATEWIDGET_H
#define OPERATEWIDGET_H

#include <QWidget>
#include "kadresult.h"

class PlayWindow;

class OperateWidget : public QWidget
{
	Q_OBJECT

public:
	OperateWidget(QWidget *parent = 0);
	~OperateWidget();

	void SetInfo(const KadResult& kadResult, bool isVideo);
private:
	void InitControl();

	KadResult m_kadResult;
	QPushButton *m_pPlayPushButton;
	QPushButton *m_pFavoritePushButton;
	QPushButton *m_pDownloadPushButton;
private slots:
	void on_playPushButton_clicked();
	void on_downloadPushButton_clicked();
signals:
	void playLink(QStringList links);
	void downloadLink(QStringList links);
};

#endif // OPERATEWIDGET_H
