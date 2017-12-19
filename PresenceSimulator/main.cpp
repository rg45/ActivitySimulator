#include "stdafx.h"

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

void hitKey(WORD vk, DWORD timeMsek = 100)
{
   INPUT input[] =
   {
      makeInput(KEYBDINPUT{ vk }),
      makeInput(KEYBDINPUT{ vk, 0, KEYEVENTF_KEYUP })
   };
   SendInput(UINT(sizeof(input) / sizeof(*input)), input, sizeof(INPUT));
}

WORD getVirtualKey(int argc, const char* const* args)
{
   WORD vk = VK_SCROLL;

   if (argc > 1)
   {
      std::istringstream input(args[1]);
      input >> vk;
   }
   return vk;
}

void run(WORD vk)
{
   std::cout << "VK = 0x" << std::hex << vk << std::endl;

   for (;;)
   {
      hitKey(vk);
      Sleep(100);
      hitKey(vk);
      Sleep(3000);
   }
}

} // namespace

#if 10
int main(int argc, const char* const* args)
{
   run(getVirtualKey(argc, args));
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

