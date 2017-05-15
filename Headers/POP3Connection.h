/**
* @brief Class for POP3 connection
*
* @file POP3Connection.h
* @author Pankin Roman (romanpankin86@gmail.com)
*
*/
#pragma once
#include "TCPConnection.h"

#include <vector>


typedef struct POP3MessageInfo {
	int id;
	int size;
} * PPOP3MessageInfo;


/********************************************************************************
* POP3 Exception
********************************************************************************/
class POP3Exception : public TCPException {
public:
	enum POP3ExceptionType {
		POP3WrongResponse = 100
	};

	POP3Exception(POP3Exception::POP3ExceptionType type) : TCPException((TCPException::TCPExceptionType)type) {};
	POP3Exception(POP3Exception::POP3ExceptionType type, std::string& message) : TCPException((TCPException::TCPExceptionType)type, message) {};
};


/********************************************************************************
* POP3 Connection
********************************************************************************/
class POP3Connection {
public:
	// Constructor and destructor
	POP3Connection();
	~POP3Connection();

	// Properties
	void setHostName(char * hostName) { connection.setHostName(hostName); };
	void setHostName(std::string &hostName) { connection.setHostName(hostName); };
	void setPort(int port) { connection.setPort(port); };
	void setUseSSL(bool useSSL) { connection.setUseSSL(useSSL); };
	
	const std::string& getHostName() const { return connection.getHostName(); };
	int getPort() const { return connection.getPort(); };
	bool getUseSSL() const { return connection.getUseSSL(); };

	// Actions
	void connect(char * userName, char * userPassword) throw(TCPException, POP3Exception);
	void list(std::vector<POP3MessageInfo> &messages) throw(TCPException, POP3Exception);
	void readMessage(int id, std::string & message) throw(TCPException, POP3Exception);
	void quit() throw(TCPException, POP3Exception);
	void disconnect() throw(TCPException);

protected:
	// Actions
	void sendCommand(std::string & request, std::string & response) throw(TCPException);

private:
	// Properties
	TCPConnection connection;
	bool isAuth;
};

