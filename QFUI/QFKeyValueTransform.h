#pragma once
#include <windows.h>
namespace QFKT {

    inline bool isShitDown()
    {
        return 0x8000 & GetKeyState(VK_SHIFT);//�Ƿ���shit��

    }
    inline bool IsCapital()
    {

        return GetKeyState(VK_CAPITAL) & 1;

    }


    inline char KeyValue2char(WPARAM wParam, LPARAM lParam)
    {
        char ret = 0;
        //��סctrl����alt��Ҳ������
        if ((0x8000 & GetKeyState(VK_CONTROL)))return 0;



        //������0x41��0x5a��Ϊ��ĸ���д�Сдת��
        if (0x41 <= wParam && 0x5a >= wParam) {
            int isUpper = !(isShitDown() ^ IsCapital());//ͬ��
            ret = isUpper * 0x20 + wParam;
            return ret;
        }
        else if (0x30 <= wParam && 0x39 >= wParam) {//����

            if (!isShitDown()) {
                return wParam;
            }
            else {
                char index[] = ")!@#$%^&*(";
                return index[wParam - 0x30];
            }
        }
        else if (0x60 <= wParam && 0x6F >= wParam) {//С����ӳ��
            char index[] = "0123456789*+\0-./";
            return index[wParam - 0x60];

        }
        else if (0xBA <= wParam && 0xC0 >= wParam) {//���̷���ӳ��
            char index[2][8] = { ";=,-./`",":+<_>?~" };
            return index[isShitDown()][wParam - 0xBA];
        }
        else if (0xDB <= wParam && 0xDE >= wParam) {//���̷���ӳ��
            char index[2][5] = { "[\\]'","{|}\"" };
            return index[isShitDown()][wParam - 0xDB];

        }
        return 0;

    }






}
