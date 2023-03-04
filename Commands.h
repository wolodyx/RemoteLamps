#ifndef RemoteLamp_Utility_Commands_h
#define RemoteLamp_Utility_Commands_h

#include <cstdint>
#include <iosfwd>
#include <vector>


enum CmdType
{
  CmdType_Empty = 0x00,
  CmdType_Hello = 0x01,
  CmdType_Exit  = 0x02,
  CmdType_On    = 0x12,
  CmdType_Off   = 0x13,
  CmdType_Color = 0x20
};


struct Command
{
  Command()
    : type(CmdType_Empty) {}

  Command(CmdType t, const std::vector<char>& val = std::vector<char>())
    : type(t), value(val) {}

  CmdType GetType() const;

  uint8_t type;
  std::vector<char> value;
};


Command Cmd_Hello();
Command Cmd_On();

//! Отправка данных типа `Command` через сокет.
bool SendCommand(int sd, const Command&);

//! Прием данных типа `Command` через сокет.
Command RecieveCommand(int sd);

std::ostream& operator<<(std::ostream&, const Command&);

#endif //RemoteLamp_Utility_Commands_h

