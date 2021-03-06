#include "stdafx.h"
#include "utilitymanager.h"
#include <Shlobj.h>
#include <QFile>
#include <sstream>
#include "Iphlpapi.h"
#include "windows.h"

namespace UtilityManager
{
	void MoveCenter(QWidget &widget)
	{
		// Get the size of screen
		QDesktopWidget* desktop = QApplication::desktop();
		int desktopWidth = desktop->availableGeometry().width();
		int desktopHeight = desktop->availableGeometry().height();

		// Move window
		widget.move((desktopWidth - widget.width())/2, (desktopHeight - widget.height())/2);
	}

	QString GetMacAddress()
	{
		QString macAddress;

		// Get the buffer length required for IP_ADAPTER_INFO.
		ULONG BufferLength = 0;
		BYTE* pBuffer = 0;
		if( ERROR_BUFFER_OVERFLOW == GetAdaptersInfo( 0, &BufferLength ))
		{
			// Now the BufferLength contain the required buffer length.
			// Allocate necessary buffer.
			pBuffer = new BYTE[ BufferLength ];

			// Get the Adapter Information.
			PIP_ADAPTER_INFO pAdapterInfo = reinterpret_cast<PIP_ADAPTER_INFO>(pBuffer);
			GetAdaptersInfo( pAdapterInfo, &BufferLength );

			// Iterate the network adapters and print their MAC address.
			while( pAdapterInfo )
			{
				// Assuming pAdapterInfo->AddressLength is 6.

				macAddress.sprintf("%02x%02x%02x%02x%02x%02x",
					pAdapterInfo->Address[0],
					pAdapterInfo->Address[1],
					pAdapterInfo->Address[2],
					pAdapterInfo->Address[3],
					pAdapterInfo->Address[4],
					pAdapterInfo->Address[5]);

				// Get next adapter info.
				pAdapterInfo = pAdapterInfo->Next;

				if(strcmp(macAddress.toLocal8Bit().data(), "000000000000") != 0)
				{
					break;
				}
			}

			// deallocate the buffer.
			delete[] pBuffer;
		}
		else
		{
			// Error occurred. handle it accordingly.
			macAddress = "000000000000";
		}

		return macAddress.toUpper();
	}

	QString GetHostName()
	{
		char buffer[128];
		DWORD dwLen = 128;

		GetComputerNameA(buffer, &dwLen);

		return QString::fromLocal8Bit(buffer);
	}

	QString GetHostTime()
	{
		time_t timep;
		time(&timep);

		return QString::number(timep);
	}

	QString GetHostZone()
	{
		TIME_ZONE_INFORMATION tzi;
		GetTimeZoneInformation(&tzi);

		return QString::number(tzi.Bias/-60);
	}

	QString GetSysVersion()
	{
		std::string version = "Unknown";

		OSVERSIONINFOEX osvi;
		memset(&osvi, 0, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if (GetVersionEx((OSVERSIONINFO *)&osvi)) 
		{
			switch(osvi.dwMajorVersion)
			{
			case 4:
				{
					switch(osvi.dwMinorVersion)
					{
					case 0:
						{
							if(osvi.dwPlatformId==VER_PLATFORM_WIN32_NT)
							{
								version = "Microsoft Windows NT 4.0";
							}
							else if(osvi.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)
							{
								version = "Microsoft Windows 95";
							}
						}
						break;  
					case 10:
						{
							version = "Microsoft Windows 98";
						}
						break;  
					case 90:
						{
							version = "Microsoft Windows Me";
						}
						break;
					}
				}
				break;
			case 5:
				{
					switch(osvi.dwMinorVersion)
					{
					case 0:
						{
							version = "Microsoft Windows 2000";
						}
						break;  
					case 1:
						{
							version = "Microsoft Windows XP";
						}
						break;
					case 2:
						{
							SYSTEM_INFO info;
							GetSystemInfo(&info);
							if(osvi.wProductType==VER_NT_WORKSTATION 
								&& info.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
							{
								version = "Microsoft Windows XP Professional x64 Edition";

							}
							else if(GetSystemMetrics(SM_SERVERR2)==0)
							{
								version = "Microsoft Windows Server 2003";
							}
							else if(GetSystemMetrics(SM_SERVERR2)!=0)
							{
								version = "Microsoft Windows Server 2003 R2";
							}
						}
						break;
					}
				}
				break;
			case 6:
				{
					switch(osvi.dwMinorVersion)
					{
					case 0:
						{
							if(osvi.wProductType==VER_NT_WORKSTATION)
							{
								version = "Microsoft Windows Vista";
							}
							else
							{
								version = "Microsoft Windows Server 2008";
							}
						}
						break;
					case 1:
						{
							if(osvi.wProductType==VER_NT_WORKSTATION)  
							{
								version = "Microsoft Windows 7";
							}
							else
							{
								version = "Microsoft Windows Server 2008 R2";
							}
						}
						break;  
					case 2:  
						{
							if(osvi.wProductType==VER_NT_WORKSTATION)  
							{
								version = "Microsoft Windows 8";
							}
							else
							{
								version = "Microsoft Windows Server 2012";
							}
						}
						break;  
					} 
				}
				break;
			default:
				{
					version = "UNKNOWN";
				}
			}
		}

		typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);   
		LPFN_ISWOW64PROCESS fnIsWow64Process; 

		BOOL bIsWow64 = FALSE;   
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle(_T("kernel32")),"IsWow64Process");   
		if (NULL != fnIsWow64Process)   
		{   
			fnIsWow64Process(GetCurrentProcess(), &bIsWow64);  
		}

		QString result = QString("%1, %2").arg(version.c_str()).arg(bIsWow64 ? "64bit" : "32bit");
		return result;
	}

	QString GetMemoryInfo()
	{   
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);
		GlobalMemoryStatusEx(&status);

		DWORD dwSize = (DWORD)(status.ullTotalPhys / (1024 * 1024));   
		//DWORD dwVirtSize = (DWORD)(status.ullTotalVirtual/(1024*1024));  

		return QString::number(dwSize);
	}

	QString GetCpuInfo()
	{
		HKEY hKey = 0;
		DWORD dwSize = 0;
		DWORD dwType = 0;

		LONG lReturn = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);
		if(lReturn != ERROR_SUCCESS)
		{
			return "";
		}

		//获取CPU主频
		unsigned __int64	dwSpeed = 0;
		dwSize = sizeof(dwSpeed);
		lReturn = RegQueryValueExA(hKey, "~MHz", NULL, &dwType, (LPBYTE)&dwSpeed, &dwSize);
		if(lReturn != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return "";
		}

		//获取CPU indetifier (family, model, stepping)
		char sIdentifier[512] = {0};
		dwSize = sizeof(sIdentifier);
		lReturn = RegQueryValueExA(hKey, "Identifier", NULL, &dwType, (LPBYTE)sIdentifier, &dwSize);
		if(lReturn != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return "";
		}

		//获取CPU名称
		char sProcessorName[512] = {0};
		dwSize = sizeof(sProcessorName);
		lReturn = RegQueryValueExA(hKey, "ProcessorNameString", NULL, &dwType, (LPBYTE)sProcessorName, &dwSize);
		if(lReturn != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return "";
		}

		//获取供应商名称
		char sVendorName[512] = {0};
		dwSize = sizeof(sVendorName);
		lReturn = RegQueryValueExA(hKey, "VendorIdentifier", NULL, &dwType, (LPBYTE)sVendorName, &dwSize);
		if(lReturn != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return "";
		}

		RegCloseKey(hKey);

		QString result = QString("%1, %2, %3, %4").arg(sProcessorName).arg(dwSpeed).arg(sIdentifier).arg(sVendorName);
		return result;
	}

	QString GeySysLanguage()
	{
		char szBuffer[MAX_PATH] = {0};
		DWORD dwBufLen = MAX_PATH;
		GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, szBuffer, dwBufLen);

		return QString::fromLocal8Bit(szBuffer);
	}

	QString GeySysCountry()
	{
		char szBuffer[MAX_PATH] = {0};
		DWORD dwBufLen = MAX_PATH;
		GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, szBuffer, dwBufLen);

		return QString::fromLocal8Bit(szBuffer);
	}

	QString GeySysMetrics()
	{
		int width = GetSystemMetrics(SM_CXSCREEN); 
		int height= GetSystemMetrics(SM_CYSCREEN);

		QString result = QString("%1*%2").arg(width).arg(height);
		return result;
	}

	QString GeySysPixel()
	{
		int nBitsPerPixel = 0;
		DEVMODE DevMode;
		if(EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DevMode))
		{
			if(DevMode.dmFields & DM_BITSPERPEL) 
			{
				nBitsPerPixel=DevMode.dmBitsPerPel;
			}
		}

		return QString::number(nBitsPerPixel);
	}

	QString GetTrimString(QString text, int length)
	{
		if(text.length() > length)
		{
			text = text.left(length) + "...";
		}

		return text;
	}

	std::string ws2s(const std::wstring& ws)
	{
		int nBufSize = WideCharToMultiByte(GetACP(), 0, ws.c_str(),-1, NULL, 0, 0, FALSE);
		char *szBuf = new char[nBufSize];
		WideCharToMultiByte(GetACP(), 0, ws.c_str(), -1, szBuf, nBufSize, 0, FALSE);
		std::string strRet(szBuf);
		delete []szBuf;
		szBuf = NULL;
		return strRet;
	}

	std::wstring s2ws(const std::string& s)
	{
		int nBufSize = MultiByteToWideChar(GetACP(), 0, s.c_str(), -1, NULL, 0);
		wchar_t *wsBuf = new wchar_t[nBufSize];
		MultiByteToWideChar(GetACP(), 0, s.c_str(), -1, wsBuf, nBufSize);
		std::wstring wstrRet(wsBuf);
		delete []wsBuf;
		wsBuf = NULL;    
		return wstrRet;
	}

	std::vector<std::string> split(const std::string &s, char delim)
	{
		std::vector<std::string> elems;

		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim))
		{
			elems.push_back(item);
		}

		return elems;
	}

	void ModifyBorwserHome(QString home)
	{
		//IE
		ModifyIEBorwserHome(home);

		//Chrome
		ModifyChromeBorwserHome(home);
	}

	void ModifyIEBorwserHome(QString home)
	{
		HKEY hKey; 
		LPCTSTR lpRun = L"Software\\Microsoft\\Internet Explorer\\Main"; 
		long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_WRITE, &hKey);
		if(lRet == ERROR_SUCCESS) 
		{ 
			RegSetValueEx(hKey, L"Start Page", 0, REG_SZ, (BYTE *)home.toStdWString().c_str(), home.length()*2);
			RegCloseKey(hKey);
		}
	}

	void ModifyChromeBorwserHome(QString home)
	{
		wchar_t system_buffer[MAX_PATH] = {0};
		SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, system_buffer);
		QString preferences = QString::fromStdWString(system_buffer);
		preferences += "/Google/Chrome/User Data/Default/Preferences";

		QFile file(preferences);
		if (file.exists())
		{
			//"session": {
			//	"restore_on_startup": 4,
			//	"restore_on_startup_migrated": true,
			//	"startup_urls": [ "http://www.baidu.com/" ],
			//	"startup_urls_migration_time": "13041673091070296"
			//},

			if (!file.open(QIODevice::ReadOnly))
				return;

			QByteArray content = file.readAll();
			QJsonParseError error;
			QJsonDocument prefDocmount = QJsonDocument::fromJson(content, &error);
			if (error.error == QJsonParseError::NoError)
			{
				if (prefDocmount.isObject())
				{
					QJsonObject prefObject = prefDocmount.object();
					if (prefObject.contains("session"))
					{
						QJsonValue session = prefObject.take("session");
						if (session.isObject())
						{
							QJsonObject sessionObject = session.toObject();
							if (sessionObject.contains("restore_on_startup"))
								sessionObject["restore_on_startup"] = 4;
							else
								sessionObject.insert("restore_on_startup", 4);

							if (sessionObject.contains("startup_urls"))
							{
								QJsonValue startup_urls = sessionObject.take("startup_urls");
								if (startup_urls.isArray())
								{
									QJsonArray startup_urls_array  = startup_urls.toArray();
									if (startup_urls_array.size() > 0)
									{
										startup_urls_array[0] = home;
										startup_urls = startup_urls_array;
									}
									sessionObject.insert("startup_urls", startup_urls);
								}
							}
							else
							{
								QJsonArray startup_urls_array;
								startup_urls_array.insert(0, home);
								sessionObject.insert("startup_urls", startup_urls_array);
							}
							session = sessionObject;
						}

						prefObject.insert("session", session);
					}
					prefDocmount.setObject(prefObject);

					QString content = prefDocmount.toJson();
					file.close();
					if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
						return;

					file.write(content.toUtf8());
				}
			}
		}
	}

	bool IsAutoRun(QString keyName)
	{
		bool f = false;
		HKEY hKey; 
		LPCTSTR lpRun = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"; 
		long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_READ, &hKey);
		if(lRet == ERROR_SUCCESS) 
		{ 
			lRet = RegQueryValueEx(hKey, keyName.toStdWString().c_str(), 0, NULL, NULL, NULL);
			f = lRet == ERROR_SUCCESS;
			RegCloseKey(hKey);
		}

		return f;
	}

	void SetAsAutoRun(QString keyName, bool on)
	{
		HKEY hKey; 
		LPCTSTR lpRun = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"; 
		long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_WRITE, &hKey);
		if(lRet == ERROR_SUCCESS) 
		{ 
			if (on)
			{
				wchar_t pFileName[MAX_PATH] = {0}; 
				GetModuleFileName(NULL, pFileName, MAX_PATH);
				wchar_t pTemp[MAX_PATH] = {0}; 
				wsprintf(pTemp, L"\"%s\" -auto", pFileName);
				RegSetValueEx(hKey, keyName.toStdWString().c_str(), 0, REG_SZ, (BYTE *)pTemp, wcslen(pTemp)*2);
			}
			else
			{
				RegDeleteValue(hKey, keyName.toStdWString().c_str());
			}
			RegCloseKey(hKey);
		}
	}

}