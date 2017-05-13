#include "SMTPClient.h"

const int BUFFER_NAME_LENGTH = 255;


/**
 * SMTP Exception
 */
STMPException::STMPException(STMPException::SMTPExceptionType type)
{
	this->type = type;
}


/**
* SMTP Connection
*/
SMTPClient::SMTPClient()
{
	hSocket = NULL;
}

SMTPClient::~SMTPClient()
{
}

void SMTPClient::connect(char * lpszServerName)
{
	// Initialize socket library
	WSADATA wsaData;
	WORD wVer = MAKEWORD(2, 2);
	if (WSAStartup(wVer, &wsaData) != NO_ERROR)
		throw STMPException(STMPException::SMTPCantInitialize);

	// Receive host name
	char hostname[BUFFER_NAME_LENGTH];
	if (gethostname((char *)&hostname, BUFFER_NAME_LENGTH) == SOCKET_ERROR)
		throw STMPException(STMPException::SMTPWrongHostName);
}
