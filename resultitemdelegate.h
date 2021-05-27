#ifndef RESULTITEMDELEGATE_H
#define RESULTITEMDELEGATE_H

#include <QItemDelegate>
#include <QTreeView>

class ResultItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	ResultItemDelegate(QObject *parent = 0);
	~ResultItemDelegate();

	void SetTreeView(QTreeView *view);

    //QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
	QTreeView *m_pTreeView;
};

#endif // RESULTITEMDELEGATE_H
