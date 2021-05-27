#include "StdAfx.h"
#include "resultsortfilterproxymodel.h"

ResultSortFilterProxyModel::ResultSortFilterProxyModel(QObject *parent)
	: QSortFilterProxyModel(parent)
{

}

ResultSortFilterProxyModel::~ResultSortFilterProxyModel()
{


}

void ResultSortFilterProxyModel::SetFilter(QString name, QStringList typeList)
{
	m_nameFilter = name;
	m_typeFilter = typeList;

	invalidateFilter();
}

bool ResultSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	QVariant leftData = sourceModel()->data(left, Qt::EditRole);
	QVariant rightData = sourceModel()->data(right, Qt::EditRole);

	if (leftData.type() == QVariant::Int)
	{
		return leftData.toInt() < rightData.toInt();
	}
	else if (leftData.type() == QVariant::UInt)
	{
		return leftData.toUInt() < rightData.toUInt();
	}
	else if (leftData.type() == QVariant::ULongLong)
	{
		return leftData.toULongLong() < rightData.toULongLong();
	}
	else if (leftData.type() == QVariant::String)
	{
		QString left = leftData.toString();
		QString right = rightData.toString();

		return QString::localeAwareCompare(left, right) < 0;
	}
}

bool ResultSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	QModelIndex nameIndex = sourceModel()->index(source_row, COLUMN_NAME, source_parent);
	QModelIndex typeIndex = sourceModel()->index(source_row, COLUMN_TYPE, source_parent);

	QString name = sourceModel()->data(nameIndex).toString();
	QString type = sourceModel()->data(typeIndex).toString();

	qDebug() << name << type;

	if((m_nameFilter.isEmpty() || name.contains(m_nameFilter)) && (m_typeFilter.empty() || m_typeFilter.contains(type)))
	{
		return true;
	}
	else
	{
		return false;
	}
}