#pragma once
#include <windows.h>
namespace QFKT {

    inline bool isShitDown()
    {
        return 0x8000 & GetKeyState(VK_SHIFT);//是否按下shit键

    }
    inline bool IsCapital()
    {

        return GetKeyState(VK_CAPITAL) & 1;

    }


    inline char KeyValue2char(WPARAM wParam, LPARAM lParam)
    {
        char ret = 0;
        //按住ctrl或者alt键也不解析
        if ((0x8000 & GetKeyState(VK_CONTROL)))return 0;



        //键码在0x41到0x5a的为字母进行大小写转换
        if (0x41 <= wParam && 0x5a >= wParam) {
            int isUpper = !(isShitDown() ^ IsCapital());//同或
            ret = isUpper * 0x20 + wParam;
            return ret;
        }
        else if (0x30 <= wParam && 0x39 >= wParam) {//数字

            if (!isShitDown()) {
                return wParam;
            }
            else {
                char index[] = ")!@#$%^&*(";
                return index[wParam - 0x30];
            }
        }
        else if (0x60 <= wParam && 0x6F >= wParam) {//小键盘映射
            char index[] = "0123456789*+\0-./";
            return index[wParam - 0x60];

        }
        else if (0xBA <= wParam && 0xC0 >= wParam) {//键盘符号映射
            char index[2][8] = { ";=,-./`",":+<_>?~" };
            return index[isShitDown()][wParam - 0xBA];
        }
        else if (0xDB <= wParam && 0xDE >= wParam) {//键盘符号映射
            char index[2][5] = { "[\\]'","{|}\"" };
            return index[isShitDown()][wParam - 0xDB];

        }
        return 0;

    }






}
