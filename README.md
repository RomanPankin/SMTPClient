# Simple TCP clinets

## POP3 Connection

```
#!c#
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
