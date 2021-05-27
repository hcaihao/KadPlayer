#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

class PlayListWidget : public QFrame
{
	Q_OBJECT

public:
	PlayListWidget(QWidget *parent = 0);
	~PlayListWidget();

	bool AddRecommend(const QString& ed2k);
	bool AddHistory(const QString& ed2k);
	bool AddFavorite(const QString& ed2k);

private:
	void InitControl();
	void AddContextMenu();
	void UpdateRoot();

	QMenu *m_pPopupMenu;
	QTreeWidgetItem *ChildAt(QTreeWidgetItem *parent, int index);
	void RemoveAllChild(QTreeWidgetItem *item);
	void UpdateRecommend(const QStringList& items);
	void UpdateHistory(const QStringList& items);
	void UpdateFavorite(const QStringList& items);

	QLineEdit *m_pSearchListLineEdit;
	QTreeWidget *m_pPlayListTreeWidget;
	QTreeWidgetItem *m_pRecommendTreeWidgetItem;
	QTreeWidgetItem *m_pFavoriteTreeWidgetItem;
	QTreeWidgetItem *m_pHistoryTreeWidgetItem;
	QTreeWidgetItem *m_pCurSelectedItem;
	QStringList m_recommendList;

private slots:
	void customContextMenuRequested(const QPoint & pos);
	void deleteFavoriteItem();
	void deleteHistoryItem();
	void on_playListTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_playListExpanded(QTreeWidgetItem * item);
	void on_searchListLineEdit_textChanged(const QString & text);
};

#endif // PLAYLISTWIDGET_H
