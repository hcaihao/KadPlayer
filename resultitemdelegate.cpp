#include "StdAfx.h"
#include "resultitemdelegate.h"

ResultItemDelegate::ResultItemDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{

}

ResultItemDelegate::~ResultItemDelegate()
{

}

void ResultItemDelegate::SetTreeView(QTreeView *view)
{
	m_pTreeView = view;
}

//QWidget* ResultItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{  
//
//};  

void ResultItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	//if (index.column() == COLUMN_OPERATE)
	//{
	//	QLabel *label = new QLabel();
	//	label->setText("hello");
	//	m_pTreeView->setIndexWidget(index, label);
	//}
	//else
	//{
	//	QStyledItemDelegate::paint(painter, option, index);
	//}


	QStyledItemDelegate::paint(painter, option, index);
}