#ifndef RESULTTREEWIDGET_H
#define RESULTTREEWIDGET_H

#include <QTreeWidget>

#include "kadresult.h"

class KadSearch;
class LoadingWidget;

class ResultTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
	ResultTreeWidget(QString key, QWidget *parent = 0);
	~ResultTreeWidget();

	void InitControl();
	void AddContextMenu();
	void FilterResult(QString nameFilter, QStringList typeFilter);
	void ShowResult(const KadResult& result);

	int m_count;
	int m_searchID;
	QString m_key;
private:
	LoadingWidget *m_pLoadingWidget;
	QMenu *m_pPopupMenu;

	KadSearch *m_pKadSearch;
	QList<KadResult> m_kadResultList;
	QStringList m_typeFilter;
	QString m_nameFilter;
signals:
	void updateTabTitle(QString title, QWidget *widget);
public slots:
	void insertResult(KadResult& result);
private slots:
	void customContextMenuRequested(const QPoint & pos);
	void itemDoubleClicked(QTreeWidgetItem * item, int column);
	void playerCurrentItem();
	void favoriteCurrentItem();
	void feedbackCurrentItem();
	void copySelectedEd2kItem();
	void saveSelectedEd2kItem();
	void downloadSelectedItem();
	void playLink(QStringList links);
	void downloadLink(QStringList links);
	void loadFinish(QUrl &url, QByteArray &content);
};

#endif // RESULTTREEWIDGET_H
