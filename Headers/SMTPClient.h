#pragma once

#include "TCPConnection.h"

/**
* SMTP Exception
*/
class SMTPException : TCPException {
};

/**
* SMTP Connection
*/
class SMTPClient {
public:
	// Constructor and destructor
	SMTPClient();
	~SMTPClient();

	// Properties
	void setHostName(char * lpszHostName) { connection.setHostName(lpszHostName); };
	void setHostName(std::string &hostName) { connection.setHostName(hostName); };
	void setPort(int nPort) { connection.setPort(nPort); };
	void setUseSSL(bool bUseSSL) { connection.setUseSSL(bUseSSL); };

	const std::string& getHostName() const { return connection.getHostName(); };
	int getPort() const { return connection.getPort(); };
	bool getUseSSL() const { return connection.getUseSSL(); };

	// Actions
	void connect();
	void disconnect();

private:
	// Properties
	TCPConnection connection;
};
