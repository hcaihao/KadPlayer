/* soapKadServiceSoapProxy.h
   Generated by gSOAP 2.8.17r from service.h

Copyright(C) 2000-2013, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#ifndef soapKadServiceSoapProxy_H
#define soapKadServiceSoapProxy_H
#include "soapH.h"

class SOAP_CMAC KadServiceSoapProxy : public soap
{ public:
	/// Endpoint URL of service 'KadServiceSoapProxy' (change as needed)
	const char *soap_endpoint;
	/// Constructor
	KadServiceSoapProxy();
	/// Construct from another engine state
	KadServiceSoapProxy(const struct soap&);
	/// Constructor with endpoint URL
	KadServiceSoapProxy(const char *url);
	/// Constructor with engine input+output mode control
	KadServiceSoapProxy(soap_mode iomode);
	/// Constructor with URL and input+output mode control
	KadServiceSoapProxy(const char *url, soap_mode iomode);
	/// Constructor with engine input and output mode control
	KadServiceSoapProxy(soap_mode imode, soap_mode omode);
	/// Destructor frees deserialized data
	virtual	~KadServiceSoapProxy();
	/// Initializer used by constructors
	virtual	void KadServiceSoapProxy_init(soap_mode imode, soap_mode omode);
	/// Delete all deserialized data (with soap_destroy and soap_end)
	virtual	void destroy();
	/// Delete all deserialized data and reset to default
	virtual	void reset();
	/// Disables and removes SOAP Header from message
	virtual	void soap_noheader();
	/// Get SOAP Header structure (NULL when absent)
	virtual	const SOAP_ENV__Header *soap_header();
	/// Get SOAP Fault structure (NULL when absent)
	virtual	const SOAP_ENV__Fault *soap_fault();
	/// Get SOAP Fault string (NULL when absent)
	virtual	const char *soap_fault_string();
	/// Get SOAP Fault detail as string (NULL when absent)
	virtual	const char *soap_fault_detail();
	/// Close connection (normally automatic, except for send_X ops)
	virtual	int soap_close_socket();
	/// Force close connection (can kill a thread blocked on IO)
	virtual	int soap_force_close_socket();
	/// Print fault
	virtual	void soap_print_fault(FILE*);
#ifndef WITH_LEAN
	/// Print fault to stream
#ifndef WITH_COMPAT
	virtual	void soap_stream_fault(std::ostream&);
#endif

	/// Put fault into buffer
	virtual	char *soap_sprint_fault(char *buf, size_t len);
#endif

	/// Web service operation 'GetSystemNotify' (returns error code or SOAP_OK)
	virtual	int GetSystemNotify(_ns1__GetSystemNotify *ns1__GetSystemNotify, _ns1__GetSystemNotifyResponse *ns1__GetSystemNotifyResponse) { return this->GetSystemNotify(NULL, NULL, ns1__GetSystemNotify, ns1__GetSystemNotifyResponse); }
	virtual	int GetSystemNotify(const char *endpoint, const char *soap_action, _ns1__GetSystemNotify *ns1__GetSystemNotify, _ns1__GetSystemNotifyResponse *ns1__GetSystemNotifyResponse);

	/// Web service operation 'GetSystemMessage' (returns error code or SOAP_OK)
	virtual	int GetSystemMessage(_ns1__GetSystemMessage *ns1__GetSystemMessage, _ns1__GetSystemMessageResponse *ns1__GetSystemMessageResponse) { return this->GetSystemMessage(NULL, NULL, ns1__GetSystemMessage, ns1__GetSystemMessageResponse); }
	virtual	int GetSystemMessage(const char *endpoint, const char *soap_action, _ns1__GetSystemMessage *ns1__GetSystemMessage, _ns1__GetSystemMessageResponse *ns1__GetSystemMessageResponse);

	/// Web service operation 'GetUserSetting' (returns error code or SOAP_OK)
	virtual	int GetUserSetting(_ns1__GetUserSetting *ns1__GetUserSetting, _ns1__GetUserSettingResponse *ns1__GetUserSettingResponse) { return this->GetUserSetting(NULL, NULL, ns1__GetUserSetting, ns1__GetUserSettingResponse); }
	virtual	int GetUserSetting(const char *endpoint, const char *soap_action, _ns1__GetUserSetting *ns1__GetUserSetting, _ns1__GetUserSettingResponse *ns1__GetUserSettingResponse);

	/// Web service operation 'GetSensitiveWords' (returns error code or SOAP_OK)
	virtual	int GetSensitiveWords(_ns1__GetSensitiveWords *ns1__GetSensitiveWords, _ns1__GetSensitiveWordsResponse *ns1__GetSensitiveWordsResponse) { return this->GetSensitiveWords(NULL, NULL, ns1__GetSensitiveWords, ns1__GetSensitiveWordsResponse); }
	virtual	int GetSensitiveWords(const char *endpoint, const char *soap_action, _ns1__GetSensitiveWords *ns1__GetSensitiveWords, _ns1__GetSensitiveWordsResponse *ns1__GetSensitiveWordsResponse);

	/// Web service operation 'GetHotWords' (returns error code or SOAP_OK)
	virtual	int GetHotWords(_ns1__GetHotWords *ns1__GetHotWords, _ns1__GetHotWordsResponse *ns1__GetHotWordsResponse) { return this->GetHotWords(NULL, NULL, ns1__GetHotWords, ns1__GetHotWordsResponse); }
	virtual	int GetHotWords(const char *endpoint, const char *soap_action, _ns1__GetHotWords *ns1__GetHotWords, _ns1__GetHotWordsResponse *ns1__GetHotWordsResponse);

	/// Web service operation 'GetHotEd2ks' (returns error code or SOAP_OK)
	virtual	int GetHotEd2ks(_ns1__GetHotEd2ks *ns1__GetHotEd2ks, _ns1__GetHotEd2ksResponse *ns1__GetHotEd2ksResponse) { return this->GetHotEd2ks(NULL, NULL, ns1__GetHotEd2ks, ns1__GetHotEd2ksResponse); }
	virtual	int GetHotEd2ks(const char *endpoint, const char *soap_action, _ns1__GetHotEd2ks *ns1__GetHotEd2ks, _ns1__GetHotEd2ksResponse *ns1__GetHotEd2ksResponse);

	/// Web service operation 'GetRandomWord' (returns error code or SOAP_OK)
	virtual	int GetRandomWord(_ns1__GetRandomWord *ns1__GetRandomWord, _ns1__GetRandomWordResponse *ns1__GetRandomWordResponse) { return this->GetRandomWord(NULL, NULL, ns1__GetRandomWord, ns1__GetRandomWordResponse); }
	virtual	int GetRandomWord(const char *endpoint, const char *soap_action, _ns1__GetRandomWord *ns1__GetRandomWord, _ns1__GetRandomWordResponse *ns1__GetRandomWordResponse);

	/// Web service operation 'SetSearchLog' (returns error code or SOAP_OK)
	virtual	int SetSearchLog(_ns1__SetSearchLog *ns1__SetSearchLog, _ns1__SetSearchLogResponse *ns1__SetSearchLogResponse) { return this->SetSearchLog(NULL, NULL, ns1__SetSearchLog, ns1__SetSearchLogResponse); }
	virtual	int SetSearchLog(const char *endpoint, const char *soap_action, _ns1__SetSearchLog *ns1__SetSearchLog, _ns1__SetSearchLogResponse *ns1__SetSearchLogResponse);

	/// Web service operation 'SetFeedback' (returns error code or SOAP_OK)
	virtual	int SetFeedback(_ns1__SetFeedback *ns1__SetFeedback, _ns1__SetFeedbackResponse *ns1__SetFeedbackResponse) { return this->SetFeedback(NULL, NULL, ns1__SetFeedback, ns1__SetFeedbackResponse); }
	virtual	int SetFeedback(const char *endpoint, const char *soap_action, _ns1__SetFeedback *ns1__SetFeedback, _ns1__SetFeedbackResponse *ns1__SetFeedbackResponse);

	/// Web service operation 'SetPlayLog' (returns error code or SOAP_OK)
	virtual	int SetPlayLog(_ns1__SetPlayLog *ns1__SetPlayLog, _ns1__SetPlayLogResponse *ns1__SetPlayLogResponse) { return this->SetPlayLog(NULL, NULL, ns1__SetPlayLog, ns1__SetPlayLogResponse); }
	virtual	int SetPlayLog(const char *endpoint, const char *soap_action, _ns1__SetPlayLog *ns1__SetPlayLog, _ns1__SetPlayLogResponse *ns1__SetPlayLogResponse);

	/// Web service operation 'GetSystemNotify' (returns error code or SOAP_OK)
	virtual	int GetSystemNotify_(_ns1__GetSystemNotify *ns1__GetSystemNotify, _ns1__GetSystemNotifyResponse *ns1__GetSystemNotifyResponse) { return this->GetSystemNotify_(NULL, NULL, ns1__GetSystemNotify, ns1__GetSystemNotifyResponse); }
	virtual	int GetSystemNotify_(const char *endpoint, const char *soap_action, _ns1__GetSystemNotify *ns1__GetSystemNotify, _ns1__GetSystemNotifyResponse *ns1__GetSystemNotifyResponse);

	/// Web service operation 'GetSystemMessage' (returns error code or SOAP_OK)
	virtual	int GetSystemMessage_(_ns1__GetSystemMessage *ns1__GetSystemMessage, _ns1__GetSystemMessageResponse *ns1__GetSystemMessageResponse) { return this->GetSystemMessage_(NULL, NULL, ns1__GetSystemMessage, ns1__GetSystemMessageResponse); }
	virtual	int GetSystemMessage_(const char *endpoint, const char *soap_action, _ns1__GetSystemMessage *ns1__GetSystemMessage, _ns1__GetSystemMessageResponse *ns1__GetSystemMessageResponse);

	/// Web service operation 'GetUserSetting' (returns error code or SOAP_OK)
	virtual	int GetUserSetting_(_ns1__GetUserSetting *ns1__GetUserSetting, _ns1__GetUserSettingResponse *ns1__GetUserSettingResponse) { return this->GetUserSetting_(NULL, NULL, ns1__GetUserSetting, ns1__GetUserSettingResponse); }
	virtual	int GetUserSetting_(const char *endpoint, const char *soap_action, _ns1__GetUserSetting *ns1__GetUserSetting, _ns1__GetUserSettingResponse *ns1__GetUserSettingResponse);

	/// Web service operation 'GetSensitiveWords' (returns error code or SOAP_OK)
	virtual	int GetSensitiveWords_(_ns1__GetSensitiveWords *ns1__GetSensitiveWords, _ns1__GetSensitiveWordsResponse *ns1__GetSensitiveWordsResponse) { return this->GetSensitiveWords_(NULL, NULL, ns1__GetSensitiveWords, ns1__GetSensitiveWordsResponse); }
	virtual	int GetSensitiveWords_(const char *endpoint, const char *soap_action, _ns1__GetSensitiveWords *ns1__GetSensitiveWords, _ns1__GetSensitiveWordsResponse *ns1__GetSensitiveWordsResponse);

	/// Web service operation 'GetHotWords' (returns error code or SOAP_OK)
	virtual	int GetHotWords_(_ns1__GetHotWords *ns1__GetHotWords, _ns1__GetHotWordsResponse *ns1__GetHotWordsResponse) { return this->GetHotWords_(NULL, NULL, ns1__GetHotWords, ns1__GetHotWordsResponse); }
	virtual	int GetHotWords_(const char *endpoint, const char *soap_action, _ns1__GetHotWords *ns1__GetHotWords, _ns1__GetHotWordsResponse *ns1__GetHotWordsResponse);

	/// Web service operation 'GetHotEd2ks' (returns error code or SOAP_OK)
	virtual	int GetHotEd2ks_(_ns1__GetHotEd2ks *ns1__GetHotEd2ks, _ns1__GetHotEd2ksResponse *ns1__GetHotEd2ksResponse) { return this->GetHotEd2ks_(NULL, NULL, ns1__GetHotEd2ks, ns1__GetHotEd2ksResponse); }
	virtual	int GetHotEd2ks_(const char *endpoint, const char *soap_action, _ns1__GetHotEd2ks *ns1__GetHotEd2ks, _ns1__GetHotEd2ksResponse *ns1__GetHotEd2ksResponse);

	/// Web service operation 'GetRandomWord' (returns error code or SOAP_OK)
	virtual	int GetRandomWord_(_ns1__GetRandomWord *ns1__GetRandomWord, _ns1__GetRandomWordResponse *ns1__GetRandomWordResponse) { return this->GetRandomWord_(NULL, NULL, ns1__GetRandomWord, ns1__GetRandomWordResponse); }
	virtual	int GetRandomWord_(const char *endpoint, const char *soap_action, _ns1__GetRandomWord *ns1__GetRandomWord, _ns1__GetRandomWordResponse *ns1__GetRandomWordResponse);

	/// Web service operation 'SetSearchLog' (returns error code or SOAP_OK)
	virtual	int SetSearchLog_(_ns1__SetSearchLog *ns1__SetSearchLog, _ns1__SetSearchLogResponse *ns1__SetSearchLogResponse) { return this->SetSearchLog_(NULL, NULL, ns1__SetSearchLog, ns1__SetSearchLogResponse); }
	virtual	int SetSearchLog_(const char *endpoint, const char *soap_action, _ns1__SetSearchLog *ns1__SetSearchLog, _ns1__SetSearchLogResponse *ns1__SetSearchLogResponse);

	/// Web service operation 'SetFeedback' (returns error code or SOAP_OK)
	virtual	int SetFeedback_(_ns1__SetFeedback *ns1__SetFeedback, _ns1__SetFeedbackResponse *ns1__SetFeedbackResponse) { return this->SetFeedback_(NULL, NULL, ns1__SetFeedback, ns1__SetFeedbackResponse); }
	virtual	int SetFeedback_(const char *endpoint, const char *soap_action, _ns1__SetFeedback *ns1__SetFeedback, _ns1__SetFeedbackResponse *ns1__SetFeedbackResponse);

	/// Web service operation 'SetPlayLog' (returns error code or SOAP_OK)
	virtual	int SetPlayLog_(_ns1__SetPlayLog *ns1__SetPlayLog, _ns1__SetPlayLogResponse *ns1__SetPlayLogResponse) { return this->SetPlayLog_(NULL, NULL, ns1__SetPlayLog, ns1__SetPlayLogResponse); }
	virtual	int SetPlayLog_(const char *endpoint, const char *soap_action, _ns1__SetPlayLog *ns1__SetPlayLog, _ns1__SetPlayLogResponse *ns1__SetPlayLogResponse);
};
#endif
