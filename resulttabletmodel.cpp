#include "StdAfx.h"
#include "resulttabletmodel.h"

bool IDLessThan(const KadResult &result1, const KadResult &result2)
{
	return result1.Index < result2.Index;
}
bool IDGreaterThan(const KadResult &result1, const KadResult &result2)
{
	return result1.Index > result2.Index;
}

bool NameLessThan(const KadResult &result1, const KadResult &result2)
{
	return result1.Name < result2.Name;
}
bool NameGreaterThan(const KadResult &result1, const KadResult &result2)
{
	return result1.Name > result2.Name;
}

bool SizeLessThan(const KadResult &result1, const KadResult &result2)
{
	return result1.Size < result2.Size;
}
bool SizeGreaterThan(const KadResult &result1, const KadResult &result2)
{
	return result1.Size > result2.Size;
}

bool TypeLessThan(const KadResult &result1, const KadResult &result2)
{
	return result1.Type < result2.Type;
}
bool TypeGreaterThan(const KadResult &result1, const KadResult &result2)
{
	return result1.Type > result2.Type;
}

bool ResourceLessThan(const KadResult &result1, const KadResult &result2)
{
	return result1.Resource < result2.Resource;
}
bool ResourceGreaterThan(const KadResult &result1, const KadResult &result2)
{
	return result1.Resource > result2.Resource;
}

ResultTableModel::ResultTableModel(QObject *parent)
	: QAbstractTableModel(parent)
{
 
}

ResultTableModel::~ResultTableModel()
{

}

void ResultTableModel::sort(int column, Qt::SortOrder order)
{
	if(column == COLUMN_ID)
	{
		qSort(m_resultList.begin(), m_resultList.end(), order == Qt::AscendingOrder ? IDLessThan : IDGreaterThan);
	}
	else if(column == COLUMN_NAME)
	{
		qSort(m_resultList.begin(), m_resultList.end(), order == Qt::AscendingOrder ? NameLessThan : NameGreaterThan);
	}
	else if(column == COLUMN_SIZE)
	{
		qSort(m_resultList.begin(), m_resultList.end(), order == Qt::AscendingOrder ? SizeLessThan : SizeGreaterThan);
	}
	else if(column == COLUMN_TYPE)
	{
		qSort(m_resultList.begin(), m_resultList.end(), order == Qt::AscendingOrder ? TypeLessThan : TypeGreaterThan);
	}
	else if(column == COLUMN_RESOURCE)
	{
		qSort(m_resultList.begin(), m_resultList.end(), order == Qt::AscendingOrder ? ResourceLessThan : ResourceGreaterThan);
	}
	else if(column == COLUMN_OPERATE)
	{
		//return "����";
	}

	emit layoutChanged();
}

QVariant ResultTableModel::data(const QModelIndex &index, int role) const
{
	int row = index.row();
	int column = index.column();

	if(role == Qt::DisplayRole)
	{
		if(column == COLUMN_ID)
		{
			return m_resultList[row].Index;
		}
		else if(column == COLUMN_NAME)
		{
			return m_resultList[row].Name;
		}
		else if(column == COLUMN_SIZE)
		{
			return GetFileSize(m_resultList[row].Size);
		}
		else if(column == COLUMN_TYPE)
		{
			return m_resultList[row].Type;
		}
		else if(column == COLUMN_RESOURCE)
		{
			return m_resultList[row].Resource;
		}
		else if(column == COLUMN_OPERATE)
		{
			//return "����";
		}
	}
	else if(role == Qt::EditRole)
	{
		if(column == COLUMN_ID)
		{
			return m_resultList[row].Index;
		}
		else if(column == COLUMN_NAME)
		{
			return m_resultList[row].Name;
		}
		else if(column == COLUMN_SIZE)
		{
			return m_resultList[row].Size;
		}
		else if(column == COLUMN_TYPE)
		{
			return m_resultList[row].Type;
		}
		else if(column == COLUMN_RESOURCE)
		{
			return m_resultList[row].Resource;
		}
		else if(column == COLUMN_OPERATE)
		{
			//return "����";
		}
	}

	return QVariant();
}

QVariant ResultTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole)
	{
		if(orientation == Qt::Horizontal)
		{
			if(section == COLUMN_ID)
			{
				return "���";
			}
			else if(section == COLUMN_NAME)
			{
				return "����";
			}
			else if(section == COLUMN_SIZE)
			{
				return "��С";
			}
			else if(section == COLUMN_TYPE)
			{
				return "��ʽ";
			}
			else if(section == COLUMN_RESOURCE)
			{
				return "�ȶ�";
			}
			else if(section == COLUMN_OPERATE)
			{
				return "����";
			}
		}
	}

	return QVariant();

}

int ResultTableModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return m_resultList.count();
}

int ResultTableModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return COLUMN_OPERATE + 1;
}

Qt::ItemFlags ResultTableModel::flags(const QModelIndex &index) const
{
	Q_UNUSED(index);

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool ResultTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
	//beginInsertRows()֪ͨ���������������ı䣬endInsertRows()�Բ�������ȷ����֪ͨ������true��ʾ�ɹ�
    beginInsertRows(parent, row, row + count - 1);

    for (int column = 0; column < count; column++)
	{
		KadResult result;
		m_resultList.insert(row, result);
    }

    endInsertRows();

    return true;
}

bool ResultTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);

    for (int column = 0; column < count; column++)
	{
		m_resultList.removeAt(row);
    }

    endRemoveRows();

    return true;
}

int ResultTableModel::InsertData(const KadResult& result)
{
	//�����һ�в���
	int row = m_resultList.count();
	if(insertRow(row))
	{
		m_resultList[row] = result;
	}

	QModelIndex topLeft = index(m_resultList.count(), 0);
	QModelIndex bottomRight = index(m_resultList.count(), COLUMN_OPERATE);

	emit dataChanged(topLeft, bottomRight);

	return row;
}

KadResult& ResultTableModel::GetData(int row)
{
	return m_resultList[row];
}
