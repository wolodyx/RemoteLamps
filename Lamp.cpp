#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "Commands.h"

#define PORT_BY_DEFAULT 9999
#define HOST_BY_DEFAULT "127.0.0.1"

#define CHECK_AND_EXIT(cond, message) \
  if(!(cond)) \
  { \
    std::cout << "Lamp: " << message << std::endl; \
    return EXIT_FAILURE; \
  }


//! \brief Функции управления лампой.
//!\{
void TurnOnLamp();
void TurnOffLamp();
void ChangeLampColor(const std::vector<char>&);
//!\}


int main(int argc, char** argv)
{
  std::string host = HOST_BY_DEFAULT;
  int port = PORT_BY_DEFAULT;
  
  // Разбор командной строки.
  if(argc > 1)
    host = argv[1];
  if(argc > 2)
    port = atoi(argv[2]);

  std::cout << "IP address: " << host << ":" << port << std::endl;

  // Подготовка структуры с адресом сервера.
  sockaddr_in addr;
  memset(&addr, 0, sizeof(sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(host.c_str());
  CHECK_AND_EXIT(
    addr.sin_addr.s_addr != INADDR_NONE,
    "Address invalid");

  // Создаем клиентский сокет для связи с сервером.
  int sd = socket(AF_INET, SOCK_STREAM, 0);
  CHECK_AND_EXIT(sd != -1, "Socket failure");

  // Связываемся с сервером.
  CHECK_AND_EXIT(
    -1 != connect(sd, (struct sockaddr*)&addr, sizeof(addr)),
    "Connect failure");

  // Регистрируемся у сервера.
  CHECK_AND_EXIT(
    SendCommand(sd, Cmd_Hello()),
    "Server response error");
  
  // Цикл обработки поступающих команд.
  while(true)
  {
    Command cmd = RecieveCommand(sd);

    switch(cmd.type)
    {
      case CmdType_On:
        TurnOnLamp();
        break;
      case CmdType_Off:
        TurnOffLamp();
        break;
      case CmdType_Color:
        ChangeLampColor(cmd.value);
        break;
      default:
        break;
    }
  }

  close(sd);
  return EXIT_SUCCESS;
}


void TurnOnLamp()
{
  std::clog << "\r\x1b[31;43mLamp\x1b[0m";
}


void TurnOffLamp()
{
  std::clog << "\r\x1b[0mLamp";
}


void ChangeLampColor(const std::vector<char>& color)
{
  std::string clr(color.begin(), color.end());
  if(clr == "blue")
    std::clog << "\r\x1b[31;44;5mLamp\x1b[0m";
  else if(clr == "red")
    std::clog << "\r\x1b[33;41;5mLamp\x1b[0m";
  else if(clr == "green")
    std::clog << "\r\x1b[33;42;5mLamp\x1b[0m";
}

