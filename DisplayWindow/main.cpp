#include <windows.h>

#include "QFWindow.h"
#include "QFUI.h"
#include "Gdiplus.h"
#include <format>

#pragma comment (lib,"Gdiplus.lib")

QFWindow g_qfw;
HMODULE m_hm =NULL;
// ���ڹ��̺�������������Ϣ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        
    default:

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}




std::string wstring_to_string(const std::wstring& wstr) {
    if (wstr.empty()) {
        return std::string();
    }

    int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}
QProgress* qpr;
QSlider* qsl;
QColorBox* qcb;
int x=0;
#include <random>
void BtClick(void* para)
{
    std::random_device rd;  // ��ȡ�������
    std::mt19937 gen(rd()); // ѡ���������������Mersenne Twister 19937

    // �������ȷֲ�
    std::uniform_int<> dist(0, 0xff); // �� [0, 99] ��Χ�������������

    // ���������
    int red = dist(gen);

    int green = dist(gen);
    int blue = dist(gen);

    int red2 = dist(gen);

    int green2 = dist(gen);
    int blue2 = dist(gen);
    qpr->SetRingProgressColor(0xff000000|(red<<16)|(green<<8)|blue, 0xff000000 | (red2 << 16) | (green2 << 8) | blue2);
    //qpr->SetLocation(QPoint(++x,10));
    //qcb->SetPopDialogHeight(150);
}

int SliderOnEvent(int ct, float v)
{
    qpr->SetCurrentValue(v);


    return 0;
}
//��ɫ�����ı�
int ColorBoxChange(int ct, unsigned int color)
{
    printf("ColorBoxChange: color= %x\n", color);

    return 0;
}




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

#ifdef _DEBUG



    AllocConsole();
    FILE* stream;
    freopen_s(&stream, "CON", "r", stdin);//�ض���������
    freopen_s(&stream, "CON", "w", stdout);//�ض���������
#endif 






    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // ע�ᴰ����
    

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc; // ���ڹ��̺���
    wc.hInstance = hInstance;
    wc.lpszClassName = g_qfw.GetWindowsClass();
    wc.style = CS_DBLCLKS;

    RegisterClass(&wc);

    // ��������
    HWND hwnd = CreateWindowEx(
        0,                              // ��չ���
        wc.lpszClassName,                     // ��������
        g_qfw.GetWindowsTitle(),                // ���ڱ���
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,            // ������ʽ
        CW_USEDEFAULT, CW_USEDEFAULT,   // ��ʼλ��
        CW_USEDEFAULT, CW_USEDEFAULT,   // ��ʼ��С
        NULL,                           // ������
        NULL,                           // �˵�
        hInstance,                      // Ӧ�ó���ʵ�����
        NULL                            // ��������
    );


    if (hwnd == NULL) {
        return 0;
    }
    

    // ��ʾ����
    ShowWindow(hwnd, nCmdShow);
    QFUSE::BindPanelWindow(hwnd);
    QWMange* pbuff = (QWMange*)QFUSE::GetQWManger();
    pbuff->SetBKPicture(L"./picture.png");

    



    // ����Ϣѭ��
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        //draw(p, wdc);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    QFUSE::UnBindPanelWindow();
    Gdiplus::GdiplusShutdown(gdiplusToken);
#ifdef _DEBUG
    FreeConsole();
#endif
    //FreeLibrary(m_hm);
    return 0;
}
