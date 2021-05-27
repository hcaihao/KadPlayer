#ifndef RESULTTREEWIDGETITEM_H
#define RESULTTREEWIDGETITEM_H

#include <QTreeWidgetItem>
#include <QTreeWidget>

class ResultTreeWidgetItem : public QTreeWidgetItem
{
public:
	ResultTreeWidgetItem(QTreeWidget *view, int type = 0);
	~ResultTreeWidgetItem();
private:
	bool operator < (const QTreeWidgetItem &other) const;
};

#endif // RESULTTREEWIDGETITEM_H
