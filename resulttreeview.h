#ifndef RESULTTREEVIEW_H
#define RESULTTREEVIEW_H

#include <QTreeView>

#include "kadresult.h"

class Kademlia;
class ResultTableModel;
class ResultSortFilterProxyModel;
class ResultItemDelegate;

class ResultTreeView : public QTreeView
{
	Q_OBJECT

public:
	ResultTreeView(QString key, QWidget *parent = 0);
	~ResultTreeView();

	void InitControl();
	void AddContextMenu();
	void FilterResult(QString name, QStringList typeList);
private:
	QMenu *m_pPopupMenu;

	QString m_key;
	QStringList m_typeFilter;
	QString m_nameFilter;
	Kademlia *m_pKademlia;
	ResultTableModel *m_pResultTableModel;
	ResultSortFilterProxyModel *m_pResultProxyModel;
	ResultItemDelegate *m_pResultItemDelegate;
signals:
	void updateTabTitle(QString title, QWidget *widget);
private slots:
	void insertResult(const KadResult& result);
	void customContextMenuRequested(const QPoint & pos);
	void playerCurrentItem();
	void copySelectedEd2kItem();
	void downloadSelectedItem();
	void on_sectionClicked_emited(int logicalIndex);
};

#endif // RESULTTREEVIEW_H
