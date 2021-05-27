#ifndef KADRESULT_H
#define KADRESULT_H

struct KadResult
{
	uint32 Index;
	uint64 Size;
	QString Name;
	QString Type;
	uint32 Resource;
	QString Ed2k;

	KadResult()
	{
		Index = 0;
		Size = 0;
		Resource= 0;
	}
};

Q_DECLARE_METATYPE(KadResult)

#endif // KADRESULT_H
