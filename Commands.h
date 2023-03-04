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


enum ColorType
{
  Color_Red   = 1,
  Color_Blue  = 2,
  Color_Green = 3
};


class Command
{
public:

  static Command Hello();
  static Command On();
  static Command Off();
  static Command Color(ColorType);

  //! Отправка данных типа `Command` через сокет.
  static bool Send(int sd, const Command&);

  //! Прием данных типа `Command` через сокет.
  static Command Recieve(int sd);

public:

  Command()
    : type(CmdType_Empty) {}

  Command(CmdType t, const std::vector<char>& val = std::vector<char>())
    : type(t), value(val) {}

  bool operator!() const;

  CmdType GetType() const;

  char GetCharValue(size_t index) const { return value[index]; }

private:
  uint8_t type;
  std::vector<char> value;
};


std::ostream& operator<<(std::ostream&, const Command&);

#endif //RemoteLamp_Utility_Commands_h

