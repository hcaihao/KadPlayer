#include "StdAfx.h"
#include "resulttreeview.h"
#include "resultitemdelegate.h"
#include "resulttabletmodel.h"
#include "resultsortfilterproxymodel.h"

#include "kademlia.h"
#include "operatewidget.h"

#include <atlbase.h>
#include <atlcom.h>

ResultTreeView::ResultTreeView(QString key, QWidget *parent)
	: m_key(key), QTreeView(parent)
{
	InitControl();

	AddContextMenu();

	m_pKademlia = new Kademlia(key, 30);
	connect(m_pKademlia, SIGNAL(insertResult(const KadResult&)), this, SLOT(insertResult(const KadResult&)));
}

ResultTreeView::~ResultTreeView()
{
	delete m_pKademlia;
}

void ResultTreeView::InitControl()
{
	m_pResultTableModel = new ResultTableModel();

	//m_pResultItemDelegate = new ResultItemDelegate();
	//m_pResultItemDelegate->SetTreeView(this);
	//setItemDelegate(m_pResultItemDelegate);

	//m_pResultProxyModel = new ResultSortFilterProxyModel(this);
	//m_pResultProxyModel->setDynamicSortFilter(true);
	//m_pResultProxyModel->setSortRole(Qt::EditRole);
	//m_pResultProxyModel->setSourceModel(m_pResultTableModel);
	//setModel(m_pResultProxyModel);

	setModel(m_pResultTableModel);

	setRootIsDecorated(false);
	setUniformRowHeights(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSortingEnabled(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setAlternatingRowColors(true);
	viewport()->setFocusPolicy(Qt::NoFocus);
	// 先设置setResizeMode
	header()->setStretchLastSection(false);
	header()->setResizeMode(COLUMN_NAME, QHeaderView::Stretch);
	header()->setResizeMode(COLUMN_OPERATE, QHeaderView::Fixed);
	// 再设置setColumnWidth
	setColumnWidth(COLUMN_ID, 80);
	setColumnWidth(COLUMN_NAME, 350);
	setColumnWidth(COLUMN_SIZE, 80);
	setColumnWidth(COLUMN_TYPE, 80);
	setColumnWidth(COLUMN_RESOURCE, 80);
	setColumnWidth(COLUMN_OPERATE, 120);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));
	connect(header(), SIGNAL(sectionClicked(int)), this, SLOT(on_sectionClicked_emited(int)));
}

void ResultTreeView::AddContextMenu()
{
	setContextMenuPolicy(Qt::CustomContextMenu);

	m_pPopupMenu = new QMenu(this);
	m_pPopupMenu->setObjectName("popupMenu");

	QAction *playerAction = new QAction(tr("立即播放"), this);
	m_pPopupMenu->addAction(playerAction);
	connect(playerAction, SIGNAL(triggered()), this, SLOT(playerCurrentItem()));

	m_pPopupMenu->addSeparator();

	QAction *copyAllEd2kAction = new QAction(tr("复制链接"), this);
	m_pPopupMenu->addAction(copyAllEd2kAction);
	connect(copyAllEd2kAction, SIGNAL(triggered()), this, SLOT(copySelectedEd2kItem()));

	QAction *downloadAllAction = new QAction(tr("下载链接"), this);
	m_pPopupMenu->addAction(downloadAllAction);
	connect(downloadAllAction, SIGNAL(triggered()), this, SLOT(downloadSelectedItem()));
}

void ResultTreeView::customContextMenuRequested(const QPoint &pos)
{
	// Set menu pop point
	QPoint myPos = viewport()->mapToGlobal(pos);
	m_pPopupMenu->exec(myPos);
}

void ResultTreeView::downloadSelectedItem()
{
	QStringList links;

	QModelIndexList indexlist = selectionModel()->selectedRows();
	foreach(QModelIndex index, indexlist)
	{
		KadResult &result = m_pResultTableModel->GetData(index.row());

		links << result.Ed2k;
	}

	int nStartMode = -1, nOnlyFromOrigin = 0, nOriginThreadCount = -1;

	foreach(QString link, links)
	{
		_bstr_t bURL = link.toStdString().c_str();
		_bstr_t bReferPage = "";
		_bstr_t bComment = "";
		_bstr_t bCookieStr = "";
		_bstr_t bCID = "";
		_bstr_t bUnused = "";

		g_thunder->AddTask( 
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

	g_thunder->CommitTasks();
}

void ResultTreeView::playerCurrentItem()
{
	//if(typeName == "全部")
	//{
	//	m_typeFilter = g_attributeHash.values().toSet().toList();
	//}
	//else if(typeName == "影视")
	//{
	//	m_typeFilter << "视频" << "字幕";
	//}
	//else if(typeName == "音乐")
	//{
	//	m_typeFilter << "音频" << "歌词";
	//}
	//else if(typeName == "文件")
	//{
	//	m_typeFilter << "压缩包" << "光盘镜像";
	//}
	//else if(typeName == "其他")
	//{
	//	m_typeFilter << "程序" << "文档" << "图片" << "网页" << "BT种子" << "电子书";
	//}

	QStringList list;
	FilterResult("mkv", list);
}

void ResultTreeView::copySelectedEd2kItem()
{
	QStringList links;

	QModelIndexList indexlist = selectionModel()->selectedRows();
	foreach(QModelIndex index, indexlist)
	{
		KadResult &result = m_pResultTableModel->GetData(index.row());

		links << result.Ed2k;
	}
	
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(links.join("\n"));
}

void ResultTreeView::insertResult(const KadResult& result)
{
	emit updateTabTitle(QString("%1 [%2]").arg(m_key).arg(result.Index), this);
	int row = m_pResultTableModel->InsertData(result);

	OperateWidget *operateWidget = new OperateWidget();
	operateWidget->SetInfo(result.Ed2k, result.Type == "视频");
	QModelIndex index = model()->index(row, COLUMN_OPERATE);	//使用proxy后，model()是filterProxyModel，不是resultTableModel
	setIndexWidget(index, operateWidget);

	if((m_nameFilter.isEmpty() || result.Name.contains(m_nameFilter)) && (m_typeFilter.empty() || m_typeFilter.contains(result.Type)))
	{
		setRowHidden(row, QModelIndex(), true);
	}
}

void ResultTreeView::on_sectionClicked_emited(int logicalIndex)
{
	Q_UNUSED(logicalIndex);

	for(int i=0; i<m_pResultTableModel->rowCount(); i++)
	{
		QModelIndex index = model()->index(i, COLUMN_OPERATE); 

		OperateWidget *operateWidget = qobject_cast<OperateWidget *>(indexWidget(index));
		if(operateWidget)
		{
			KadResult &result = m_pResultTableModel->GetData(i);
			operateWidget->SetInfo(result.Ed2k, result.Type == "视频");
		}
	}
}

void ResultTreeView::FilterResult(QString name, QStringList typeList)
{
	if(m_nameFilter != name || m_typeFilter != typeList)
	{
		m_nameFilter = name;
		m_typeFilter = typeList;
	}

	for(int i=0; i<m_pResultTableModel->rowCount(); i++)
	{
		KadResult &result = m_pResultTableModel->GetData(i);

		if((m_nameFilter.isEmpty() || result.Name.contains(m_nameFilter)) && (m_typeFilter.empty() || m_typeFilter.contains(result.Type)))
		{
			setRowHidden(i, QModelIndex(), false);
		}
		else
		{
			setRowHidden(i, QModelIndex(), true);
		}
	}

}
