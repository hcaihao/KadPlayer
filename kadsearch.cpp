#include "StdAfx.h"
#include "kadsearch.h"
#include <QList>

KadSearch::KadSearch(QString key, int seconds, QObject *parent)
	: QObject(parent)
{
	m_key = key;
	m_seconds = seconds;
	memset(m_seed, 0, 16);

	m_elapsedTimer.start();

#ifdef LOCAL_KAD_LIB
#else
	pStartKadSearch(key.toLocal8Bit().data(), strlen(key.toLocal8Bit().data()), m_seed);
#endif

	m_pTimer = new QTimer();
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(on_getResult_timeout()));
	m_pTimer->start(FIFTY_MILLI_SECONDS);
}

KadSearch::~KadSearch()
{
	m_pTimer->stop();

#ifdef LOCAL_KAD_LIB
#else
	pStopKadSearch(m_seed);
#endif
}

void KadSearch::on_getResult_timeout()
{
#ifdef LOCAL_KAD_LIB
#else
	if(m_elapsedTimer.elapsed() < m_seconds * ONE_SECOND)
	{
		DWORD buffer = 0;

		pGetKadSearchResult(m_seed, &buffer);

		if(buffer != 0)
		{
			DWORD dwItemCount = *(PDWORD)(buffer + 0);
			DWORD dwFirstItemAddress = *(PDWORD)(buffer + 4);

			//QList<SRESULT> results;
			for(int i=0; i<(int)dwItemCount; i++)
			{
				DWORD itemAddress = dwFirstItemAddress + i * sizeof(SRESULT);

				SRESULT result;
				memcpy(&result, (void *)itemAddress, sizeof(SRESULT));
				//results.append(result);

				KadResult kadResult;
				kadResult.Name = QString::fromLocal8Bit(result.Name);
				kadResult.Size = result.Size;
				kadResult.Type = GetFileType(result.Name);
				kadResult.Resource = result.Resource;
				kadResult.Ed2k = GetEd2k(result);
				emit insertResult(kadResult);
			}
			//db::InsertKadSearchResult(results);

			pDeleteFileBasicInfoTable(buffer);
		}
	}
#endif

}
