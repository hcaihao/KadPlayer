#ifndef RESULTSORTFILTERPROXYMODEL_H
#define RESULTSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class ResultSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	ResultSortFilterProxyModel(QObject *parent);
	~ResultSortFilterProxyModel();

	void SetFilter(QString name, QStringList typeList);

protected:
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
private:
	QStringList m_typeFilter;
	QString m_nameFilter;
};

#endif // RESULTSORTFILTERPROXYMODEL_H
