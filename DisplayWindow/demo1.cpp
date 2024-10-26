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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////3D��Ⱦ������غ���
//bool DefineDomain(float x, float y)
//{
//    //�������жϣ���Ϊ���еĵ㶼�ж�����ֱ�ӷ���true
//    return true;
//}
////ȡ������Yֵ
//int DefineDomainYValue(float x, float* yarr)
//{
//    //���е㶼�ж���y��ȡֵ��ȥ����߽�
//    yarr[0] = 32;
//    yarr[1] = -32;
//    return 2;
//
//}
////ȡ������Xֵ
//int DefineDomainXValue(float y, float* xarr)
//{
//    //���е㶼�ж���x��ȡֵ��ȥ����߽�
//    xarr[0] = 32;
//    xarr[1] = -32;
//    return 2;
//}
//bool GetZValue(float x, float y, float* z)
//{
//    if (!DefineDomain(x, y))return FALSE;
//
//    //����ָ�����zֵ
//    *z = sin(sqrtf(x * x + y * y));
//    return true;
//}
////����һ���ϰ���
//bool DefineDomain_Ball(float x, float y)
//{
//
//    //�ײ�������ΪԲ�Σ��о������0.0001����ֹԲ����ȡ��ʱ�жϲ��ڶ�������
//
//    //�����ж϶�����
//    return 16 - x * x - y * y >= -0.0001;//
//}
////ȡ������Yֵ
//int DefineDomainYValue_Ball(float x, float* yarr)
//{
//    //��
//    float y1 = sqrtf(16 - x * x);
//    yarr[0] = y1;
//    if (!y1)return 1;
//    yarr[1] = -y1;
//    return 2;
//}
////ȡ������Xֵ
//int DefineDomainXValue_Ball(float y, float* xarr)
//{
//    float x1 = sqrtf(16 - y * y);
//    xarr[0] = x1;
//    if (!x1)return 1;
//    xarr[1] = -x1;
//
//    return 2;
//}
//bool GetZValue_Ball(float x, float y, float* z)
//{
//    if (!DefineDomain(x, y))return FALSE;
//
//
//    *z = pow(abs(16 - x * x - y * y), 0.5);
//    return true;
//}
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#include <random>
//bool isRunThread = true;
//void threadFun(QCATLTempeChart* qctc)
//{
//    while (isRunThread) {
//        std::srand((unsigned int)time(NULL));
//        int c = 0xff000000;
//        c |= (std::rand() % 256);
//        c |= ((std::rand() % 256) << 8);
//        c |= ((std::rand() % 256) << 16);
//        for (int i = -1; ++i < 10;)
//            qctc->SetUnitColor(std::rand() % 12, std::rand() % 15, c);
//        c = 0xff000000;
//        c |= (std::rand() % 256);
//        c |= ((std::rand() % 256) << 8);
//        c |= ((std::rand() % 256) << 16);
//        for (int i = -1; ++i < 10;)
//            qctc->SetUnitColor(std::rand() % 12, std::rand() % 15, c);
//        c = 0xff000000;
//        c |= (std::rand() % 256);
//        c |= ((std::rand() % 256) << 8);
//        c |= ((std::rand() % 256) << 16);
//        for (int i = -1; ++i < 10;)
//            qctc->SetUnitColor(std::rand() % 12, std::rand() % 15, c);
//        for (int i = -1; ++i < 10;) {
//            std::wstring str = std::to_wstring(std::rand() % 100);
//            qctc->SetUnitText(std::rand() % 12, std::rand() % 15, str.c_str());
//        }
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//
//    }
//
//
//}
//void threadFun2(QCATLBarChart* qcbc)
//{
//    while (isRunThread) {
//        std::srand((unsigned int)time(NULL));
//        int c = 0xff000000;
//        c |= (std::rand() % 256);
//        c |= ((std::rand() % 256) << 8);
//        c |= ((std::rand() % 256) << 16);
//        for (int i = -1; ++i < 10;) {
//            qcbc->SetBarChartValue(std::rand() % 12, std::rand() % 3, 65535);
//            qcbc->SetBarChartText(std::rand() % 12, std::rand() % 3, L"123");
//        }
//
//        c = 0xff000000;
//        c |= (std::rand() % 256);
//        c |= ((std::rand() % 256) << 8);
//        c |= ((std::rand() % 256) << 16);
//        for (int i = -1; ++i < 10;) {
//            qcbc->SetBarChartValue(std::rand() % 12, std::rand() % 3, std::rand() % 65535);
//        }
//        c = 0xff000000;
//        c |= (std::rand() % 256);
//        c |= ((std::rand() % 256) << 8);
//        c |= ((std::rand() % 256) << 16);
//        for (int i = -1; ++i < 10;) {
//            qcbc->SetBarChartValue(std::rand() % 12, std::rand() % 3, std::rand() % 65535);
//        }
//
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//
//    }
//
//
//}
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
////��Ŀѡ��ص�
//QDrawer* g_qd = NULL;
//int selectEvent(int i, int j)
//{
//    printf("selectEvent:%d,%d\n", i, j);
//    std::wstring str = g_qd->GetSubObjectName(i, j);
//    //wprintf(L"selectEvent: %s\n", str.c_str());
//    std::string s = wstring_to_string(str);
//    printf("selectEvent:%s\n", s.c_str());
//    return 1;
//}
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
//
//    //typedef QWND* (*CQWDM)(HDC winddc, QRect  rf);
//    //m_hm=LoadLibraryA("QWNDHandle.dll");
//    //CQWDM cqwdm = (CQWDM)GetProcAddress(m_hm, "CreateQWMange");
//
//
//    //HDC wdc = GetDC(hwnd);
//    //RECT rect;
//    //GetWindowRect(hwnd, &rect);
//    //QWND* p = cqwdm(wdc, QRect(0, 0, rect.right - rect.left, rect.bottom - rect.top));
//    //QWND* p = cqwdm(wdc, QRect(0, 0, 100, 100));
//
//    //DRAW draw = (DRAW)GetProcAddress(m_hm, "Draw");
//
//    QFUSE::BindPanelWindow(hwnd);
//    QWMange* pbuff = (QWMange*)QFUSE::GetQWManger();
//    //pbuff->SetBKBrush(NULL);
//    pbuff->SetBKPicture(L"./picture.png");
//    QTabs* qt = (QTabs*)QFUSE::CreateQWND("QFTabs", QRect(100, 10, 800, 600));
//    QFUSE::AttachCtrl(qt);
//    //����ǩ�ؼ���������ǩ
//    for (int i = -1; ++i < 7;) {
//        qt->AddTab((L"����" + std::to_wstring(i)).c_str(), L"Tabico.png");
//
//    }
//
//
//    //����һ����ť
//    QButton* qb = (QButton*)QFUSE::CreateQWND("QFButton", QRect(10, 10, 100, 50));
//    //���ǩ�ؼ�����һ���ؼ�������1���ؼ�ָ�룬����2����ǩ����
//    qt->AttachCtrl(qb, 0);
//    qb = (QButton*)QFUSE::CreateQWND("QFButton", QRect(110, 10, 100, 50));
//    qt->AttachCtrl(qb, 0);
//
//    //����һ������ͼ��
//    QLineChart* qlc = (QLineChart*)QFUSE::CreateQWND("QFLineChart", QRect(10, 10, 700, 550));
//    //������ͼ������һ������ͼ��
//    std::vector<QPointF> srcPF;
//    for (QREAL x = 0.0; x < 20; x += 3.14 / 8) {
//        srcPF.push_back(QPointF(x, std::sin(x)));
//    }
//    qlc->AddCurve(srcPF.data(), srcPF.size());
//    //���ǩ�ؼ�����һ���ؼ�������1���ؼ�ָ�룬����2����ǩ����
//    qt->AttachCtrl(qlc, 1);
//
//    //����һ������ؼ�
//    QReportForm* wnd = (QReportForm*)QFUSE::CreateQWND("QFReportForm", QRect(10, 10, 700, 550));
//    wnd->InsertTitle(L"��", 100, -1);
//    wnd->InsertTitle(L"��", 100, -1);
//    wnd->InsertTitle(L"rap", 200, -1);
//    wnd->InsertRecorder(L"����");
//    for (int i = 0; ++i < 21;) {
//        std::wstring wstr(L"���");
//        wstr = std::to_wstring(i) + wstr;
//        wnd->InsertRecorder(wstr.c_str(), -1);
//    }
//    for (int i = -1; ++i < 20;) {
//        wnd->SetRecorderText(i, 1, L"��ϰ��");
//        wnd->SetRecorderText(i, 2, L"��ͨ");
//    }
//    wnd->IsIsDrag(true);    //�Ƿ�����϶�
//    qt->AttachCtrl(wnd, 2);
//
//    qt->SetFontSize(18);//���ñ�ǩ�����С
//    QEdit* qed = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(10, 10, 400, 50));
//    qt->AttachCtrl(qed, 3);
//
//    //QGraph3D* q3dc = (QGraph3D*)QFUSE::CreateQWND("QF3DGraph", QRect(10, 10, 700, 550));
//    //float xr[2] = {-32,32 };
//    //float yr[2] = { -32,32 };
//    //q3dc->AddCurve(xr,yr, DefineDomain, DefineDomainYValue, DefineDomainXValue, GetZValue, 500, 500);
//    //q3dc->SetVisualHeigh(100);
//    //xr[0] = -4;
//    //xr[1] =4;
//    //yr[0] = -4;
//    //yr[1] = 4;
//    //q3dc->AddCurve(xr, yr, DefineDomain_Ball, DefineDomainYValue_Ball, DefineDomainXValue_Ball, GetZValue_Ball, 100, 100);//����һ������
//    //q3dc->IsVisivalAxis(false);
//    //qt->AttachCtrl(q3dc, 4);
//    QCATLBarChart* Qcbc = (QCATLBarChart*)QFUSE::CreateQWND("QFCATLBarChart", QRect(10, 10, 750, 550));
//
//    for (int i = -1; ++i < 20;) {
//        Qcbc->InsertGroup(-1);
//        Qcbc->InsertBarChart(i, -1, 50000, 0xfff00000);
//        Qcbc->InsertBarChart(i, -1, 40000, 0xff00f000);
//        Qcbc->InsertBarChart(i, -1, 30000, 0xff0000f0);
//    }
//    qt->SetTabName(4, L"��״ͼ");
//    qt->AttachCtrl(Qcbc, 4);
//    qt->SetTabName(5, L"�¶�ͼ��");
//    QCATLTempeChart* Qctc = (QCATLTempeChart*)QFUSE::CreateQWND("QFCATLTempeChart", QRect(10, 10, 750, 550));
//    for (int i = -1; ++i < 20;) {
//        Qctc->InsertGroup(-1);
//        std::wstring ws = std::format(L"�¶�ͼ��{:d}", i + 1);
//        Qctc->SetGroupName(i, ws.c_str());
//        int c = 0xff323232;
//        for (int j = -1; ++j < 5;) {
//            Qctc->InsertUnit(i, -1, c | i * 10, std::to_wstring(i).c_str());
//            Qctc->InsertUnit(i, -1, c | ((i * 10) << 8), std::to_wstring(i * 10).c_str());
//            Qctc->InsertUnit(i, -1, c | ((i * 10) << 16), std::to_wstring(i * 100).c_str());
//        }
//    }
//    Qctc->SetGroupLineCount(-1, 6);
//    qt->AttachCtrl(Qctc, 5);
//
//    QDrawer* qd = (QDrawer*)QFUSE::CreateQWND("QFDrawer", QRect(10, 10, 200, 550));
//    g_qd = qd;
//    std::vector<std::wstring> arr = { L"��������", L"С������", L"��������"  , L"��������" , L"��������"  , L"һ��-����"    , L"���ǵ�����"  , L"��������"  , L"��������"  , L"��;����" };
//    for (int i = -1; ++i < 10;) {
//        qd->InsertDrawer(-1);   //����10��Drawer
//            //����Ϊ10��Drawer��������
//        qd->SetDrawerName(i, arr[i].c_str());
//    }
//    std::vector<std::wstring> arrJL = { L"�ۺ�" , L"����GE-MHEV"  , L"Զ�����γ���"  , L"�ۺ�GSe��SUV��" };
//    std::vector<std::wstring> arrMi = { L"SU7" };
//    std::vector<std::wstring> arrCC = { L"������", L"ŷ��iQ" , L"WEY P8" , L"����H4", L"̹��300" , L"����H6" };
//    for (int i = -1; ++i < arrJL.size();) {
//        qd->InsertSubObject(0, -1);
//        qd->SetSubObjectName(0, i, arrJL[i].c_str());
//    }
//    for (int i = -1; ++i < arrMi.size();) {
//        qd->InsertSubObject(1, -1);
//        qd->SetSubObjectName(1, i, arrMi[i].c_str());
//    }
//    for (int i = -1; ++i < arrCC.size();) {
//        qd->InsertSubObject(2, -1);
//        qd->SetSubObjectName(2, i, arrCC[i].c_str());
//    }
//    qd->SetSelectEvent(selectEvent);
//    qt->AttachCtrl(qd, 6);
//    qt->SetTabName(6, L"����ؼ�չʾ");
//
//    std::thread t(threadFun, Qctc);
//    std::thread t2(threadFun2, Qcbc);
//
//    QHost* qh = (QHost*)QFUSE::CreateQWND("QFHostCtrl", QRect(10, 10, 300, 300));
//    QTabs* qtb = (QTabs*)QFUSE::CreateQWND("QFTabs", QRect(100, 10, 100, 60));
//    for (int i = -1; ++i < 3;) {
//        qtb->AddTab((L"TEST" + std::to_wstring(i)).c_str(), L"Tabico.png");
//
//    }
//    QHost* qh1 = (QHost*)QFUSE::CreateQWND("QFHostCtrl", QRect(10, 100, 100, 50));
//    QButton* qb1 = (QButton*)QFUSE::CreateQWND("QFButton", QRect(10, 10, 100, 50));
//    qh1->SetParasiticCtrl(qb1);
//    //qtb->AttachCtrl(qh1,0);
//
//
//    qh->SetParasiticCtrl(qtb);
//
//
//    qt->AttachCtrl(qh, 0);
//
//
//
//
//    //��ֱ������
//    QVLayout* qvL = (QVLayout*)QFUSE::CreateQWND("QFVLayout", QRect(0, 10, 400, 300));
//    qtb->AttachCtrl(qvL, 0);
//
//
//
//
//    //ˮƽ��������ʹ��
//    {
//        QHLayout* qhL = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(10, 10, 300, 300));
//        qvL->InsertCtrl(qhL, 1);
//        QStaticText* qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(10, 10, 100, 50));
//        QEdit* edit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(10, 10, 100, 50));
//        qhL->InsertCtrl(qst, 1);
//        qhL->InsertCtrl(edit, 1);
//    }
//    {
//        QHLayout* qhL = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(10, 10, 300, 300));
//        qvL->InsertCtrl(qhL, 1);
//        QButton* qb = (QButton*)QFUSE::CreateQWND("QFButton", QRect(10, 10, 100, 50));
//        QEdit* edit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(10, 10, 100, 50));
//        qhL->InsertCtrl(qb, 1);
//        qhL->InsertCtrl(edit, 1);
//    }
//
//    /*QHLayout* qhL1 = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(10, 500, 100, 50));
//    QButton* qb12 = (QButton*)QFUSE::CreateQWND("QFButton", QRect(10, 10, 100, 50));
//    QEdit* edit2 = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(10, 10, 100, 50));
//    qhL1->InsertCtrl(qb12,1);
//    qhL1->InsertCtrl(edit2, 2);
//    QFUSE::AttachCtrl(qhL1);*/
//
//    //qhL1->SetLocation(QPoint(100, 100));
//
//    // ����Ϣѭ��
//    MSG msg = {};
//    while (GetMessage(&msg, NULL, 0, 0)) {
//        //draw(p, wdc);
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//    isRunThread = false;
//    t.join();
//    t2.join();
//    QFUSE::UnBindPanelWindow();
//    Gdiplus::GdiplusShutdown(gdiplusToken);
//#ifdef _DEBUG
//    FreeConsole();
//#endif
//    //FreeLibrary(m_hm);
//    return 0;
//}
