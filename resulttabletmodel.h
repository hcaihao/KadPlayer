#ifndef RESULTTABLEMODEL_H
#define RESULTTABLEMODEL_H

#include <QAbstractTableModel>

#include "kadresult.h"


class ResultTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	ResultTableModel(QObject *parent = 0);
	~ResultTableModel();
	int InsertData(const KadResult& result);
	KadResult& GetData(int row);

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
	bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
private:
	QList<KadResult> m_resultList;
};

#endif // RESULTTABLEMODEL_H
