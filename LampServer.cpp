#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <iostream>
#include <map>

#include "Commands.h"


std::map<int,int> g_lamps;

static void* thread_register_lamps(void* arg);
void Print(const char* message);
int RegisterLamp(int sd);
bool ParseCommandLine(const std::string&, int&, Command&);

#define CHECK_AND_EXIT(cond, message) \
  if(!(cond)) \
  { \
    Print(message); \
    return EXIT_FAILURE; \
  }


int main(int argc, char** argv)
{
  sockaddr_in addr;
  memset(&addr, 0, sizeof(sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(9999);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  int sd = socket(AF_INET, SOCK_STREAM, 0);
  CHECK_AND_EXIT(sd != -1, "Socket error");

  CHECK_AND_EXIT(
    0 == bind(sd, (struct sockaddr*)&addr, sizeof(addr)),
    "Bind error");

  CHECK_AND_EXIT(0 == listen(sd, 10), "Listen error");

  pthread_t thread;
  pthread_create(&thread, NULL, thread_register_lamps, (void*)&sd);

  while(true)
  {
    std::string cmdline;
    std::getline(std::cin, cmdline);

    int index;
    Command cmd;
    if(!ParseCommandLine(cmdline,index,cmd))
    {
      Print("Parse error");
      continue;
    }

    auto it = g_lamps.find(index);
    if(it == g_lamps.end())
    {
      Print("Not lamp");
      continue;
    }

    SendCommand(it->second, cmd);
  }

  return 0;
}


void Print(const char* message)
{
  std::cout << "Server: " << message << std::endl;
}


int RegisterLamp(int sd)
{
  static int s_nextIndex = 1;
  g_lamps.insert(std::make_pair(s_nextIndex,sd));
  return s_nextIndex++;
}


// "#1 on", "#2 off", "#3 color red".
bool ParseCommandLine(
  const std::string& cmdline,
  int& index,
  Command& cmd)
{
  char cmdName[128] = {0}, cmdArg[128] = {0};
  int nitem = sscanf(cmdline.c_str(), "%d %s %s", &index, cmdName, cmdArg);
  if(nitem < 2)
    return false;

  CmdType cmdType;
  if(strcmp(cmdName,"on") == 0)
    cmdType = CmdType_On;
  else if(strcmp(cmdName,"off") == 0)
    cmdType = CmdType_Off;
  else if(strcmp(cmdName,"color") == 0)
    cmdType = CmdType_Color;
  else return false;

  if(cmdType == CmdType_Color)
  {
    if(  strcmp(cmdArg,"red") != 0
      && strcmp(cmdArg,"green") != 0
      && strcmp(cmdArg,"blue") != 0)
      return false;
  }
  
  std::vector<char> value(cmdArg, cmdArg + strlen(cmdArg));
  cmd = Command(cmdType, value);
  return true;
}


void* thread_register_lamps(void* arg)
{
  int sd = *reinterpret_cast<int*>(arg);

  while(true)
  {
    sockaddr addr;
    socklen_t addrLen;
    int sd2 = accept(sd, &addr, &addrLen);
    if(sd2 == -1)
    {
      Print("Accept error");
      continue;
    }

    auto cmd = RecieveCommand(sd2);
    if(cmd.GetType() == CmdType_Hello)
    {
      int index = RegisterLamp(sd2);
      std::cout << "Server: Register lamp #" << index << std::endl;
    }
  }

  close(sd);
  return NULL;
}

