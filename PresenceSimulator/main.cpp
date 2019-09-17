#include "stdafx.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <windows.h>

namespace
{

INPUT makeInput(const KEYBDINPUT& ki)
{
   INPUT i { INPUT_KEYBOARD };
   i.ki = ki;
   return i;
}

void hitKey(WORD vk)
{
   INPUT input[] =
   {
      makeInput(KEYBDINPUT{ vk }),
      makeInput(KEYBDINPUT{ vk, 0, KEYEVENTF_KEYUP })
   };
   SendInput(UINT(sizeof(input) / sizeof(*input)), input, sizeof(INPUT));
}

std::tm getLocalTime()
{
   std::time_t t = std::time(nullptr);
   return *std::localtime(&t);
}

bool operator < (const std::tm& lhs, const std::tm& rhs)
{
   return
      lhs.tm_year < rhs.tm_year || lhs.tm_year == rhs.tm_year && (
         lhs.tm_yday < rhs.tm_yday || lhs.tm_yday == rhs.tm_yday && (
            lhs.tm_hour < rhs.tm_hour || lhs.tm_hour == rhs.tm_hour && (
               lhs.tm_min < rhs.tm_min || lhs.tm_min == rhs.tm_min &&
                  lhs.tm_sec < rhs.tm_sec
            )
         )
      );
}
bool operator > (const std::tm& lhs, const std::tm& rhs) { return rhs < lhs; }
bool operator != (const std::tm& lhs, const std::tm& rhs) { return lhs < rhs || rhs < lhs; }
bool operator == (const std::tm& lhs, const std::tm& rhs) { return !(lhs != rhs); }
bool operator <= (const std::tm& lhs, const std::tm& rhs) { return !(rhs < lhs); }
bool operator >= (const std::tm& lhs, const std::tm& rhs) { return !(lhs < rhs); }

std::ostream& operator << (std::ostream& output, const std::tm& t)
{
   return output << std::put_time(&t, "%Y/%j/%H:%M:%S");
}

std::istream& operator >> (std::istream& input, std::tm& t)
{
   return input >> std::get_time(&t, "%H:%M:%S");
}

std::tm makeGreater(const std::tm& lhs, const std::tm& rhs)
{
   if (lhs > rhs)
      return lhs;

   auto t = rhs;
   t.tm_hour = lhs.tm_hour;
   t.tm_min = lhs.tm_min;
   t.tm_sec = lhs.tm_sec;

   if (t <= rhs)
   {
      ++t.tm_yday;
   }
   return t;
}

template <typename T>
T parse(const std::string& str)
{
   T t{};
   std::istringstream input(str);
   input >> t;
   return t;
}

struct Params
{
   WORD key = VK_SCROLL;
   std::tm endTime {};
};

std::ostream& operator << (std::ostream& output, const Params& params)
{
   return std::cout
      << "Key: " << "VK = 0x" << std::hex << params.key;
}

char paramKey(const std::string& param)
{
   if (param.size() < 2 ||
      std::string("-/").find(param.front()) == std::string::npos ||
      param.size() == 3 ||
      param.size() > 3 && std::string(":=").find(param[2]) == std::string::npos)
   {
      throw std::invalid_argument("Invalid parameter: '" + param + "'");
   }
   return param[1];
}

template <typename T>
T paramValue(const std::string& param)
{
   return param.size() > 3 ? parse<T>(param.substr(3)) : T();
}

Params parseCommandLine(int argc, const char* const* args)
{
   Params params;

   for (int i = 1; i < argc; ++i)
   {
      switch (paramKey(args[i]))
      {
      case 'e':
      case 'E':
         params.endTime = makeGreater(paramValue<std::tm>(args[i]), getLocalTime());
         break;

      default:
         throw std::invalid_argument("Unexpected command line parameter: " + std::string(args[i]));
      }
   }

   return params;
}

void simulateKeyActivity(WORD key)
{
   hitKey(key);
   Sleep(100);
   hitKey(key);
   Sleep(3000);
}

void run(const Params& params)
{
   std::cout
      << params << std::endl
      << "Started: " << getLocalTime() << std::endl
      << "End Time: " << params.endTime << std::endl;

   for (;;)
   {
      if (params.endTime != std::tm {} && getLocalTime() >= params.endTime)
         break;

      simulateKeyActivity(params.key);
   }
   std::cout << "Finished: " << getLocalTime() << std::endl;
}



} // namespace

#if 10
int main(int argc, const char* const* args)
{
   try
   {
      run(parseCommandLine(argc, args));
   }
   catch (std::exception& ex)
   {
      std::cout << "[Uncaught exception]: " << ex.what() << std::endl;
   }
}
#else
int WINAPI WinMain(
   HINSTANCE, //hInstance,
   HINSTANCE, //hPrevInstance,
   LPSTR, //lpCmdLine,
   int //nShowCmd
)
{
   run();
}
#endif

