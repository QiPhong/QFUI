//#include <windows.h>
//
//#include "QFWindow.h"
//#include "QFUI.h"
//#include "Gdiplus.h"
//#include <format>
//
//#pragma comment (lib,"Gdiplus.lib")
//
//QFWindow g_qfw;
//HMODULE m_hm = NULL;
//// ���ڹ��̺�������������Ϣ
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//    switch (uMsg) {
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//
//    default:
//
//        return DefWindowProc(hwnd, uMsg, wParam, lParam);
//    }
//    return 0;
//}
//
//
//
//
//std::string wstring_to_string(const std::wstring& wstr) {
//    if (wstr.empty()) {
//        return std::string();
//    }
//
//    int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
//    std::string str(size_needed, 0);
//    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
//    return str;
//}
//QProgress* qpr;
//QSlider* qsl;
//QColorBox* qcb;
//int x = 0;
//#include <random>
//void BtClick(void* para)
//{
//    std::random_device rd;  // ��ȡ�������
//    std::mt19937 gen(rd()); // ѡ���������������Mersenne Twister 19937
//
//    // �������ȷֲ�
//    std::uniform_int<> dist(0, 0xff); // �� [0, 99] ��Χ�������������
//
//    // ���������
//    int red = dist(gen);
//
//    int green = dist(gen);
//    int blue = dist(gen);
//
//    int red2 = dist(gen);
//
//    int green2 = dist(gen);
//    int blue2 = dist(gen);
//    qpr->SetRingProgressColor(0xff000000 | (red << 16) | (green << 8) | blue, 0xff000000 | (red2 << 16) | (green2 << 8) | blue2);
//    //qpr->SetLocation(QPoint(++x,10));
//    //qcb->SetPopDialogHeight(150);
//}
//
//int SliderOnEvent(int ct, float v)
//{
//    qpr->SetCurrentValue(v);
//
//
//    return 0;
//}
////��ɫ�����ı�
//int ColorBoxChange(int ct, unsigned int color)
//{
//    printf("ColorBoxChange: color= %x\n", color);
//
//    return 0;
//}
//
//
//
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
//
//#ifdef _DEBUG
//
//
//
//    AllocConsole();
//    FILE* stream;
//    freopen_s(&stream, "CON", "r", stdin);//�ض���������
//    freopen_s(&stream, "CON", "w", stdout);//�ض���������
//#endif 
//
//
//
//
//
//
//    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
//    ULONG_PTR gdiplusToken;
//    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
//
//    // ע�ᴰ����
//
//
//    WNDCLASS wc = {};
//    wc.lpfnWndProc = WindowProc; // ���ڹ��̺���
//    wc.hInstance = hInstance;
//    wc.lpszClassName = g_qfw.GetWindowsClass();
//    wc.style = CS_DBLCLKS;
//
//    RegisterClass(&wc);
//
//    // ��������
//    HWND hwnd = CreateWindowEx(
//        0,                              // ��չ���
//        wc.lpszClassName,                     // ��������
//        g_qfw.GetWindowsTitle(),                // ���ڱ���
//        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,            // ������ʽ
//        CW_USEDEFAULT, CW_USEDEFAULT,   // ��ʼλ��
//        CW_USEDEFAULT, CW_USEDEFAULT,   // ��ʼ��С
//        NULL,                           // ������
//        NULL,                           // �˵�
//        hInstance,                      // Ӧ�ó���ʵ�����
//        NULL                            // ��������
//    );
//
//
//    if (hwnd == NULL) {
//        return 0;
//    }
//
//
//    // ��ʾ����
//    ShowWindow(hwnd, nCmdShow);
//    QFUSE::BindPanelWindow(hwnd);
//    QWMange* pbuff = (QWMange*)QFUSE::GetQWManger();
//    pbuff->SetBKPicture(L"./picture.png");
//
//    QTabs* qt = (QTabs*)QFUSE::CreateQWND("QFTabs", QRect(10, 10, 500, 500));
//    QFUSE::AttachCtrl(qt);
//    qt->AddTab(L"����");
//    qt->AddTab(L"����2");
//    DLLImportQWND* diq = nullptr;
//    //diq = (DLLImportQWND*)QFUSE::CreateQWND("QFSwitch", QRect(10, 10, 300, 20));
//    //qt->AttachCtrl(diq,0);
//    //////QFUSE::AttachCtrl(diq);
//    //diq = (DLLImportQWND*)QFUSE::CreateQWND("QFSwitch", QRect(10, 50, 300, 20));
//    //qt->AttachCtrl(diq, 0);
//    ////QFUSE::AttachCtrl(diq);
//    //diq = (DLLImportQWND*)QFUSE::CreateQWND("QFSlider", QRect(10, 200, 20, 200));
//    //qt->AttachCtrl(diq, 0);
//
//    qpr = (QProgress*)QFUSE::CreateQWND("QFProgress", QRect(10, 200, 200, 200));
//    qpr->SetFontSize(28);
//    qpr->SetCurrentValue(78.5);
//    qpr->SetRingProgressThickness(30);
//    qpr->SetDecimalPlaces(1);
//    qpr->SetFontColor(0xff00ff00);
//    qpr->SetRingProgressColor(0xffffff00, 0xff00a0ff);
//    qt->AttachCtrl(qpr, 0);
//
//    qsl = (QSlider*)QFUSE::CreateQWND("QFSlider", QRect(10, 150, 200, 20));
//    qsl->SetValueChangeEvent(SliderOnEvent);
//    qt->AttachCtrl(qsl, 0);
//    qsl->IsSegmentation(true);
//    /*   qsl->AddSegmVuale(0.0);
//       qsl->AddSegmVuale(50.0);
//       qsl->AddSegmVuale(100.0);*/
//    QSwitch* qsw = (QSwitch*)QFUSE::CreateQWND("QFSwitch", QRect(10, 80, 50, 20));
//    qt->AttachCtrl(qsw, 0);
//    QButton2* qpb2 = (QButton2*)QFUSE::CreateQWND("QFPButton", QRect(10, 10, 100, 50));
//    //qpb2->SetBKColor(0xffff0000);
//    qpb2->SetText(L"��ť");
//    qpb2->SetFontSize(16);
//    //qpb2->SetMargin(20);
//    //qpb2->SetXRadius(40);
//    //qpb2->SetYRadius(40);
//    qpb2->SetClickEvent(BtClick);
//    qt->AttachCtrl(qpb2, 0);
//
//    qcb = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(1300, 10, 100, 20));
//    qcb->SetFontSize(16);
//    qcb->SetColorChangeEvent(ColorBoxChange);
//    //qcb->SetPopDialogHeight(150);
//    qcb->SetPopDialogWidth(250);
//    QFUSE::AttachCtrl(qcb);
//
//    qcb = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(1300, 50, 100, 20));
//    qcb->SetFontSize(16);
//    qcb->SetPopDialogHeight(150);
//
//    QFUSE::AttachCtrl(qcb);
//
//
//    QSelect* qs = (QSelect*)QFUSE::CreateQWND("QFSelect", QRect(250, 10, 100, 20));
//    qs->InsertItem(L"item1");
//    qs->InsertItem(L"item2");
//    qs->InsertItem(L"item3");
//    qs->InsertItem(L"item4");
//    qs->InsertItem(L"item5");
//    qs->InsertItem(L"item6");
//    qs->InsertItem(L"item7");
//    qs->InsertItem(L"item8");
//    qs->SetItemHeight(20);
//    qs->SetCurrentItem(-1);
//    qt->AttachCtrl(qs, 0);
//
//    qs = (QSelect*)QFUSE::CreateQWND("QFSelect", QRect(360, 10, 150, 50));
//    qs->InsertItem(L"��Ŀ1");
//    qs->InsertItem(L"��Ŀ2");
//    qs->InsertItem(L"��Ŀ3");
//    qs->InsertItem(L"��Ŀ4");
//    qs->InsertItem(L"��Ŀ5");
//    qs->InsertItem(L"��Ŀ6");
//    qs->InsertItem(L"��Ŀ7");
//    qs->InsertItem(L"��Ŀ8");
//    qs->SetItemFontSize(18);
//    qs->SetCurrentItem(-1);
//    qt->AttachCtrl(qs, 0);
//
//
//    QHLayout* qhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(360, 10, 200, 50));
//
//    qt->AttachCtrl(qhl, 1);
//    qsl = (QSlider*)QFUSE::CreateQWND("QFSlider", QRect(10, 150, 200, 20));
//    qsl->SetValueChangeEvent(SliderOnEvent);
//    qhl->InsertCtrl(qsl, 0);
//
//
//
//    // ����Ϣѭ��
//    MSG msg = {};
//    while (GetMessage(&msg, NULL, 0, 0)) {
//        //draw(p, wdc);
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//
//    QFUSE::UnBindPanelWindow();
//    Gdiplus::GdiplusShutdown(gdiplusToken);
//#ifdef _DEBUG
//    FreeConsole();
//#endif
//    //FreeLibrary(m_hm);
//    return 0;
//}
