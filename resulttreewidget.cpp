#include "StdAfx.h"
#include "resulttreewidget.h"
#include "resulttreewidgetitem.h"
#include "kadsearch.h"
#include "operatewidget.h"
#include "nofocusdelegate.h"
#include "loadingwidget.h"
#include "messagewindow.h"
#include "playwindow.h"
#include "mainwindow.h"
#include "playlistwidget.h"
#include "mainwindow.h"
#include "netmanager.h"

extern MainWindow *g_pMainWindow;

ResultTreeWidget::ResultTreeWidget(QString key, QWidget *parent)
	: m_count(0), m_searchID(0), m_key(key), QTreeWidget(parent)
{
	InitControl();

	AddContextMenu();

#ifdef LOCAL_KAD_LIB
	m_searchID = g_kadLibrary.Search(key.toLocal8Bit().data(), this);
#else
	m_pKadSearch = new KadSearch(key, ONE_MINUTE, this);
	connect(m_pKadSearch, SIGNAL(insertResult(const KadResult&)), this, SLOT(insertResult(const KadResult&)));

	//NetManager::GetInstance().GetUrl(tr("http://donkey4u.com/search/%1?page=1&mode=list").arg(key));
	//connect(&NetManager::GetInstance(), SIGNAL(loadFinish(QUrl &, QByteArray &)), this, SLOT(loadFinish(QUrl &, QByteArray &)));
#endif
}

ResultTreeWidget::~ResultTreeWidget()
{
#ifdef LOCAL_KAD_LIB
	g_kadLibrary.Stop(m_searchID);
#else
	delete m_pKadSearch;
#endif

}

void ResultTreeWidget::InitControl()
{
	QStringList headers;
	headers << tr("ID") << tr("Name") << tr("Size") << tr("Format") << tr("Hot") << tr("Operate");
	setHeaderLabels(headers);
	setRootIsDecorated(false);
	setUniformRowHeights(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSortingEnabled(true);
	setAlternatingRowColors(true);
	sortByColumn(COLUMN_ID, Qt::AscendingOrder);
	// 先设置setResizeMode
	header()->setStretchLastSection(false);
	header()->setSectionResizeMode(COLUMN_NAME, QHeaderView::Stretch);
	header()->setSectionResizeMode(COLUMN_OPERATE, QHeaderView::Fixed);
	// 再设置setColumnWidth
	setColumnWidth(COLUMN_ID, 80);
	setColumnWidth(COLUMN_NAME, 350);
	setColumnWidth(COLUMN_SIZE, 80);
	setColumnWidth(COLUMN_TYPE, 80);
	setColumnWidth(COLUMN_RESOURCE, 80);
	setColumnWidth(COLUMN_OPERATE, 120);

	setItemDelegate(new NoFocusDelegate());
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));
	connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));

	QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+C"), this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(copySelectedEd2kItem()));


	// Loading animation
	m_pLoadingWidget = new LoadingWidget();
	m_pLoadingWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	m_pLoadingWidget->Start();

	QVBoxLayout *contentLayout = new QVBoxLayout();
	contentLayout->setSpacing(10);
	contentLayout->setContentsMargins(0, 0, 0, 0);
	contentLayout->addWidget(m_pLoadingWidget);
	setLayout(contentLayout);
}

void ResultTreeWidget::AddContextMenu()
{
	setContextMenuPolicy(Qt::CustomContextMenu);

	m_pPopupMenu = new QMenu(this);
	m_pPopupMenu->setObjectName("popupMenu");

	QAction *playerAction = new QAction(QIcon(":/Image/menu_play.png"), tr("Play"), this);
	m_pPopupMenu->addAction(playerAction);
	connect(playerAction, SIGNAL(triggered()), this, SLOT(playerCurrentItem()));
#ifdef KAD_SEARCHER
	m_pPopupMenu->removeAction(playerAction);
#endif

	QAction *favoriteAction = new QAction(QIcon(":/Image/menu_favorite.png"), tr("Favorite"), this);
	m_pPopupMenu->addAction(favoriteAction);
	connect(favoriteAction, SIGNAL(triggered()), this, SLOT(favoriteCurrentItem()));



	QMenu *feedbackMenu = new QMenu(tr("Report"), this);
	m_pPopupMenu->addMenu(feedbackMenu);

	QAction *feedbackSexAction = new QAction(tr("Pornographic"), this);
	feedbackSexAction->setObjectName("feedbackSexAction");
	feedbackMenu->addAction(feedbackSexAction);
	connect(feedbackSexAction, SIGNAL(triggered()), this, SLOT(feedbackCurrentItem()));

	QAction *feedbackPirateAction = new QAction(tr("Piratical"), this);
	feedbackPirateAction->setObjectName("feedbackPirateAction");
	feedbackMenu->addAction(feedbackPirateAction);
	connect(feedbackPirateAction, SIGNAL(triggered()), this, SLOT(feedbackCurrentItem()));


	m_pPopupMenu->addSeparator();

	QAction *copyAllEd2kAction = new QAction(tr("Copy link"), this);
	m_pPopupMenu->addAction(copyAllEd2kAction);
	connect(copyAllEd2kAction, SIGNAL(triggered()), this, SLOT(copySelectedEd2kItem()));

	QAction *downloadAllAction = new QAction(tr("Download link"), this);
	m_pPopupMenu->addAction(downloadAllAction);
	connect(downloadAllAction, SIGNAL(triggered()), this, SLOT(downloadSelectedItem()));
#ifdef KAD_SEARCHER
	m_pPopupMenu->removeAction(downloadAllAction);
#endif

	m_pPopupMenu->addSeparator();

	QAction *saveAllEd2kAction = new QAction(tr("Export result"), this);
	m_pPopupMenu->addAction(saveAllEd2kAction);
	connect(saveAllEd2kAction, SIGNAL(triggered()), this, SLOT(saveSelectedEd2kItem()));
}

void ResultTreeWidget::FilterResult(QString nameFilter, QStringList typeFilter)
{
	if(m_nameFilter != nameFilter || m_typeFilter != typeFilter)
	{
		m_nameFilter = nameFilter;
		m_typeFilter = typeFilter;

		clear();

		for(int i=0; i<m_kadResultList.count(); i++)
		{
			KadResult result = m_kadResultList[i];

			ShowResult(result);
		}
	}

}

void ResultTreeWidget::ShowResult(const KadResult& result)
{
	if((m_nameFilter.isEmpty() || result.Name.contains(m_nameFilter)) && (m_typeFilter.empty() || m_typeFilter.contains(result.Type)))
	{
		ResultTreeWidgetItem *item = new ResultTreeWidgetItem(this);
		//item->setCheckState(COLUMN_ID, Qt::Unchecked);
		item->setText(COLUMN_ID, QString::number(result.Index));
		item->setData(COLUMN_ID, Qt::UserRole, QString::number(result.Index));
		item->setText(COLUMN_NAME, result.Name);
		item->setData(COLUMN_NAME, Qt::UserRole, result.Ed2k);
		item->setText(COLUMN_SIZE, GetFileSize(result.Size));
		item->setData(COLUMN_SIZE, Qt::UserRole, result.Size);
		item->setText(COLUMN_TYPE, result.Type);
		item->setText(COLUMN_RESOURCE, QString::number(result.Resource));
		item->setData(COLUMN_RESOURCE, Qt::UserRole, result.Resource);

		OperateWidget *operateWidget = new OperateWidget(this);
		connect(operateWidget, SIGNAL(playLink(QStringList)), this, SLOT(playLink(QStringList)));
		connect(operateWidget, SIGNAL(downloadLink(QStringList)), this, SLOT(downloadLink(QStringList)));
		operateWidget->SetInfo(result, result.Type == tr("Video"));
		setItemWidget(item, COLUMN_OPERATE, operateWidget);
	}
}

void ResultTreeWidget::customContextMenuRequested(const QPoint &pos)
{
	// Set menu pop point
	QPoint myPos = viewport()->mapToGlobal(pos);
	m_pPopupMenu->exec(myPos);
}

void ResultTreeWidget::itemDoubleClicked(QTreeWidgetItem * item, int column)
{
	Q_UNUSED(column);

	if(item)
	{
		QString link = item->data(COLUMN_NAME, Qt::UserRole).toString();
		
		if(g_doubleClick == CLICK_COPY)
		{
			QClipboard *clipboard = QApplication::clipboard();
			clipboard->setText(link);
		}
		else if(g_doubleClick == CLICK_DOWNLOAD)
		{
			downloadLink(QStringList(link));
		}
		else if(g_doubleClick == CLICK_PLAY)
		{
			playLink(QStringList(link));
		}
	}
}

void ResultTreeWidget::playerCurrentItem()
{
	QStringList links;

	QList<QTreeWidgetItem *> list = selectedItems();
	foreach(QTreeWidgetItem *item, list)
	{
		QString link = item->data(COLUMN_NAME, Qt::UserRole).toString();
		links << link;
	}

	playLink(links);
}

void ResultTreeWidget::favoriteCurrentItem()
{
	QStringList links;

	QList<QTreeWidgetItem *> list = selectedItems();
	foreach(QTreeWidgetItem *item, list)
	{
		QString link = item->data(COLUMN_NAME, Qt::UserRole).toString();

		if(item->text(COLUMN_TYPE) == tr("Movie"))
		{
			links << link;
		}
	}

	foreach(QString link, links)
	{
		if(!g_favoriteMovieList.contains(link))
		{
			g_favoriteMovieList.append(link);
			PlayListWidget* widget = g_pMainWindow->GetPlayListWidget();
			if (widget)
			{
				widget->AddFavorite(link);
			}
		}
	}
}

void ResultTreeWidget::feedbackCurrentItem()
{
	int feedbackType = FEEDBACK_BUG;

	QObject *obj = sender();
	if(obj)
	{
		if(obj->objectName() == "feedbackSexAction")
		{
			feedbackType = FEEDBACK_SEX;
		}
		else if(obj->objectName() == "feedbackPirateAction")
		{
			feedbackType = FEEDBACK_PIRATE;
		}
	}

	QList<QTreeWidgetItem *> list = selectedItems();
	foreach(QTreeWidgetItem *item, list)
	{
		QString link = item->data(COLUMN_NAME, Qt::UserRole).toString();

		QString content = tr("Keyword:%1\r\nLink:%2").arg(m_key).arg(link);

		QMetaObject::invokeMethod(g_pMainWindow, "showFeedbackWindow", Qt::QueuedConnection, Q_ARG(int, feedbackType), Q_ARG(QString, content));

		break;
	}
}

void ResultTreeWidget::downloadSelectedItem()
{
	QStringList links;

	QList<QTreeWidgetItem *> list = selectedItems();
	foreach(QTreeWidgetItem *item, list)
	{
		QString link = item->data(COLUMN_NAME, Qt::UserRole).toString();
		links << link;
	}

	downloadLink(links);
}

void ResultTreeWidget::copySelectedEd2kItem()
{
	QStringList links;

	QList<QTreeWidgetItem *> list = selectedItems();
	foreach(QTreeWidgetItem *item, list)
	{
		QString link = item->data(COLUMN_NAME, Qt::UserRole).toString();
		links << link;
	}

	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(links.join("\n"));
}

void ResultTreeWidget::saveSelectedEd2kItem()
{
	QStringList links;

	for(int i=0; i<topLevelItemCount(); i++)
	{
		QTreeWidgetItem *item = topLevelItem(i);

		QString link = item->data(COLUMN_NAME, Qt::UserRole).toString();
		links << link;
	}

	QString fileName = QFileDialog::getSaveFileName(this, tr("Export result"), tr("%1_%2_%3.txt").arg(APP_TITLE).arg(m_key).arg(links.count()), "txt (*.txt)");
	if(!fileName.isEmpty())
	{
		QFile file(fileName);
		if(file.open( QIODevice::WriteOnly ))
		{
			QString data = links.join("\r\n");

			file.write(data.toLocal8Bit().data());
			file.close();
		}
	}
}

void ResultTreeWidget::insertResult(KadResult& result)
{
	m_pLoadingWidget->Stop();

	result.Index = ++m_count;
	m_kadResultList.append(result);

	ShowResult(m_kadResultList.last());

	emit updateTabTitle(QString("%1 [%2]").arg(m_key).arg(m_kadResultList.count()), this);
}

void ResultTreeWidget::playLink(QStringList links)
{
	if(links.size() > 5)
	{
		bool result = MessageWindow::GetInstance().ShowMessage(tr("Notice"), tr("Play 5 or more videos may lead to system no response, continue?"), true);
		if(!result)
		{
			return;
		}
	}

	foreach(QString link, links)
	{
		QString name = GetFileName(link);

		PlayWindow *playWindow = new PlayWindow(g_pMainWindow);
		playWindow->setObjectName("playWindow");
		playWindow->Play(name, link);
		playWindow->show();


		if(!g_historyMovieList.contains(link))
		{
			g_historyMovieList.append(link);
			PlayListWidget* widget = g_pMainWindow->GetPlayListWidget();
			if (widget)
			{
				widget->AddHistory(link);
			}
		}
	}
}

void ResultTreeWidget::downloadLink(QStringList links)
{
	int nStartMode = -1, nOnlyFromOrigin = 0, nOriginThreadCount = -1;

	if(g_pThunder)
	{
		foreach(QString link, links)
		{
			_bstr_t bURL = link.toStdWString().c_str();
			_bstr_t bReferPage = "";
			_bstr_t bComment = "";
			_bstr_t bCookieStr = "";
			_bstr_t bCID = "";
			_bstr_t bUnused = "";

			g_pThunder->AddTask( 
				bURL, /* pURL */
				bUnused, /* pFileName */
				bUnused, /* pPath */
				bComment, /* pComment */
				bReferPage, /* pReferURL */
				nStartMode, /* nStartMode = -1*/
				nOnlyFromOrigin, /* nOnlyFromOrigin = 0 */
				nOriginThreadCount /* nOriginThreadCount = -1 */
				);
		}

		g_pThunder->CommitTasks();
	}
	else
	{
		MessageWindow::GetInstance().ShowMessage(tr("Notice"), tr("Please install the thunder first."));
	}
}

void ResultTreeWidget::loadFinish(QUrl &url, QByteArray &content)
{
	qDebug() << url << content;
}