#include "stdafx.h"

void RefreshStyle(QWidget *widget, const char* name, QVariant value)
{
	widget->setProperty(name, value);

	//widget->parentWidget()->style()->unpolish(widget);
	//widget->parentWidget()->style()->polish(widget);
	//widget->update();

	qApp->style()->unpolish(widget);
	qApp->style()->polish(widget);
	widget->update();
}

void LoadStyle(QWidget *widget, QString filePath)
{
	QString styleSheet;

	{
		QFile file(":/Skin/common.qss");
		//QFile file("Z:\\VC Project\\KadPlayer\\Project\\KadPlayer\\Resources\\Skin\\common.qss");
		if(file.open(QFile::ReadOnly))
		{
			styleSheet.append(QString::fromLocal8Bit(file.readAll()));

			file.close();
		}
	}

	{
		QFile file(filePath);
		if(file.open(QFile::ReadOnly))
		{
			styleSheet.append(QString::fromLocal8Bit(file.readAll()));

			file.close();
		}
	}

	widget->setStyleSheet(styleSheet);
}

void LoadStyle(QString filePath)
{
	QString styleSheet;

	{
		QFile file(":/Skin/common.qss");
		//QFile file("Z:\\VC Project\\KadPlayer\\Project\\KadPlayer\\Resources\\Skin\\common.qss");
		if(file.open(QFile::ReadOnly))
		{
			styleSheet.append(QString::fromLocal8Bit(file.readAll()));

			file.close();
		}
	}

	{
		QFile file(filePath);
		if(file.open(QFile::ReadOnly))
		{
			styleSheet.append(QString::fromLocal8Bit(file.readAll()));

			file.close();
		}
	}

	qApp->setStyleSheet(styleSheet);

}

QString GetHexString(const BYTE *pBuffer, int len)
{
	QString result;

	for(int i=0; i<len; i++)
	{
		result.append(QString("%1").arg(pBuffer[i], 2, 16, QLatin1Char('0')));
	}

	return result.trimmed().toUpper();
}

QString GetFileSize(unsigned __int64 size)
{
	quint64 KB = 1024;
	quint64 MB = 1024 * KB;
	quint64 GB = 1024 * MB;
	quint64 TB = 1024 * GB;

	//转换成TB
	if (size > TB)
	{
		return QString("%1 TB").arg((double)size / TB, 0, 'f', 2);
	}
	//转换成GB
	else if (size > GB)
	{
		return QString("%1 GB").arg((double)size / GB, 0, 'f', 2);
	}
	//转换成MB
	else if (size > MB)
	{
		return QString("%1 MB").arg((double)size / MB, 0, 'f', 2);
	}
	//转换成KB
	else if (size > KB)
	{
		return QString("%1 KB").arg((double)size / KB, 0, 'f', 2);
	}
	else
	{
		return QString("%1 Byte").arg(size);
	}

	return QString("%1").arg(size);
}

QString GetFileType(const char *name)
{
	if(g_attributeHash.empty())
	{
		// 影视Movie
		g_attributeHash.insert(".rm", QObject::tr("Video"));
		g_attributeHash.insert(".rmvb", QObject::tr("Video"));
		g_attributeHash.insert(".mp4", QObject::tr("Video"));
		g_attributeHash.insert(".mkv", QObject::tr("Video"));
		g_attributeHash.insert(".mov", QObject::tr("Video"));
		g_attributeHash.insert(".mpg", QObject::tr("Video"));
		g_attributeHash.insert(".wmv", QObject::tr("Video"));
		g_attributeHash.insert(".avi", QObject::tr("Video"));
		g_attributeHash.insert(".3gp", QObject::tr("Video"));
		g_attributeHash.insert(".flv", QObject::tr("Video"));
		g_attributeHash.insert(".mpeg", QObject::tr("Video"));
		g_attributeHash.insert(".asf", QObject::tr("Video"));
		g_attributeHash.insert(".vob", QObject::tr("Video"));
		g_attributeHash.insert(".ts", QObject::tr("Video"));

		g_attributeHash.insert(".srt", QObject::tr("Subtitle"));
		g_attributeHash.insert(".sub", QObject::tr("Subtitle"));
		g_attributeHash.insert(".ass", QObject::tr("Subtitle"));
		g_attributeHash.insert(".ssa", QObject::tr("Subtitle"));

		// 音乐Music
		g_attributeHash.insert(".mp3", QObject::tr("Audio"));
		g_attributeHash.insert(".wma", QObject::tr("Audio"));
		g_attributeHash.insert(".mid", QObject::tr("Audio"));
		g_attributeHash.insert(".wav", QObject::tr("Audio"));
		g_attributeHash.insert(".ape", QObject::tr("Audio"));
		g_attributeHash.insert(".flac", QObject::tr("Audio"));
		g_attributeHash.insert(".m4a", QObject::tr("Audio"));

		g_attributeHash.insert(".lrc", QObject::tr("Lyric"));
		g_attributeHash.insert(".qrc", QObject::tr("Lyric"));
		g_attributeHash.insert(".krc", QObject::tr("Lyric"));

		// 文件File
		g_attributeHash.insert(".zip", QObject::tr("Zip"));
		g_attributeHash.insert(".rar", QObject::tr("Zip"));
		g_attributeHash.insert(".7z", QObject::tr("Zip"));

		g_attributeHash.insert(".iso", QObject::tr("CD"));
		g_attributeHash.insert(".mdf", QObject::tr("CD"));
		g_attributeHash.insert(".dmg", QObject::tr("CD"));

		// 其他Other
		g_attributeHash.insert(".exe", QObject::tr("Program"));
		g_attributeHash.insert(".com", QObject::tr("Program"));
		g_attributeHash.insert(".bat", QObject::tr("Program"));
		g_attributeHash.insert(".dll", QObject::tr("Program"));

		g_attributeHash.insert(".txt", QObject::tr("Document"));
		g_attributeHash.insert(".ini", QObject::tr("Document"));
		g_attributeHash.insert(".pdf", QObject::tr("Document"));
		g_attributeHash.insert(".chm", QObject::tr("Document"));
		g_attributeHash.insert(".doc", QObject::tr("Document"));
		g_attributeHash.insert(".xls", QObject::tr("Document"));
		g_attributeHash.insert(".ppt", QObject::tr("Document"));

		g_attributeHash.insert(".jpg", QObject::tr("Picture"));
		g_attributeHash.insert(".gif", QObject::tr("Picture"));
		g_attributeHash.insert(".bmp", QObject::tr("Picture"));
		g_attributeHash.insert(".png", QObject::tr("Picture"));

		g_attributeHash.insert(".mht", QObject::tr("Web"));
		g_attributeHash.insert(".htm", QObject::tr("Web"));
		g_attributeHash.insert(".html", QObject::tr("Web"));

		g_attributeHash.insert(".epub", QObject::tr("Ebook"));
		g_attributeHash.insert(".torrent", QObject::tr("Torrent"));
	}

	QString myName = QString::fromLocal8Bit(name);
	int pos = myName.lastIndexOf(".");
	if(pos != -1)
	{
		QString type = myName.mid(pos).toLower();

		return g_attributeHash.value(type);
	}

	return "";
}

QString GetFileFormat(const char *name)
{
	QString myName = QString::fromLocal8Bit(name);

	int pos = myName.lastIndexOf(".");
	if(pos != -1)
	{
		return myName.mid(pos + 1);
	}

	return "";
}

QString GetFileName(QString link)
{
	QString fileName;

	QRegExp linkReg("ed2k://\\|file\\|(.*)\\|.*\\|.*\\|/");
	linkReg.setMinimal(true);
	if(linkReg.indexIn(link) != -1)
	{
		fileName = linkReg.cap(1);
	}

	return fileName;
}

QString GetFileSize(QString link)
{
	QString fileSize;

	QRegExp linkReg("ed2k://\\|file\\|.*\\|(.*)\\|.*\\|/");
	linkReg.setMinimal(true);
	if(linkReg.indexIn(link) != -1)
	{
		fileSize = linkReg.cap(1);
	}

	return fileSize;
}

QString GetHash(QString link)
{
	QString hash;

	QRegExp linkReg("ed2k://\\|file\\|.*\\|.*\\|(.*)\\|/");
	linkReg.setMinimal(true);
	if(linkReg.indexIn(link) != -1)
	{
		hash = linkReg.cap(1);
	}

	return hash;

}

QString GetEd2k(const SRESULT &sResult)
{
	QString link = QString("ed2k://|file|%1|%2|%3|/")
		.arg(QString::fromLocal8Bit(sResult.Name))
		.arg(sResult.Size)
		.arg(GetHexString(sResult.Hash, 16));

	return link;
}
