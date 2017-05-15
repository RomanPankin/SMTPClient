/**
* @brief Class for POP3 connection
*
* @file POP3Connection.h
* @author Pankin Roman (romanpankin86@gmail.com)
*
*/
#include "../Headers/POP3Connection.h"
#include <regex>

const std::string POP3_ENDLINE("\r\n");


/********************************************************************************
* POP3 Connection
********************************************************************************/
POP3Connection::POP3Connection()
{
	isAuth = false;
}

POP3Connection::~POP3Connection()
{
	try
	{
		if (isAuth) quit();
	}
	catch (...)
	{
	}
}

void POP3Connection::connect(char * userName, char * userPassword)
{
	// Authorization
	std::string request, response;

	if (!connection.isConnected())
	{
		connection.connect();
		connection.readLine(response);		// Hello message
	}

	request.assign("USER ").append(userName);
	sendCommand(request, response);

	request.assign("PASS ").append(userPassword);
	sendCommand(request, response);

	isAuth = true;
}

void POP3Connection::disconnect()
{
	connection.disconnect();
}

void POP3Connection::list(std::vector<POP3MessageInfo> &messages)
{
	std::string request, response;

	// Request
	request.assign("LIST");
	sendCommand(request, response);

	// Amount of messages
	int amount = -1;
	std::regex expr("^\\+OK (\\d+)");
	std::smatch matches;

	while (true)
	{
		if (!std::regex_search(response, matches, expr))
		{
			connection.readLine(response);
			continue;
		}

		amount = std::stoi(matches[1].str());
		break;
	}

	// Read ids
	std::regex idExpr("^(\\d+) (\\d+)");
	std::smatch idMatches;

	for (int I=0; I<amount; I++)
	{
		connection.readLine(response);
		
		if (!std::regex_search(response, idMatches, idExpr))
			throw POP3Exception(POP3Exception::POP3WrongResponse);

		POP3MessageInfo info;
		info.id = std::stoi(idMatches[1].str());
		info.size = std::stoi(idMatches[2].str());

		messages.push_back(info);
	}

	// Read the final line
	connection.readLine(response);
	if (response != ".")
		throw POP3Exception(POP3Exception::POP3WrongResponse);
}

void POP3Connection::readMessage(int id, std::string & message)
{
	std::string request, response;

	request.assign("RETR ").append(std::to_string(id));
	sendCommand(request, response);

	message.clear();
	while (connection.readLine(response))
	{
		if (response == ".")
			break;

		if (message.length() > 0) message.append(POP3_ENDLINE);
		
		if (response.length() > 0)
		{
			if (response.find_first_not_of(".") == std::string::npos)
				message.append(response, 0, response.length() - 1);
			else
				message.append(response);
		}
	}
}

void POP3Connection::quit()
{
	std::string request, response;

	request.append("QUIT");
	sendCommand(request, response);

	isAuth = false;
}

void POP3Connection::sendCommand(std::string & request, std::string & response)
{
	// Request and response
	connection.sendData(request);
	connection.sendData(POP3_ENDLINE);

	if (!connection.readLine(response))
		throw POP3Exception(POP3Exception::POP3WrongResponse);

	if (response.compare(0, 3, "+OK") != 0)
		throw POP3Exception(POP3Exception::POP3WrongResponse, response.substr(4));
}