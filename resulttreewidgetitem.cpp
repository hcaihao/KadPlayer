#include "stdafx.h"
#include "resulttreewidgetitem.h"


ResultTreeWidgetItem::ResultTreeWidgetItem(QTreeWidget *view, int type)
	: QTreeWidgetItem(view, type)
{

}

ResultTreeWidgetItem::~ResultTreeWidgetItem()
{

}

bool ResultTreeWidgetItem::operator < (const QTreeWidgetItem &other) const
{
	int column = treeWidget()->sortColumn();
	//Qt::SortOrder order = treeWidget()->header()->sortIndicatorOrder();

	if(column == COLUMN_ID || column == COLUMN_SIZE || column == COLUMN_RESOURCE)
	{
		return data(column, Qt::UserRole).toLongLong() < other.data(column, Qt::UserRole).toLongLong();
	}
	else
	{
		return text(column).toLower() < other.text(column).toLower();
	}
	
} 