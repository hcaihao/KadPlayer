#ifndef CRYPTMANAGER_H
#define CRYPTMANAGER_H

#include <string>
using namespace std;

namespace CryptManager
{
	string EncodeDesString(string text);

	string EncodeDesString(string text, string key);

	string DecodeDesString(string text);

	string DecodeDesString(string text, string key);

	string EncodeBase64String(string text);

	string DecodeBase64String(string text);

	string GetFileBase64(string path);

	string EncodeMd5String(string text);

	string GetFileMd5(string path);

	string EncodeShaString(string value);

	QString EncodeUrlString(QString text);

	QString DecodeUrlString(QString text);

}

#endif // CRYPTMANAGER_H
