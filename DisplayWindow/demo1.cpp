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
//// 窗口过程函数，处理窗口消息
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
////3D渲染函数相关函数
//bool DefineDomain(float x, float y)
//{
//    //定义域判断，认为所有的点都有定义域，直接返回true
//    return true;
//}
////取定义域Y值
//int DefineDomainYValue(float x, float* yarr)
//{
//    //所有点都有定义y的取值就去区间边界
//    yarr[0] = 32;
//    yarr[1] = -32;
//    return 2;
//
//}
////取定义域X值
//int DefineDomainXValue(float y, float* xarr)
//{
//    //所有点都有定义x的取值就去区间边界
//    xarr[0] = 32;
//    xarr[1] = -32;
//    return 2;
//}
//bool GetZValue(float x, float y, float* z)
//{
//    if (!DefineDomain(x, y))return FALSE;
//
//    //返回指定点的z值
//    *z = sin(sqrtf(x * x + y * y));
//    return true;
//}
////绘制一个上半球
//bool DefineDomain_Ball(float x, float y)
//{
//
//    //底部定义域为圆形，有精度误差0.0001，防止圆附近取点时判断不在定义域内
//
//    //这里判断定义域
//    return 16 - x * x - y * y >= -0.0001;//
//}
////取定义域Y值
//int DefineDomainYValue_Ball(float x, float* yarr)
//{
//    //用
//    float y1 = sqrtf(16 - x * x);
//    yarr[0] = y1;
//    if (!y1)return 1;
//    yarr[1] = -y1;
//    return 2;
//}
////取定义域X值
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
////项目选择回调
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
//    freopen_s(&stream, "CON", "r", stdin);//重定向输入流
//    freopen_s(&stream, "CON", "w", stdout);//重定向输入流
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
//    // 注册窗口类
//
//
//    WNDCLASS wc = {};
//    wc.lpfnWndProc = WindowProc; // 窗口过程函数
//    wc.hInstance = hInstance;
//    wc.lpszClassName = g_qfw.GetWindowsClass();
//    wc.style = CS_DBLCLKS;
//
//    RegisterClass(&wc);
//
//    // 创建窗口
//    HWND hwnd = CreateWindowEx(
//        0,                              // 扩展风格
//        wc.lpszClassName,                     // 窗口类名
//        g_qfw.GetWindowsTitle(),                // 窗口标题
//        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,            // 窗口样式
//        CW_USEDEFAULT, CW_USEDEFAULT,   // 初始位置
//        CW_USEDEFAULT, CW_USEDEFAULT,   // 初始大小
//        NULL,                           // 父窗口
//        NULL,                           // 菜单
//        hInstance,                      // 应用程序实例句柄
//        NULL                            // 其他参数
//    );
//
//
//    if (hwnd == NULL) {
//        return 0;
//    }
//
//
//    // 显示窗口
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
//    //给标签控件添加五个标签
//    for (int i = -1; ++i < 7;) {
//        qt->AddTab((L"测试" + std::to_wstring(i)).c_str(), L"Tabico.png");
//
//    }
//
//
//    //创建一个按钮
//    QButton* qb = (QButton*)QFUSE::CreateQWND("QFButton", QRect(10, 10, 100, 50));
//    //向标签控件关联一个控件，参数1：控件指针，参数2：标签索引
//    qt->AttachCtrl(qb, 0);
//    qb = (QButton*)QFUSE::CreateQWND("QFButton", QRect(110, 10, 100, 50));
//    qt->AttachCtrl(qb, 0);
//
//    //创建一个线性图表
//    QLineChart* qlc = (QLineChart*)QFUSE::CreateQWND("QFLineChart", QRect(10, 10, 700, 550));
//    //给线性图标增加一个函数图像
//    std::vector<QPointF> srcPF;
//    for (QREAL x = 0.0; x < 20; x += 3.14 / 8) {
//        srcPF.push_back(QPointF(x, std::sin(x)));
//    }
//    qlc->AddCurve(srcPF.data(), srcPF.size());
//    //向标签控件关联一个控件，参数1：控件指针，参数2：标签索引
//    qt->AttachCtrl(qlc, 1);
//
//    //创建一个报表控件
//    QReportForm* wnd = (QReportForm*)QFUSE::CreateQWND("QFReportForm", QRect(10, 10, 700, 550));
//    wnd->InsertTitle(L"唱", 100, -1);
//    wnd->InsertTitle(L"跳", 100, -1);
//    wnd->InsertTitle(L"rap", 200, -1);
//    wnd->InsertRecorder(L"半年");
//    for (int i = 0; ++i < 21;) {
//        std::wstring wstr(L"年半");
//        wstr = std::to_wstring(i) + wstr;
//        wnd->InsertRecorder(wstr.c_str(), -1);
//    }
//    for (int i = -1; ++i < 20;) {
//        wnd->SetRecorderText(i, 1, L"练习生");
//        wnd->SetRecorderText(i, 2, L"精通");
//    }
//    wnd->IsIsDrag(true);    //是否可以拖动
//    qt->AttachCtrl(wnd, 2);
//
//    qt->SetFontSize(18);//设置标签字体大小
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
//    //q3dc->AddCurve(xr, yr, DefineDomain_Ball, DefineDomainYValue_Ball, DefineDomainXValue_Ball, GetZValue_Ball, 100, 100);//加入一个半球
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
//    qt->SetTabName(4, L"柱状图");
//    qt->AttachCtrl(Qcbc, 4);
//    qt->SetTabName(5, L"温度图表");
//    QCATLTempeChart* Qctc = (QCATLTempeChart*)QFUSE::CreateQWND("QFCATLTempeChart", QRect(10, 10, 750, 550));
//    for (int i = -1; ++i < 20;) {
//        Qctc->InsertGroup(-1);
//        std::wstring ws = std::format(L"温度图表{:d}", i + 1);
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
//    std::vector<std::wstring> arr = { L"吉利汽车", L"小米汽车", L"长城汽车"  , L"东风汽车" , L"长安汽车"  , L"一汽-大众"    , L"比亚迪汽车"  , L"奇瑞汽车"  , L"北京汽车"  , L"领途汽车" };
//    for (int i = -1; ++i < 10;) {
//        qd->InsertDrawer(-1);   //插入10个Drawer
//            //下面为10个Drawer设置名称
//        qd->SetDrawerName(i, arr[i].c_str());
//    }
//    std::vector<std::wstring> arrJL = { L"帝豪" , L"博瑞GE-MHEV"  , L"远景（轿车）"  , L"帝豪GSe（SUV）" };
//    std::vector<std::wstring> arrMi = { L"SU7" };
//    std::vector<std::wstring> arrCC = { L"长城炮", L"欧拉iQ" , L"WEY P8" , L"哈弗H4", L"坦克300" , L"哈弗H6" };
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
//    qt->SetTabName(6, L"抽屉控件展示");
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
//    //垂直布局期
//    QVLayout* qvL = (QVLayout*)QFUSE::CreateQWND("QFVLayout", QRect(0, 10, 400, 300));
//    qtb->AttachCtrl(qvL, 0);
//
//
//
//
//    //水平布局器的使用
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
//    // 主消息循环
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
