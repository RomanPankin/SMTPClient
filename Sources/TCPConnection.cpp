#include "..\Headers\TCPConnection.h"


const int TCP_DEFAULT_PORT = 0;
const int TCP_CONNECTION_READ_TIMEOUT = 4000;
const int TCP_CONNECTION_WRITE_TIMEOUT = 4000;



/********************************************************************************
* TCP Exception
********************************************************************************/
TCPException::TCPException(TCPException::TCPExceptionType type)
{
	this->type = type;
}



/********************************************************************************
* TCP Connection
********************************************************************************/
TCPConnection::TCPConnection()
{
	hSocket = INVALID_SOCKET;
	port = TCP_DEFAULT_PORT;
	useSSL = false;

	readTimeout = TCP_CONNECTION_READ_TIMEOUT;
	writeTimeout = TCP_CONNECTION_WRITE_TIMEOUT;

	sslContext = NULL;
	sslObject = NULL;
}

TCPConnection::~TCPConnection()
{
	disconnect();
}

void TCPConnection::connect()
{
	// Receive information about server
	LPHOSTENT host = gethostbyname(hostName.c_str());
	if (host == NULL)
		throw TCPException(TCPException::TCPWrongHostName);

	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(inet_ntoa(*(in_addr *)host->h_addr));

	// Connection (non-blocking)
	if ((hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		throw TCPException(TCPException::TCPCantInitializeSocket);

	unsigned long iMode = 1;
	int iResult = ioctlsocket(hSocket, FIONBIO, &iMode);

	if (::connect(hSocket, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
			throw TCPException(TCPException::TCPCantConnect);
	}

	checkWrite();

	// SSL
	if (useSSL) {
		sslContext = SSL_CTX_new(SSLv23_client_method());
		if (sslContext == NULL)
			throw TCPException(TCPException::TCPSSLError);

		sslObject = SSL_new(sslContext);
		if (sslObject == NULL)
			throw TCPException(TCPException::TCPSSLError);

		SSL_set_fd(sslObject, (int)hSocket);
		SSL_set_mode(sslObject, SSL_MODE_AUTO_RETRY);

		while (true)
		{
			int sslResult = SSL_connect(sslObject);

			int sslError = SSL_get_error(sslObject, sslResult);
			if (sslError == SSL_ERROR_NONE) break;
			if (sslError != SSL_ERROR_WANT_WRITE && sslError != SSL_ERROR_WANT_READ)
				throw TCPException(TCPException::TCPSSLError);
		}
	}
}

void TCPConnection::disconnect()
{
	// SSL
	if (sslContext) SSL_CTX_free(sslContext);
	sslContext = NULL;

	// Socket
	if (hSocket) closesocket(hSocket);
	hSocket = INVALID_SOCKET;
}


void TCPConnection::checkRead()
{
	if (hSocket == INVALID_SOCKET)
		throw TCPException(TCPException::TCPCantRead);

	TIMEVAL Timeout;
	Timeout.tv_sec = readTimeout / 1000;
	Timeout.tv_usec = readTimeout * 1000;

	fd_set fd_read;
	FD_ZERO(&fd_read);
	FD_SET(hSocket, &fd_read);

	int result = select(0, &fd_read, NULL, NULL, &Timeout);
	if (result == SOCKET_ERROR)
		throw TCPException(TCPException::TCPCantRead);
	else if (result == 0)
		throw TCPException(TCPException::TCPTimeout);

	FD_CLR(hSocket, &fd_read);
}

void TCPConnection::checkWrite()
{
	if (hSocket == INVALID_SOCKET)
		throw TCPException(TCPException::TCPCantWrite);

	TIMEVAL Timeout;
	Timeout.tv_sec = writeTimeout / 1000;
	Timeout.tv_usec = writeTimeout * 1000;

	fd_set fd_write;
	FD_ZERO(&fd_write);
	FD_SET(hSocket, &fd_write);

	int result = select(0, NULL, &fd_write, NULL, &Timeout);
	if (result == SOCKET_ERROR)
		throw TCPException(TCPException::TCPCantWrite);
	else if (result == 0)
		throw TCPException(TCPException::TCPTimeout);

	FD_CLR(hSocket, &fd_write);
}

void TCPConnection::sendData(std::string & data)
{
	if (data.length() == 0) return;

	int len = data.length();
	const char * sendingData = data.c_str();

	while (len > 0)
	{
		checkWrite();

		int res = 0;

		if (useSSL)
		{
			while (true)
			{
				res = SSL_write(sslObject, sendingData, len);
				int sslError = SSL_get_error(sslObject, res);

				if (sslError == SSL_ERROR_NONE) break;
				if (sslError == SSL_ERROR_WANT_WRITE || sslError == SSL_ERROR_WANT_READ) continue;

				throw TCPException(TCPException::TCPCantWrite);
			}
		}
		else
		{
			res = send(hSocket, sendingData, len, 0);
			if (res == SOCKET_ERROR || res == 0)
				throw TCPException(TCPException::TCPCantWrite);
		}

		len -= res;
		sendingData += res;
	}
}

bool TCPConnection::readChar(char * result)
{
	checkRead();

	if (useSSL)
	{
		while (true)
		{
			int res = SSL_read(sslObject, result, sizeof(result[0]));
			int sslError = SSL_get_error(sslObject, res);

			if (sslError == SSL_ERROR_NONE) break;
			if (sslError == SSL_ERROR_ZERO_RETURN) return false;
			if (sslError == SSL_ERROR_WANT_WRITE || sslError == SSL_ERROR_WANT_READ) continue;

			throw TCPException(TCPException::TCPCantRead);
		}
	}
	else
	{
		int res = recv(hSocket, result, sizeof(result[0]), 0);
		if (res == 0) return false;
		if (res < 0) throw TCPException(TCPException::TCPCantRead);
	}

	return true;
}

bool TCPConnection::readLine(std::string & data)
{
	data.clear();

	char cChar;
	bool result = false;

	while (readChar(&cChar)) {
		result = true;

		if (cChar == '\n')
			break;

		data.append(1, cChar);
	}

	return result;
}

bool TCPConnection::readChars(std::string & data, int amount)
{
	char cChar;
	bool result = false;

	while (amount > 0 && readChar(&cChar)) {
		if (cChar == '\n')
			break;

		data.append(1, cChar);

		result = true;
		amount--;
	}

	return result;
}

void TCPConnection::initialize()
{
	// Initialize socket library
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		WSACleanup();
		throw TCPException(TCPException::TCPCantInitialize);
	}

	// Initialize SSL
	SSL_load_error_strings();
	SSL_library_init();
	OpenSSL_add_all_algorithms();
}

void TCPConnection::finalize()
{
	// Finalize SSL
	ERR_free_strings();
	EVP_cleanup();

	// Finalize socket
	WSACleanup();
}
