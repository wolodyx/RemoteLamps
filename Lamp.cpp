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


//! Лампа: состояние, управление и отрисовка.
class LampState
{
private:
  bool m_on;
  ColorType m_color;
public:
  LampState() : m_on(false), m_color(Color_Red) {}
  void TurnOn() { m_on = true; }
  void TurnOff() { m_on = false; }
  void SetColor(ColorType clr) { m_color = clr; }
  void Render()
  {
    if(!m_on)
    {
      std::clog << "\r\x1b[0mLamp";
      return;
    }

    if(m_color == Color_Blue)
      std::clog << "\r\x1b[37;44;5mLamp\x1b[0m";
    else if(m_color == Color_Red)
      std::clog << "\r\x1b[37;41;5mLamp\x1b[0m";
    else if(m_color == Color_Green)
      std::clog << "\r\x1b[37;42;5mLamp\x1b[0m";
  }
};


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
    Command::Send(sd, Command::Hello()),
    "Server response error");
  
  LampState lamp;
  // Цикл обработки поступающих команд.
  while(true)
  {
    lamp.Render();
    Command cmd = Command::Recieve(sd);

    if(!cmd)
      break;

    switch(cmd.GetType())
    {
      case CmdType_On:
        lamp.TurnOn();
        break;
      case CmdType_Off:
        lamp.TurnOff();
        break;
      case CmdType_Color:
        lamp.SetColor(static_cast<ColorType>(cmd.GetCharValue(0)));
        break;
    }
  }

  close(sd);
  return EXIT_SUCCESS;
}

