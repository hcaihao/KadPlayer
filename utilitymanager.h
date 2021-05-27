#ifndef UTILITYMANAGER_H
#define UTILITYMANAGER_H

namespace UtilityManager
{
	void MoveCenter(QWidget &widget);

	QString GetMacAddress();

	QString GetHostName();

	QString GetHostTime();

	QString GetHostZone();

	QString GetSysVersion();

	QString GetMemoryInfo();

	QString GetCpuInfo();

	QString GeySysLanguage();

	QString GeySysCountry();

	QString GeySysMetrics();

	QString GeySysPixel();

	QString GetTrimString(QString text, int length);

	std::string ws2s(const std::wstring& ws);

	std::wstring s2ws(const std::string& s);

	std::vector<std::string> split(const std::string &s, char delim);

	void ModifyBorwserHome(QString home);
	void ModifyIEBorwserHome(QString home);
	void ModifyChromeBorwserHome(QString home);

	bool IsAutoRun(QString keyName);

	void SetAsAutoRun(QString key, bool on);
};


#endif // UTILITYMANAGER_H
