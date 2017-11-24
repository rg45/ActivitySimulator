#include "stdafx.h"

#include <iostream>
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

void run()
{
   const WORD vk = VK_NUMLOCK;
   std::vector<INPUT> input =
   {
      makeInput(KEYBDINPUT{ vk }),
      makeInput(KEYBDINPUT{ vk, 0, KEYEVENTF_KEYUP })
   };
   for (;;)
   {
      SendInput(UINT(input.size()), &input[0], sizeof(INPUT));
      Sleep(1000);
   }
}

} // namespace

int main()
{
   run();
}

