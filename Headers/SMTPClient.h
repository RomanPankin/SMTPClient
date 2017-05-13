#pragma once

#include <windows.h>

/**
* SMTP Exception
*/
class STMPException {
public:
	enum SMTPExceptionType {
		SMTPCantInitialize,
		SMTPWrongHostName
	};

	STMPException(STMPException::SMTPExceptionType type);

private:
	SMTPExceptionType type;
};

/**
* SMTP Connection
*/
class SMTPClient {
public:
	// Constructor and destructor
	SMTPClient();
	~SMTPClient();

	// Actions
	void connect(char * lpszServerName);

private:
	HANDLE hSocket;
};
