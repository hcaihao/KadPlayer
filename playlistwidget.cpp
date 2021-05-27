#include "StdAfx.h"
#include "playlistwidget.h"
#include "playwindow.h"
#include "mainwindow.h"

extern MainWindow *g_pMainWindow;

PlayListWidget::PlayListWidget(QWidget *parent)
	: QFrame(parent), m_pCurSelectedItem(0)
{
	InitControl();
	AddContextMenu();

	UpdateRoot();
}

PlayListWidget::~PlayListWidget()
{

}

void PlayListWidget::InitControl()
{
	m_pSearchListLineEdit = new QLineEdit();
	m_pSearchListLineEdit->setObjectName("searchListLineEdit");
	m_pSearchListLineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	connect(m_pSearchListLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(on_searchListLineEdit_textChanged(const QString &)));

	m_pPlayListTreeWidget = new QTreeWidget();
	m_pPlayListTreeWidget->setObjectName("playListTreeWidget");
	m_pPlayListTreeWidget->setCursor(Qt::ArrowCursor);

	QStringList labels;
	labels << tr("Name");
	m_pPlayListTreeWidget->setHeaderLabels(labels);
	m_pPlayListTreeWidget->setRootIsDecorated(false);
	m_pPlayListTreeWidget->setUniformRowHeights(true);
	m_pPlayListTreeWidget->header()->hide();
	m_pPlayListTreeWidget->viewport()->setFocusPolicy(Qt::NoFocus);
	m_pPlayListTreeWidget->setFrameStyle(QFrame::NoFrame); //È¥µô±ß¿ò

	connect(m_pPlayListTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(on_playListTreeWidget_itemDoubleClicked(QTreeWidgetItem *, int)));
	connect(m_pPlayListTreeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));
	connect(m_pPlayListTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(on_playListExpanded(QTreeWidgetItem *)));
	connect(m_pPlayListTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem *)), this, SLOT(on_playListExpanded(QTreeWidgetItem *)));

	m_pRecommendTreeWidgetItem = new QTreeWidgetItem(m_pPlayListTreeWidget);
	m_pRecommendTreeWidgetItem->setText(0, tr("Recommend videos"));
	m_pRecommendTreeWidgetItem->setIcon(0, QIcon(":/Image/expand.png"));
	m_pRecommendTreeWidgetItem->setExpanded(true);

	m_pFavoriteTreeWidgetItem = new QTreeWidgetItem(m_pPlayListTreeWidget);
	m_pFavoriteTreeWidgetItem->setText(0, tr("Favorite videos"));
	m_pFavoriteTreeWidgetItem->setIcon(0, QIcon(":/Image/expand.png"));
	m_pFavoriteTreeWidgetItem->setExpanded(true);

	m_pHistoryTreeWidgetItem = new QTreeWidgetItem(m_pPlayListTreeWidget);
	m_pHistoryTreeWidgetItem->setText(0, tr("Recently videos"));
	m_pHistoryTreeWidgetItem->setIcon(0, QIcon(":/Image/expand.png"));
	m_pHistoryTreeWidgetItem->setExpanded(true);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(m_pSearchListLineEdit);
	layout->addWidget(m_pPlayListTreeWidget);
	setLayout(layout);
}

void PlayListWidget::AddContextMenu()
{
	m_pPlayListTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	m_pPopupMenu = new QMenu(this);
	m_pPopupMenu->setObjectName("popupMenu");

	QAction *deleteFavoriteAction = new QAction(tr("Clean Favorite videos"), this);
	m_pPopupMenu->addAction(deleteFavoriteAction);
	connect(deleteFavoriteAction, SIGNAL(triggered()), this, SLOT(deleteFavoriteItem()));

	QAction *deleteHistoryAction = new QAction(tr("Clean recently videos"), this);
	m_pPopupMenu->addAction(deleteHistoryAction);
	connect(deleteHistoryAction, SIGNAL(triggered()), this, SLOT(deleteHistoryItem()));
}

QTreeWidgetItem *PlayListWidget::ChildAt(QTreeWidgetItem *parent, int index)
{
	if (parent)
	{
		return parent->child(index);
	}
	else
	{
		return m_pPlayListTreeWidget->topLevelItem(index);
	}
}

void PlayListWidget::UpdateRoot()
{
	UpdateHistory(g_historyMovieList);
	UpdateFavorite(g_favoriteMovieList);
	UpdateRecommend(m_recommendList);
}

bool PlayListWidget::AddRecommend(const QString& ed2k)
{
	if (m_pRecommendTreeWidgetItem)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, GetFileName(ed2k));
		item->setData(0, Qt::UserRole, ed2k);
		item->setToolTip(0, GetFileName(ed2k));
		m_pRecommendTreeWidgetItem->addChild(item);

		if(!m_recommendList.contains(ed2k, Qt::CaseInsensitive))
		{
			m_recommendList.append(ed2k);
		}

		return true;
	}
	return false;
}

bool PlayListWidget::AddHistory(const QString& ed2k)
{
	if (m_pHistoryTreeWidgetItem)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, GetFileName(ed2k));
		item->setData(0, Qt::UserRole, ed2k);
		item->setToolTip(0, GetFileName(ed2k));
		m_pHistoryTreeWidgetItem->addChild(item);
		return true;
	}
	return false;
}

bool PlayListWidget::AddFavorite(const QString& ed2k)
{
	if (m_pFavoriteTreeWidgetItem)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, GetFileName(ed2k));
		item->setData(0, Qt::UserRole, ed2k);
		item->setToolTip(0, GetFileName(ed2k));
		m_pFavoriteTreeWidgetItem->addChild(item);
		return true;
	}
	return false;
}

void PlayListWidget::UpdateRecommend(const QStringList& items)
{
	RemoveAllChild(m_pRecommendTreeWidgetItem);

	int size = items.size();
	for (int i = 0; i < size; ++i)
	{
		AddRecommend(items[i]);
	}
}

void PlayListWidget::UpdateHistory(const QStringList& items)
{
	RemoveAllChild(m_pHistoryTreeWidgetItem);

	int size = items.size();
	for (int i = 0; i < size; ++i)
	{
		AddHistory(items[i]);
	}
}

void PlayListWidget::UpdateFavorite(const QStringList& items)
{
	RemoveAllChild(m_pFavoriteTreeWidgetItem);

	int size = items.size();
	for (int i = 0; i < size; ++i)
	{
		AddFavorite(items[i]);
	}
}

void PlayListWidget::RemoveAllChild(QTreeWidgetItem *item)
{
	int count  = item->childCount();
	for (int i = count - 1; i >= 0; --i)
	{
		QTreeWidgetItem* child = item->child(i);
		item->removeChild(child);
	}
}

void PlayListWidget::customContextMenuRequested(const QPoint &pos)
{
	// Set menu pop point
	QPoint myPos = m_pPlayListTreeWidget->viewport()->mapToGlobal(pos);
	m_pPopupMenu->exec(myPos);
}

void PlayListWidget::deleteFavoriteItem()
{
	g_favoriteMovieList.clear();

	UpdateFavorite(g_favoriteMovieList);
}

void PlayListWidget::deleteHistoryItem()
{
	g_historyMovieList.clear();

	UpdateHistory(g_historyMovieList);
}

void PlayListWidget::on_playListTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
	if (item != NULL)
	{
		QString url = item->data(0, Qt::UserRole).toString();
		if (!url.isEmpty())
		{
			PlayWindow *playWindow = new PlayWindow(g_pMainWindow);
			playWindow->setObjectName("playWindow");
			playWindow->Play(item->text(0), url);
			playWindow->show();
		}
	}
}

void PlayListWidget::on_playListExpanded(QTreeWidgetItem * item)
{
	if (item->parent() ==  NULL)
	{
		if (!item->isExpanded())
		{
			item->setIcon(0, QIcon(":/Image/expand.png"));
		}
		else
		{
			item->setIcon(0, QIcon(":/Image/fold.png"));
		}
	}
}

void PlayListWidget::on_searchListLineEdit_textChanged(const QString & text)
{
	if (!text.isEmpty())
	{
		QStringList items;
		int size = g_historyMovieList.size();
		for (int i = 0; i < size; ++ i)
		{
			QString &name = g_historyMovieList[i];
			if (name.contains(text))
			{
				items.append(name);
			}
		}
		UpdateHistory(items);

		items.clear();
		size = g_favoriteMovieList.size();
		for (int i = 0; i < size; ++ i)
		{
			QString &name = g_favoriteMovieList[i];
			if (name.contains(text))
			{
				items.append(name);
			}
		}
		UpdateFavorite(items);

		items.clear();
		size = m_recommendList.size();
		for (int i = 0; i < size; ++ i)
		{
			QString &name = m_recommendList[i];
			if (name.contains(text))
			{
				items.append(name);
			}
		}
		UpdateRecommend(items);
		m_pHistoryTreeWidgetItem->setExpanded(true);
		m_pFavoriteTreeWidgetItem->setExpanded(true);
		m_pRecommendTreeWidgetItem->setExpanded(true);
	}
	else
	{
		UpdateRoot();
	}
}