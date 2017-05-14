#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */
#include <string.h> /* memcpy, memset */
#include <windows.h>
#include <conio.h>  
#include <ctype.h> 

#include "Headers\TCPConnection.h"



int main()
{
	TCPConnection::initialize();

	try
	{
		TCPConnection connection;
		connection.setHostName("yandex.ru");
		connection.setPort(443);
		connection.setUseSSL(true);
		connection.connect();

		std::string request = 
"GET / HTTP/1.1\n\
Host: yandex.ru\n\n";

		std::string response;

		connection.sendData(request);
		printf("data sent\n");

		while (connection.readLine(response))
		{
			printf("%s\n", response.c_str());
		}
	}
	catch (TCPException &error)
	{
		printf("Error code %d\n", (int)error.getType());
	}

	/*try
	{

		SMTPClient smtpClient;
		smtpClient.setHostName("www.example.com");
		smtpClient.setPort(80);
		smtpClient.connect();
	}
	catch (SMTPException &error)
	{
		printf("Error code %d\n", (int)error.getType());
		printf("WSA Error %ld\n", WSAGetLastError());
	}*/

	printf("Press any key to exit\n");
	_getch();

    return 0;
}

