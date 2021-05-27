#include "StdAfx.h"
#include "servicemanager.h"
#include "utilitymanager.h"
#include "cryptmanager.h"

#include "GSoap\KadServiceSoap.nsmap"
#include "GSoap\soapKadServiceSoapProxy.h"

KadServiceSoapProxy g_soap;

ServiceManager::ServiceManager(QObject *parent)
	: QObject(parent)
{
	//GSoap生成方法
	//wsdl2h -o service.h http://kadplayer.com:81/kadservice.asmx?wsdl
	//soapcpp2 -C -L -x -i -I"E:\GSoap\gsoap\import" service.h
	//soapC.cpp, soapClient.cpp, stdsoap2.cpp不使用头文件
	//复制stdsoap2.h, stdsoap2.cpp到工程目录

	g_soap.connect_timeout = 10;
	g_soap.accept_timeout = 10;
	g_soap.send_timeout = 10;
	g_soap.recv_timeout = 10;
	g_soap.mode = SOAP_C_UTFSTRING;
}

ServiceManager::~ServiceManager()
{
	
}

void ServiceManager::GetSystemMessage(QString version)
{
	QString result;

	std::string myVersion = UtilityManager::ws2s(version.toStdWString());
	myVersion = CryptManager::EncodeDesString(myVersion, DES_KEY);

	QString mac = UtilityManager::GetMacAddress();
	std::string myMac = UtilityManager::ws2s(mac.toStdWString());
	myMac = CryptManager::EncodeDesString(myMac, DES_KEY);


	QString extension = tr("time=%1|utc=%2|mac=%3|os=%4|cpu=%5|memory=%6|language=%7|country=%8|screen=%9|bit=%10")
		.arg(UtilityManager::GetHostTime().trimmed())
		.arg(UtilityManager::GetHostZone().trimmed())
		.arg(UtilityManager::GetMacAddress().trimmed())
		.arg(UtilityManager::GetSysVersion().trimmed())
		.arg(UtilityManager::GetCpuInfo().trimmed())
		.arg(UtilityManager::GetMemoryInfo().trimmed())
		.arg(UtilityManager::GeySysLanguage().trimmed())
		.arg(UtilityManager::GeySysCountry().trimmed())
		.arg(UtilityManager::GeySysMetrics().trimmed())
		.arg(UtilityManager::GeySysPixel().trimmed())
		;
	std::string myExtension = UtilityManager::ws2s(extension.toStdWString());
	myExtension = CryptManager::EncodeDesString(myExtension, DES_KEY);

	try
	{
		_ns1__GetSystemMessage request;
		request.mac = &myMac;
		request.version = &myVersion;
		request.extension = &myExtension;
		_ns1__GetSystemMessageResponse response;
		int gsoapResult = g_soap.GetSystemMessage(WEB_SERVICE_URL, NULL, &request, &response);
		if(gsoapResult == SOAP_OK)
		{
			std::wstring notify = UtilityManager::s2ws(CryptManager::DecodeDesString(*response.GetSystemMessageResult, DES_KEY));
			result = QString::fromStdWString(notify);

			emit getSystemMessage(result);
		}
	}
	catch(...)
	{
		int a = 0;
	}
}

void ServiceManager::GetSensitiveWords()
{
	QStringList result;

	try
	{
		_ns1__GetSensitiveWords request;
		_ns1__GetSensitiveWordsResponse response;
		int gsoapResult = g_soap.GetSensitiveWords(WEB_SERVICE_URL, NULL, &request, &response);
		if(gsoapResult == SOAP_OK)
		{
			std::wstring words = UtilityManager::s2ws(CryptManager::DecodeDesString(*response.GetSensitiveWordsResult, DES_KEY));
			result = QString::fromStdWString(words).split(",", QString::SkipEmptyParts);

			emit getSensitiveWords(result);
		}
	}
	catch(...)
	{
		int a = 0;
	}
}

void ServiceManager::GetHotEd2ks()
{
	QStringList result;

	try
	{
		_ns1__GetHotEd2ks request;
		_ns1__GetHotEd2ksResponse response;
		int gsoapResult = g_soap.GetHotEd2ks(WEB_SERVICE_URL, NULL, &request, &response);
		if(gsoapResult == SOAP_OK)
		{
			std::wstring words = UtilityManager::s2ws(CryptManager::DecodeDesString(*response.GetHotEd2ksResult, DES_KEY));
			result = QString::fromStdWString(words).split(",", QString::SkipEmptyParts);

			emit getHotEd2ks(result);
		}
	}
	catch(...)
	{
		int a = 0;
	}
}

void ServiceManager::GetHotWords()
{
	QStringList result;

	try
	{
		_ns1__GetHotWords request;
		_ns1__GetHotWordsResponse response;
		int gsoapResult = g_soap.GetHotWords(WEB_SERVICE_URL, NULL, &request, &response);
		if(gsoapResult == SOAP_OK)
		{
			std::wstring words = UtilityManager::s2ws(CryptManager::DecodeDesString(*response.GetHotWordsResult, DES_KEY));
			result = QString::fromStdWString(words).split(",", QString::SkipEmptyParts);

			emit getHotWords(result);
		}
	}
	catch(...)
	{
		int a = 0;
	}
}

void ServiceManager::GetRandomWord()
{
	QString result;

	try
	{
		_ns1__GetRandomWord request;
		_ns1__GetRandomWordResponse response;
		int gsoapResult = g_soap.GetRandomWord(WEB_SERVICE_URL, NULL, &request, &response);
		if(gsoapResult == SOAP_OK)
		{
			std::wstring word = UtilityManager::s2ws(CryptManager::DecodeDesString(*response.GetRandomWordResult, DES_KEY));
			result = QString::fromStdWString(word);

			emit getRandomWord(result);
		}
	}
	catch(...)
	{
		int a = 0;
	}
}

void ServiceManager::SetSearchLog(QString key)
{
	std::string myKey = UtilityManager::ws2s(key.toStdWString());
	myKey = CryptManager::EncodeDesString(myKey, DES_KEY);

	QString mac = UtilityManager::GetMacAddress();
	std::string myMac = UtilityManager::ws2s(mac.toStdWString());
	myMac = CryptManager::EncodeDesString(myMac, DES_KEY);

	try
	{
		_ns1__SetSearchLog request;
		request.keys = &myKey;
		request.mac = &myMac;
		_ns1__SetSearchLogResponse response;
		int gsoapResult = g_soap.SetSearchLog(WEB_SERVICE_URL, NULL, &request, &response);
		if(gsoapResult == SOAP_OK)
		{
			emit setSearchLog(key);
		}
	}
	catch(...)
	{
		int a = 0;
	}
}

void ServiceManager::SetFeedback(QString version, int type, QString content)
{
	std::string myVersion = UtilityManager::ws2s(version.toStdWString());
	myVersion = CryptManager::EncodeDesString(myVersion, DES_KEY);

	std::string myContent = UtilityManager::ws2s(content.toStdWString());
	myContent = CryptManager::EncodeDesString(myContent, DES_KEY);

	QString mac = UtilityManager::GetMacAddress();
	std::string myMac = UtilityManager::ws2s(mac.toStdWString());
	myMac = CryptManager::EncodeDesString(myMac, DES_KEY);

	try
	{
		_ns1__SetFeedback request;
		request.mac = &myMac;
		request.version = &myVersion;
		request.type = type;
		request.content = &myContent;
		_ns1__SetFeedbackResponse response;
		int gsoapResult = g_soap.SetFeedback(WEB_SERVICE_URL, NULL, &request, &response);
		if(gsoapResult == SOAP_OK)
		{
			emit setFeedback(version, type, content);
		}
	}
	catch(...)
	{
		int a = 0;
	}
}

void ServiceManager::SetPlayLog(QString link)
{
	std::string myLink = UtilityManager::ws2s(link.toStdWString());
	myLink = CryptManager::EncodeDesString(myLink, DES_KEY);

	QString mac = UtilityManager::GetMacAddress();
	std::string myMac = UtilityManager::ws2s(mac.toStdWString());
	myMac = CryptManager::EncodeDesString(myMac, DES_KEY);

	try
	{
		_ns1__SetPlayLog request;
		request.links = &myLink;
		request.mac = &myMac;
		_ns1__SetPlayLogResponse response;
		int gsoapResult = g_soap.SetPlayLog(WEB_SERVICE_URL, NULL, &request, &response);
		if(gsoapResult == SOAP_OK)
		{
			emit setPlayLog(link);
		}
	}
	catch(...)
	{
		int a = 0;
	}
}