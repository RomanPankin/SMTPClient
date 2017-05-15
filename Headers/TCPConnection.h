/**
* @brief Class for TCP connection
*
* @file TCPConnection.h
* @author Pankin Roman (romanpankin86@gmail.com)
*
*/
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
		TCPCantInitialize = 1,
		TCPWrongHostName = 2,
		TCPCantInitializeSocket = 3,
		TCPCantConnect = 4,
		TCPCantWrite = 5,
		TCPCantRead = 6,
		TCPTimeout = 7,
		TCPSSLError = 8
	};

	// Constructor and destructor 
	TCPException(TCPException::TCPExceptionType type) : type(type) {};
	TCPException(TCPException::TCPExceptionType type, std::string& message) : type(type), message(message) {};

	// Properties
	int getErrorType() const { return type; }
	const std::string& getMessage() const { return message; }

private:
	TCPExceptionType type;
	std::string message;
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

	bool isConnected() const;

	// Actions
	void connect() throw(TCPException);
	void disconnect() throw(TCPException);

	bool readChar(char * result) throw(TCPException);
	bool readLine(std::string & data) throw(TCPException);
	bool readChars(std::string & data, int amount) throw(TCPException);
	void sendData(const std::string & data) throw(TCPException);

	static void initialize() throw(TCPException);
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
