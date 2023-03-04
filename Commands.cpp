#include "Commands.h"

#include <unistd.h>

#include <iostream>


CmdType Command::GetType() const
{
  return static_cast<CmdType>(type);
}


Command Cmd_Hello()
{
  return Command(CmdType_Hello);
}


Command Cmd_On()
{
  return Command(CmdType_On);
}


bool SendCommand(int sd, const Command& cmd)
{
  std::vector<char> buf(3+cmd.value.size());
  buf[0] = static_cast<char>(cmd.type);
  *reinterpret_cast<uint16_t*>(&buf.front()+1) = cmd.value.size();
  for(size_t i = 0; i < cmd.value.size(); ++i)
    buf[i+3] = cmd.value[i];
  ssize_t n = write(sd, &buf.front(), buf.size());
  return n == buf.size();
}


Command RecieveCommand(int sd)
{
  char buf[128];
  ssize_t n = read(sd, buf, 1);
  if(n == 0)
    return Command();
  CmdType type = static_cast<CmdType>(buf[0]);

  n = read(sd, buf, 2);
  uint16_t len = *reinterpret_cast<uint16_t*>(buf);

  std::vector<char> value(len);
  uint16_t iValue = 0;
  while(true)
  {
    n = read(sd, buf, std::min(128,len-iValue));
    for(size_t i = 0; i < n; ++i)
      value[iValue++] = buf[i];
    if(n < 128)
      break;
  }

  return Command(type, value);
}


std::ostream& operator<<(
  std::ostream& s,
  const Command& cmd)
{
  s << "Command ";
  switch(cmd.GetType())
  {
  case CmdType_Hello:
    s << "Hello"; break;
  case CmdType_On:
    s << "On"; break;
  case CmdType_Off:
    s << "Off"; break;
  case CmdType_Color:
    s << "Color"; break;
  default:
    s << "---"; break;
  }
  
  s << std::endl;
  return s;
}

