#pragma once

#include <windows.h>
#include <string>

#include <openssl/ssl.h>
#include <openssl/err.h>


/********************************************************************************
* TCP Exception
********************************************************************************/
class TCPException {
public:
	enum TCPExceptionType {
		TCPCantInitialize,
		TCPWrongHostName,
		TCPCantInitializeSocket,
		TCPCantConnect,
		TCPCantWrite,
		TCPCantRead,
		TCPTimeout,
		TCPSSLError
	};

	// Constructor and destructor 
	TCPException(TCPException::TCPExceptionType type);

	// Properties
	TCPExceptionType getType() const { return this->type; }

private:
	TCPExceptionType type;
};



/********************************************************************************
* TCP Connection
********************************************************************************/
class TCPConnection {
public:
	// Constructor and destructor
	TCPConnection();
	~TCPConnection();

	// Properties
	void setHostName(char * lpszHostName) { this->hostName.assign(lpszHostName); };
	void setHostName(std::string &hostName) { this->hostName.assign(hostName); };
	void setPort(int port) { this->port = port; };
	void setUseSSL(bool useSSL) { this->useSSL = useSSL; };
	void setWriteTimeout(int nTimeMSec) { this->writeTimeout = nTimeMSec; };
	void setReadTimeout(int nTimeMSec) { this->readTimeout = nTimeMSec; };

	const std::string& getHostName() const { return hostName; };
	int getPort() const { return port; };
	bool getUseSSL() const { return useSSL; };
	int getWriteTimeout() const { return writeTimeout; };
	int getReadTimeout() const { return readTimeout; };

	// Actions
	void connect();
	void disconnect();

	bool readChar(char * result);
	bool readLine(std::string & data);
	bool readChars(std::string & data, int amount);
	void sendData(std::string & data);

	static void initialize();
	static void finalize();

protected:
	// Actions
	void checkRead();
	void checkWrite();

private:
	// Properties
	SOCKET hSocket;
	std::string hostName;
	int port;
	bool useSSL;

	int writeTimeout;
	int readTimeout;

	// SSL
	SSL_CTX * sslContext;
	SSL* sslObject;
};
