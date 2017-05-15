# Simple TCP clients

## Overview

Implementation of the following protocols: TCP, POP3 (demo)


## POP3 Connection

Implementation of the standard protocol for receiving e-mail. At the moment, part of the protocol commands are implemented.
Tested on hMailServer.

```
POP3Connection connection;
connection.setHostName("mail.local");
connection.setPort(110);
connection.setUseSSL(false);
connection.connect("test@mail.local", "admin");

// List of messages
std::vector<POP3MessageInfo> messages;
connection.list(messages);

std::vector<POP3MessageInfo>::iterator it = messages.begin();
while (it != messages.end())
{
  std::cout << "message #" << (*it).id << ", size " << (*it).size << " bytes\n";

  // Read message
  std::string message;
  connection.readMessage((*it).id, message);
  std::cout << message;

  it++;
}
```

## TCP Connection

Implementation of the basic protocol for all connections

```
TCPConnection connection;
connection.setHostName("google.com");
connection.setPort(80);
connection.setUseSSL(true);
connection.connect();
connection.readLine();
```
