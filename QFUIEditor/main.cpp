#include <windows.h>
#include <iostream>
#include "QFWindow.h"
#include "QFUI.h"
#include "Gdiplus.h"
#include <format>
#include "tinyxml2.h"
#include <fstream>
#include "QFXmlRelolver.h"
#pragma comment (lib,"Gdiplus.lib")
QEditorWin* g_qew = nullptr;
QFWindow g_qfw;
HMODULE m_hm = NULL;
// 窗口过程函数，处理窗口消息
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

#include <random>


std::string wstring_to_string(const std::wstring& wstr) {
    if (wstr.empty()) {
        return std::string();
    }

    int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}
//项目选择回调
QDrawer* g_qd = NULL;

int selectEvent(int i, int j)
{
    printf("selectEvent:%d,%d\n", i, j);
    std::wstring str = g_qd->GetSubObjectName(i, j);
    //wprintf(L"selectEvent: %s\n", str.c_str());
    std::string s = wstring_to_string(str);
    printf("selectEvent:%s\n", s.c_str());
    return 1;
}
std::wstring stringToWString(const std::string& str) {
    // 计算转换所需的缓冲区大小
    int size_needed = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0);
    std::wstring wstr(size_needed, 0);

    // 执行转换
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], size_needed);

    return wstr;
}
#include <map>
std::map<int, std::string> g_ctrl;
std::string cText = "";

//按钮事件
void BTClickEvent(void* bt)
{
    printf("btclick:%p\n",bt);
    auto it = g_ctrl.find(int(bt));
    if (it == g_ctrl.end()) {
        cText = "";
        return;
    }
    cText = it->second;

}
//struct CtrlAttribute {
//    std::string createT;
//    QHost* qhp=nullptr;
//    DLLImportQWND* parent = nullptr;
//
//};





std::map<int, QFUSE_EW::CtrlAttribute> g_allCtrl;
QCtrlToolbox* qct = nullptr;
void ClearCtrl()
{
    int count = g_qew->TransferMessage(QM_GETSUBCTRL, 0, 0, 0);
    for (int i = -1; ++i < count;) {

        void* p = nullptr;
        void* hp = nullptr;

        if (g_qew->TransferMessage(QM_GETSUBCTRL, QDWORD(&p), 0, QDWORD(&hp)); p) {
            auto it = g_allCtrl.find(int(p));
            if (it != g_allCtrl.end()) {
                int pCtrl = it->first;
                QFUSE_EW::CtrlAttribute ca = it->second;
                //对一些容器类的控件进行特殊处理
                if ("QFTabs" == ca.createT) {
                    ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(ca.qhp->qwm), 0, 0);
                    delete ca.qhp;
                    QWNDHANDLE::QWNDs.erase(ca.qhp);//从控件集合里面摘掉这个控件不然退出是会报错！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
                    //取控件内的子控件
                    QTabs qt(nullptr, QRect(0, 0, 1, 1));
                    qt.qwm = (QWND*)pCtrl;
                    int tcount = qt.GetTabeCount();
                    for (int i = -1; ++i < tcount;) {
                        int ccount = qt.GetTabeCtrlCount(i);
                        for (int j = -1; ++j < ccount;) {

                            QHost qh(NULL, QRect(0, 0, 10, 10));
                            qh.qwm = qt.GetCtrl(i, j);
                            auto it = g_allCtrl.find(int(qh.GetParasiticCtrl()));
                            if (it != g_allCtrl.end()) {
                                delete it->second.qhp;
                                QWNDHANDLE::QWNDs.erase(it->second.qhp);

                            }

                            qh.qwm = nullptr;

                        }

                    }
                    //ca.parent->TransferMessage(QM_REMOVE_CTRL,QDWORD(qt.qwm),0,0);

                }
                else if ("QFHLayout" == ca.createT) {
                    ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(ca.qhp->qwm), 0, 0);
                    delete ca.qhp;
                    QWNDHANDLE::QWNDs.erase(ca.qhp);//从控件集合里面摘掉这个控件不然退出是会报错！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
                    QHLayout qlc(nullptr, QRect(0, 0, 1, 1));
                    qlc.qwm = (QWND*)pCtrl;
                    int ccount = qlc.GetCtrlCount();

                    for (int i = -1; ++i < ccount;) {
                        QHost hs(nullptr, QRect(0, 0, 10, 10));
                        hs.qwm = qlc.GetCtrl(i);
                        auto it = g_allCtrl.find(int(hs.GetParasiticCtrl()));
                        if (it != g_allCtrl.end()) {
                            delete it->second.qhp;
                            QWNDHANDLE::QWNDs.erase(it->second.qhp);

                        }
                        hs.qwm = nullptr;

                    }
                    //ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(qlc.qwm), 0, 0);
                }
                else if ("QFVLayout" == ca.createT) {
                    ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(ca.qhp->qwm), 0, 0);
                    delete ca.qhp;
                    QWNDHANDLE::QWNDs.erase(ca.qhp);//从控件集合里面摘掉这个控件不然退出是会报错！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
                    QVLayout qvl(nullptr, QRect(0, 0, 1, 1));
                    qvl.qwm = (QWND*)pCtrl;
                    int ccount = qvl.GetCtrlCount();

                    for (int i = -1; ++i < ccount;) {
                        QHost hs(nullptr, QRect(0, 0, 10, 10));
                        hs.qwm = qvl.GetCtrl(i);
                        auto it = g_allCtrl.find(int(hs.GetParasiticCtrl()));
                        if (it != g_allCtrl.end()) {
                            delete it->second.qhp;
                            QWNDHANDLE::QWNDs.erase(it->second.qhp);

                        }
                        hs.qwm = nullptr;

                    }
                    //ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(qvl.qwm), 0, 0);
                }
                else if ("QFTLayout" == ca.createT) {
                    if (IDYES != MessageBoxA(0, "此控件为TLayout控件，删除将连它内部控件一起删除，是否删除", "删除控件", MB_YESNO))return;
                    ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(ca.qhp->qwm), 0, 0);
                    delete ca.qhp;
                    QWNDHANDLE::QWNDs.erase(ca.qhp);//从控件集合里面摘掉这个控件不然退出是会报错！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
                    QTLayout qtl(nullptr, QRect(0, 0, 1, 1));
                    qtl.qwm = (QWND*)pCtrl;
                    QHost hs(nullptr, QRect(0, 0, 10, 10));
                    hs.qwm = qtl.GetCtrl();
                    auto it = g_allCtrl.find(int(hs.GetParasiticCtrl()));
                    if (it != g_allCtrl.end()) {
                        delete it->second.qhp;
                        QWNDHANDLE::QWNDs.erase(it->second.qhp);


                    }
                    hs.qwm = nullptr;

                    //ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(qtl.qwm), 0, 0);
                }
                else {
                    ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(ca.qhp->qwm), 0, 0);
                    delete ca.qhp;
                    QWNDHANDLE::QWNDs.erase(ca.qhp);//从控件集合里面摘掉这个控件不然退出是会报错！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
                    DLLImportQWND dq(nullptr, QRect(0, 0, 10, 10));

                    dq.qwm = (QWND*)it->first;


                }
            }
        }
    }
    g_allCtrl.clear();

   


}
std::string xmlFilepath;

//打开文件按钮事件
void OpenBTClickEvent(void* bt)
{


    // 初始化OPENFILENAME结构体
    OPENFILENAMEA ofn;
    char szFile[MAX_PATH] = { 0 }; // 用于存储文件路径
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "XML Files\0*.xml\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // 显示文件打开对话框
    if (GetOpenFileNameA(&ofn) == TRUE) {
        std::cout << "Selected file: " << ofn.lpstrFile << std::endl;
    }
    else {
        return;
    }
    xmlFilepath = ofn.lpstrFile;

    if (0 < g_allCtrl.size())
        if (IDYES != MessageBoxA(0, "载入XML将清空当前界面编辑，是否继续", "打开XML", MB_YESNO))return;
    ClearCtrl();
    std::ifstream file(ofn.lpstrFile, std::ios::in | std::ios::binary);

    if (!file) {
        std::cerr << "Error opening file!" << std::endl;
        return ;
    }

    // 获取文件的大小
    file.seekg(0, std::ios::end);
    std::streampos size = file.tellg();
    file.seekg(0, std::ios::beg);

    // 读取文件内容到字符串中
    std::string content(size, ' ');
    file.read(&content[0], size);
    file.close();
    g_allCtrl.clear();
    QFUSE_EW::ExplainXML(content.c_str(), g_qew);
    g_allCtrl = QFUSE_EW::g_allCtrl;

}



tinyxml2::XMLElement* SavaCtrl(tinyxml2::XMLDocument& xml, QHost& hostw, void* pWnd);

//创建控件的xml
//1.创建按钮
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument&xml, QButton* bt,QHost* hostw)
{
    tinyxml2::XMLElement* button = xml.NewElement("QFButton");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        button->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();

    button->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    button->SetAttribute("Text", wstring_to_string(bt->GetText()).c_str());
    button->SetAttribute("name", bt->GetQWNDName().c_str());
    return button;
}
//1.创建按钮2
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QButton2* bt, QHost* hostw)
{
    tinyxml2::XMLElement* button = xml.NewElement("QFPButton");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        button->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();

    button->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    button->SetAttribute("name", bt->GetQWNDName().c_str());
    button->SetAttribute("Text", wstring_to_string(bt->GetText()).c_str());
    return button;
}
//2.创建Edit
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QEdit* ed, QHost* hostw)
{
    tinyxml2::XMLElement* edit = xml.NewElement("QFEdit");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        edit->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();

    edit->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    edit->SetAttribute("name", ed->GetQWNDName().c_str());
    edit->SetAttribute("Text", wstring_to_string(ed->GetText()).c_str());
    return edit;
}
//3.创建ReportForm
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QReportForm* ef, QHost* hostw)
{
    tinyxml2::XMLElement* reportForm = xml.NewElement("QFReportForm");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        reportForm->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();

    reportForm->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    reportForm->SetAttribute("name", ef->GetQWNDName().c_str());
    //配置字段
    int tcount = ef->GetTitleCount();
    tinyxml2::XMLElement* QFRForm_Title = nullptr;
    if (tcount > 0) {
        QFRForm_Title = xml.NewElement("FTitle");//报表表项
        reportForm->InsertEndChild(QFRForm_Title);
    }
    for (int i = -1; ++i < tcount;) {
        tinyxml2::XMLElement* QFRForm_Title_Item = xml.NewElement("FItem");//报表表项内容
        QFRForm_Title_Item->SetAttribute("text", wstring_to_string(ef->GetTitleText(i)).c_str());
        QFRForm_Title_Item->SetAttribute("width", std::to_string(ef->GetTitleWidth(i)).c_str());
        QFRForm_Title->InsertEndChild(QFRForm_Title_Item);
        
    }




    return reportForm;
}
//4.创建PictrueBox
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QPictureBox* pb, QHost* hostw)
{
    tinyxml2::XMLElement* PictrueBox = xml.NewElement("QFPictureBox");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        PictrueBox->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();

    PictrueBox->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    PictrueBox->SetAttribute("name", pb->GetQWNDName().c_str());
    return PictrueBox;
}
//5.创建ProgressBarCar
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QProgressBarCar* pbc, QHost* hostw)
{
    tinyxml2::XMLElement* ProgressBarCar = xml.NewElement("QFProgressBarCar");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        ProgressBarCar->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    double percen = pbc->GetPercentage();
    ProgressBarCar->SetAttribute("posit",std::to_string(percen).c_str());
    ProgressBarCar->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    ProgressBarCar->SetAttribute("name", pbc->GetQWNDName().c_str());
    return ProgressBarCar;
}
//6.创建LineChart
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QLineChart* qlc, QHost* hostw)
{
    tinyxml2::XMLElement* LineChart = xml.NewElement("QFLineChart");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        LineChart->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    LineChart->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    LineChart->SetAttribute("name", qlc->GetQWNDName().c_str());
    return LineChart;
}
//7.创建Tabe
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QTabs* qt, QHost* hostw)
{
    tinyxml2::XMLElement* Tabs = xml.NewElement("QFTabs");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        Tabs->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    Tabs->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    Tabs->SetAttribute("name", qt->GetQWNDName().c_str());
    //记录每一个tabe的内容
    int tcount = qt->GetTabeCount();
    if (0 < tcount) {
        tinyxml2::XMLElement* QFTabs_tables = xml.NewElement("Tables");//多页标签的标签集
        Tabs->InsertEndChild(QFTabs_tables);
        for (int i = -1; ++i < tcount;) {
            tinyxml2::XMLElement* QFTabs_table = xml.NewElement("Table");//多页标签的单个标签
            QFTabs_tables->InsertEndChild(QFTabs_table);
           
            QFTabs_table->SetAttribute("text",wstring_to_string(qt->GetTabeText(i)).c_str());
            QFTabs_table->SetAttribute("ico", wstring_to_string(qt->GetTabeBMPPath(i)).c_str());
            int ccount = qt->GetTabeCtrlCount(i);
            for (int j = -1; ++j < ccount;) {

                QHost qh(NULL ,QRect(0,0,10,10));
                qh.qwm = qt->GetCtrl(i, j);
                tinyxml2::XMLElement* element= SavaCtrl(xml, qh, qh.GetParasiticCtrl());
                qh.qwm = nullptr;
                if (element) {
                    QFTabs_table->InsertEndChild(element);
                
                }
            }

        }

    }


    return Tabs;
}
//8.创建CATLBarChart
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QCATLBarChart* qlc, QHost* hostw)
{
    tinyxml2::XMLElement* CATLBarChart = xml.NewElement("QFCATLBarChart");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        CATLBarChart->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    CATLBarChart->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    CATLBarChart->SetAttribute("name", qlc->GetQWNDName().c_str());
    return CATLBarChart;
}
//9.创建CATLTempeChart
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QCATLTempeChart* qlc, QHost* hostw)
{
    tinyxml2::XMLElement* CATLTempeChart = xml.NewElement("QFCATLTempeChart");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        CATLTempeChart->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    CATLTempeChart->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    CATLTempeChart->SetAttribute("name", qlc->GetQWNDName().c_str());
    return CATLTempeChart;
}
//10.创建Drawer
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QDrawer* qlc, QHost* hostw)
{
    tinyxml2::XMLElement* Drawer = xml.NewElement("QFDrawer");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        Drawer->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    Drawer->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    Drawer->SetAttribute("name", qlc->GetQWNDName().c_str());
    //把项目记录下来
    int dCount =qlc->GetDrawerCount();
    if (0 < dCount) {
        tinyxml2::XMLElement* Items = xml.NewElement("Items");
        Drawer->InsertEndChild(Items);
        for (int i = -1; ++i < dCount;) {
            tinyxml2::XMLElement* Category = xml.NewElement("Category");
            Items->InsertEndChild(Category);
            std::wstring dname =qlc->GetDrawerName(i);
            Category->SetAttribute("name", wstring_to_string(dname).c_str());
            int sCount = qlc->GetSubObjectCount(i);
            for (int j = -1; ++j < sCount;) {
                tinyxml2::XMLElement* Subkey = xml.NewElement("Subkey");
                std::wstring sname = qlc->GetSubObjectName(i, j);
                Subkey->SetAttribute("name", wstring_to_string(sname).c_str());
                Category->InsertEndChild(Subkey);
      
            
            }


        }
    }



    return Drawer;
}
//11.创建HLayout
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QHLayout* qlc, QHost* hostw)
{
    tinyxml2::XMLElement* HLayout = xml.NewElement("QFHLayout");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        HLayout->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    HLayout->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    HLayout->SetAttribute("name", qlc->GetQWNDName().c_str());
    //获得布局的控件
    int ccount = qlc->GetCtrlCount();
    QHost hs(nullptr, QRect(0, 0, 10, 10));
    for (int i = -1; ++i < ccount;) {
        
        hs.qwm=qlc->GetCtrl(i);
        tinyxml2::XMLElement* element = SavaCtrl(xml,hs,hs.GetParasiticCtrl());
        if (element) {
            element->SetAttribute("weight",std::to_string(qlc->GetCtrlWeight(i)).c_str());
            HLayout->InsertEndChild(element);
        }
        

    }
    hs.qwm = nullptr;

    return HLayout;
}
//12.创建VLayout
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QVLayout* qlc, QHost* hostw)
{
    tinyxml2::XMLElement* VLayout = xml.NewElement("QFVLayout");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        VLayout->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    VLayout->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    VLayout->SetAttribute("name", qlc->GetQWNDName().c_str());
    //获得布局的控件
    int ccount = qlc->GetCtrlCount();
    QHost hs(nullptr, QRect(0, 0, 10, 10));
    for (int i = -1; ++i < ccount;) {

        hs.qwm = qlc->GetCtrl(i);
        tinyxml2::XMLElement* element = SavaCtrl(xml, hs, hs.GetParasiticCtrl());
        if (element) {
            element->SetAttribute("weight", std::to_string(qlc->GetCtrlWeight(i)).c_str());
            VLayout->InsertEndChild(element);
        }


    }
    hs.qwm = nullptr;
    return VLayout;
}
//13.创建StaticText
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QStaticText* qlc, QHost* hostw)
{
    tinyxml2::XMLElement* StaticText = xml.NewElement("QFStaticText");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        StaticText->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    StaticText->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    StaticText->SetAttribute("name", qlc->GetQWNDName().c_str());
    return StaticText;
}
//14.创建TLayout
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QTLayout* qtl, QHost* hostw)
{
    tinyxml2::XMLElement* TLayout = xml.NewElement("QFTLayout");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        TLayout->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    TLayout->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    TLayout->SetAttribute("name", qtl->GetQWNDName().c_str());
    //字控件
    QHost hs(nullptr, QRect(0, 0, 10, 10));
    hs.qwm = qtl->GetCtrl();
    if (hs.qwm) {
        tinyxml2::XMLElement* element = SavaCtrl(xml, hs, hs.GetParasiticCtrl());
        if (element) {
            TLayout->InsertEndChild(element);
        }
    }
    hs.qwm = nullptr;
    return TLayout;
}
//15.创建QSlider
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QSlider* qlc, QHost* hostw)
{
    tinyxml2::XMLElement* ctrl = xml.NewElement("QFSlider");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        ctrl->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    ctrl->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    ctrl->SetAttribute("name", qlc->GetQWNDName().c_str());
    //分段值保存下来
    int count = qlc->GetSegmValue(nullptr,0);
    if (count > 0) {
        tinyxml2::XMLElement* Items = xml.NewElement("Items");
        ctrl->InsertEndChild(Items);
        float* SegArr = new float[count];
        qlc->GetSegmValue(SegArr, count);
        for (int i = -1; ++i < count; ){
            tinyxml2::XMLElement* SegmValue = xml.NewElement("SegmValue");
            Items->InsertEndChild(SegmValue);
            SegmValue->SetAttribute("value", std::to_string(SegArr[i]).c_str());

        }

        

        delete[] SegArr;
    }


    return ctrl;
}
//16.创建QSwitch
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QSwitch* qlc, QHost* hostw)
{
    tinyxml2::XMLElement* ctrl = xml.NewElement("QFSwitch");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        ctrl->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    ctrl->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    ctrl->SetAttribute("name", qlc->GetQWNDName().c_str());
    return ctrl;
}
//17.创建QProgress
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QProgress* qlc, QHost* hostw)
{
    tinyxml2::XMLElement* ctrl = xml.NewElement("QFProgress");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        ctrl->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    ctrl->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    ctrl->SetAttribute("name", qlc->GetQWNDName().c_str());
    ctrl->SetAttribute("Type", std::format("{:d}", qlc->GetStyle()).c_str());
    return ctrl;
}
//18.创建QColorBox
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QColorBox* qlc, QHost* hostw)
{
    tinyxml2::XMLElement* ctrl = xml.NewElement("QFColorBox");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        ctrl->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    ctrl->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    ctrl->SetAttribute("name", qlc->GetQWNDName().c_str());
    return ctrl;
}
//19.创建QSelect
tinyxml2::XMLElement* CtreateCtrlXML(tinyxml2::XMLDocument& xml, QSelect* qlc, QHost* hostw)
{
    tinyxml2::XMLElement* ctrl = xml.NewElement("QFSelect");
    int size = hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
    for (int j = -1; ++j < size;) {
        char* k = nullptr;
        char* v = nullptr;
        hostw->TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
        ctrl->SetAttribute(k, v);
    }
    QRect rect = hostw->GetRect();
    ctrl->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());
    ctrl->SetAttribute("name", qlc->GetQWNDName().c_str());
    //读取项目
    int countItem = qlc->GetItemCount();
    if (countItem > 0) {
        tinyxml2::XMLElement* Items = xml.NewElement("Items");
        ctrl->InsertEndChild(Items);
        for (int i = -1; ++i < countItem;) {
            tinyxml2::XMLElement* SegmValue = xml.NewElement("Item");
            Items->InsertEndChild(SegmValue);
            unsigned char d=0;
            unsigned int c1 = 0;
            unsigned int c2 = 0;
            if (1 == qlc->GetItemFontColor(i, &d, &c1, &c2)) {
                SegmValue->SetAttribute("fcolorD",std::format("{:d}",d).c_str());
                SegmValue->SetAttribute("fcolor1", std::format("0x{:x}", c1).c_str());
                SegmValue->SetAttribute("fcolor2", std::format("0x{:x}", c2).c_str());
            }
            if (1 == qlc->GetItemBKColor(i, &d, &c1, &c2)) {
                SegmValue->SetAttribute("colorD", std::format("{:d}", d).c_str());
                SegmValue->SetAttribute("color1", std::format("0x{:x}", c1).c_str());
                SegmValue->SetAttribute("color2", std::format("0x{:x}", c2).c_str());
            }
            SegmValue->SetAttribute("value", wstring_to_string(qlc->GetItemText(i)).c_str());

        }




    }

    
    return ctrl;
}

/// <summary>
/// SavaCtrl
/// </summary>
/// <param name="xml"></param>
/// <param name="hostw">得用QHost装载的QHostCtrl</param>
/// <param name="pWnd">真实的DLL控件指针</param>
/// <returns></returns>
tinyxml2::XMLElement* SavaCtrl(tinyxml2::XMLDocument& xml, QHost& hostw, void* pWnd)
{
    auto it = g_allCtrl.find(int(pWnd));
    if (it == g_allCtrl.end()) {
        return nullptr;
    }

    //QRect rect = hostw.GetRect();


    if ("QFButton" == it->second.createT) {

        QButton qbt(NULL, QRect(0, 0, 10, 10));
        qbt.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* button = CtreateCtrlXML(xml, &qbt, (QHost*)&hostw);
        qbt.qwm = nullptr;
        return button;
    }
    else if ("QFPButton" == it->second.createT) {
        QButton2 qbt(NULL, QRect(0, 0, 10, 10));
        qbt.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* button = CtreateCtrlXML(xml, &qbt, (QHost*)&hostw);
        qbt.qwm = nullptr;
        return button;

    }
    else if ("QFEdit" == it->second.createT) {
        QEdit ed(NULL, QRect(0, 0, 10, 10));
        ed.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* edit = CtreateCtrlXML(xml, &ed, (QHost*)&hostw);
        ed.qwm = nullptr;
        return edit;

    }
    else if ("QFReportForm" == it->second.createT) {
        QReportForm rf(NULL, QRect(0, 0, 10, 10));
        rf.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* reportForm = CtreateCtrlXML(xml, &rf, (QHost*)&hostw);
        rf.qwm = nullptr;
        return reportForm;

    }
    else if ("QFPictureBox" == it->second.createT) {
        QPictureBox pb(NULL, QRect(0, 0, 10, 10));
        pb.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &pb, (QHost*)&hostw);
        pb.qwm = nullptr;
        return eleme;

    }
    else if ("QFProgressBarCar" == it->second.createT) {
        QProgressBarCar pbc(NULL, QRect(0, 0, 10, 10));
        pbc.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &pbc, (QHost*)&hostw);
        pbc.qwm = nullptr;
        return eleme;

    }
    else if ("QFLineChart" == it->second.createT) {
        QLineChart qlc(NULL, QRect(0, 0, 10, 10));
        qlc.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &qlc, (QHost*)&hostw);
        qlc.qwm = nullptr;
        return eleme;
    }
    else if ("QFTabs" == it->second.createT) {
        QTabs qtb(NULL, QRect(0, 0, 10, 10));
        qtb.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &qtb, (QHost*)&hostw);
        qtb.qwm = nullptr;
        return eleme;
    }
    else if ("QFCATLBarChart" == it->second.createT) {
        QCATLBarChart qcbc(NULL, QRect(0, 0, 10, 10));
        qcbc.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &qcbc, (QHost*)&hostw);
        qcbc.qwm = nullptr;
        return eleme;
    }
    else if ("QFCATLTempeChart" == it->second.createT) {
        QCATLTempeChart qctc(NULL, QRect(0, 0, 10, 10));
        qctc.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &qctc, (QHost*)&hostw);
        qctc.qwm = nullptr;
        return eleme;
    }
    else if ("QFDrawer" == it->second.createT) {
        QDrawer qd(NULL, QRect(0, 0, 10, 10));
        qd.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &qd, (QHost*)&hostw);
        qd.qwm = nullptr;
        return eleme;
    }
    else if ("QFHLayout" == it->second.createT) {
        QHLayout qhl(NULL, QRect(0, 0, 10, 10));
        qhl.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &qhl, (QHost*)&hostw);
        qhl.qwm = nullptr;
        return eleme;
    }
    else if ("QFVLayout" == it->second.createT) {
        QVLayout qvl(NULL, QRect(0, 0, 10, 10));
        qvl.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &qvl, (QHost*)&hostw);
        qvl.qwm = nullptr;
        return eleme;
    }
    else if ("QFTLayout" == it->second.createT) {
        QTLayout qtl(NULL, QRect(0, 0, 10, 10));
        qtl.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &qtl, (QHost*)&hostw);
        qtl.qwm = nullptr;
        return eleme;
        
    }
    else if ("QFStaticText" == it->second.createT) {
        QStaticText qst(NULL, QRect(0, 0, 10, 10));
        qst.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &qst, (QHost*)&hostw);
        qst.qwm = nullptr;
        return eleme;
    }
    else if ("QFSlider" == it->second.createT) {
        QSlider ctrl(NULL, QRect(0, 0, 10, 10));
        ctrl.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &ctrl, (QHost*)&hostw);
        ctrl.qwm = nullptr;
        return eleme;
    }
    else if ("QFSwitch" == it->second.createT) {
        QSwitch ctrl(NULL, QRect(0, 0, 10, 10));
        ctrl.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &ctrl, (QHost*)&hostw);
        ctrl.qwm = nullptr;
        return eleme;
    }
    else if ("QFProgress" == it->second.createT) {
        QProgress ctrl(NULL, QRect(0, 0, 10, 10));
        ctrl.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &ctrl, (QHost*)&hostw);
        ctrl.qwm = nullptr;
        return eleme;
    }
    else if ("QFColorBox" == it->second.createT) {
        QColorBox ctrl(NULL, QRect(0, 0, 10, 10));
        ctrl.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &ctrl, (QHost*)&hostw);
        ctrl.qwm = nullptr;
        return eleme;
    }
    else if ("QFSelect" == it->second.createT) {
        QSelect ctrl(NULL, QRect(0, 0, 10, 10));
        ctrl.qwm = (QWND*)pWnd;
        tinyxml2::XMLElement* eleme = CtreateCtrlXML(xml, &ctrl, (QHost*)&hostw);
        ctrl.qwm = nullptr;
        return eleme;
    }

    return nullptr;
}

//保存文件按钮事件
void SavaBTClickEvent(void* bt)
{
    printf("SavaFile:%p\n", bt);
    //第一步得先拿到窗口的全部控件
    tinyxml2::XMLDocument xml;
    xml.Parse("<?xml version=\"1.0\" encoding=\"UTF - 8\"?>"
        "<!--奇风UI窗口布局文件-->"
    );
    tinyxml2::XMLComment* comment = xml.NewComment("窗口布局主体必须QWMange（目前有且只能有一个），name随意设置");
    xml.InsertEndChild(comment);

    tinyxml2::XMLElement* rootNode = xml.NewElement("QWMange");
    rootNode->SetAttribute("name", g_qew->GetQWNDName().c_str());
    //把编辑窗口的矩形大小也保存起来
    QRect rect =g_qew->GetRect();
    rootNode->SetAttribute("Rect", std::format("({},{},{},{})", rect.X, rect.Y, rect.Width, rect.Height).c_str());

    xml.InsertEndChild(rootNode);

    rootNode->InsertEndChild(xml.NewComment("每一个并行节点都是一个控件，控件节点元素名为：控件名，如按钮：QButton"));
    rootNode->InsertEndChild(xml.NewComment("控件属性有name：可以随意设置，尽量控件之间不要相同；Rect：控件矩形"));

    int count = g_qew->TransferMessage(QM_GETSUBCTRL, 0, 0, 0);
    printf("SavaFile:CtrlCount -> ( %d )\n", count);
    //保存控件

   
    for (int i = -1; ++i < count;) {
        
        void* p =nullptr;
        void* hp = nullptr;
       
        if (g_qew->TransferMessage(QM_GETSUBCTRL, QDWORD(&p), i, QDWORD(&hp)); p) {
            QHost hostw(NULL, QRect(0, 0, 10, 10));
            hostw.qwm = (QWND*)hp;
            tinyxml2::XMLElement* ctrl = SavaCtrl(xml,hostw,p);
            rootNode->InsertEndChild(ctrl);
            hostw.qwm = nullptr;
        }
    }
    auto newSava = [&xml]() {
        // 初始化OPENFILENAME结构体
        OPENFILENAMEA ofn;
        char szFile[MAX_PATH] = { 0 }; // 用于存储文件路径

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "XML Files\0*.xml\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        // 显示文件打开对话框
        if (GetSaveFileNameA(&ofn) == FALSE) {
            return;
        }
        std::string filePath = ofn.lpstrFile;

        // Lambda 表达式：检查扩展名并添加 .xml
        auto ensureXmlExtension = [](std::string& filename) {
            size_t dotPos = filename.rfind('.');
            if (dotPos == std::string::npos || dotPos == filename.length() - 1) {
                filename += ".xml";
            }
        };
        ensureXmlExtension(filePath);
        xml.SaveFile(filePath.c_str());
    };
    if ("" == xmlFilepath) {
        newSava();
    }
    else {
        if (IDYES != MessageBoxA(0, "是否覆盖打开的文件", "保存XML", MB_YESNO)) {
            newSava();
        }
        else {
            xml.SaveFile(xmlFilepath.c_str());
        }
    
    }


    
    
    

    //auto it = m_allCtrl.find(int(p));
    //if (it == m_allCtrl.end()) {
    //    printf("SavaFile:没有控件消息\n");
    //    return;
    //}
    //printf("SavaFile:这个控件是：%s\n", it->second.c_str());
    //for (int i = 0; ++i < count;) {
    //    g_qew->TransferMessage(QM_GETSUBCTRL, DWORD(&p), i, 0);
    //    auto it = m_allCtrl.find(int(p));
    //    if (it == m_allCtrl.end()) {
    //        printf("SavaFile:没有控件消息\n");
    //        return;
    //    }
    //    printf("SavaFile:这个控件是：%s\n", it->second.c_str());
    //}
}

void DeleteCtrlEvent(int ctrl)
{

    auto it = g_allCtrl.find(ctrl);
    if (it == g_allCtrl.end()) {
        return ;
    }
 
    int pCtrl = it->first;
    QFUSE_EW::CtrlAttribute ca = it->second;
    printf("deleteCtrl:%p ====>%s \n", ctrl, it->second.createT.c_str());
    //对一些容器类的控件进行特殊处理
    if ("QFTabs" == ca.createT) {
        if (IDYES != MessageBoxA(0, "此控件为Tabe控件，删除将连它内部控件一起删除，是否删除", "删除控件", MB_YESNO))return;
        ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(ca.qhp->qwm), 0, 0);
        delete ca.qhp;
        QWNDHANDLE::QWNDs.erase(ca.qhp);//从控件集合里面摘掉这个控件不然退出是会报错！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
        //取控件内的子控件
        QTabs qt(nullptr, QRect(0, 0, 1, 1));
        qt.qwm = (QWND*)pCtrl;
        int tcount = qt.GetTabeCount();
        for (int i = -1; ++i < tcount;) {
            int ccount = qt.GetTabeCtrlCount(i);
            for (int j = -1; ++j < ccount;) {

                QHost qh(NULL, QRect(0, 0, 10, 10));
                qh.qwm = qt.GetCtrl(i, j);
                auto it = g_allCtrl.find(int(qh.GetParasiticCtrl()));
                if (it != g_allCtrl.end()) {
                    delete it->second.qhp;
                    QWNDHANDLE::QWNDs.erase(it->second.qhp);
                    g_allCtrl.erase(it);
                }

                qh.qwm = nullptr;

            }

        }
        //ca.parent->TransferMessage(QM_REMOVE_CTRL,QDWORD(qt.qwm),0,0);
        
    }
    else if ("QFHLayout" == ca.createT) {
        if (IDYES != MessageBoxA(0, "此控件为HLayout控件，删除将连它内部控件一起删除，是否删除", "删除控件", MB_YESNO))return;
        ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(ca.qhp->qwm), 0, 0);
        delete ca.qhp;
        QWNDHANDLE::QWNDs.erase(ca.qhp);//从控件集合里面摘掉这个控件不然退出是会报错！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
        QHLayout qlc(nullptr, QRect(0, 0, 1, 1));
        qlc.qwm = (QWND*)pCtrl;
        int ccount = qlc.GetCtrlCount();
      
        for (int i = -1; ++i < ccount;) {
            QHost hs(nullptr, QRect(0, 0, 10, 10));
            hs.qwm = qlc.GetCtrl(i);
            auto it = g_allCtrl.find(int(hs.GetParasiticCtrl()));
            if (it != g_allCtrl.end()) {
                delete it->second.qhp;
                QWNDHANDLE::QWNDs.erase(it->second.qhp);
                g_allCtrl.erase(it);
            }
            hs.qwm = nullptr;

        }
        //ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(qlc.qwm), 0, 0);
    }
    else if ("QFVLayout" == ca.createT) {
        if (IDYES != MessageBoxA(0, "此控件为VLayout控件，删除将连它内部控件一起删除，是否删除", "删除控件", MB_YESNO))return;
        ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(ca.qhp->qwm), 0, 0);
        delete ca.qhp;
        QWNDHANDLE::QWNDs.erase(ca.qhp);//从控件集合里面摘掉这个控件不然退出是会报错！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
        QVLayout qvl(nullptr, QRect(0, 0, 1, 1));
        qvl.qwm = (QWND*)pCtrl;
        int ccount = qvl.GetCtrlCount();

        for (int i = -1; ++i < ccount;) {
            QHost hs(nullptr, QRect(0, 0, 10, 10));
            hs.qwm = qvl.GetCtrl(i);
            auto it = g_allCtrl.find(int(hs.GetParasiticCtrl()));
            if (it != g_allCtrl.end()) {
                delete it->second.qhp;
                QWNDHANDLE::QWNDs.erase(it->second.qhp);
                g_allCtrl.erase(it);
            }
            hs.qwm = nullptr;

        }
        //ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(qvl.qwm), 0, 0);
    }
    else if ("QFTLayout" == ca.createT) {
        if (IDYES != MessageBoxA(0, "此控件为TLayout控件，删除将连它内部控件一起删除，是否删除", "删除控件", MB_YESNO))return;
        ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(ca.qhp->qwm), 0, 0);
        delete ca.qhp;
        QWNDHANDLE::QWNDs.erase(ca.qhp);//从控件集合里面摘掉这个控件不然退出是会报错！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
        QTLayout qtl(nullptr, QRect(0, 0, 1, 1));
        qtl.qwm = (QWND*)pCtrl;
        QHost hs(nullptr, QRect(0, 0, 10, 10));
        hs.qwm = qtl.GetCtrl();
        auto it = g_allCtrl.find(int(hs.GetParasiticCtrl()));
        if (it != g_allCtrl.end()) {
            delete it->second.qhp;
            QWNDHANDLE::QWNDs.erase(it->second.qhp);
            g_allCtrl.erase(it);

        }
        hs.qwm = nullptr;

        //ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(qtl.qwm), 0, 0);
    }
    else {
        ca.parent->TransferMessage(QM_REMOVE_CTRL, QDWORD(ca.qhp->qwm), 0, 0);
        delete ca.qhp;
        QWNDHANDLE::QWNDs.erase(ca.qhp);//从控件集合里面摘掉这个控件不然退出是会报错！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
        DLLImportQWND dq(nullptr,QRect(0,0,10,10));
       
        dq.qwm = (QWND*)it->first;
        g_allCtrl.erase(it);
        
    }
    qct->ClearAttributeCtrl();
  


}

void SelectCtrlEvent(int ctrl)
{
    printf("SelectCtrlEvent:%x ==>\n", ctrl);
    qct->ClearAttributeCtrl();
    if ((int)g_qew->qwm != ctrl) {
        //来自非窗口的
    
    }
    else {
        //来自窗口的，不能修改xy坐标


    }
    void BindCtrlAttribute(int ctrl);
    BindCtrlAttribute(ctrl);

}
//窗口鼠标左键按下事件
int WLDownEvent(int bt,int x,int y)
{
    printf("WLDownEvent:%x ==> (%d,%d)\n", bt,x,y);
   

    return 0;
}

int EditChangeEvent(int p, int pt, int param)
{
    printf("EditChangeEvent:%p ==> (%s,%d)\n", p, wstring_to_string(std::wstring((wchar_t*)pt)).c_str(), param);
    return 0;
}


//窗口增加控件
int WPlaceCtrlEvent(int bt, int x, int y, int r, int b)
{
    if ("" != cText) {

        QHost* qfhc = (QHost*)QFUSE::CreateQWND("QFHostCtrl", QRect(x, y, (std::max)(r-x,10),(std::max)(b-y,10)));
        qfhc->TransferMessage(QM_HOSTCTRL_INSERT_PAIR,DWORD("name"), DWORD(cText.c_str()),0);
        DLLImportQWND* qwd = (DLLImportQWND*)QFUSE::CreateQWND(cText.c_str(), QRect(0, 0, 100, 50));
        if ("QFTabs" == cText) {
            QTabs* p = (QTabs*)qwd;
            p->AddTab(L"标签1");
            p->AddTab(L"标签2");
        }
        else if ("QFReportForm" == cText) {
            QReportForm* p = (QReportForm*)qwd;
            p->InsertTitle(L"test1",80);
            p->InsertTitle(L"测试1", 150);
        }
        else if ("QFCATLTempeChart" == cText) {
            QCATLTempeChart* p = (QCATLTempeChart*)qwd;
            for (int i = -1; ++i < 2;) {
                p->InsertGroup(-1);
                std::wstring ws = std::format(L"温度图表{:d}", i + 1);
                p->SetGroupName(i, ws.c_str());
                int c = 0xffA0A0A0;
                for (int j = -1; ++j < 5;) {
                    p->InsertUnit(i, -1, c | i * 20, std::to_wstring(i).c_str());
                    p->InsertUnit(i, -1, c | ((i * 20) << 8), std::to_wstring(i * 10).c_str());
                    p->InsertUnit(i, -1, c | ((i * 20) << 16), std::to_wstring(i * 100).c_str());
                }
            }
           
           
            
        }
        else if ("QFDrawer" == cText) {
            QDrawer* qd = (QDrawer*)qwd;
            std::vector<std::wstring> arr = { L"坤坤", L"分组2", L"分组3" };
            for (int i = -1; ++i < arr.size();) {
                qd->InsertDrawer(-1);   //插入10个Drawer
                    //下面为10个Drawer设置名称
                qd->SetDrawerName(i, arr[i].c_str());
            }
            std::vector<std::wstring> arrJL = { L"唱" , L"跳"  , L"rap"  , L"篮球" };
            std::vector<std::wstring> arrMi = { L"分组2的项目1" };
            std::vector<std::wstring> arrCC = { L"分组3的项目1", L"分组3的项目2" , L"分组3的项目3" , L"分组3的项目4", L"分组3的项目5" , L"分组3的项目6" };
            for (int i = -1; ++i < arrJL.size();) {
                qd->InsertSubObject(0, -1);
                qd->SetSubObjectName(0, i, arrJL[i].c_str());
            }
            for (int i = -1; ++i < arrMi.size();) {
                qd->InsertSubObject(1, -1);
                qd->SetSubObjectName(1, i, arrMi[i].c_str());
            }
            for (int i = -1; ++i < arrCC.size();) {
                qd->InsertSubObject(2, -1);
                qd->SetSubObjectName(2, i, arrCC[i].c_str());
            }
        
        
        }
        
        int no = 1;
        bool isPass = false;
        do
        {
            std::string currentNM = cText+ std::to_string(no++);
            isPass = false;
            DLLImportQWND pdq(nullptr, QRect());
            for (auto& it : g_allCtrl) {
                if (cText == it.second.createT) {
                    QHost* pqh= it.second.qhp;
               

                   
                    pdq.qwm = (DLLImportQWND*)pqh->GetParasiticCtrl();
                    
                    std::string name=pdq.GetQWNDName();
                    if (currentNM == name) {
                        isPass = true;
                        break;
                    }


                   
  
                }
            
            }
            pdq.qwm = nullptr;
            if (!isPass) {
                qwd->SetQWNDName(currentNM.c_str());
                qfhc->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("name"), DWORD(currentNM.c_str()), 0);
                break;
            }

        } while (true);



        
        DLLImportQWND* ctrl = (DLLImportQWND*)bt;
        ctrl->TransferMessage(QM_PLACE_CTRL, QDWORD(qfhc->qwm), 1, 0);//第二个参数写个1为了遇到布局控件时权重为1
        g_allCtrl.insert(std::pair<int, QFUSE_EW::CtrlAttribute>(int(qwd->qwm), { cText,qfhc,(DLLImportQWND*)bt }));
        qfhc->SetParasiticCtrl(qwd);
        qwd->qwm = nullptr;
        delete qwd;
        QWNDHANDLE::QWNDs.erase(qwd);//从控件集合里面摘掉这个控件不然退出是会报错！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
     
      
  
        cText = "";
    }
    //cText = "";
    //cText.clear();
    return 0;
}

#define ____BASE_ATTRIBUTE__
#ifdef ____BASE_ATTRIBUTE__



//基础属性
class bAttribute
{
public:
    QWND* ctrl =nullptr;
    QHLayout* m_Xqhl =nullptr;
    QHLayout* m_Yqhl = nullptr;
    QHLayout* m_Wqhl = nullptr;
    QHLayout* m_Hqhl = nullptr;
    QHLayout* m_Nameqhl = nullptr;


    QStaticText* m_Xqst = nullptr;
    QStaticText* m_Yqst = nullptr;
    QStaticText* m_Wqst = nullptr;
    QStaticText* m_Hqst = nullptr;
    QStaticText* m_Nameqst = nullptr;

    QEdit* m_Xedit = nullptr;
    QEdit* m_Yedit = nullptr;
    QEdit* m_Wedit = nullptr;
    QEdit* m_Hedit = nullptr;
    QEdit* m_Nameedit = nullptr;



    bAttribute() {
        int height = 32;
        m_Xqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText",QRect(0,0,64,100));
        m_Xqst->SetBKColor(0xfff0f0f0);
        m_Xqst->SetText(L"X:");
        m_Xedit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 64, 100));
        m_Xedit->SetChangeEvent(bAttribute::XChange,(int)this);
        m_Xqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, height));
        m_Xqhl->InsertCtrl(m_Xqst,0);
        m_Xqhl->InsertCtrl(m_Xedit, 1);

        m_Yqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 64, 50));
        m_Yqst->SetBKColor(0xfff0f0f0);
        m_Yqst->SetText(L"Y:");
        m_Yedit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
        m_Yedit->SetChangeEvent(bAttribute::YChange, (int)this);
        m_Yqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, height));
        m_Yqhl->InsertCtrl(m_Yqst, 0);
        m_Yqhl->InsertCtrl(m_Yedit, 1);

        m_Wqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 64, 50));
        m_Wqst->SetBKColor(0xfff0f0f0);
        m_Wqst->SetText(L"Width:");
        m_Wedit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
        m_Wedit->SetChangeEvent(bAttribute::WChange, (int)this);
        m_Wqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, height));
        m_Wqhl->InsertCtrl(m_Wqst, 0);
        m_Wqhl->InsertCtrl(m_Wedit, 1);

        m_Hqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 64, 50));
        m_Hqst->SetBKColor(0xfff0f0f0);
        m_Hqst->SetText(L"Height:");
        m_Hedit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
        m_Hedit->SetChangeEvent(bAttribute::HChange, (int)this);
        m_Hqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, height));
        m_Hqhl->InsertCtrl(m_Hqst, 0);
        m_Hqhl->InsertCtrl(m_Hedit, 1);

        m_Nameqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 64, 50));
        m_Nameqst->SetBKColor(0xfff0f0f0);
        m_Nameqst->SetText(L"Name:");
        m_Nameedit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
        m_Nameedit->SetChangeEvent(bAttribute::NameChange, (int)this);
        m_Nameqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, height));
        m_Nameqhl->InsertCtrl(m_Nameqst, 0);
        m_Nameqhl->InsertCtrl(m_Nameedit, 1);

    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        if (ctrl != g_qew->qwm) {
            QRect rect = host.GetRect();
            DLLImportQWND pdq2(nullptr, QRect(0, 0, 10, 10));
            pdq2.qwm = host.GetParasiticCtrl();
            m_Xedit->SetText(std::to_wstring(rect.X).c_str());
            m_Yedit->SetText(std::to_wstring(rect.Y).c_str());
            m_Wedit->SetText(std::to_wstring(rect.Width).c_str());
            m_Hedit->SetText(std::to_wstring(rect.Height).c_str());
            m_Nameedit->SetText(stringToWString(pdq2.GetQWNDName()).c_str());
        
            pdq2.qwm = nullptr;
        }
        else {
            DLLImportQWND pdq(nullptr, QRect(0, 0, 10, 10));
            pdq.qwm = ctrl;
            QRect rect = pdq.GetRect();
            m_Xedit->SetText(std::to_wstring(rect.X).c_str());
            m_Yedit->SetText(std::to_wstring(rect.Y).c_str());
            m_Wedit->SetText(std::to_wstring(rect.Width).c_str());
            m_Hedit->SetText(std::to_wstring(rect.Height).c_str());
            m_Nameedit->SetText(stringToWString(pdq.GetQWNDName()).c_str());


            pdq.qwm = nullptr;
        }
        host.qwm = nullptr;
    }


    static int XChange(int p, int pt, int param)
    {
        bAttribute* pba = (bAttribute*)param;
        if (pba->ctrl) {
            DLLImportQWND pdq(nullptr,QRect(0,0,10,10));
            pdq.qwm = pba->ctrl;
            QRect rect= pdq.GetRect();
            rect.X = std::strtol(wstring_to_string((wchar_t*)pt).c_str(),NULL,10);
            pdq.SetLocation(QPoint(rect.X, rect.Y));
            pdq.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pdq.qwm = nullptr;
       
           
        }
        return 0;
    }
    static int YChange(int p, int pt, int param)
    {
        bAttribute* pba = (bAttribute*)param;
        if (pba->ctrl) {
            DLLImportQWND pdq(nullptr, QRect(0, 0, 10, 10));
            pdq.qwm = pba->ctrl;
            QRect rect = pdq.GetRect();
            rect.Y = std::strtol(wstring_to_string((wchar_t*)pt).c_str(), NULL, 10);
            pdq.SetLocation(QPoint(rect.X, rect.Y));
            pdq.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pdq.qwm = nullptr;

        }
        return 0;
    }
    static int WChange(int p, int pt, int param)
    {
        bAttribute* pba = (bAttribute*)param;
        if (pba->ctrl) {
            DLLImportQWND pdq(nullptr, QRect(0, 0, 10, 10));
            pdq.qwm = pba->ctrl;
            QRect rect = pdq.GetRect();
            rect.Width = std::strtol(wstring_to_string((wchar_t*)pt).c_str(), NULL, 10);
            pdq.Reconstruct(nullptr, rect);
            pdq.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pdq.qwm = nullptr;

        }
        return 0;
    }
    static int HChange(int p, int pt, int param)
    {
        bAttribute* pba = (bAttribute*)param;
        if (pba->ctrl) {
            DLLImportQWND pdq(nullptr, QRect(0, 0, 10, 10));
            pdq.qwm = pba->ctrl;
            QRect rect = pdq.GetRect();
            rect.Height = std::strtol(wstring_to_string((wchar_t*)pt).c_str(), NULL, 10);
            pdq.Reconstruct(nullptr, rect);
            pdq.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pdq.qwm = nullptr;

        }
        return 0;
    }
    static int NameChange(int p, int pt, int param)
    {
        bAttribute* pba = (bAttribute*)param;
        std::string name = wstring_to_string((wchar_t*)pt);
        if (pba->ctrl) {
            if (pba->ctrl != g_qew->qwm) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
                DLLImportQWND ddi(nullptr, QRect(0, 0, 10, 10));
                ddi.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
           
                ddi.SetQWNDName(name.c_str());
                ddi.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("name"), DWORD(name.c_str()), 0);
                pqh.qwm = nullptr;
            }
            else {
                g_qew->SetQWNDName(name.c_str());
            }

        }
        return 0;
    }
};
//button属性
class btAttribute
{
public:
    QWND* ctrl = nullptr;
    QHLayout* m_Clickqhl = nullptr;
    QHLayout* m_FNqhl = nullptr;
    QHLayout* m_FSqhl = nullptr;
    QHLayout* m_TEXTqhl = nullptr;



    QStaticText* m_Clickqst = nullptr;
    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_TEXTqst = nullptr;

    QEdit* m_Clickedit = nullptr;
    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;
    QEdit* m_TEXTedit = nullptr;



    btAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring test(L"Text:");
        createCtr(&m_TEXTqst, &m_TEXTedit, &m_TEXTqhl, test, textChange);
        test = L"click:";
        createCtr(&m_Clickqst, &m_Clickedit, &m_Clickqhl, test, clickChange);
        test=L"fontName:";
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, test, FNChange);
        test = L"fontSize:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, test,FSChange);



    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        DLLImportQWND pdq(nullptr, QRect(0, 0, 10, 10));
        pdq.qwm = host.GetParasiticCtrl();
        QRect rect = pdq.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("onClick" == key) {
                m_Clickedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
            else if ("Text" == key) {
                m_TEXTedit->SetText(stringToWString(value).c_str());
            
            }
        }
        
   
        host.qwm = nullptr;
        pdq.qwm = nullptr;
    }
    void Clear()
    {
        m_Clickedit->SetText(L"");
        m_FNedit->SetText(L"");
        m_FSedit->SetText(L"");
        m_TEXTedit->SetText(L"");
    }

    static int clickChange(int p, int pt, int param)
    {
        btAttribute* pba = (btAttribute*)param;
        
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            //DLLImportQWND ddi(nullptr, QRect(0, 0, 10, 10));
            //ddi.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

            //ddi.SetQWNDName(name.c_str());
            //ddi.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("onClick"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int FNChange(int p, int pt, int param)
    {
        btAttribute* pba = (btAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QButton ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontName((wchar_t*)pt);
            
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontName"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FSChange(int p, int pt, int param)
    {
        btAttribute* pba = (btAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QButton ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontSize(std::strtod(wstring_to_string((wchar_t*)pt).c_str(), NULL));

            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontSize"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int textChange(int p, int pt, int param)
    {
        btAttribute* pba = (btAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QButton ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetText((wchar_t*)pt);

            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("Text"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
};
//button2属性
class bt2Attribute
{
public:
    QWND* ctrl = nullptr;
    QHLayout* m_Clickqhl = nullptr;
    QHLayout* m_FNqhl = nullptr;
    QHLayout* m_FSqhl = nullptr;
    QHLayout* m_TEXTqhl = nullptr;
    QHLayout* m_BKCqhl = nullptr;
    QHLayout* m_FCqhl = nullptr;



    QStaticText* m_Clickqst = nullptr;
    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_TEXTqst = nullptr;
    QStaticText* m_BKCqst = nullptr;
    QStaticText* m_FCqst = nullptr;

    QEdit* m_Clickedit = nullptr;
    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;
    QEdit* m_TEXTedit = nullptr;

    QColorBox* m_bkColo = nullptr;
    QColorBox* m_fColo = nullptr;

    bt2Attribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring test(L"Text:");
        createCtr(&m_TEXTqst, &m_TEXTedit, &m_TEXTqhl, test, textChange);
        test = L"click:";
        createCtr(&m_Clickqst, &m_Clickedit, &m_Clickqhl, test, clickChange);
        test = L"fontName:";
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, test, FNChange);
        test = L"fontSize:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, test, FSChange);

        m_bkColo = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 32));
        m_bkColo->RecordAttribute(0, (long long)this);
        m_bkColo->SetColorChangeEvent(bkColorChange);
        m_bkColo->SetFontSize(16);
        m_BKCqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 80, 100));
        m_BKCqst->SetBKColor(0xfff0f0f0);
        m_BKCqst->SetText(L"背景颜色:");
        m_BKCqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_BKCqhl->InsertCtrl(m_BKCqst, 0);
        m_BKCqhl->InsertCtrl(m_bkColo, 1);

        m_fColo = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 32));
        m_fColo->RecordAttribute(0, (long long)this);
        m_fColo->SetColorChangeEvent(fColorChange);
        m_fColo->SetFontSize(16);
        m_FCqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 80, 100));
        m_FCqst->SetBKColor(0xfff0f0f0);
        m_FCqst->SetText(L"字体颜色:");
        m_FCqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_FCqhl->InsertCtrl(m_FCqst, 0);
        m_FCqhl->InsertCtrl(m_fColo, 1);



    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        DLLImportQWND pdq(nullptr, QRect(0, 0, 10, 10));
        pdq.qwm = host.GetParasiticCtrl();
        QRect rect = pdq.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("onClick" == key) {
                m_Clickedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
            else if ("Text" == key) {
                m_TEXTedit->SetText(stringToWString(value).c_str());

            }
            else if ("bkColor" == key) {
                int para = 10;
                if ('x' == std::tolower(v[1]))para = 16;
                unsigned long color = std::strtoul(v, nullptr, para);
                m_bkColo->SetColor(color);

            }
            else if ("fontColor" == key) {
                int para = 10;
                if ('x' == std::tolower(v[1]))para = 16;
                unsigned long color = std::strtoul(v, nullptr, para);
                m_fColo->SetColor(color);

            }
        }


        host.qwm = nullptr;
        pdq.qwm = nullptr;
    }
    void Clear()
    {
        m_Clickedit->SetText(L"");
        m_FNedit->SetText(L"");
        m_FSedit->SetText(L"");
        m_TEXTedit->SetText(L"");
        m_bkColo->SetColor(0xffffffff);
        m_fColo->SetColor(0xff000000);
    }

    static int clickChange(int p, int pt, int param)
    {
        bt2Attribute* pba = (bt2Attribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            //DLLImportQWND ddi(nullptr, QRect(0, 0, 10, 10));
            //ddi.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

            //ddi.SetQWNDName(name.c_str());
            //ddi.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("onClick"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int FNChange(int p, int pt, int param)
    {
        bt2Attribute* pba = (bt2Attribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QButton2 ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontName((wchar_t*)pt);

            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontName"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FSChange(int p, int pt, int param)
    {
        bt2Attribute* pba = (bt2Attribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QButton2 ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontSize(std::strtod(wstring_to_string((wchar_t*)pt).c_str(), NULL));

            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontSize"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int textChange(int p, int pt, int param)
    {
        bt2Attribute* pba = (bt2Attribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QButton2 ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetText((wchar_t*)pt);

            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("Text"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int bkColorChange(QPOINTER ct, unsigned int color)
    {
        QColorBox qcb(nullptr, QRect(0, 0, 10, 10));
        qcb.qwm = (QWND*)ct;
       
        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            bt2Attribute* p = (bt2Attribute*)data;
            if (p) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QButton2 ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

                ctrl.SetBKColor(color);


                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("bkColor"), DWORD(std::format("0x{:x}", color).c_str()), 0);
                pqh.qwm = nullptr;
            }
        
        }


        qcb.qwm = nullptr;
        return 1;
    }
    static int fColorChange(QPOINTER ct, unsigned int color)
    {
        QColorBox qcb(nullptr, QRect(0, 0, 10, 10));
        qcb.qwm = (QWND*)ct;

        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            bt2Attribute* p = (bt2Attribute*)data;
            if (p) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QButton2 ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

                ctrl.SetFontColor(color);


                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontColor"), DWORD(std::format("0x{:x}", color).c_str()), 0);
                pqh.qwm = nullptr;
            }

        }


        qcb.qwm = nullptr;
        return 1;
    }
};
//EDit属性
class edAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_FNqhl = nullptr;
    QHLayout* m_FSqhl = nullptr;
    QHLayout* m_TEXTqhl = nullptr;
    QHLayout* m_FUNqhl = nullptr;





    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_TEXTqst = nullptr;
    QStaticText* m_FUNqst = nullptr;


    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;
    QEdit* m_TEXTedit = nullptr;
    QEdit* m_FUNedit = nullptr;


    edAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev,int w=72)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, w, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring test(L"Text:");
        createCtr(&m_TEXTqst, &m_TEXTedit, &m_TEXTqhl, test, edAttribute::textChange);

        test = L"fontName:";
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, test, edAttribute::FNChange);
        test = L"fontSize:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, test, edAttribute::FSChange);
        test = L"内容改变事件:";
        createCtr(&m_FUNqst, &m_FUNedit, &m_FUNqhl, test, FUNChange,110);


    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QEdit ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("fontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
            else if ("TextChange" == key) {
                m_FUNedit->SetText(stringToWString(value).c_str());
            }
            
        }
        m_TEXTedit->SetText(ctrl.GetText());


        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {
        
        m_FNedit->SetText(L"黑体");
        m_FSedit->SetText(L"16");
        m_TEXTedit->SetText(L"");
        m_FUNedit->SetText(L"");
    }

    static int FNChange(int p, int pt, int param)
    {
        edAttribute* pba = (edAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QEdit ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontName((wchar_t*)pt);

            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontName"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FSChange(int p, int pt, int param)
    {
        edAttribute* pba = (edAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QEdit ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontSize(std::strtod(wstring_to_string((wchar_t*)pt).c_str(), NULL));

            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontSize"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int textChange(int p, int pt, int param)
    {
        edAttribute* pba = (edAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QEdit ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetText((wchar_t*)pt);

            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("Text"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int FUNChange(int p, int pt, int param)
    {
        edAttribute* pba = (edAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
    
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("TextChange"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
};
//QReportForm属性
class rfAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_THqhl = nullptr;
    QHLayout* m_FNqhl = nullptr;
    QHLayout* m_FSqhl = nullptr;
    QHLayout* m_AddTqhl = nullptr;//增加字段布局
    QHLayout* m_RMTqhl = nullptr;//移除字段布局
    QHLayout* m_UDTTqhl = nullptr;//修改字段布局
    QHLayout* m_UDTWqhl = nullptr;//修改字段布局
    QHLayout* m_TCqhl = nullptr;//提示操作布局
    QHLayout* m_Dragqhl = nullptr;//滑动布局


    QStaticText* m_THqst = nullptr;
    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_Dragqst = nullptr;

    //提示操作
    QStaticText* m_TC1qst = nullptr;
    QStaticText* m_TC2qst = nullptr;
    QStaticText* m_TC3qst = nullptr;
    QStaticText* m_TC4qst = nullptr;


    QEdit* m_THedit = nullptr;
    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;
    QEdit* m_AddTIedit = nullptr;//增加字段索引输入框
    QEdit* m_AddTTedit = nullptr;//增加字段标题输入框
    QEdit* m_AddTWedit = nullptr;//增加字段宽度输入框
    QEdit* m_RMTIedit = nullptr;//移除字段索引输入框
    //QEdit* m_RMTTedit = nullptr;//移除字段标题输入框
    QEdit* m_UDTIedit1 = nullptr;//修改字段索引输入框
    QEdit* m_UDTIedit2 = nullptr;//修改字段索引输入框
    QEdit* m_UDTTedit = nullptr;//修改字段标题输入框
    QEdit* m_UDTWedit = nullptr;//修改字段宽度输入框

    QButton* m_AddTButton = nullptr;//增加字段的按钮
    QButton* m_UDTButton = nullptr;//修改字段的按钮
    QButton* m_UDTWButton = nullptr;//修改字段的按钮
    QButton* m_RMTButton = nullptr;//移除字段的按钮

    QSwitch* m_DragQS = nullptr;//是否下滑

    rfAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring text(L"TitleHeight:");
        createCtr(&m_THqst, &m_THedit, &m_THqhl, text, rfAttribute::THChange);

        text = L"fontName:";
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, text, rfAttribute::FNChange);
        text = L"fontSize:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, text, rfAttribute::FSChange);

        //创建增加和移除字段的按钮
        auto createARCtrl = [this](QButton** qbt, QEdit** qte, QEdit** qie, QEdit** qwe, QHLayout** qhl, std::wstring& text, QFBUTTONCLICK ev)
        {
            
            *qbt = (QButton*)QFUSE::CreateQWND("QFButton", QRect(0, 0, 50, 100));
            (*qbt)->SetText(text.c_str());
            (*qbt)->SetFontSize(6);
            (*qbt)->SetClickEvent(ev);

            (*qie) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qie)->SetFontSize(10);
            if (qte) {
                (*qte) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            }
            if (qwe) {
                (*qwe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
                (*qwe)->SetFontSize(10);
            }
            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qbt), 0);
            (*qhl)->InsertCtrl((*qie), 0);
            if (qte) {
                (*qhl)->InsertCtrl((*qte), 1);
            }
            if (qwe) {
                (*qhl)->InsertCtrl((*qwe), 0);
            }

        };
        text = L"插入字段";
        createARCtrl(&m_AddTButton,&m_AddTTedit,&m_AddTIedit,&m_AddTWedit,&m_AddTqhl, text,rfAttribute::BtOnClick);
        text = L"删除字段";
        createARCtrl(&m_RMTButton, nullptr, &m_RMTIedit, nullptr, &m_RMTqhl, text, rfAttribute::BtOnClick);
        text = L"修改标题";
        createARCtrl(&m_UDTButton, &m_UDTTedit, &m_UDTIedit1, NULL, &m_UDTTqhl, text, rfAttribute::BtOnClick);
        text = L"修改宽度";
        createARCtrl(&m_UDTWButton, NULL, &m_UDTIedit2, &m_UDTWedit, &m_UDTWqhl, text, rfAttribute::BtOnClick);
        text = L"是否可拖拉:";
        m_Dragqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 100, 100));
        m_Dragqst->SetBKColor(0xfff0f0f0);
        m_Dragqst->SetText(text.c_str());

        m_DragQS = (QSwitch*)QFUSE::CreateQWND("QFSwitch", QRect(0, 0, 50, 100));
        m_DragQS->RecordAttribute(0, (long long)this);
        m_DragQS->SetStateChangeEvent(DragChange);
        m_Dragqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_Dragqhl->InsertCtrl(m_Dragqst, 0);
        m_Dragqhl->InsertCtrl(m_DragQS, 1);

        //提示操作布局
        auto createTCtr = [this](QStaticText** qst1,const wchar_t* t1, QStaticText** qst2, const wchar_t* t2, QStaticText** qst3, const wchar_t* t3, QStaticText** qst4, const wchar_t* t4, QHLayout** qhl)
        {

            *qst1 = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 50, 100));
            (*qst1)->SetBKColor(0xffa0f0f0);
            (*qst1)->SetText(t1);

            *qst2 = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 50, 100));
            (*qst2)->SetBKColor(0xfff0a0f0);
            (*qst2)->SetText(t2);

            *qst3 = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 50, 100));
            (*qst3)->SetBKColor(0xfff0f0a0);
            (*qst3)->SetText(t3);

            *qst4 = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 50, 100));
            (*qst4)->SetBKColor(0xfff0a0a0);
            (*qst4)->SetText(t4);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));

            (*qhl)->InsertCtrl((*qst4), 0);
            (*qhl)->InsertCtrl((*qst1), 0);
            (*qhl)->InsertCtrl((*qst2), 1);
            (*qhl)->InsertCtrl((*qst3), 0);

        };
        createTCtr(&m_TC1qst,L"索引",&m_TC2qst,L"设置值",&m_TC3qst,L"宽度",&m_TC4qst,L"操作", &m_TCqhl);

    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QReportForm ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("fontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
            else if ("TitleHeight" == key) {
                m_THedit->SetText(stringToWString(value).c_str());
            }
            else if ("IsDrag" == key) {
                if ("true" == value) {
                    m_DragQS->SetState(1);
                }
                else {
                    m_DragQS->SetState(0);
                }
                
            }

        }
        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {
        m_DragQS->SetState(0);
        m_FNedit->SetText(L"");
        m_FSedit->SetText(L"");
        m_THedit->SetText(L"");
        m_AddTIedit->SetText(L"-1");
        //m_AddTTedit->SetText(L"");
        m_AddTWedit->SetText(L"80");
 /*       m_RMTTedit->SetText(L"");*/
        m_RMTIedit->SetText(L"0");
        m_UDTIedit1->SetText(L"0");
        m_UDTIedit2->SetText(L"0");
        //m_UDTTedit->SetText(L"");
        //m_UDTWedit->SetText(L"80");
    }

    static int FNChange(int p, int pt, int param)
    {
        rfAttribute* pba = (rfAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QReportForm ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontName((wchar_t*)pt);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontName"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FSChange(int p, int pt, int param)
    {
        rfAttribute* pba = (rfAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QReportForm ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontSize(std::strtod(wstring_to_string((wchar_t*)pt).c_str(), NULL));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontSize"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int THChange(int p, int pt, int param)
    {
        rfAttribute* pba = (rfAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QReportForm ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetTitleHeight(std::strtol(wstring_to_string(std::wstring((wchar_t*)pt)).c_str(),nullptr,10));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("TitleHeight"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;


        }
     
        return 0;
    }
    static void BtOnClick(void* addr)
    {
        extern rfAttribute* g_prfa;
        if (addr == g_prfa->m_AddTButton->qwm) {
            //增加一条字段

            std::wstring sindex = g_prfa->m_AddTIedit->GetText();
            std::wstring sText = g_prfa->m_AddTTedit->GetText();
            std::wstring sWidth = g_prfa->m_AddTWedit->GetText();

            int width = std::strtol(wstring_to_string(sWidth).c_str(),nullptr,10);
            int index = std::strtol(wstring_to_string(sindex).c_str(), nullptr, 10);
            if (width < 1)return;
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = g_prfa->ctrl;
            QReportForm ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.InsertTitle(sText, width, index);


            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;

        }
        else if (addr == g_prfa->m_RMTButton->qwm) {
            //删除指定的记录
            std::wstring sindex = g_prfa->m_RMTIedit->GetText();
            int index = std::strtol(wstring_to_string(sindex).c_str(), nullptr, 10);
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = g_prfa->ctrl;
            QReportForm ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.RemovTitle(index);


            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        else if (addr == g_prfa->m_UDTButton->qwm) {
            //修改字段字符
            std::wstring sindex = g_prfa->m_UDTIedit1->GetText();
            std::wstring sText = g_prfa->m_UDTTedit->GetText();

            int index = std::strtol(wstring_to_string(sindex).c_str(), nullptr, 10);
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = g_prfa->ctrl;
            QReportForm ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            //ctrl.set(sText, width, index);
            //std::wstring ttext = ctrl.GetTitleText(index);
            int width = ctrl.GetTitleWidth(index);
            ctrl.SetTitle(sText, width, index);

            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;
        }
        else if (addr == g_prfa->m_UDTWButton->qwm) {
            //修改字段宽度

            std::wstring sindex = g_prfa->m_UDTIedit2->GetText();
            std::wstring sWidth = g_prfa->m_UDTWedit->GetText();

            int width = std::strtol(wstring_to_string(sWidth).c_str(), nullptr, 10);
            int index = std::strtol(wstring_to_string(sindex).c_str(), nullptr, 10);
            if (width < 1)return;
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = g_prfa->ctrl;
            QReportForm ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring ttext = ctrl.GetTitleText(index);
            ctrl.SetTitle(ttext, width, index);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;
        
        }

    
    }
    static int DragChange(QPOINTER ct)
    {
        QSwitch qcb(nullptr, QRect(0, 0, 10, 10));
        qcb.qwm = (QSwitch*)ct;

        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            rfAttribute* p = (rfAttribute*)data;
            if (p) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QReportForm ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

                std::string IsDrag = "true";
                bool isd = true;
                if (1 != qcb.GetState()) {
                    IsDrag = "false";
                    isd = false;
                }
                ctrl.IsIsDrag(isd);
                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("IsDrag"), DWORD(IsDrag.c_str()), 0);
                pqh.qwm = nullptr;
            }

        }


        qcb.qwm = nullptr;
        return 1;
    
    }

};
//QFPictureBox属性
class picAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_TSqhl = nullptr;
    QHLayout* m_Pathqhl = nullptr;
 
    QStaticText* m_TSqst = nullptr;//透明度设置标签
    QStaticText* m_Pathqst = nullptr;//路径设置标签
    


    //QEdit* m_TSedit = nullptr;
    QEdit* m_Pathedit = nullptr;

    QButton* m_PathBt = nullptr;

    QSlider* m_TSSd = nullptr;

    picAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
       


     

        m_TSqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 96, 100));
        m_TSqst->SetBKColor(0xfff0f0f0);
        m_TSqst->SetText(L"透明度");
        m_TSSd = (QSlider*)QFUSE::CreateQWND("QFSlider", QRect(0, 0, 50, 100));
        m_TSSd->SetValueChangeEvent(TSChange);
        m_TSSd->RecordAttribute(0,(long long)this);
       
        m_TSqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_TSqhl->InsertCtrl(m_TSqst, 0);
        m_TSqhl->InsertCtrl(m_TSSd, 1);

        m_Pathqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 96, 100));
        m_Pathqst->SetBKColor(0xfff0f0f0);
        m_Pathqst->SetText(L"路径");
        m_Pathedit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
        //m_Pathedit->SetChangeEvent(TSChange, (int)this);
        m_PathBt = (QButton*)QFUSE::CreateQWND("QFButton", QRect(0, 0, 24, 100));
        m_PathBt->SetFontSize(8);
        m_PathBt->SetText(L"...");
        m_PathBt->SetClickEvent(BtOnClick);

        m_Pathqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_Pathqhl->InsertCtrl(m_Pathqst, 0);
        m_Pathqhl->InsertCtrl(m_Pathedit, 1);
        m_Pathqhl->InsertCtrl(m_PathBt, 0);


    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QPictureBox ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        m_TSSd->SetCurrentValue(100);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("Transparency" == key) {

                //m_TSedit->SetText(stringToWString(value).c_str());
                float ato = std::strtod(v, nullptr);
                m_TSSd->SetCurrentValue(ato*100);
            }
            else if ("path" == key) {
                m_Pathedit->SetText(stringToWString(value).c_str());
            }

        }
   

        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {
        //m_TSedit->SetText(L"");
        m_Pathedit->SetText(L"");

    }

    static int TSChange(QPOINTER ct, float v)
    {
        QSlider qcb(nullptr, QRect(0, 0, 10, 10));
        qcb.qwm = (QWND*)ct;

        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            picAttribute* p = (picAttribute*)data;
            if (p) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QPictureBox ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                int t = v/100 * 255;
                ctrl.SetTransparency(t&0xff);
                


                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("Transparency"), DWORD(std::format("{:f}", v/100).c_str()), 0);
                pqh.qwm = nullptr;


            }

        }
        qcb.qwm = nullptr;
        return 1;
    }

    //static int TSChange(int p, int pt, int param)
    //{
    //    picAttribute* pba = (picAttribute*)param;
    //    if (pba->ctrl) {
    //        QHost pqh(nullptr, QRect(0, 0, 10, 10));
    //        pqh.qwm = pba->ctrl;
    //        QPictureBox ctrl(nullptr, QRect(0, 0, 10, 10));
    //        ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
    //        std::wstring text((wchar_t*)pt);
    //        std::string str = wstring_to_string(text);
    //        float ato = std::strtod(str.c_str(), nullptr);
    //        int tp = ato * 255;
    //        ctrl.SetTransparency(tp&0xff);

    //        ctrl.qwm = nullptr;
    //        pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("Transparency"), DWORD(str.c_str()), 0);
    //        pqh.qwm = nullptr;

    //    }
    //    return 0;
    //}
    static void BtOnClick(void* param)
    {
        extern picAttribute* g_pica;
        QWND* ctrl = (QWND*)param;
        if (ctrl == g_pica->m_PathBt->qwm) {
            //打开文件
            // 初始化OPENFILENAME结构体
            OPENFILENAMEA ofn;
            char szFile[MAX_PATH] = { 0 }; // 用于存储文件路径

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "PNG Files\0*.png\0JPG Files\0*.jpg\0位图 Files\0*.bmp\0All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            // 显示文件打开对话框
            if (GetOpenFileNameA(&ofn) == TRUE) {
                QPictureBox pcib(nullptr,QRect());
                QHost host(nullptr, QRect());
                host.qwm = g_pica->ctrl;
                pcib.qwm = host.GetParasiticCtrl();
                std::string path = ofn.lpstrFile;
                std::wstring wpath = stringToWString(path);
                pcib.LoadPicture(wpath);
                host.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("path"), DWORD(path.c_str()), 0);
                g_pica->m_Pathedit->SetText(wpath.c_str());
                host.qwm = nullptr;
                pcib.qwm = nullptr;
            }
            
        }
    
    }
};
//QFProgressBarCar属性
class pbAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_FNqhl = nullptr;
    QHLayout* m_FSqhl = nullptr;
    QHLayout* m_FCqhl = nullptr;
    QHLayout* m_POSqhl = nullptr;




    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_FCqst = nullptr;
    QStaticText* m_POSqst = nullptr;


    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;
    //QEdit* m_FCedit = nullptr;
    //QEdit* m_POSedit = nullptr;

    QColorBox* m_FCCB = nullptr;
    QSlider* m_POSSD = nullptr;


    pbAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring text(L"posit:");
        //createCtr(&m_POSqst, &m_POSedit, &m_POSqhl, text, positChange);
        m_POSqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
        m_POSqst->SetBKColor(0xfff0f0f0);
        m_POSqst->SetText(text.c_str());

        m_POSSD = (QSlider*)QFUSE::CreateQWND("QFSlider", QRect(0, 0, 50, 100));
        m_POSSD->SetValueChangeEvent(POSChange);
        m_POSSD->RecordAttribute(0, (long long)this);
   
        m_POSqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_POSqhl->InsertCtrl(m_POSqst, 0);
        m_POSqhl->InsertCtrl(m_POSSD, 1);
        
        text = L"fontName:";
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, text, FNChange);
        text = L"fontSize:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, text, FSChange);
        text = L"fontColor:";
        //createCtr(&m_FCqst, &m_FCedit, &m_FCqhl, text, FCChange);
        m_FCqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
        m_FCqst->SetBKColor(0xfff0f0f0);
        m_FCqst->SetText(text.c_str());
        m_FCCB = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 100));
        m_FCCB->SetColorChangeEvent(CBChange);
        m_FCCB->RecordAttribute(0, (long long)this);
        m_FCCB->SetFontSize(16);
        m_FCqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_FCqhl->InsertCtrl(m_FCqst, 0);
        m_FCqhl->InsertCtrl(m_FCCB, 1);

    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QProgressBarCar ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("fontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontColor" == key) {
                //m_FCedit->SetText(stringToWString(value).c_str());
                int para = 10;
                if ('x' == std::tolower(v[1]))para = 16;
                unsigned long color = std::strtoul(v, nullptr, para);
                m_FCCB->SetColor(color);

            }

        }
        double percent = ctrl.GetPercentage();
        m_POSSD->SetCurrentValue(percent);
  /*      if(percent == (int)percent)
            m_POSedit->SetText(std::to_wstring((int)percent).c_str());
        else
            m_POSedit->SetText(std::to_wstring(percent).c_str());*/

        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {

        m_FNedit->SetText(L"");
        m_FSedit->SetText(L"");
        //m_FCedit->SetText(L"");
        m_FCCB->SetColor(0xff000000);
        //m_POSedit->SetText(L"");
        m_POSSD->SetCurrentValue(50);
    }

    static int FNChange(int p, int pt, int param)
    {
        pbAttribute* pba = (pbAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QProgressBarCar ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontName((wchar_t*)pt);
            ctrl.TransferMessage(QM_UPDATE_CTRL,1,0,0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontName"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FCChange(int p, int pt, int param)
    {
        pbAttribute* pba = (pbAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QProgressBarCar ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr((wchar_t*)pt);
            std::string str = wstring_to_string(wstr);
            unsigned int color = std::strtoul(str.c_str(), NULL,16);
            ctrl.SetFontColor(color);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontColor"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FSChange(int p, int pt, int param)
    {
        pbAttribute* pba = (pbAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QProgressBarCar ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontSize(std::strtod(wstring_to_string((wchar_t*)pt).c_str(), NULL));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontSize"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int positChange(int p, int pt, int param)
    {
        pbAttribute* pba = (pbAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QProgressBarCar ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

            std::wstring wstr((wchar_t*)pt);
            std::string str = wstring_to_string(wstr);
            double value = std::strtod(str.c_str(),NULL);
            ctrl.SetPercentage(value);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("posit"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }

    static int CBChange(QPOINTER ct, unsigned int v)
    {
        QColorBox cb(nullptr, QRect(0, 0, 10, 10));
        cb.qwm = (QWND*)ct;

        if (ct) {
            QDWORD data = 0;
            cb.FindAttribute(0, &data);
            pbAttribute* p = (pbAttribute*)data;
            if (p) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QProgressBarCar ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                
                ctrl.SetFontColor(v);
                


                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontColor"), DWORD(std::format("0x{:x}", v).c_str()), 0);
                pqh.qwm = nullptr;


            }

        }
        cb.qwm = nullptr;
        return 1;
    }
    static int POSChange(QPOINTER ct, float v)
    {
        QSlider qcb(nullptr, QRect(0, 0, 10, 10));
        qcb.qwm = (QWND*)ct;

        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            pbAttribute* p = (pbAttribute*)data;
            if (p) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QProgressBarCar ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                ctrl.SetPercentage(v);
               



                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("posit"), DWORD(std::format("{:f}", v / 100).c_str()), 0);
                pqh.qwm = nullptr;


            }

        }
        qcb.qwm = nullptr;
        return 1;
    }

};
//QFLineChart属性
class lcAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_FNqhl = nullptr;
    QHLayout* m_FSqhl = nullptr;
    QHLayout* m_LFNqhl = nullptr;//图例字体名字
    QHLayout* m_LFSqhl = nullptr;//图例字体大小
    QHLayout* m_XASHqhl = nullptr;//XAxisStrHeight
    QHLayout* m_YASHqhl = nullptr;//YAxisStrWidth
    QHLayout* m_XASWqhl = nullptr;//XAxisScaleWidth
    QHLayout* m_YASWqhl = nullptr;//YAxisScaleWidth
    QHLayout* m_LWqhl = nullptr;//LineWidth
    QHLayout* m_Tipqhl = nullptr;//输入提示
    QHLayout* m_XARqhl = nullptr;//XAxisRange
    QHLayout* m_YARqhl = nullptr;//YAxisRange
    QHLayout* m_LEGWqhl = nullptr;//LegWidth

    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_LFNqst = nullptr;//图例字体名字
    QStaticText* m_LFSqst = nullptr;//图例字体大小
    QStaticText* m_XASHqst = nullptr;//XAxisStrHeight
    QStaticText* m_YASHqst = nullptr;//YAxisStrWidth
    QStaticText* m_XASWqst = nullptr;//XAxisScaleWidth
    QStaticText* m_YASWqst = nullptr;//YAxisScaleWidth
    QStaticText* m_LWqst = nullptr;//LineWidth
    QStaticText* m_XARqst = nullptr;//XAxisRange
    QStaticText* m_YARqst = nullptr;//YAxisRange
    QStaticText* m_LEGWqst = nullptr;//LegWidth
    QStaticText* m_Tipqst1 = nullptr;//输入提示
    QStaticText* m_Tipqst2 = nullptr;//输入提示

    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;
    QEdit* m_LFNedit = nullptr;//图例字体名字
    QEdit* m_LFSedit = nullptr;//图例字体大小
    QEdit* m_XASHedit = nullptr;//XAxisStrHeight
    QEdit* m_YASHedit = nullptr;//YAxisStrWidth
    QEdit* m_XASWedit = nullptr;//XAxisScaleWidth
    QEdit* m_YASWedit = nullptr;//YAxisScaleWidth
    QEdit* m_LWedit = nullptr;//LineWidth
    QEdit* m_XARedit1 = nullptr;//XAxisRange
    QEdit* m_XARedit2 = nullptr;//XAxisRange
    QEdit* m_YARedit1 = nullptr;//YAxisRange
    QEdit* m_YARedit2 = nullptr;//YAxisRange
    QEdit* m_LEGWedit = nullptr;//LegWidth



    lcAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 100, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
  

        std::wstring test = L"fontName:";
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, test, FNChange);
        test = L"fontSize:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, test, FSChange);
        test=L"legFontName:";
        createCtr(&m_LFNqst, &m_LFNedit, &m_LFNqhl, test, LFNChange);
        test = L"legfontSize:";
        createCtr(&m_LFSqst, &m_LFSedit, &m_LFSqhl, test, LFSChange);
        test = L"X轴字符高度:";
        createCtr(&m_XASHqst, &m_XASHedit, &m_XASHqhl, test, XASHChange);
        test = L"Y轴字符宽度:";
        createCtr(&m_YASHqst, &m_YASHedit, &m_YASHqhl, test, YASHChange);
        test = L"X轴刻度宽度:";
        createCtr(&m_XASWqst, &m_XASWedit, &m_XASWqhl, test, XASWChange);
        test = L"Y轴刻度宽度:";
        createCtr(&m_YASWqst, &m_YASWedit, &m_YASWqhl, test, YASWChange);
        test = L"线宽:";
        createCtr(&m_LWqst, &m_LWedit, &m_LWqhl, test, LWChange);
        test = L"图例宽度:";
        createCtr(&m_LEGWqst, &m_LEGWedit, &m_LEGWqhl, test, LEGWChange);
        //提示
        m_Tipqst1 = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 100, 100));
        m_Tipqst1->SetBKColor(0xfff0f0f0);
        m_Tipqst1->SetText(L"开始范围");
        m_Tipqst2 = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 100, 100));
        m_Tipqst2->SetBKColor(0xfff0f0f0);
        m_Tipqst2->SetText(L"结束范围");

        m_Tipqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_Tipqhl->InsertCtrl(m_Tipqst1, 1);
        m_Tipqhl->InsertCtrl(m_Tipqst2, 1);
        //X范围
        m_XARqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 100, 100));
        m_XARqst->SetBKColor(0xfff0f0f0);
        m_XARqst->SetText(L"X轴范围");

        m_XARedit1 = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
        m_XARedit1->SetChangeEvent(XRChange, (int)this);
        m_XARedit2 = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
        m_XARedit2->SetChangeEvent(XRChange, (int)this);
        m_XARqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_XARqhl->InsertCtrl(m_XARqst, 0);
        m_XARqhl->InsertCtrl(m_XARedit1, 1);
        m_XARqhl->InsertCtrl(m_XARedit2, 1);
        //Y范围
        m_YARqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 100, 100));
        m_YARqst->SetBKColor(0xfff0f0f0);
        m_YARqst->SetText(L"Y轴范围");

        m_YARedit1 = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
        m_YARedit1->SetChangeEvent(YRChange, (int)this);
        m_YARedit2 = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
        m_YARedit2->SetChangeEvent(YRChange, (int)this);
        m_YARqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_YARqhl->InsertCtrl(m_YARqst, 0);
        m_YARqhl->InsertCtrl(m_YARedit1, 1);
        m_YARqhl->InsertCtrl(m_YARedit2, 1);
    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QEdit ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("fontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
            else if ("legfontName" == key) {
                m_LFNedit->SetText(stringToWString(value).c_str());
            }
            else if ("legfontSize" == key) {
                m_LFSedit->SetText(stringToWString(value).c_str());
            }
            else if ("XAxisStrHeight" == key) {
                m_XASHedit->SetText(stringToWString(value).c_str());
            }
            else if ("YAxisStrWidth" == key) {
                m_YASHedit->SetText(stringToWString(value).c_str());
            }
            else if ("XAxisScaleWidth" == key) {
                m_XASWedit->SetText(stringToWString(value).c_str());
            }else if ("YAxisScaleWidth" == key) {
                m_YASWedit->SetText(stringToWString(value).c_str());
            }
            else if ("LineWidth" == key) {
                m_LWedit->SetText(stringToWString(value).c_str());
            }
            else if ("LegWidth" == key) {
                m_LEGWedit->SetText(stringToWString(value).c_str());
            }
            else if ("XAxisRange" == key) {
                std::string str = value;
                QPointF point=QFUSE::XML2Point(str);
                m_XARedit1->SetText(std::to_wstring(point.X).c_str());
                m_XARedit2->SetText(std::to_wstring(point.Y).c_str());
                /*std::wstring wstr = stringToWString(value);*/

               
            }
            else if ("YAxisRange" == key) {
                std::string str = value;
                QPointF point = QFUSE::XML2Point(str);
                m_YARedit1->SetText(std::to_wstring(point.X).c_str());
                m_YARedit2->SetText(std::to_wstring(point.Y).c_str());
            }

        }
      


        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {

        m_FNedit->SetText(L"");
        m_FSedit->SetText(L"");
      
    }

    static int FNChange(int p, int pt, int param)
    {
        lcAttribute* pba = (lcAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QLineChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr = (wchar_t*)pt;
            std::string str = wstring_to_string(wstr);

            ctrl.SetFontName(wstr.c_str());
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontName"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }

    static int FSChange(int p, int pt, int param)
    {
        lcAttribute* pba = (lcAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QLineChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr = (wchar_t*)pt;
            std::string str = wstring_to_string(wstr);
            ctrl.SetFontSize(std::strtod(str.c_str(), NULL));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontSize"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int LFNChange(int p, int pt, int param)
    {
        lcAttribute* pba = (lcAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QLineChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr = (wchar_t*)pt;
            std::string str = wstring_to_string(wstr);
            ctrl.SetLegFontName(wstr.c_str());
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("legfontName"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int LFSChange(int p, int pt, int param)
    {
        lcAttribute* pba = (lcAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QLineChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr = (wchar_t*)pt;
            std::string str = wstring_to_string(wstr);
            ctrl.SetLegFontSize(std::strtod(str.c_str(), NULL));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("legfontSize"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int XASHChange(int p, int pt, int param)
    {
        lcAttribute* pba = (lcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QLineChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr = (wchar_t*)pt;
            std::string str = wstring_to_string(wstr);
            ctrl.SetXAxisStrHeight(std::strtod(str.c_str(),nullptr));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("XAxisStrHeight"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int YASHChange(int p, int pt, int param)
    {
        lcAttribute* pba = (lcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QLineChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr = (wchar_t*)pt;
            std::string str = wstring_to_string(wstr);
            ctrl.SetYAxisStrWidth(std::strtod(str.c_str(), nullptr));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("YAxisStrWidth"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int XASWChange(int p, int pt, int param)
    {
        lcAttribute* pba = (lcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QLineChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr = (wchar_t*)pt;
            std::string str = wstring_to_string(wstr);
            ctrl.SetXAxisScaleWidth(std::strtod(str.c_str(), nullptr));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("XAxisScaleWidth"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int YASWChange(int p, int pt, int param)
    {
        lcAttribute* pba = (lcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QLineChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr = (wchar_t*)pt;
            std::string str = wstring_to_string(wstr);
            ctrl.SetYAxisScaleWidth(std::strtod(str.c_str(), nullptr));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("YAxisScaleWidth"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int LWChange(int p, int pt, int param)
    {
        lcAttribute* pba = (lcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QLineChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr = (wchar_t*)pt;
            std::string str = wstring_to_string(wstr);
            ctrl.SetLineWidth(std::strtod(str.c_str(), nullptr));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("LineWidth"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int XRChange(int p, int pt, int param)
    {
        lcAttribute* pba = (lcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QLineChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr1 = pba->m_XARedit1->GetText();
            std::wstring wstr2 = pba->m_XARedit2->GetText();
            std::string str1 = wstring_to_string(wstr1);
            std::string str2 = wstring_to_string(wstr2);
            double v1 = std::strtod(str1.c_str(), nullptr);
            double v2 = std::strtod(str2.c_str(), nullptr);
            ctrl.SetXAxisRange(v1,v2);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            std::wstring AxisRange=std::format(L"({},{})",std::to_wstring(v1), std::to_wstring(v2));
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("XAxisRange"), DWORD(wstring_to_string(AxisRange).c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int YRChange(int p, int pt, int param)
    {
        lcAttribute* pba = (lcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QLineChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr1 = pba->m_YARedit1->GetText();
            std::wstring wstr2 = pba->m_YARedit2->GetText();
            std::string str1 = wstring_to_string(wstr1);
            std::string str2 = wstring_to_string(wstr2);
            double v1 = std::strtod(str1.c_str(), nullptr);
            double v2 = std::strtod(str2.c_str(), nullptr);
            ctrl.SetYAxisRange(v1, v2);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            std::wstring AxisRange = std::format(L"({},{})", std::to_wstring(v1), std::to_wstring(v2));
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("YAxisRange"), DWORD(wstring_to_string(AxisRange).c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int LEGWChange(int p, int pt, int param)
    {
        lcAttribute* pba = (lcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QLineChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr = (wchar_t*)pt;
            std::string str = wstring_to_string(wstr);
            ctrl.SetLegWidth(std::strtod(str.c_str(), nullptr));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("LegWidth"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
};
//QFTabs属性
class tabeAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_FNqhl = nullptr;
    QHLayout* m_FSqhl = nullptr;
    QHLayout* m_FCqhl = nullptr;
    QHLayout* m_TXTqhl = nullptr;
    QHLayout* m_ICOqhl = nullptr;
    QHLayout* m_TIPqhl = nullptr;
    QHLayout* m_Addqhl = nullptr;
    QHLayout* m_RMqhl = nullptr;

    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_FCqst = nullptr;
    QStaticText* m_TXTqst = nullptr;
    QStaticText* m_ICOqst = nullptr;
    QStaticText* m_TIPqst1 = nullptr;
    QStaticText* m_TIPqst2 = nullptr;
    QStaticText* m_TIPqst3 = nullptr;


    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;
    QEdit* m_FCedit = nullptr;
    QEdit* m_TXTedit = nullptr;
    QEdit* m_ICOedit = nullptr;
    QEdit* m_AddIedit = nullptr;
    QEdit* m_AddTedit = nullptr;
    QEdit* m_RMIedit = nullptr;

    QButton* m_ICObt = nullptr;
    QButton* m_Addbt = nullptr;
    QButton* m_RMbt = nullptr;
    tabeAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 96, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring test(L"fontColor:");
        createCtr(&m_FCqst, &m_FCedit, &m_FCqhl, test, FCChange);

        test = L"fontName:";
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, test, FNChange);
        test = L"fontSize:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, test, FSChange);

        test = L"标题:";
        createCtr(&m_TXTqst, &m_TXTedit, &m_TXTqhl, test, TXTChange);
        test = L"ICO:";
        createCtr(&m_ICOqst, &m_ICOedit, &m_ICOqhl, test, ICOChange);
        m_ICObt = (QButton*)QFUSE::CreateQWND("QFButton", QRect(0, 0, 32, 100));
        m_ICObt->SetFontSize(8);
        m_ICObt->SetText(L"...");
        m_ICObt->SetClickEvent(BtOnClick);
        m_ICOqhl->InsertCtrl(m_ICObt, 0);

        //增减提示
        float fs = 10.0;
        m_TIPqst1 = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 50, 100));
        m_TIPqst1->SetBKColor(0xfff0f0f0);
        m_TIPqst1->SetText(L"操作");
        m_TIPqst1->SetFontSize(fs);

        m_TIPqst2 = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 50, 100));
        m_TIPqst2->SetBKColor(0xfff0f0f0);
        m_TIPqst2->SetText(L"索引");
        m_TIPqst2->SetFontSize(fs);

        m_TIPqst3 = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 96, 100));
        m_TIPqst3->SetBKColor(0xfff0f0f0);
        m_TIPqst3->SetText(L"内容");
        m_TIPqst3->SetFontSize(fs);
        m_TIPqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_TIPqhl->InsertCtrl(m_TIPqst1, 0);
        m_TIPqhl->InsertCtrl(m_TIPqst2, 0);
        m_TIPqhl->InsertCtrl(m_TIPqst3, 1);
        //增按钮
        m_Addbt = (QButton*)QFUSE::CreateQWND("QFButton", QRect(0, 0, 50, 32));
        m_Addbt->SetFontSize(8);
        m_Addbt->SetText(L"增加分页");
        m_Addbt->SetClickEvent(BtOnClick);
        m_AddIedit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 32));
        m_AddTedit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 32));
        m_Addqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_Addqhl->InsertCtrl(m_Addbt, 0);
        m_Addqhl->InsertCtrl(m_AddIedit, 0);
        m_Addqhl->InsertCtrl(m_AddTedit, 1);
        //减按钮
        m_RMbt = (QButton*)QFUSE::CreateQWND("QFButton", QRect(0, 0, 50, 32));
        m_RMbt->SetFontSize(8);
        m_RMbt->SetText(L"移除分页");
        m_RMbt->SetClickEvent(BtOnClick);
        m_RMIedit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 32));
        m_RMqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_RMqhl->InsertCtrl(m_RMbt, 0);
        m_RMqhl->InsertCtrl(m_RMIedit, 0);

    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QTabs ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("fontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontColor" == key) {
                m_FCedit->SetText(stringToWString(value).c_str());
            }
        }
        //当前选中的tabe标题了，ico
         m_TXTedit->SetText(ctrl.GetCurrentTabeText().c_str());
         m_ICOedit->SetText(ctrl.GetCurrentTabeIcoPath().c_str());

        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {

        m_FNedit->SetText(L"");
        m_FSedit->SetText(L"");
        m_FCedit->SetText(L"");
        m_TXTedit->SetText(L"");
        m_ICOedit->SetText(L"");
        m_AddIedit->SetText(L"-1");
        m_AddTedit->SetText(L"");
        m_RMIedit->SetText(L"0");
  
    }

    static int FNChange(int p, int pt, int param)
    {
        tabeAttribute* pba = (tabeAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QTabs ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontName((wchar_t*)pt);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontName"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FSChange(int p, int pt, int param)
    {
        tabeAttribute* pba = (tabeAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QTabs ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontSize(std::strtod(wstring_to_string((wchar_t*)pt).c_str(), NULL));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontSize"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FCChange(int p, int pt, int param)
    {
        tabeAttribute* pba = (tabeAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QTabs ctrl(nullptr, QRect(0, 0, 10, 10));
            std::string str = wstring_to_string((wchar_t*)pt);
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontColor(std::strtoul(str.c_str(),nullptr,16));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontColor"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int TXTChange(int p, int pt, int param)
    {
        tabeAttribute* pba = (tabeAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QTabs ctrl(nullptr, QRect(0, 0, 10, 10));
            std::string str = wstring_to_string((wchar_t*)pt);
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetCurrentTabeText((wchar_t*)pt);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int ICOChange(int p, int pt, int param)
    {
        tabeAttribute* pba = (tabeAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QTabs ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetCurrentTabeIcoPath((wchar_t*)pt);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static void BtOnClick(void* param)
    {
        extern tabeAttribute* g_ptabea;
        QWND* ctrl = (QWND*)param;
        if (ctrl == g_ptabea->m_ICObt->qwm) {
            //打开文件
            // 初始化OPENFILENAME结构体
            OPENFILENAMEA ofn;
            char szFile[MAX_PATH] = { 0 }; // 用于存储文件路径

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "PNG Files\0*.png\0JPG Files\0*.jpg\0位图 Files\0*.bmp\0All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            // 显示文件打开对话框
            if (GetOpenFileNameA(&ofn) == TRUE) {
                QTabs ctrl(nullptr, QRect());
                QHost host(nullptr, QRect());
                host.qwm = g_ptabea->ctrl;
                ctrl.qwm = host.GetParasiticCtrl();
                std::string path = ofn.lpstrFile;
                std::wstring wpath = stringToWString(path);
                ctrl.SetCurrentTabeIcoPath(wpath.c_str());
                ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                g_ptabea->m_ICOedit->SetText(wpath.c_str());
            
                host.qwm = nullptr;
                ctrl.qwm = nullptr;
            }

        }
        else if (ctrl == g_ptabea->m_Addbt->qwm) {

            QTabs ctrl(nullptr, QRect());
            QHost host(nullptr, QRect());
            host.qwm = g_ptabea->ctrl;
            ctrl.qwm = host.GetParasiticCtrl();
            
            std::wstring wt = g_ptabea->m_AddTedit->GetText();
            std::wstring wi = g_ptabea->m_AddIedit->GetText();
            std::string si = wstring_to_string(wi);
            int index = std::strtol(si.c_str(),nullptr,10);
            ctrl.InsertTab(wt.c_str(),index);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);

            host.qwm = nullptr;
            ctrl.qwm = nullptr;
        }
        else if (ctrl == g_ptabea->m_RMbt->qwm) {//移除tabe

            QTabs ctrl(nullptr, QRect());
            QHost host(nullptr, QRect());
            host.qwm = g_ptabea->ctrl;
            ctrl.qwm = host.GetParasiticCtrl();
            std::wstring wi = g_ptabea->m_RMIedit->GetText();
            std::string si = wstring_to_string(wi);
            int index = std::strtol(si.c_str(), nullptr, 10);
            ctrl.RemoveTab( index);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);

            host.qwm = nullptr;
            ctrl.qwm = nullptr;
        }


    }
};
//QFCATLBarChart属性
class catlbcAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_FNqhl = nullptr;
    QHLayout* m_FSqhl = nullptr;
    QHLayout* m_FCqhl = nullptr;
    QHLayout* m_SMAXqhl = nullptr;
    QHLayout* m_SMINqhl = nullptr;
    QHLayout* m_SWqhl = nullptr;
    QHLayout* m_SHqhl = nullptr;
    QHLayout* m_BCWqhl = nullptr;
    QHLayout* m_GWqhl = nullptr;
    QHLayout* m_LWqhl = nullptr;
    QHLayout* m_YASWqhl = nullptr;


    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_FCqst = nullptr;
    QStaticText* m_SMAXqst = nullptr;
    QStaticText* m_SMINqst = nullptr;
    QStaticText* m_SWqst = nullptr;
    QStaticText* m_SHqst = nullptr;
    QStaticText* m_BCWqst = nullptr;
    QStaticText* m_GWqst = nullptr;
    QStaticText* m_LWqst = nullptr;
    QStaticText* m_YASWqst = nullptr;


    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;
    //QEdit* m_FCedit = nullptr;
    QEdit* m_SMAXedit = nullptr;
    QEdit* m_SMINedit = nullptr;
    QEdit* m_SWedit = nullptr;
    QEdit* m_SHedit = nullptr;
    QEdit* m_BCWedit = nullptr;
    QEdit* m_GWedit = nullptr;
    QEdit* m_LWedit = nullptr;
    QEdit* m_YASWedit = nullptr;

    QColorBox* m_SCColor = nullptr;

    catlbcAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 96, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring text(L"ScaleColor:");
        //createCtr(&m_FCqst, &m_FCedit, &m_FCqhl, test, FCChange);

        m_FCqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 96, 100));
        m_FCqst->SetBKColor(0xfff0f0f0);
        m_FCqst->SetText(text.c_str());

        m_SCColor = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 100));
        m_SCColor->RecordAttribute(0, (long long)this);
        m_SCColor->SetColorChangeEvent(scColorChange);
        m_SCColor->SetFontSize(16);

        m_FCqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_FCqhl->InsertCtrl(m_FCqst, 0);
        m_FCqhl->InsertCtrl(m_SCColor, 1);



        text = L"fontName:";
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, text, FNChange);
        text = L"fontSize:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, text, FSChange);

        text = L"坐标最大值:";
        createCtr(&m_SMAXqst, &m_SMAXedit, &m_SMAXqhl, text, SMAXChange);
        text = L"坐标最小值:";
        createCtr(&m_SMINqst, &m_SMINedit, &m_SMINqhl, text, SMINChange);
        text = L"Y轴字符宽度:";
        createCtr(&m_SWqst, &m_SWedit, &m_SWqhl, text, SWChange);
        text = L"X轴字符高度:";
        createCtr(&m_SHqst, &m_SHedit, &m_SHqhl, text, SHChange);
        text = L"柱体宽度:";
        createCtr(&m_BCWqst, &m_BCWedit, &m_BCWqhl, text, BCWChange);
        text = L"组间宽度:";
        createCtr(&m_BCWqst, &m_GWedit, &m_GWqhl, text, GWChange);
        text = L"线宽:";
        createCtr(&m_LWqst, &m_LWedit, &m_LWqhl, text, LWChange);
        text = L"Y轴刻度宽度:";
        createCtr(&m_YASWqst, &m_YASWedit, &m_YASWqhl, text, YASWChange);


    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QTabs ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("fontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
            else if ("ScaleColor" == key) {
                //m_FCedit->SetText(stringToWString(value).c_str());
                int para = 10;
                if ('x' == std::tolower(v[1]))para = 16;
                unsigned long color = std::strtoul(v, nullptr, para);
                m_SCColor->SetColor(color);
            }
            else if ("ScaleMax" == key) {
                m_SMAXedit->SetText(stringToWString(value).c_str());
            }
            else if ("ScaleMin" == key) {
                m_SMINedit->SetText(stringToWString(value).c_str());
            }
            else if ("ScaleWidth" == key) {
                m_SWedit->SetText(stringToWString(value).c_str());
            }
            else if ("ScaleHeight" == key) {
                m_SHedit->SetText(stringToWString(value).c_str());
            }
            else if ("BarChartWidth" == key) {
                m_BCWedit->SetText(stringToWString(value).c_str());
            }
            else if ("GridWidth" == key) {
                m_GWedit->SetText(stringToWString(value).c_str());
            }
            else if ("LineWidth" == key) {
                m_LWedit->SetText(stringToWString(value).c_str());
            }
            else if ("YAxisStrWidth" == key) {
                m_YASWedit->SetText(stringToWString(value).c_str());
            }
        }

        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {

        m_FNedit->SetText(L"");
        m_FSedit->SetText(L"");
        //m_FCedit->SetText(L"");
        m_SCColor->SetColor(0xff000000);
        m_SMAXedit->SetText(L"");
        m_SMINedit->SetText(L"");
        m_SWedit->SetText(L"");
        m_SHedit->SetText(L"");
        m_BCWedit->SetText(L"");
        m_GWedit->SetText(L"");
        m_LWedit->SetText(L"");
        m_YASWedit->SetText(L"");

    }
    static int scColorChange(QPOINTER ct, unsigned int color)
    {
        QColorBox qcb(nullptr, QRect(0, 0, 10, 10));
        qcb.qwm = (QWND*)ct;

        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            catlbcAttribute* p = (catlbcAttribute*)data;
            if (p) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QCATLBarChart ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                
                ctrl.SetScaleColor(color);
                ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("ScaleColor"), DWORD(std::format("0x{:x}", color).c_str()), 0);
                pqh.qwm = nullptr;

            }

        }


        qcb.qwm = nullptr;
        return 1;
    }
    static int FNChange(int p, int pt, int param)
    {
        catlbcAttribute* pba = (catlbcAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLBarChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontName((wchar_t*)pt);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontName"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FSChange(int p, int pt, int param)
    {
        catlbcAttribute* pba = (catlbcAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLBarChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontSize(std::strtod(wstring_to_string((wchar_t*)pt).c_str(), NULL));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontSize"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FCChange(int p, int pt, int param)
    {
        catlbcAttribute* pba = (catlbcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLBarChart ctrl(nullptr, QRect(0, 0, 10, 10));
            std::string str = wstring_to_string((wchar_t*)pt);
            unsigned long color = std::strtoul(str.c_str(), nullptr, 16);
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetScaleColor(color);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("ScaleColor"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int SMAXChange(int p, int pt, int param)
    {
        catlbcAttribute* pba = (catlbcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLBarChart ctrl(nullptr, QRect(0, 0, 10, 10));
            std::string str = wstring_to_string((wchar_t*)pt);
           
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            float fv = std::strtod(str.c_str(), nullptr);
            ctrl.SetScaleMax(fv);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("ScaleMax"), DWORD(str.c_str()), 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int SMINChange(int p, int pt, int param)
    {
        catlbcAttribute* pba = (catlbcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLBarChart ctrl(nullptr, QRect(0, 0, 10, 10));
            std::string str = wstring_to_string((wchar_t*)pt);

            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            float fv = std::strtod(str.c_str(), nullptr);
            ctrl.SetScaleMin(fv);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("ScaleMin"), DWORD(str.c_str()), 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int SWChange(int p, int pt, int param)
    {
        catlbcAttribute* pba = (catlbcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLBarChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::string str = wstring_to_string((wchar_t*)pt);
            float sw = std::strtod(str.c_str(), nullptr);
            ctrl.SetScaleWidth(sw);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("ScaleWidth"), DWORD(str.c_str()), 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int SHChange(int p, int pt, int param)
    {
        catlbcAttribute* pba = (catlbcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLBarChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::string str = wstring_to_string((wchar_t*)pt);
            float sh = std::strtod(str.c_str(), nullptr);
            ctrl.SetScaleHeight(sh);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("ScaleHeight"), DWORD(str.c_str()), 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int BCWChange(int p, int pt, int param)
    {
        catlbcAttribute* pba = (catlbcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLBarChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::string str = wstring_to_string((wchar_t*)pt);
            float bcw = std::strtod(str.c_str(), nullptr);
            ctrl.SetBarChartWidth(bcw);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("BarChartWidth"), DWORD(str.c_str()), 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int GWChange(int p, int pt, int param)
    {
        catlbcAttribute* pba = (catlbcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLBarChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::string str = wstring_to_string((wchar_t*)pt);
            float gw = std::strtod(str.c_str(), nullptr);
            ctrl.SetGridWidth(gw);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("GridWidth"), DWORD(str.c_str()), 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int LWChange(int p, int pt, int param)
    {
        catlbcAttribute* pba = (catlbcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLBarChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::string str = wstring_to_string((wchar_t*)pt);
            float lw = std::strtod(str.c_str(), nullptr);
            ctrl.SetLineWidth(lw);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("LineWidth"), DWORD(str.c_str()), 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int YASWChange(int p, int pt, int param)
    {
        catlbcAttribute* pba = (catlbcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLBarChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::string str = wstring_to_string((wchar_t*)pt);
            float yasw = std::strtod(str.c_str(), nullptr);
            ctrl.SetYAxisStrWidth(yasw);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("YAxisStrWidth"), DWORD(str.c_str()), 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        return 0;
    }
};
//QFCATLTempeChart属性
class catltcAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_FNqhl = nullptr;
    QHLayout* m_FSqhl = nullptr;
    QHLayout* m_UFSqhl = nullptr;
    QHLayout* m_UWqhl = nullptr;
    QHLayout* m_UIWqhl = nullptr;
    QHLayout* m_AWqhl = nullptr;



    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_UFSqst = nullptr;
    QStaticText* m_UWqst = nullptr;
    QStaticText* m_UIWqst = nullptr;
    QStaticText* m_AWqst = nullptr;



    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;
    QEdit* m_UFSedit = nullptr;
    QEdit* m_UWedit = nullptr;
    QEdit* m_UIWedit = nullptr;
    QEdit* m_AWedit = nullptr;




    catltcAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 120, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring test(L"温度单元字体大小:");
        createCtr(&m_UFSqst, &m_UFSedit, &m_UFSqhl, test, UFSChange);

        test = L"fontName:";
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, test, FNChange);
        test = L"fontSize:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, test, FSChange);

        test = L"单元宽度:";
        createCtr(&m_UWqst, &m_UWedit, &m_UWqhl, test, UWChange);
        test = L"单元间宽度:";
        createCtr(&m_UIWqst, &m_UIWedit, &m_UIWqhl, test, UIWChange);
        test = L"外围单元到边框距离:";
        createCtr(&m_AWqst, &m_AWedit, &m_AWqhl, test, AWChange);
        


    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QTabs ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("fontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
            else if ("UnitFontSize" == key) {
                m_UFSedit->SetText(stringToWString(value).c_str());
            }
            else if ("UnitWidth" == key) {
                m_UWedit->SetText(stringToWString(value).c_str());
            }
            else if ("UnitIntervalWidth" == key) {
                m_UIWedit->SetText(stringToWString(value).c_str());
            }
            else if ("AlignWidth" == key) {
                m_AWedit->SetText(stringToWString(value).c_str());
            }
          
        }

        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {

        m_FNedit->SetText(L"");
        m_FSedit->SetText(L"");

        m_UFSedit->SetText(L"");
        m_UWedit->SetText(L"");
        m_UIWedit->SetText(L"");
        m_AWedit->SetText(L"");

        
    }

    static int FNChange(int p, int pt, int param)
    {
        catltcAttribute* pba = (catltcAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLTempeChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontName((wchar_t*)pt);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontName"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FSChange(int p, int pt, int param)
    {
        catltcAttribute* pba = (catltcAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLTempeChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontSize(std::strtod(wstring_to_string((wchar_t*)pt).c_str(), NULL));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontSize"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int UFSChange(int p, int pt, int param)
    {
        catltcAttribute* pba = (catltcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLTempeChart ctrl(nullptr, QRect(0, 0, 10, 10));
            std::string str = wstring_to_string((wchar_t*)pt);
            double size = std::strtod(str.c_str(), nullptr);
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetUnitFontSize(size);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("UnitFontSize"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int UWChange(int p, int pt, int param)
    {
        catltcAttribute* pba = (catltcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLTempeChart ctrl(nullptr, QRect(0, 0, 10, 10));
            std::string str = wstring_to_string((wchar_t*)pt);

            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            float w = std::strtod(str.c_str(), nullptr);

            ctrl.SetUnitWidth(w);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("UnitWidth"), DWORD(str.c_str()), 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int UIWChange(int p, int pt, int param)
    {
        catltcAttribute* pba = (catltcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLTempeChart ctrl(nullptr, QRect(0, 0, 10, 10));
            std::string str = wstring_to_string((wchar_t*)pt);

            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            float w = std::strtod(str.c_str(), nullptr);
            ctrl.SetUnitIntervalWidth(w);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("UnitIntervalWidth"), DWORD(str.c_str()), 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int AWChange(int p, int pt, int param)
    {
        catltcAttribute* pba = (catltcAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QCATLTempeChart ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::string str = wstring_to_string((wchar_t*)pt);
            float aw = std::strtod(str.c_str(), nullptr);
            ctrl.SetAlignWidth(aw);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("AlignWidth"), DWORD(str.c_str()), 0);
            ctrl.qwm = nullptr;
            pqh.qwm = nullptr;


        }
        return 0;
    }

    
};
//QFDrawer属性
class qfdAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_FNqhl = nullptr;
    QHLayout* m_FSqhl = nullptr;
    QHLayout* m_OSqhl = nullptr;
    QHLayout* m_AddDqhl = nullptr;//添加抽屉
    QHLayout* m_RMDqhl = nullptr;//移除抽屉
    QHLayout* m_AddDTipqhl = nullptr;//添加抽屉提示
    QHLayout* m_AddSqhl = nullptr;//添加子项目
    QHLayout* m_RMSqhl = nullptr;//移除子项目
    QHLayout* m_AddSTipqhl = nullptr;//添加子项目提示


    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_OSqst = nullptr;
    //添加抽屉
    QStaticText* m_AddDqst1 = nullptr;//提示
    QStaticText* m_AddDqst2 = nullptr;//索引提示
    QStaticText* m_AddDqst3 = nullptr;//内容提示
    //添加子项目
    QStaticText* m_AddSqst1 = nullptr;//提示
    QStaticText* m_AddSqst2 = nullptr;//索引提示
    QStaticText* m_AddSqst3 = nullptr;//索引提示2
    QStaticText* m_AddSqst4 = nullptr;//内容提示



    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;
    QEdit* m_OSedit = nullptr;
    //添加抽屉
    QEdit* m_AddDIedit = nullptr;//索引输入
    QEdit* m_AddDTedit = nullptr;//内容输入
    //移除抽屉
    QEdit* m_RMDIedit = nullptr;//索引输入
    //添加子项目
    QEdit* m_AddSIedit = nullptr;//索引输入
    QEdit* m_AddSIedit2 = nullptr;//索引输入
    QEdit* m_AddSTedit = nullptr;//内容输入
    //移除子项目
    QEdit* m_RMSIedit = nullptr;//索引输入
    QEdit* m_RMSIedit2 = nullptr;//索引输入


    QButton* m_AddDbt = nullptr;
    QButton* m_AddSbt = nullptr;
    QButton* m_RMDbt = nullptr;
    QButton* m_RMSbt = nullptr;

    qfdAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 120, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring test(L"被选择事件函数:");
        createCtr(&m_OSqst, &m_OSedit, &m_OSqhl, test, OSSChange);

        test = L"fontName:";
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, test, FNChange);
        test = L"fontSize:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, test, FSChange);

        auto createTip = [this](QStaticText** qst1, const wchar_t* text1, QStaticText** qst2, const wchar_t* text2, QStaticText** qst3, const wchar_t* text3, QHLayout** qhl)
        {

            float fs = 10.0;
            *qst1 = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 48, 100));
            (*qst1)->SetBKColor(0xfff0f0f0);
            (*qst1)->SetText(text1);
            (*qst1)->SetFontSize(fs);

            *qst2 = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 48, 100));
            (*qst2)->SetBKColor(0xfff0f0f0);
            (*qst2)->SetText(text2);
            (*qst2)->SetFontSize(fs);

            *qst3 = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 48, 100));
            (*qst3)->SetBKColor(0xfff0f0f0);
            (*qst3)->SetText(text3);
            (*qst3)->SetFontSize(fs);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst1), 0);
            (*qhl)->InsertCtrl((*qst2), 0);
            (*qhl)->InsertCtrl((*qst3), 1);
        };
        createTip(&m_AddDqst1,L"操作", &m_AddDqst2,L"索引", &m_AddDqst3,L"内容",&m_AddDTipqhl);

        auto createControl = [this](QEdit** edit1, QEdit** edit2,QButton**bt ,const wchar_t* btt, QFBUTTONCLICK ev, QHLayout** qhl)
        {

            *edit1 = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 48, 100));
            if (edit2) {
                *edit2 = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 48, 100));
            }


;

            *bt = (QButton*)QFUSE::CreateQWND("QFButton", QRect(0, 0, 48, 100));
            (*bt)->SetClickEvent(ev);
            (*bt)->SetFontSize(8);
            (*bt)->SetText(btt) ;

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*bt), 0);
            (*qhl)->InsertCtrl((*edit1), 0);
            if (edit2) {
                (*qhl)->InsertCtrl((*edit2), 1);
            }
        };
        createControl(&m_AddDIedit,&m_AddDTedit, &m_AddDbt,L"增加抽屉", OnBTclick, &m_AddDqhl);
        createTip(&m_AddSqst1, L"操作", &m_AddSqst2, L"抽屉索引", &m_AddSqst4, L"内容", &m_AddSTipqhl);
        m_AddSqst3 = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 48, 100));
        m_AddSqst3->SetBKColor(0xfff0f0f0);
        m_AddSqst3->SetText(L"子项目索引");
        m_AddSqst3->SetFontSize(10);
        m_AddSTipqhl->InsertCtrl(m_AddSqst3, 0,2);

        createControl(&m_AddSIedit, &m_AddSTedit, &m_AddSbt, L"增加项目", OnBTclick, &m_AddSqhl);
        m_AddSIedit2 = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 48, 100));
        m_AddSqhl->InsertCtrl(m_AddSIedit2, 0, 2);
        createControl(&m_RMDIedit,nullptr, &m_RMDbt, L"移除抽屉", OnBTclick, &m_RMDqhl);
        createControl(&m_RMSIedit, nullptr, &m_RMSbt, L"移除项目", OnBTclick, &m_RMSqhl);
        m_RMSIedit2 = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 48, 100));
        m_RMSqhl->InsertCtrl(m_RMSIedit2, 0);
    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QTabs ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("fontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
            else if ("onSelected" == key) {
                m_OSedit->SetText(stringToWString(value).c_str());
            }


        }

        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {

        m_FNedit->SetText(L"");
        m_FSedit->SetText(L"");
        m_OSedit->SetText(L"");
        m_AddDIedit->SetText(L"-1");
        m_AddDTedit->SetText(L"");
        m_AddSTedit->SetText(L"");
        m_AddSIedit->SetText(L"0");
        m_AddSIedit2->SetText(L"-1");

        m_RMDIedit->SetText(L"");
        m_RMSIedit->SetText(L"");
        m_RMSIedit2->SetText(L"");
    }

    static int FNChange(int p, int pt, int param)
    {
        qfdAttribute* pba = (qfdAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QDrawer ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontName((wchar_t*)pt);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontName"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FSChange(int p, int pt, int param)
    {
        qfdAttribute* pba = (qfdAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QDrawer ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontSize(std::strtod(wstring_to_string((wchar_t*)pt).c_str(), NULL));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontSize"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int OSSChange(int p, int pt, int param)
    {
        qfdAttribute* pba = (qfdAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            std::string str = wstring_to_string((wchar_t*)pt);
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("onSelected"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static void OnBTclick(void* param)
    {
        extern qfdAttribute* g_pqfda;
        QWND* ctrl = (QWND*)param;
        if (ctrl == g_pqfda->m_AddDbt->qwm) {
            
            QDrawer ctrl(nullptr, QRect());
            QHost host(nullptr, QRect());
            host.qwm = g_pqfda->ctrl;
            ctrl.qwm = host.GetParasiticCtrl();
            
            std::wstring wsindex = g_pqfda->m_AddDIedit->GetText();
            std::string sindex = wstring_to_string(wsindex);
            int ii = std::strtol(sindex.c_str(),nullptr,10);
            int index = ctrl.InsertDrawer(ii);
            ctrl.SetDrawerName(index, g_pqfda->m_AddDTedit->GetText());
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            host.qwm = nullptr;
            ctrl.qwm = nullptr;
           

        }
        else if (ctrl == g_pqfda->m_AddSbt->qwm) {
            
            QDrawer ctrl(nullptr, QRect());
            QHost host(nullptr, QRect());
            host.qwm = g_pqfda->ctrl;
            ctrl.qwm = host.GetParasiticCtrl();

            std::wstring wsindex = g_pqfda->m_AddSIedit->GetText();
            std::wstring wsindex2 = g_pqfda->m_AddSIedit2->GetText();
            std::string sindex = wstring_to_string(wsindex);
            std::string sindex2 = wstring_to_string(wsindex2);
            int ii = std::strtol(sindex.c_str(), nullptr, 10);
            int ii2 = std::strtol(sindex2.c_str(), nullptr, 10);
            int index = ctrl.InsertSubObject(ii, ii2);
            ctrl.SetSubObjectName(ii,index, g_pqfda->m_AddSTedit->GetText());
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            host.qwm = nullptr;
            ctrl.qwm = nullptr;
        }
        else if (ctrl == g_pqfda->m_RMDbt->qwm) {//删除抽屉

            QDrawer ctrl(nullptr, QRect());
            QHost host(nullptr, QRect());
            host.qwm = g_pqfda->ctrl;
            ctrl.qwm = host.GetParasiticCtrl();

            std::wstring wsindex = g_pqfda->m_RMDIedit->GetText();
            if (L"" != wsindex) {
                std::string sindex = wstring_to_string(wsindex);
                int ii = std::strtol(sindex.c_str(), nullptr, 10);
                std::string name = wstring_to_string(ctrl.GetDrawerName(ii));
                int sCount = ctrl.GetSubObjectCount(ii);
                std::string tip = std::format("确定要删除《{}》及其 {} 个子项目吗？", name.c_str(), sCount);
                if (IDYES == MessageBoxA(0, tip.c_str(), "删除抽屉", MB_YESNO)) {

                    ctrl.RemoveDrawer(ii);
                    ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                }
            }
            host.qwm = nullptr;
            ctrl.qwm = nullptr;
        }
        else if (ctrl == g_pqfda->m_RMSbt->qwm) {//删除子项

            QDrawer ctrl(nullptr, QRect());
            QHost host(nullptr, QRect());
            host.qwm = g_pqfda->ctrl;
            ctrl.qwm = host.GetParasiticCtrl();

            std::wstring wsindex = g_pqfda->m_RMDIedit->GetText();
            std::wstring wsindex2 = g_pqfda->m_RMSIedit2->GetText();
            if (L"" != wsindex || L"" != wsindex2) {
                std::string sindex = wstring_to_string(wsindex);
                std::string sindex2 = wstring_to_string(wsindex2);
                int ii = std::strtol(sindex.c_str(), nullptr, 10);
                int ii2 = std::strtol(sindex2.c_str(), nullptr, 10);
                std::string name = wstring_to_string(ctrl.GetDrawerName(ii));
                std::string sname = wstring_to_string(ctrl.GetSubObjectName(ii,ii2));
                std::string tip = std::format("确定要删除《{}》的子项目 《{}》 吗？", name.c_str(), sname.c_str());
                if (IDYES == MessageBoxA(0, tip.c_str(), "删除抽屉的子项目", MB_YESNO)) {

                    ctrl.RemoveSubObject(ii,ii2);
                    ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                }
            }
            host.qwm = nullptr;
            ctrl.qwm = nullptr;
        }
    }

    

};
//QFStaticText属性
class stAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_FNqhl = nullptr;
    QHLayout* m_FSqhl = nullptr;
    QHLayout* m_FCqhl = nullptr;
    QHLayout* m_TEXTqhl = nullptr;
    QHLayout* m_BKCqhl = nullptr;


    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_FCqst = nullptr;
    QStaticText* m_TEXTqst = nullptr;
    QStaticText* m_BKCqst = nullptr;


    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;
    QEdit* m_TEXTedit = nullptr;
    //QEdit* m_FCedit = nullptr;
    //QEdit* m_BKCedit = nullptr;
    QColorBox* m_FCColor = nullptr;
    QColorBox* m_BKColor = nullptr;

    stAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring text(L"Text:");
        createCtr(&m_TEXTqst, &m_TEXTedit, &m_TEXTqhl, text, textChange);

        text = L"fontName:";
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, text,FNChange);
        text = L"fontSize:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, text, FSChange);
        text = L"fontColor:";
        //createCtr(&m_FCqst, &m_FCedit, &m_FCqhl, text, FCChange);
        m_FCqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
        m_FCqst->SetBKColor(0xfff0f0f0);
        m_FCqst->SetText(text.c_str());

        m_FCColor = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 100));
        m_FCColor->RecordAttribute(0, (long long)this);
        m_FCColor->SetColorChangeEvent(fColorChange);
        m_FCColor->SetFontSize(16);

        m_FCqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_FCqhl->InsertCtrl(m_FCqst, 0);
        m_FCqhl->InsertCtrl(m_FCColor, 1);
        text = L"背景颜色:";
        //createCtr(&m_BKCqst, &m_BKCedit, &m_BKCqhl, test, BKCChange);
        m_BKCqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
        m_BKCqst->SetBKColor(0xfff0f0f0);
        m_BKCqst->SetText(text.c_str());

        m_BKColor = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 100));
        m_BKColor->RecordAttribute(0, (long long)this);
        m_BKColor->SetColorChangeEvent(BKColorChange);
        m_BKColor->SetFontSize(16);

        m_BKCqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_BKCqhl->InsertCtrl(m_BKCqst, 0);
        m_BKCqhl->InsertCtrl(m_BKColor, 1);

    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QStaticText ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("fontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontColor" == key) {
                //m_FCedit->SetText(stringToWString(value).c_str());
                int para = 10;
                if ('x' == std::tolower(v[1]))para = 16;
                unsigned long color = std::strtoul(v, nullptr, para);
                m_BKColor->SetColor(color);
            }
            else if ("backgroundColor" == key) {
                //m_BKCedit->SetText(stringToWString(value).c_str());
                int para = 10;
                if ('x' == std::tolower(v[1]))para = 16;
                unsigned long color = std::strtoul(v, nullptr, para);
                m_FCColor->SetColor(color);
            }
        }
        m_TEXTedit->SetText(ctrl.GetText().c_str());


        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {

        m_FNedit->SetText(L"");
        m_FSedit->SetText(L"");
        m_TEXTedit->SetText(L"");
        //m_FCedit->SetText(L"");
        //m_BKCedit->SetText(L"");
        m_FCColor->SetColor(0xff000000);
        m_BKColor->SetColor(0xff000000);
    }

    static int FNChange(int p, int pt, int param)
    {
        stAttribute* pba = (stAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QStaticText ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontName((wchar_t*)pt);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontName"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FSChange(int p, int pt, int param)
    {
        stAttribute* pba = (stAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QStaticText ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontSize(std::strtod(wstring_to_string((wchar_t*)pt).c_str(), NULL));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontSize"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FCChange(int p, int pt, int param)
    {
        stAttribute* pba = (stAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QStaticText ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr((wchar_t*)pt);
            std::string str = wstring_to_string(wstr);
            unsigned long color = std::strtoul(str.c_str(),nullptr,16);
            ctrl.SetFontColor(color);
            ctrl.TransferMessage(QM_UPDATE_CTRL,1,0,0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontColor"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int textChange(int p, int pt, int param)
    {
        stAttribute* pba = (stAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QStaticText ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetText((wchar_t*)pt);

            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("Text"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int BKCChange(int p, int pt, int param)
    {
        stAttribute* pba = (stAttribute*)param;

        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QStaticText ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            std::wstring wstr((wchar_t*)pt);
            std::string str = wstring_to_string(wstr);
            unsigned long color = std::strtoul(str.c_str(), nullptr, 16);
            ctrl.SetBKColor(color);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("backgroundColor"), DWORD(str.c_str()), 0);
            pqh.qwm = nullptr;


        }
        return 0;
    }
    static int BKColorChange(QPOINTER ct, unsigned int color)
    {
        QColorBox qcb(nullptr, QRect(0, 0, 10, 10));
        qcb.qwm = (QWND*)ct;

        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            stAttribute* p = (stAttribute*)data;
            if (p) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QStaticText ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

                ctrl.SetBKColor(color);


                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("backgroundColor"), DWORD(std::format("0x{:x}", color).c_str()), 0);
                pqh.qwm = nullptr;
            }

        }


        qcb.qwm = nullptr;
        return 1;
    }
    
    static int fColorChange(QPOINTER ct, unsigned int color)
    {
        QColorBox qcb(nullptr, QRect(0, 0, 10, 10));
        qcb.qwm = (QWND*)ct;

        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            stAttribute* p = (stAttribute*)data;
            if (p) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QStaticText ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

                ctrl.SetFontColor(color);


                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontColor"), DWORD(std::format("0x{:x}", color).c_str()), 0);
                pqh.qwm = nullptr;
            }

        }


        qcb.qwm = nullptr;
        return 1;
    }





};
//QSlider属性
class qsAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_minqhl = nullptr;//最大值
    QHLayout* m_maxqhl = nullptr;//最小值
    QHLayout* m_CVqhl = nullptr;//当前值
    QHLayout* m_RCqhl = nullptr;//轨道颜色
    QHLayout* m_BKCqhl = nullptr;//轨道颜色
    QHLayout* m_RRqhl = nullptr;//轨道比例
    QHLayout* m_IsSegqhl = nullptr;//是否分段
    QHLayout* m_AddSegqhl = nullptr;//增加分段
    QHLayout* m_RMSegqhl = nullptr;//移除分段
    QHLayout* m_DIRqhl = nullptr;//方向
    QHLayout* m_SLIDqhl = nullptr;//滑动时回调


    QStaticText* m_minqst = nullptr;
    QStaticText* m_maxqst = nullptr;
    QStaticText* m_CVqst = nullptr;
    QStaticText* m_RCqst = nullptr;
    QStaticText* m_BKCqst = nullptr;
    QStaticText* m_RRqst = nullptr;
    QStaticText* m_IsSegqst = nullptr;
    QStaticText* m_DIRqst = nullptr;
    QStaticText* m_SLIDqst = nullptr;

    QButton2* m_AddSegBt = nullptr;
    QButton2* m_RMSegBt = nullptr;

    QEdit* m_minedit = nullptr;
    QEdit* m_maxedit = nullptr;
    QEdit* m_CVedit = nullptr;
    QEdit* m_AddSegedit = nullptr;
    QEdit* m_RMSegedit = nullptr;
    QEdit* m_SLIDedit = nullptr;
    //QEdit* m_FCedit = nullptr;
    //QEdit* m_BKCedit = nullptr;
    QColorBox* m_RCColor = nullptr;
    QColorBox* m_BKColor = nullptr;

    QSlider* m_RRSd = nullptr;

    QSwitch* m_IsSegSC = nullptr;

    QSelect* m_DIRST = nullptr;

    qsAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev,int w=72)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, w, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring text(L"最小值:");
        createCtr(&m_minqst, &m_minedit, &m_minqhl, text, editChange);

        text = L"最大值:";
        createCtr(&m_maxqst, &m_maxedit, &m_maxqhl, text, editChange);
        text = L"当前值:";
        createCtr(&m_CVqst, &m_CVedit, &m_CVqhl, text, editChange);
        text = L"滑动事件回调:";
        createCtr(&m_SLIDqst, &m_SLIDedit, &m_SLIDqhl, text, editChange,110);


        auto createColorBox = [this](QStaticText** qst, QColorBox** cb, QHLayout** qhl, std::wstring& text, QCOLORBOX_CHANGE ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*cb) = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 100));
            (*cb)->RecordAttribute(0, (long long)this);
            (*cb)->SetColorChangeEvent(ev);
            (*cb)->SetFontSize(16);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*cb), 1);

        };
        text = L"轨道颜色:";
        createColorBox(&m_RCqst, &m_RCColor, &m_RCqhl, text, ColorChange);
        text = L"背景颜色:";
        createColorBox(&m_BKCqst, &m_BKColor, &m_BKCqhl, text, ColorChange);
        auto createSolider = [this](QStaticText** qst, QSlider** cd, QHLayout** qhl, std::wstring& text, QSLIDER_VALUE_CHANGE ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*cd) = (QSlider*)QFUSE::CreateQWND("QFSlider", QRect(0, 0, 50, 100));
            (*cd)->RecordAttribute(0, (long long)this);
            (*cd)->SetValueChangeEvent(ev);
      

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*cd), 1);

        };
        text = L"轨道比例:";
        createSolider(&m_RRqst, &m_RRSd, &m_RRqhl, text, soliderChange);
        m_RRSd->SetMaxValue(1);
        auto createSwitch = [this](QStaticText** qst, QSwitch** sw, QHLayout** qhl, std::wstring& text, QSWITCH_STATE_CHANGE ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*sw) = (QSwitch*)QFUSE::CreateQWND("QFSwitch", QRect(0, 0, 50, 100));
            (*sw)->RecordAttribute(0, (long long)this);
            (*sw)->SetStateChangeEvent(ev);


            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*sw), 1);

        };
        text = L"是否分段:";
        createSwitch(&m_IsSegqst, &m_IsSegSC, &m_IsSegqhl, text, SWChange);
        auto createCtrl = [this](QButton2** bt, QEdit** qe, QHLayout** qhl, std::wstring& text, QFBUTTONCLICK ev)
        {

            (*bt) = (QButton2*)QFUSE::CreateQWND("QFPButton", QRect(0, 0, 100, 10));
            (*bt)->SetClickEvent(ev);
            (*bt)->SetText(text.c_str());
            (*bt)->RecordAttribute(0,(long long)this);
            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 100, 10));
               

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*bt), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        text = L"添加分段:";
        createCtrl(&m_AddSegBt,&m_AddSegedit,&m_AddSegqhl, text, BTClick);
        text = L"删除分段:";
        createCtrl(&m_RMSegBt, &m_RMSegedit, &m_RMSegqhl, text, BTClick);
        text = L"方向:";
        m_DIRqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
        m_DIRqst->SetBKColor(0xfff0f0f0);
        m_DIRqst->SetText(text.c_str());

        m_DIRST = (QSelect*)QFUSE::CreateQWND("QFSelect", QRect(0, 0, 50, 100));
        m_DIRST->RecordAttribute(0, (long long)this);
        m_DIRST->SetSelectChangeEvent(SelectOnChange);
        m_DIRST->InsertItem(L"水平");
        m_DIRST->InsertItem(L"垂直");
        m_DIRST->SetCurrentItem(0);
        m_DIRqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_DIRqhl->InsertCtrl(m_DIRqst, 0);
        m_DIRqhl->InsertCtrl(m_DIRST, 1);

       
}
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QStaticText ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("minValue" == key) {
                m_minedit->SetText(stringToWString(value).c_str());
            }
            else if ("m_maxedit" == key) {
                m_maxedit->SetText(stringToWString(value).c_str());
            }
            else if ("currentValue" == key) {
                m_CVedit->SetText(stringToWString(value).c_str());
            }
            else if ("RailColor" == key) {
                //m_BKCedit->SetText(stringToWString(value).c_str());
                int para = 10;
                if ('x' == std::tolower(v[1]))para = 16;
                unsigned long color = std::strtoul(v, nullptr, para);
                m_RCColor->SetColor(color);
            }
            else if ("backgroundColor" == key) {
                //m_BKCedit->SetText(stringToWString(value).c_str());
                int para = 10;
                if ('x' == std::tolower(v[1]))para = 16;
                unsigned long color = std::strtoul(v, nullptr, para);
                m_BKColor->SetColor(color);
            }
            else if ("RailRatio" == key) {
                double rat = std::strtod(value.c_str(),nullptr);
                m_RRSd->SetCurrentValue(rat);
            
            }
            else if ("IsSegmentation" == key) {
                if ("true" == value) {
                    m_IsSegSC->SetState(1);
                }
                else {
                    m_IsSegSC->SetState(0);
                }

            }
            else if ("Direction" == key) {
                int index=std::strtol(value.c_str(),nullptr,10);
                m_DIRST->SetCurrentItem(index);
            }
            else if ("ValueChangeEvent" == key) {
                m_SLIDedit->SetText(stringToWString(value).c_str());
            }
        }
        //m_TEXTedit->SetText(ctrl.GetText().c_str());


        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {
        m_minedit->SetText(L"");
        m_maxedit->SetText(L"");
        m_CVedit->SetText(L"");
        m_AddSegedit->SetText(L"");
        m_RMSegedit->SetText(L"");
        m_BKColor->SetColor(0xffffffff);
        m_RCColor->SetColor(0xff00f0f0);
        m_RRSd->SetCurrentValue(0.3);
        m_IsSegSC->SetState(0);
        //int count = m_DIRST->GetItemCount();
        //for (int i = -1; ++i < count;) {
        //    m_DIRST->RemoveItem(0);
        //}
        m_DIRST->SetCurrentItem(0);

    }

    static int editChange(int p, int pt, int param)
    {

        qsAttribute* pba = (qsAttribute*)param;
        QEdit* pedit = (QEdit*)p;
        if (pedit == pba->m_minedit->qwm) {//最小值
            if (pba->ctrl) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
                QSlider ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                std::wstring ws = (wchar_t*)pt;
                std::string str = wstring_to_string(ws);
                double v= std::strtod(str.c_str(),nullptr);
                ctrl.SetMinValue(v);
     
                ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("minValue"), DWORD(str.c_str()), 0);
                pqh.qwm = nullptr;

            }
        
        }
        else if (pedit == pba->m_maxedit->qwm) {//最大值
            if (pba->ctrl) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
                QSlider ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                std::wstring ws = (wchar_t*)pt;
                std::string str = wstring_to_string(ws);
                double v = std::strtod(str.c_str(), nullptr);
                ctrl.SetMaxValue(v);

                ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("maxValue"), DWORD(str.c_str()), 0);
                pqh.qwm = nullptr;

            }
        
        }
        else if (pedit == pba->m_CVedit->qwm) {//当前值
            if (pba->ctrl) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
                QSlider ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                std::wstring ws = (wchar_t*)pt;
                std::string str = wstring_to_string(ws);
                double v = std::strtod(str.c_str(), nullptr);
                ctrl.SetCurrentValue(v);

                ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("currentValue"), DWORD(str.c_str()), 0);
                pqh.qwm = nullptr;

            }

        }
        else if (pedit == pba->m_SLIDedit->qwm) {//滑动函数设置
            if (pba->ctrl) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
                std::wstring ws = (wchar_t*)pt;
                std::string str = wstring_to_string(ws);
              
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("ValueChangeEvent"), DWORD(str.c_str()), 0);
                pqh.qwm = nullptr;

            }
        }

        return 0;
    }
   
    static int ColorChange(QPOINTER ct, unsigned int color)
    {
        QColorBox qcb(nullptr, QRect(0, 0, 10, 10));
        QColorBox* pcb = (QColorBox*)ct;
        qcb.qwm = (QWND*)ct;
   
        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            qsAttribute* p = (qsAttribute*)data;
            if (p) {
                if (pcb == p->m_RCColor->qwm) {//
                    QHost pqh(nullptr, QRect(0, 0, 10, 10));
                    pqh.qwm = p->ctrl;
                    QSlider ctrl(nullptr, QRect(0, 0, 10, 10));
                    ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

                    ctrl.SetRailColor(color);


                    ctrl.qwm = nullptr;
                    pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("RailColor"), DWORD(std::format("0x{:x}", color).c_str()), 0);
                    pqh.qwm = nullptr;
                }
                else  if (pcb == p->m_BKColor->qwm) {
                    QHost pqh(nullptr, QRect(0, 0, 10, 10));
                    pqh.qwm = p->ctrl;
                    QSlider ctrl(nullptr, QRect(0, 0, 10, 10));
                    ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

                    ctrl.SetColor(color);


                    ctrl.qwm = nullptr;
                    pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("backgroundColor"), DWORD(std::format("0x{:x}", color).c_str()), 0);
                    pqh.qwm = nullptr;
                }
            }

        }
        


        qcb.qwm = nullptr;
        return 1;
    }
    static int soliderChange(QPOINTER ct, float v)
    {
        QSlider qcb(nullptr, QRect(0, 0, 10, 10));
        qcb.qwm = (QWND*)ct;

        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            qsAttribute* p = (qsAttribute*)data;
            if (p) {
                QSlider* sld = (QSlider*)ct;
                if (sld == p->m_RRSd->qwm) {
                    QHost pqh(nullptr, QRect(0, 0, 10, 10));
                    pqh.qwm = p->ctrl;
                    QSlider ctrl(nullptr, QRect(0, 0, 10, 10));
                    ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                    ctrl.SetRailRatio(v);
                    

                    ctrl.qwm = nullptr;
                    pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("RailRatio"), DWORD(std::format("{:f}", v).c_str()), 0);
                    pqh.qwm = nullptr;
                }


            }

        }
        qcb.qwm = nullptr;
        return 1;
    }
    static int SWChange(QPOINTER ct)
    {
        QSwitch qcb(nullptr, QRect(0, 0, 10, 10));

        qcb.qwm = (QSwitch*)ct;

        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            qsAttribute* p = (qsAttribute*)data;
            if (p) {
                QSwitch* swh = (QSwitch*)ct;
                if (swh == p->m_IsSegSC->qwm) {
                    QHost pqh(nullptr, QRect(0, 0, 10, 10));
                    pqh.qwm = p->ctrl;
                    QSlider ctrl(nullptr, QRect(0, 0, 10, 10));
                    ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

                    std::string IsDrag = "true";
                    bool isd = true;
                    if (1 != qcb.GetState()) {
                        IsDrag = "false";
                        isd = false;
                    }
                    ctrl.IsSegmentation(isd);
                    ctrl.qwm = nullptr;
                    pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("IsSegmentation"), DWORD(IsDrag.c_str()), 0);
                    pqh.qwm = nullptr;
                }
            }

        }


        qcb.qwm = nullptr;
        return 1;

    }
    static void BTClick(void* addr)
    {
        QButton2* bt2 = (QButton2*)addr;
        QButton2 qbt2(nullptr,QRect());
        qbt2.qwm = bt2;
        QDWORD data = 0;
        qbt2.FindAttribute(0, &data);
        qsAttribute* p = (qsAttribute*)data;
        if (p) {
            
            if (bt2 == p->m_AddSegBt->qwm) {//增加分段
                //先拿到编辑框里面的内容
                std::wstring text = p->m_AddSegedit->GetText();
                std::string str = wstring_to_string(text);
                double value = std::strtod(str.c_str(), nullptr);//转换数值
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QSlider ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                ctrl.AddSegmVuale(value);


                //销毁对象，撤销运行内容
                ctrl.qwm = nullptr;
                pqh.qwm = nullptr;
            
            }
            else if (bt2 == p->m_RMSegBt->qwm) {//移除分段
                 //先拿到编辑框里面的内容
                std::wstring text = p->m_AddSegedit->GetText();
                std::string str = wstring_to_string(text);
                double value = std::strtod(str.c_str(), nullptr);//转换数值
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QSlider ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                ctrl.RemoveSegmVuale(value);

                //销毁对象，撤销运行内容
                ctrl.qwm = nullptr;
                pqh.qwm = nullptr;
            
            }
        
        
        
        }
        qbt2.qwm = nullptr;
    
    }

    static int SelectOnChange(QPOINTER ct, int index)
    {
        QSelect* qs = (QSelect*)ct;
        QSelect qst(nullptr,QRect());
        qst.qwm = qs;
        QDWORD data = 0;
        qst.FindAttribute(0, &data);
        qsAttribute* p = (qsAttribute*)data;
        if (p) {
            if (qs == p->m_DIRST->qwm) {//方向择
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QSlider ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                ctrl.SetDirection(index);

                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("Direction"), DWORD(std::to_string(index).c_str()), 0);
                //销毁对象，撤销运行内容
                ctrl.qwm = nullptr;
                pqh.qwm = nullptr;


            
            }
        
        
        }
        qst.qwm = nullptr;
        return 1;
    
    }



};
//QSwitch属性
class swAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_SCEqhl = nullptr;
    QHLayout* m_Stateqhl = nullptr;
    QHLayout* m_MTXqhl = nullptr;



    QStaticText* m_SCEqst = nullptr;
    QStaticText* m_Stateqst = nullptr;
    QStaticText* m_MTXqst = nullptr;



    QEdit* m_SCEedit = nullptr;

    QSwitch* m_StateSW = nullptr;
    QSwitch* m_MTXSW = nullptr;

swAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev, int w = 72)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, w, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring text(L"状态改变事件:");
        createCtr(&m_SCEqst, &m_SCEedit, &m_SCEqhl, text, editChange,110);

        text = L"当前状态:";
        m_Stateqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 96, 100));
        m_Stateqst->SetBKColor(0xfff0f0f0);
        m_Stateqst->SetText(text.c_str());
        m_StateSW = (QSwitch*)QFUSE::CreateQWND("QFSwitch", QRect(0, 0, 50, 100));
        m_StateSW->RecordAttribute(0, (long long)this);
        m_StateSW->SetStateChangeEvent(SWChange);
        m_Stateqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_Stateqhl->InsertCtrl(m_Stateqst, 0);
        m_Stateqhl->InsertCtrl(m_StateSW, 1);
        text = L"是否互斥:";
        m_MTXqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 96, 100));
        m_MTXqst->SetBKColor(0xfff0f0f0);
        m_MTXqst->SetText(text.c_str());
        m_MTXSW = (QSwitch*)QFUSE::CreateQWND("QFSwitch", QRect(0, 0, 50, 100));
        m_MTXSW->RecordAttribute(0, (long long)this);
        m_MTXSW->SetStateChangeEvent(SWChange);
        m_MTXqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_MTXqhl->InsertCtrl(m_MTXqst, 0);
        m_MTXqhl->InsertCtrl(m_MTXSW, 1);



    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QStaticText ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("StateChangeEvent" == key) {
                m_SCEedit->SetText(stringToWString(value).c_str());
            }
            else if ("State" == key) {
                int state = 0;
                if ("true" == value) {
                    state = 1;
                }
                m_StateSW->SetState(state);
            }
            else if ("IsMutex" == key) {
                int state = 0;
                if ("true" == value) {
                    state = 1;
                }
                m_MTXSW->SetState(state);
            }
            
        }



        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {

        m_SCEedit->SetText(L"");
        m_StateSW->SetState(0);
        m_MTXSW->SetState(0);
  
    }

    static int editChange(int p, int pt, int param)
    {
        swAttribute* pba = (swAttribute*)param;
        QEdit* pedit = (QEdit*)p;
        if (pedit == pba->m_SCEedit->qwm) {//最小值
            if (pba->ctrl) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
              
                std::wstring ws = (wchar_t*)pt;
                std::string str = wstring_to_string(ws);
      
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("StateChangeEvent"), DWORD(str.c_str()), 0);
                pqh.qwm = nullptr;

            }

        }
        return 1;
    }
    static int SWChange(QPOINTER ct)
    {
        QSwitch qcb(nullptr, QRect(0, 0, 10, 10));

        qcb.qwm = (QSwitch*)ct;

        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            swAttribute* p = (swAttribute*)data;
            if (p) {
                QSwitch* swh = (QSwitch*)ct;
                if (swh == p->m_StateSW->qwm) {
                    QHost pqh(nullptr, QRect(0, 0, 10, 10));
                    pqh.qwm = p->ctrl;
                    QSwitch ctrl(nullptr, QRect(0, 0, 10, 10));
                    ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

                    std::string State = "true";
                    int isd = 1;
                    if (1 != qcb.GetState()) {
                        State = "false";
                        isd = 0;
                    }
                    ctrl.SetState(isd);
                    ctrl.qwm = nullptr;
                    pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("State"), DWORD(State.c_str()), 0);
                    pqh.qwm = nullptr;
                }
                else if (swh == p->m_MTXSW->qwm) {
                    QHost pqh(nullptr, QRect(0, 0, 10, 10));
                    pqh.qwm = p->ctrl;
                    QSwitch ctrl(nullptr, QRect(0, 0, 10, 10));
                    ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

                    std::string IsMutex = "true";
                    bool isd = true;
                    if (1 != qcb.GetState()) {
                        IsMutex = "false";
                        isd = false;
                    }
                    ctrl.IsMutex(isd);
                    ctrl.qwm = nullptr;
                    pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("IsMutex"), DWORD(IsMutex.c_str()), 0);
                    pqh.qwm = nullptr;
                
                }
            }

        }


        qcb.qwm = nullptr;
        return 1;

    }




};
//QProgress属性
class pgrAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_TKqhl = nullptr;//圆环宽度
    QHLayout* m_DIRqhl = nullptr;//方向
    QHLayout* m_CVqhl = nullptr;//当前值
    QHLayout* m_typeqhl = nullptr;//样式
    QHLayout* m_DPqhl = nullptr;//保留小数
    QHLayout* m_LCqhl = nullptr;//线型进度条的颜色
    QHLayout* m_RCqhl = nullptr;//线型进度条的颜色
    QHLayout* m_FNqhl = nullptr;//字体名字
    QHLayout* m_FSqhl = nullptr;//字体大小
    QHLayout* m_FCqhl = nullptr;//字体颜色


    QStaticText* m_DIRqst = nullptr;
    QStaticText* m_CVqst = nullptr;
    QStaticText* m_typeqst = nullptr;
    QStaticText* m_DPqst = nullptr;
    QStaticText* m_LCqst = nullptr;
    QStaticText* m_RCqst = nullptr;
    QStaticText* m_TKqst = nullptr;
    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_FCqst = nullptr;


    QEdit* m_TKedit = nullptr;
    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;

    QSlider* m_CVSd = nullptr;

    QColorBox* m_LCCB = nullptr;
    QColorBox* m_RCCB1 = nullptr;
    QColorBox* m_RCCB2 = nullptr;
    QColorBox* m_FCCB = nullptr;

    QSelect* m_DIRST = nullptr;
    QSelect* m_typeST = nullptr;
    QSelect* m_DPST = nullptr;
    pgrAttribute() {

        auto createSD = [this](QStaticText** qst, QSlider** sd, QHLayout** qhl, std::wstring& text, QSLIDER_VALUE_CHANGE ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*sd) = (QSlider*)QFUSE::CreateQWND("QFSlider", QRect(0, 0, 50, 100));
            (*sd)->RecordAttribute(0, (int)this);
            (*sd)->SetValueChangeEvent(ev);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*sd), 1);

        };
        std::wstring text(L"当前进度:");
        createSD(&m_CVqst, &m_CVSd, &m_CVqhl, text, soliderChange);
    
        auto createSET = [this](QStaticText** qst, QSelect** se, QHLayout** qhl, std::wstring& text, QSELECT_SELECTCHANGE ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*se) = (QSelect*)QFUSE::CreateQWND("QFSelect", QRect(0, 0, 50, 100));
            (*se)->RecordAttribute(0, (int)this);
            (*se)->SetSelectChangeEvent(ev);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*se), 1);

        };
        text = L"方向:";
        createSET(&m_DIRqst,&m_DIRST,&m_DIRqhl, text, SelectOnChange);
        m_DIRST->InsertItem(L"水平方向");
        m_DIRST->InsertItem(L"垂直方向");
        text = L"样式:";
        createSET(&m_typeqst, &m_typeST, &m_typeqhl, text, SelectOnChange);
        m_typeST->InsertItem(L"直线型");
        m_typeST->InsertItem(L"圆环型");
        text = L"小数位数:";
        createSET(&m_DPqst, &m_DPST, &m_DPqhl, text, SelectOnChange);
        m_DPST->InsertItem(L"0位小数");
        m_DPST->InsertItem(L"1位小数");
        m_DPST->InsertItem(L"2位小数");
        m_DPST->InsertItem(L"3位小数");

        auto createColor = [this](QStaticText** qst, QColorBox** cb, QHLayout** qhl, std::wstring& text, QCOLORBOX_CHANGE ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 100, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*cb) = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 100));
            (*cb)->RecordAttribute(0, (int)this);
            (*cb)->SetColorChangeEvent(ev);
            (*cb)->SetFontSize(16);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*cb), 1);

        };
        text = L"线性进度条颜色:";
        createColor(&m_LCqst,&m_LCCB,&m_LCqhl, text, ColorChange);
        text = L"字体颜色:";
        createColor(&m_FCqst, &m_FCCB, &m_FCqhl, text, ColorChange);

        auto createColor2 = [this](QStaticText** qst, QColorBox** cb, QColorBox** cb2, QHLayout** qhl, std::wstring& text, QCOLORBOX_CHANGE ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 100, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*cb) = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 100));
            (*cb)->RecordAttribute(0, (int)this);
            (*cb)->SetColorChangeEvent(ev);

            (*cb2) = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 100));
            (*cb2)->RecordAttribute(0, (int)this);
            (*cb2)->SetColorChangeEvent(ev);
    

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*cb), 1);
            (*qhl)->InsertCtrl((*cb2), 1);

        };
        text = L"环形进度条颜色:";
        createColor2(&m_RCqst, &m_RCCB1, &m_RCCB2, &m_RCqhl, text, ColorChange);

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        text = L"环形宽度:";
        createCtr(&m_TKqst,&m_TKedit,&m_TKqhl, text, editChange);
        text = L"字体名称:";
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, text, editChange);
        text = L"字体大小:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, text, editChange);

    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QProgress ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("CurrentValue" == key) {
                double cv = std::strtod(value.c_str(),nullptr);
                m_CVSd->SetCurrentValue(cv);
                
            }
            else if ("Direction" == key) {
                int index = std::strtol(value.c_str(), nullptr,10);
  
                m_DIRST->SetCurrentItem(index);
            }
            else if ("Type" == key) {
                int index = std::strtol(value.c_str(), nullptr, 10);

                m_typeST->SetCurrentItem(index);
            }
            else if ("DecimalPlaces" == key) {
                int index = std::strtol(value.c_str(), nullptr, 10);

                m_DPST->SetCurrentItem(index);
            }
            else if ("LineTypeColor" == key) {
                int para = 10;
                if ('x' == std::tolower(v[1]))para = 16;
                unsigned long color = std::strtoul(v, nullptr, para);

                m_LCCB->SetColor(color);
            }
            else if ("RingTypeColor1" == key) {
                int para = 10;
                if ('x' == std::tolower(v[1]))para = 16;
                unsigned long color = std::strtoul(v, nullptr, para);

                m_RCCB1->SetColor(color);
            }
            else if ("RingTypeColor2" == key) {
                int para = 10;
                if ('x' == std::tolower(v[1]))para = 16;
                unsigned long color = std::strtoul(v, nullptr, para);

                m_RCCB2->SetColor(color);
            }
            else if ("RingProgressThickness" == key) {
                m_TKedit->SetText(stringToWString(value).c_str());
            }
            else if ("FontColor" == key) {
                int para = 10;
                if ('x' == std::tolower(v[1]))para = 16;
                unsigned long color = std::strtoul(v, nullptr, para);

                m_FCCB->SetColor(color);
            }
            else if ("FontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("FontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
        }
        m_DIRST->SetCurrentItem(ctrl.GetStyle());



        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {
        m_CVSd->SetCurrentValue(60);

        m_LCCB->SetColor(0xffff0000);
        m_RCCB1->SetColor(0xff000000);
        m_RCCB2->SetColor(0xff00ff00);
        m_FCCB->SetColor(0xff000000);
        m_FNedit->SetText(L"黑体");
        m_FSedit->SetText(L"12");
        m_DIRST->SetCurrentItem(0);
        m_typeST->SetCurrentItem(0);
        m_DPST->SetCurrentItem(0);
        m_TKedit->SetText(L"50");

    }
    static int editChange(int p, int pt, int param)
    {

        pgrAttribute* pba = (pgrAttribute*)param;
        QEdit* pedit = (QEdit*)p;
        if (pedit == pba->m_TKedit->qwm) {//圆环宽度
            if (pba->ctrl) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
                QProgress ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                std::wstring ws = (wchar_t*)pt;
                std::string str = wstring_to_string(ws);
                double v = std::strtod(str.c_str(), nullptr);
                ctrl.SetRingProgressThickness(v);

                ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("RingProgressThickness"), DWORD(str.c_str()), 0);
                pqh.qwm = nullptr;

            }

        }
        else if (pedit == pba->m_FNedit->qwm) {//字体名称
            
            if (pba->ctrl) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
                QProgress ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                std::wstring ws = (wchar_t*)pt;
                std::string str = wstring_to_string(ws);
                ctrl.SetFontName((wchar_t*)pt);

                ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("FontName"), DWORD(str.c_str()), 0);
                pqh.qwm = nullptr;

            }
        }
        else if (pedit == pba->m_FSedit->qwm) {//字体大小

            if (pba->ctrl) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
                QProgress ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                std::wstring ws = (wchar_t*)pt;
                std::string str = wstring_to_string(ws);
                int size = std::strtol(str.c_str(),nullptr,10);
                ctrl.SetFontSize(size);
          
                ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("FontSize"), DWORD(str.c_str()), 0);
                pqh.qwm = nullptr;

            }
        }

        return 0;
    }
    
    static int soliderChange(QPOINTER ct, float v)
    {
        QSlider qcb(nullptr, QRect(0, 0, 10, 10));
        qcb.qwm = (QWND*)ct;

        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            pgrAttribute* p = (pgrAttribute*)data;
            if (p) {
                QSlider* sld = (QSlider*)ct;
                if (sld == p->m_CVSd->qwm) {
                    QHost pqh(nullptr, QRect(0, 0, 10, 10));
                    pqh.qwm = p->ctrl;
                    QProgress ctrl(nullptr, QRect(0, 0, 10, 10));
                    ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                    ctrl.SetCurrentValue(v);
                    ctrl.qwm = nullptr;
                    pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("CurrentValue"), DWORD(std::format("{:f}", v).c_str()), 0);
                    pqh.qwm = nullptr;
                }


            }

        }
        qcb.qwm = nullptr;
        return 1;
    }
    static int SelectOnChange(QPOINTER ct, int index)
    {
        QSelect* qs = (QSelect*)ct;
        QSelect qst(nullptr, QRect());
        qst.qwm = qs;
        QDWORD data = 0;
        qst.FindAttribute(0, &data);
        pgrAttribute* p = (pgrAttribute*)data;
        if (p) {
            if (qs == p->m_DIRST->qwm) {//方向择
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QProgress ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                ctrl.SetDirection(index);

                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("Direction"), DWORD(std::to_string(index).c_str()), 0);
                //销毁对象，撤销运行内容
                ctrl.qwm = nullptr;
                pqh.qwm = nullptr;
            }
            else if (qs == p->m_typeST->qwm) {//样式
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QProgress ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                ctrl.SetStyle(index);

                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("Type"), DWORD(std::to_string(index).c_str()), 0);
                //销毁对象，撤销运行内容
                ctrl.qwm = nullptr;
                pqh.qwm = nullptr;
            }
            else if (qs == p->m_DPST->qwm) {//保留小数
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QProgress ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                ctrl.SetDecimalPlaces(index);

                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("DecimalPlaces"), DWORD(std::to_string(index).c_str()), 0);
                //销毁对象，撤销运行内容
                ctrl.qwm = nullptr;
                pqh.qwm = nullptr;
            }

        }
        qst.qwm = nullptr;
        return 1;

    }
    static int ColorChange(QPOINTER ct, unsigned int color)
    {
        QColorBox qcb(nullptr, QRect(0, 0, 10, 10));
        QColorBox* pcb = (QColorBox*)ct;
        qcb.qwm = (QWND*)ct;

        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            pgrAttribute* p = (pgrAttribute*)data;
            if (p) {
                if (pcb == p->m_LCCB->qwm) {//
                    QHost pqh(nullptr, QRect(0, 0, 10, 10));
                    pqh.qwm = p->ctrl;
                    QProgress ctrl(nullptr, QRect(0, 0, 10, 10));
                    ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

                    ctrl.SetLineTypeProgressColor(color);


                    ctrl.qwm = nullptr;
                    pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("LineTypeColor"), DWORD(std::format("0x{:x}", color).c_str()), 0);
                    pqh.qwm = nullptr;
                }
                else  if ((pcb == p->m_RCCB1->qwm) || (pcb == p->m_RCCB2->qwm)) {
                    QHost pqh(nullptr, QRect(0, 0, 10, 10));
                    pqh.qwm = p->ctrl;
                    QProgress ctrl(nullptr, QRect(0, 0, 10, 10));
                    ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                    unsigned int c1=p->m_RCCB1->GetColor();
                    unsigned int c2 = p->m_RCCB2->GetColor();
                    ctrl.SetRingProgressColor(c1, c2);


                    ctrl.qwm = nullptr;
                    pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("RingTypeColor1"), DWORD(std::format("0x{:x}", c1).c_str()), 0);
                    pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("RingTypeColor2"), DWORD(std::format("0x{:x}", c2).c_str()), 0);
                    pqh.qwm = nullptr;
                }
                else if (pcb == p->m_FCCB->qwm) {//字体颜色
                    QHost pqh(nullptr, QRect(0, 0, 10, 10));
                    pqh.qwm = p->ctrl;
                    QProgress ctrl(nullptr, QRect(0, 0, 10, 10));
                    ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

                    ctrl.SetFontColor(color);


                    ctrl.qwm = nullptr;
                    pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("FontColor"), DWORD(std::format("0x{:x}", color).c_str()), 0);
                    pqh.qwm = nullptr;
                
                }

            }

        }



        qcb.qwm = nullptr;
        return 1;
    }


};
//QColorBox属性
class cbxAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_FNqhl = nullptr;
    QHLayout* m_FSqhl = nullptr;
    QHLayout* m_FUNqhl = nullptr;


    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_FUNqst = nullptr;


    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;
    QEdit* m_FUNedit = nullptr;

    cbxAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev,int w=72)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, w, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring text(L"fontName:");
    
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, text, FNChange);
        text = L"fontSize:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, text, FSChange);
        text = L"颜色改变事件:";
        createCtr(&m_FUNqst, &m_FUNedit, &m_FUNqhl, text, FUNChange,96);

    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QStaticText ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("fontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
            else if ("ColorChangeEvent" == key) {
                m_FUNedit->SetText(stringToWString(value).c_str());
            }
        }
      


        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {

        m_FNedit->SetText(L"黑体");
        m_FSedit->SetText(L"16");
        m_FUNedit->SetText(L"");
    }

    static int FNChange(int p, int pt, int param)
    {
        cbxAttribute* pba = (cbxAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QColorBox ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontName((wchar_t*)pt);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontName"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FSChange(int p, int pt, int param)
    {
        cbxAttribute* pba = (cbxAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QColorBox ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontSize(std::strtod(wstring_to_string((wchar_t*)pt).c_str(), NULL));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontSize"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FUNChange(int p, int pt, int param)
    {
        cbxAttribute* pba = (cbxAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;

            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("ColorChangeEvent"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }





};
//QSelect属性
class sltAttribute
{
public:
    QWND* ctrl = nullptr;

    QHLayout* m_FNqhl = nullptr;//字体名字
    QHLayout* m_FSqhl = nullptr;//字体大小
    QHLayout* m_FCqhl = nullptr;//字体颜色
    QHLayout* m_PDHqhl = nullptr;//弹框高度
    QHLayout* m_PDWqhl = nullptr;//弹框宽度
    QHLayout* m_IHqhl = nullptr;//项目的高度
    QHLayout* m_IFSqhl = nullptr;//项目字体大小
    QHLayout* m_CIqhl = nullptr;//当前项目
    QHLayout* m_RMIqhl = nullptr;//移除项目
    QHLayout* m_AddIqhl = nullptr;//移除项目
    QHLayout* m_FUNqhl = nullptr;//选中项目事件


    QStaticText* m_FNqst = nullptr;
    QStaticText* m_FUNqst = nullptr;
    QStaticText* m_FSqst = nullptr;
    QStaticText* m_FCqst = nullptr;
    QStaticText* m_PDHqst = nullptr;
    QStaticText* m_PDWqst = nullptr;
    QStaticText* m_IHqst = nullptr;
    QStaticText* m_IFSqst = nullptr;
    QStaticText* m_CIqst = nullptr;
    QStaticText* m_AddIqst = nullptr;//添加项目，这个单独一行
    QStaticText* m_AddIBKqst = nullptr;//添加项目，这个单独一行
    QStaticText* m_AddIFontqst = nullptr;//添加项目，这个单独一行

    QEdit* m_FNedit = nullptr;
    QEdit* m_FSedit = nullptr;
    QEdit* m_FUNedit = nullptr;

    QEdit* m_PDHedit = nullptr;
    QEdit* m_PDWedit = nullptr;
    QEdit* m_IHedit = nullptr;
    QEdit* m_IFSedit = nullptr;
    QEdit* m_RMIedit = nullptr;
    QEdit* m_AddIedit = nullptr;//添加项目
    QEdit* m_AddIIedit = nullptr;//添加项目

    QSelect* m_CIslt = nullptr;
    QSelect* m_AddIslt = nullptr;//添加项目渐变方向，单独一行
    QSelect* m_AddIFontslt = nullptr;//添加项目渐变方向，单独一行

    QColorBox* m_AddIcb1 = nullptr;//添加项目颜色1，单独一行
    QColorBox* m_AddIcb2 = nullptr;//添加项目颜色2，单独一行
    QColorBox* m_AddIFontcb1 = nullptr;//添加项目颜色1，单独一行
    QColorBox* m_AddIFontcb2 = nullptr;//添加项目颜色2，单独一行
    QColorBox* m_FCcb2 = nullptr;

    QButton2* m_AddIbt2 = nullptr;//添加项目按钮，单独一行
    QButton2* m_RMIbt2 = nullptr;

    sltAttribute() {

        auto createCtr = [this](QStaticText** qst, QEdit** qe, QHLayout** qhl, std::wstring& text, QEDIT_CHANGE_EVENT ev,int stW=72)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, stW, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*qe) = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
            (*qe)->SetChangeEvent(ev, (int)this);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*qe), 1);

        };
        std::wstring text(L"fontName:");
        createCtr(&m_FNqst, &m_FNedit, &m_FNqhl, text, FNChange);
        text = L"fontSize:";
        createCtr(&m_FSqst, &m_FSedit, &m_FSqhl, text, FSChange);
        text = L"选中项目事件:";
        createCtr(&m_FUNqst, &m_FUNedit, &m_FUNqhl, text, editChange,110);

        text = L"fontColor:";
        m_FCqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
        m_FCqst->SetBKColor(0xfff0f0f0);
        m_FCqst->SetText(text.c_str());

        m_FCcb2 = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 100));
        m_FCcb2->RecordAttribute(0,(long long)this);
        m_FCcb2->SetColorChangeEvent(ColorChange);
        m_FCcb2->SetFontSize(16);

        m_FCqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
        m_FCqhl->InsertCtrl(m_FCqst, 0);
        m_FCqhl->InsertCtrl(m_FCcb2, 1);
     

        text = L"弹框高度:";
        createCtr(&m_PDHqst, &m_PDHedit, &m_PDHqhl, text, editChange,96);
        text = L"弹框宽度:";
        createCtr(&m_PDWqst, &m_PDWedit, &m_PDWqhl, text, editChange,96);
        text = L"项目的高度:";
        createCtr(&m_IHqst, &m_IHedit, &m_IHqhl, text, editChange,120);
        text = L"项目字体大小:";
        createCtr(&m_IFSqst, &m_IFSedit, &m_IFSqhl, text, editChange,120);
        auto createSLT = [this](QStaticText** qst, QSelect** st, QHLayout** qhl, std::wstring& text, QSELECT_SELECTCHANGE ev)
        {

            *qst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 72, 100));
            (*qst)->SetBKColor(0xfff0f0f0);
            (*qst)->SetText(text.c_str());

            (*st) = (QSelect*)QFUSE::CreateQWND("QFSelect", QRect(0, 0, 50, 100));
            (*st)->RecordAttribute(0,(long long)this);
            (*st)->SetSelectChangeEvent(ev);

            (*qhl) = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 50, 32));
            (*qhl)->InsertCtrl((*qst), 0);
            (*qhl)->InsertCtrl((*st), 1);

        };
        text = L"当前项目:";
        createSLT(&m_CIqst,&m_CIslt,&m_CIqhl,text, SelectOnChange);

        text = L"索引 | 内容";
        m_AddIqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 50, 32));
        m_AddIqst->SetBKColor(0xfff0f0f0);
        m_AddIqst->SetText(text.c_str());

        m_AddIedit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 32));
        m_AddIIedit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
        m_AddIqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 72, 32));
        m_AddIqhl->InsertCtrl(m_AddIIedit, 0);
        m_AddIqhl->InsertCtrl(m_AddIedit, 1);

        text = L"背景颜色";
        m_AddIBKqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 50, 32));
        m_AddIBKqst->SetBKColor(0xfff0f0f0);
        m_AddIBKqst->SetText(text.c_str());

        m_AddIcb1 = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 24));
        m_AddIcb1->SetFontSize(16);
        m_AddIcb2 = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 24));
        m_AddIcb2->SetFontSize(16);
        m_AddIslt = (QSelect*)QFUSE::CreateQWND("QFSelect", QRect(0, 0, 50, 32));
        m_AddIslt->InsertItem(L"水平渐变");
        m_AddIslt->InsertItem(L"垂直渐变");

        text = L"字符颜色";
        m_AddIFontqst = (QStaticText*)QFUSE::CreateQWND("QFStaticText", QRect(0, 0, 50, 32));
        m_AddIFontqst->SetBKColor(0xfff0f0f0);
        m_AddIFontqst->SetText(text.c_str());

        m_AddIFontcb1 = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 24));
        m_AddIFontcb1->SetFontSize(16);
        m_AddIFontcb2 = (QColorBox*)QFUSE::CreateQWND("QFColorBox", QRect(0, 0, 50, 24));
        m_AddIFontcb2->SetFontSize(16);
        m_AddIFontslt = (QSelect*)QFUSE::CreateQWND("QFSelect", QRect(0, 0, 50, 32));
        m_AddIFontslt->InsertItem(L"水平渐变");
        m_AddIFontslt->InsertItem(L"垂直渐变");


        text = L"增加项目";
        m_AddIbt2 = (QButton2*)QFUSE::CreateQWND("QFPButton", QRect(0, 0, 50, 32));
        m_AddIbt2->SetClickEvent(BTOnClick);
        m_AddIbt2->RecordAttribute(0,(long long)this);
        m_AddIbt2->SetText(text.c_str());



        text = L"移除项目";
        m_RMIbt2 = (QButton2*)QFUSE::CreateQWND("QFPButton", QRect(0, 0, 96, 32));
        m_RMIbt2->SetClickEvent(BTOnClick);
        m_RMIbt2->RecordAttribute(0, (long long)this);
        m_RMIbt2->SetText(text.c_str());

        m_RMIedit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(0, 0, 50, 100));
        m_RMIedit->SetChangeEvent(editChange, (int)this);

        m_RMIqhl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", QRect(0, 0, 72, 32));
        m_RMIqhl->InsertCtrl(m_RMIbt2, 0);
        m_RMIqhl->InsertCtrl(m_RMIedit, 1);

    }
    void ReadAttribute()
    {
        QHost host(nullptr, QRect(0, 0, 10, 10));
        host.qwm = ctrl;
        QSelect ctrl(nullptr, QRect(0, 0, 10, 10));
        ctrl.qwm = host.GetParasiticCtrl();
        QRect rect = ctrl.GetRect();
        int size = host.TransferMessage(QM_HOSTCTRL_GET_PAIR, 0, 0, 0);
        for (int j = -1; ++j < size;) {
            char* k = nullptr;
            char* v = nullptr;
            host.TransferMessage(QM_HOSTCTRL_GET_PAIR, QDWORD(&k), QDWORD(&v), j);
            std::string key(k);
            std::string value(v);
            if ("fontName" == key) {
                m_FNedit->SetText(stringToWString(value).c_str());
            }
            else if ("fontSize" == key) {
                m_FSedit->SetText(stringToWString(value).c_str());
            }
            else if ("FontColor" == key) {
                int para = 10;
                if ('x' == std::tolower(v[1]))para = 16;
                unsigned long color = std::strtoul(v, nullptr, para);

                m_FCcb2->SetColor(color);
            }
            else if ("PopDialogHeight" == key) {
                m_PDHedit->SetText(stringToWString(value).c_str());
            }
            else if ("PopDialogWidth" == key) {
                m_PDWedit->SetText(stringToWString(value).c_str());
            }
            else if ("ItemHeight" == key) {
                m_IHedit->SetText(stringToWString(value).c_str());
            }
            else if ("ItemFontSize" == key) {
                m_IFSedit->SetText(stringToWString(value).c_str());
            }
            else if ("SelectChangeEvent" == key) {
                m_FUNedit->SetText(stringToWString(value).c_str());
            }
        }
        int count = ctrl.GetItemCount();
        for (int i = -1; ++i < count;) {

            m_CIslt->InsertItem(std::format(L"第{:d}项", i).c_str());
        }

        m_CIslt->SetCurrentItem(ctrl.GetIndex());


        host.qwm = nullptr;
        ctrl.qwm = nullptr;
    }
    void Clear()
    {
        m_FNedit->SetText(L"黑体");
        m_FSedit->SetText(L"16");
        m_PDHedit->SetText(L"180");
        m_PDWedit->SetText(L"90");
        m_IHedit->SetText(L"32");
        m_IFSedit->SetText(L"16");
        m_RMIedit->SetText(L"0");
        m_AddIIedit->SetText(L"-1");
        m_FUNedit->SetText(L"");
        int count =m_CIslt->GetItemCount();
        for (int i = -1; ++i < count;) {
            m_CIslt->RemoveItem(0);
        }
        m_AddIslt->SetCurrentItem(0);

        m_AddIFontslt->SetCurrentItem(0);
        m_AddIcb1 ->SetColor(0xffffffff);
        m_AddIcb2->SetColor(0xffffffff);
        m_AddIFontcb1->SetColor(0xff000000);
        m_AddIFontcb2->SetColor(0xff000000);
        m_FCcb2->SetColor(0xff000000);
    
    }

    static void BTOnClick(void* addr)
    {
        QButton2* bt2 = (QButton2*)addr;
        QButton2 qbt2(nullptr, QRect());
        qbt2.qwm = bt2;
        QDWORD data = 0;
        qbt2.FindAttribute(0, &data);
        sltAttribute* p = (sltAttribute*)data;
        if (p) {

            if (bt2 == p->m_AddIbt2->qwm) {//增加分段
                //先拿到编辑框里面的内容
                std::wstring text = p->m_AddIedit->GetText();
                std::string str = wstring_to_string(text);

                std::wstring texti = p->m_AddIIedit->GetText();
                std::string stri = wstring_to_string(texti);

                int index = std::strtol(stri.c_str(),nullptr,10);
               
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QSelect ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                ctrl.InsertItem(text.c_str(), index);
                unsigned int c1 = p->m_AddIcb1->GetColor();
                unsigned int c2 = p->m_AddIcb2->GetColor();
                int d = p->m_AddIslt->GetIndex();
                ctrl.SetItemBKColor(index,c1, c2, d);
                c1 = p->m_AddIFontcb1->GetColor();
                c2 = p->m_AddIFontcb2->GetColor();
                d = p->m_AddIFontslt->GetIndex();
                ctrl.SetItemFontColor(index, c1, c2, d);
                p->m_CIslt->InsertItem(std::format(L"第{:d}项", p->m_CIslt->GetItemCount()).c_str());
                //销毁对象，撤销运行内容
                ctrl.qwm = nullptr;
                pqh.qwm = nullptr;

            }




        }
        qbt2.qwm = nullptr;
    }

    static int FNChange(int p, int pt, int param)
    {
        sltAttribute* pba = (sltAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QSelect ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontName((wchar_t*)pt);
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontName"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int FSChange(int p, int pt, int param)
    {
        sltAttribute* pba = (sltAttribute*)param;
        if (pba->ctrl) {
            QHost pqh(nullptr, QRect(0, 0, 10, 10));
            pqh.qwm = pba->ctrl;
            QSelect ctrl(nullptr, QRect(0, 0, 10, 10));
            ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
            ctrl.SetFontSize(std::strtod(wstring_to_string((wchar_t*)pt).c_str(), NULL));
            ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
            ctrl.qwm = nullptr;
            pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("fontSize"), DWORD(wstring_to_string((wchar_t*)pt).c_str()), 0);
            pqh.qwm = nullptr;

        }
        return 0;
    }
    static int editChange(int p, int pt, int param)
    {
        sltAttribute* pba = (sltAttribute*)param;
        QEdit* pedit = (QEdit*)p;
        if (pedit == pba->m_PDHedit->qwm) {//弹窗高度
            if (pba->ctrl) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
                QSelect ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                std::wstring ws = (wchar_t*)pt;
                std::string str = wstring_to_string(ws);
                int v = std::strtol(str.c_str(), nullptr,10);
                ctrl.SetPopDialogHeight(v);

                ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("PopDialogHeight"), DWORD(str.c_str()), 0);
                pqh.qwm = nullptr;

            }

        }
        else if (pedit == pba->m_PDWedit->qwm) {//弹窗宽度

            if (pba->ctrl) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
                QSelect ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                std::wstring ws = (wchar_t*)pt;
                std::string str = wstring_to_string(ws);
                int v = std::strtol(str.c_str(), nullptr, 10);
                ctrl.SetPopDialogWidth(v);

                ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("PopDialogWidth"), DWORD(str.c_str()), 0);
                pqh.qwm = nullptr;

            }
        }
        else if (pedit == pba->m_IHedit->qwm) {//项目高度

            if (pba->ctrl) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
                QSelect ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                std::wstring ws = (wchar_t*)pt;
                std::string str = wstring_to_string(ws);
                int h = std::strtol(str.c_str(), nullptr, 10);
                ctrl.SetItemHeight(h);

                ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("ItemHeight"), DWORD(str.c_str()), 0);
                pqh.qwm = nullptr;

            }
        }
        else if (pedit == pba->m_IFSedit->qwm) {//项目字体大小

            if (pba->ctrl) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
                QSelect ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                std::wstring ws = (wchar_t*)pt;
                std::string str = wstring_to_string(ws);
                int h = std::strtol(str.c_str(), nullptr, 10);
                ctrl.SetItemFontSize(h);

                ctrl.TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
                ctrl.qwm = nullptr;
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("ItemFontSize"), DWORD(str.c_str()), 0);
                pqh.qwm = nullptr;

            }
        }
        else if (pedit == pba->m_FUNedit->qwm) {//项目被选中事件

            if (pba->ctrl) {
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = pba->ctrl;
                std::wstring ws = (wchar_t*)pt;
                std::string str = wstring_to_string(ws);
                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("SelectChangeEvent"), DWORD(str.c_str()), 0);
                pqh.qwm = nullptr;

            }
        }
        return 0;
    }
    static int SelectOnChange(QPOINTER ct, int index)
    {
        QSelect* qs = (QSelect*)ct;
        QSelect qst(nullptr, QRect());
        qst.qwm = qs;
        QDWORD data = 0;
        qst.FindAttribute(0, &data);
        sltAttribute* p = (sltAttribute*)data;
        if (p) {
            if (qs == p->m_CIslt->qwm) {//方向择
                QHost pqh(nullptr, QRect(0, 0, 10, 10));
                pqh.qwm = p->ctrl;
                QSelect ctrl(nullptr, QRect(0, 0, 10, 10));
                ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();
                ctrl.SetCurrentItem(index);

                pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("CurrentItem"), DWORD(std::to_string(index).c_str()), 0);
                //销毁对象，撤销运行内容
                ctrl.qwm = nullptr;
                pqh.qwm = nullptr;
            }
           

        }
        qst.qwm = nullptr;
        return 1;

    }
    static int ColorChange(QPOINTER ct, unsigned int color)
    {
        QColorBox qcb(nullptr, QRect(0, 0, 10, 10));
        QColorBox* pcb = (QColorBox*)ct;
        qcb.qwm = (QWND*)ct;
 
        if (ct) {
            QDWORD data = 0;
            qcb.FindAttribute(0, &data);
            sltAttribute* p = (sltAttribute*)data;
            if (p) {
                if (pcb == p->m_FCcb2->qwm) {//
                    QHost pqh(nullptr, QRect(0, 0, 10, 10));
                    pqh.qwm = p->ctrl;
                    QSelect ctrl(nullptr, QRect(0, 0, 10, 10));
                    ctrl.qwm = (DLLImportQWND*)pqh.GetParasiticCtrl();

                    ctrl.SetFontColor(color);


                    ctrl.qwm = nullptr;
                    pqh.TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD("FontColor"), DWORD(std::format("0x{:x}", color).c_str()), 0);
                    pqh.qwm = nullptr;
                }
              

            }

        }



        qcb.qwm = nullptr;
        return 1;
    }


};

bAttribute* g_pba =nullptr;
btAttribute* g_pbta = nullptr;
bt2Attribute* g_pbt2a = nullptr;
edAttribute* g_peda = nullptr;
rfAttribute* g_prfa = nullptr;
picAttribute* g_pica = nullptr;
pbAttribute* g_ppba = nullptr;
lcAttribute* g_plca = nullptr;
tabeAttribute* g_ptabea = nullptr;
catlbcAttribute* g_pcatlbca = nullptr;
catltcAttribute* g_pcatltca = nullptr;
qfdAttribute* g_pqfda = nullptr;
stAttribute* g_psta = nullptr;
qsAttribute* g_pqsa = nullptr;
swAttribute* g_pswa = nullptr;
pgrAttribute* g_ppgra = nullptr;
cbxAttribute* g_pcbta = nullptr;
sltAttribute* g_pslta = nullptr;
#endif // DEBUG
void BindCtrlBaseAttribute(int ctrl)
{
    g_pba->ctrl = (QWND*)ctrl;
    g_pba->ReadAttribute();
    //分别加入，x，y，宽度，高度，名字
    qct->InsertAttributeCtrl(g_pba->m_Xqhl);
    qct->InsertAttributeCtrl(g_pba->m_Yqhl);
    qct->InsertAttributeCtrl(g_pba->m_Wqhl);
    qct->InsertAttributeCtrl(g_pba->m_Hqhl);
    qct->InsertAttributeCtrl(g_pba->m_Nameqhl);


}


void BindCtrlAttribute(int ctrl)
{
   

    BindCtrlBaseAttribute(ctrl);
    if ((QWND*)ctrl != g_qew->qwm) {
        QHost qhost(nullptr, QRect(0, 0, 10, 10));
        qhost.qwm = (QWND*)ctrl;
        int cl = (int)qhost.GetParasiticCtrl();
        auto it = g_allCtrl.find(cl);
        if (it != g_allCtrl.end()) {
            QFUSE_EW::CtrlAttribute ca = it->second;
            if ("QFButton" == ca.createT) {
                g_pbta->Clear();
                g_pbta->ctrl = (QWND*)ctrl;
                g_pbta->ReadAttribute();
                qct->InsertAttributeCtrl(g_pbta->m_TEXTqhl);
                qct->InsertAttributeCtrl(g_pbta->m_Clickqhl);
                qct->InsertAttributeCtrl(g_pbta->m_FNqhl);
                qct->InsertAttributeCtrl(g_pbta->m_FSqhl);
                

            }
            else if ("QFPButton" == ca.createT) {
                g_pbt2a->Clear();
                g_pbt2a->ctrl = (QWND*)ctrl;
                g_pbt2a->ReadAttribute();
                qct->InsertAttributeCtrl(g_pbt2a->m_TEXTqhl);
                qct->InsertAttributeCtrl(g_pbt2a->m_Clickqhl);
                qct->InsertAttributeCtrl(g_pbt2a->m_FNqhl);
                qct->InsertAttributeCtrl(g_pbt2a->m_FSqhl);
                qct->InsertAttributeCtrl(g_pbt2a->m_BKCqhl);
                qct->InsertAttributeCtrl(g_pbt2a->m_FCqhl);

            }
            else if ("QFEdit" == ca.createT) {
                g_peda->Clear();
                g_peda->ctrl = (QWND*)ctrl;
                g_peda->ReadAttribute();
                qct->InsertAttributeCtrl(g_peda->m_TEXTqhl);
                qct->InsertAttributeCtrl(g_peda->m_FNqhl);
                qct->InsertAttributeCtrl(g_peda->m_FSqhl);
                qct->InsertAttributeCtrl(g_peda->m_FUNqhl);
            }
            else if ("QFReportForm" == ca.createT) {
                g_prfa->Clear();
                g_prfa->ctrl = (QWND*)ctrl;
                g_prfa->ReadAttribute();
                qct->InsertAttributeCtrl(g_prfa->m_FNqhl);
                qct->InsertAttributeCtrl(g_prfa->m_FSqhl);
                qct->InsertAttributeCtrl(g_prfa->m_THqhl); 
                qct->InsertAttributeCtrl(g_prfa->m_Dragqhl);
                qct->InsertAttributeCtrl(g_prfa->m_TCqhl);
                qct->InsertAttributeCtrl(g_prfa->m_AddTqhl);
                qct->InsertAttributeCtrl(g_prfa->m_RMTqhl);
                qct->InsertAttributeCtrl(g_prfa->m_UDTTqhl);
                qct->InsertAttributeCtrl(g_prfa->m_UDTWqhl);
            }
            else if ("QFPictureBox" == ca.createT) {
                g_pica->Clear();
                g_pica->ctrl = (QWND*)ctrl;
                g_pica->ReadAttribute();
                qct->InsertAttributeCtrl(g_pica->m_TSqhl);
                qct->InsertAttributeCtrl(g_pica->m_Pathqhl);
            
            }
            else if ("QFProgressBarCar" == ca.createT) {
                g_ppba->Clear();
                g_ppba->ctrl = (QWND*)ctrl;
                g_ppba->ReadAttribute();
                qct->InsertAttributeCtrl(g_ppba->m_POSqhl);
                qct->InsertAttributeCtrl(g_ppba->m_FNqhl);
                qct->InsertAttributeCtrl(g_ppba->m_FSqhl);
                qct->InsertAttributeCtrl(g_ppba->m_FCqhl);

            }
            else if ("QFLineChart" == ca.createT) {
                g_plca->Clear();
                g_plca->ctrl = (QWND*)ctrl;
                g_plca->ReadAttribute();
                qct->InsertAttributeCtrl(g_plca->m_FNqhl);
                qct->InsertAttributeCtrl(g_plca->m_FSqhl);
                qct->InsertAttributeCtrl(g_plca->m_LFNqhl);
                qct->InsertAttributeCtrl(g_plca->m_LFSqhl);
                qct->InsertAttributeCtrl(g_plca->m_XASHqhl);
                qct->InsertAttributeCtrl(g_plca->m_YASHqhl);
                qct->InsertAttributeCtrl(g_plca->m_XASWqhl);
                qct->InsertAttributeCtrl(g_plca->m_YASWqhl);
                qct->InsertAttributeCtrl(g_plca->m_LWqhl);
                qct->InsertAttributeCtrl(g_plca->m_LEGWqhl);
                qct->InsertAttributeCtrl(g_plca->m_Tipqhl);
                qct->InsertAttributeCtrl(g_plca->m_XARqhl);
                qct->InsertAttributeCtrl(g_plca->m_YARqhl);
            }
            else if ("QFTabs" == ca.createT) {
                g_ptabea->Clear();
                g_ptabea->ctrl = (QWND*)ctrl;
                g_ptabea->ReadAttribute();
                qct->InsertAttributeCtrl(g_ptabea->m_FNqhl);
                qct->InsertAttributeCtrl(g_ptabea->m_FSqhl);
                qct->InsertAttributeCtrl(g_ptabea->m_FCqhl);
                qct->InsertAttributeCtrl(g_ptabea->m_TXTqhl);
                qct->InsertAttributeCtrl(g_ptabea->m_ICOqhl);
                qct->InsertAttributeCtrl(g_ptabea->m_TIPqhl);
                qct->InsertAttributeCtrl(g_ptabea->m_Addqhl);
                qct->InsertAttributeCtrl(g_ptabea->m_RMqhl);
             
            }
            else if ("QFCATLBarChart" == ca.createT) {
                g_pcatlbca->Clear();
                g_pcatlbca->ctrl = (QWND*)ctrl;
                g_pcatlbca->ReadAttribute();
                qct->InsertAttributeCtrl(g_pcatlbca->m_FNqhl);
                qct->InsertAttributeCtrl(g_pcatlbca->m_FSqhl);
                qct->InsertAttributeCtrl(g_pcatlbca->m_FCqhl);
                qct->InsertAttributeCtrl(g_pcatlbca->m_BCWqhl);
                qct->InsertAttributeCtrl(g_pcatlbca->m_GWqhl);
                qct->InsertAttributeCtrl(g_pcatlbca->m_LWqhl);
                qct->InsertAttributeCtrl(g_pcatlbca->m_SHqhl);
                qct->InsertAttributeCtrl(g_pcatlbca->m_SMAXqhl);
                qct->InsertAttributeCtrl(g_pcatlbca->m_SMINqhl);
                qct->InsertAttributeCtrl(g_pcatlbca->m_SWqhl);
                qct->InsertAttributeCtrl(g_pcatlbca->m_YASWqhl);
            }
            else if ("QFCATLTempeChart" == ca.createT) {
                g_pcatltca->Clear();
                g_pcatltca->ctrl = (QWND*)ctrl;
                g_pcatltca->ReadAttribute();
                qct->InsertAttributeCtrl(g_pcatltca->m_FNqhl);
                qct->InsertAttributeCtrl(g_pcatltca->m_FSqhl);
                qct->InsertAttributeCtrl(g_pcatltca->m_UFSqhl);
                qct->InsertAttributeCtrl(g_pcatltca->m_AWqhl);
                qct->InsertAttributeCtrl(g_pcatltca->m_UIWqhl);
                qct->InsertAttributeCtrl(g_pcatltca->m_UWqhl);
            }
            else if ("QFDrawer" == ca.createT) {
                g_pqfda->Clear();
                g_pqfda->ctrl = (QWND*)ctrl;
                g_pqfda->ReadAttribute();
                qct->InsertAttributeCtrl(g_pqfda->m_FNqhl);
                qct->InsertAttributeCtrl(g_pqfda->m_FSqhl);
                qct->InsertAttributeCtrl(g_pqfda->m_OSqhl);
                qct->InsertAttributeCtrl(g_pqfda->m_AddDTipqhl);
                qct->InsertAttributeCtrl(g_pqfda->m_AddDqhl);
                qct->InsertAttributeCtrl(g_pqfda->m_RMDqhl);
                qct->InsertAttributeCtrl(g_pqfda->m_AddSTipqhl);
                qct->InsertAttributeCtrl(g_pqfda->m_AddSqhl);
                qct->InsertAttributeCtrl(g_pqfda->m_RMSqhl);


            }
            else if ("QFStaticText" == ca.createT) {
                g_psta->Clear();
                g_psta->ctrl = (QWND*)ctrl;
                g_psta->ReadAttribute();
                qct->InsertAttributeCtrl(g_psta->m_TEXTqhl);
                qct->InsertAttributeCtrl(g_psta->m_FNqhl);
                qct->InsertAttributeCtrl(g_psta->m_FSqhl);
                qct->InsertAttributeCtrl(g_psta->m_FCqhl);
                qct->InsertAttributeCtrl(g_psta->m_BKCqhl);



            }
            else if ("QFSlider" == ca.createT) {
                g_pqsa->Clear();
                g_pqsa->ctrl = (QWND*)ctrl;
                g_pqsa->ReadAttribute();
                qct->InsertAttributeCtrl(g_pqsa->m_minqhl);
                qct->InsertAttributeCtrl(g_pqsa->m_maxqhl);
                qct->InsertAttributeCtrl(g_pqsa->m_CVqhl);
                qct->InsertAttributeCtrl(g_pqsa->m_SLIDqhl);
                qct->InsertAttributeCtrl(g_pqsa->m_RCqhl);
                qct->InsertAttributeCtrl(g_pqsa->m_BKCqhl);
                qct->InsertAttributeCtrl(g_pqsa->m_RRqhl);
                qct->InsertAttributeCtrl(g_pqsa->m_IsSegqhl);
                qct->InsertAttributeCtrl(g_pqsa->m_DIRqhl);
                qct->InsertAttributeCtrl(g_pqsa->m_AddSegqhl);
                qct->InsertAttributeCtrl(g_pqsa->m_RMSegqhl);


            }
            else if ("QFSwitch" == ca.createT) {
                g_pswa->Clear();
                g_pswa->ctrl = (QWND*)ctrl;
                g_pswa->ReadAttribute();
                qct->InsertAttributeCtrl(g_pswa->m_SCEqhl);
                qct->InsertAttributeCtrl(g_pswa->m_Stateqhl);
                qct->InsertAttributeCtrl(g_pswa->m_MTXqhl);

            }
            else if ("QFProgress" == ca.createT) {
                g_ppgra->Clear();
                g_ppgra->ctrl = (QWND*)ctrl;
                g_ppgra->ReadAttribute();
                qct->InsertAttributeCtrl(g_ppgra->m_FNqhl);
                qct->InsertAttributeCtrl(g_ppgra->m_FSqhl);
                qct->InsertAttributeCtrl(g_ppgra->m_FCqhl);
                qct->InsertAttributeCtrl(g_ppgra->m_TKqhl);
                qct->InsertAttributeCtrl(g_ppgra->m_CVqhl);
                qct->InsertAttributeCtrl(g_ppgra->m_DIRqhl);
                qct->InsertAttributeCtrl(g_ppgra->m_DPqhl);
                qct->InsertAttributeCtrl(g_ppgra->m_typeqhl);
                qct->InsertAttributeCtrl(g_ppgra->m_LCqhl);
                qct->InsertAttributeCtrl(g_ppgra->m_RCqhl);

            }
            else if ("QFColorBox" == ca.createT) {
                g_pcbta->Clear();
                g_pcbta->ctrl = (QWND*)ctrl;
                g_pcbta->ReadAttribute();
                qct->InsertAttributeCtrl(g_pcbta->m_FNqhl);
                qct->InsertAttributeCtrl(g_pcbta->m_FSqhl);
                qct->InsertAttributeCtrl(g_pcbta->m_FUNqhl);
            }
            else if ("QFSelect" == ca.createT) {
                g_pslta->Clear();
                g_pslta->ctrl = (QWND*)ctrl;
                g_pslta->ReadAttribute();
                qct->InsertAttributeCtrl(g_pslta->m_FNqhl);
                qct->InsertAttributeCtrl(g_pslta->m_FSqhl);
                qct->InsertAttributeCtrl(g_pslta->m_FCqhl);
                qct->InsertAttributeCtrl(g_pslta->m_FUNqhl);
                qct->InsertAttributeCtrl(g_pslta->m_PDHqhl);
                qct->InsertAttributeCtrl(g_pslta->m_PDWqhl);
                qct->InsertAttributeCtrl(g_pslta->m_IHqhl);
                qct->InsertAttributeCtrl(g_pslta->m_IFSqhl);
                qct->InsertAttributeCtrl(g_pslta->m_CIqhl);
                qct->InsertAttributeCtrl(g_pslta->m_AddIqst);
                qct->InsertAttributeCtrl(g_pslta->m_AddIqhl);
                qct->InsertAttributeCtrl(g_pslta->m_AddIBKqst);
                qct->InsertAttributeCtrl(g_pslta->m_AddIcb1);
                qct->InsertAttributeCtrl(g_pslta->m_AddIcb2);
                qct->InsertAttributeCtrl(g_pslta->m_AddIslt);
                qct->InsertAttributeCtrl(g_pslta->m_AddIFontqst);
                qct->InsertAttributeCtrl(g_pslta->m_AddIFontcb1);
                qct->InsertAttributeCtrl(g_pslta->m_AddIFontcb2);
                qct->InsertAttributeCtrl(g_pslta->m_AddIFontslt);
                qct->InsertAttributeCtrl(g_pslta->m_AddIbt2);


                qct->InsertAttributeCtrl(g_pslta->m_RMIqhl);
            }
        }
        qct->TransferMessage(QM_UPDATE_CTRL, 1, 0, 0);
        qhost.qwm = nullptr;
    }

}





int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

#ifdef _DEBUG



    AllocConsole();
    FILE* stream;
    freopen_s(&stream, "CON", "r", stdin);//重定向输入流
    freopen_s(&stream, "CON", "w", stdout);//重定向输入流
#endif 






    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 注册窗口类


    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc; // 窗口过程函数
    wc.hInstance = hInstance;
    wc.lpszClassName = g_qfw.GetWindowsClass();
    wc.style = CS_DBLCLKS;

    RegisterClass(&wc);

    // 创建窗口
    HWND hwnd = CreateWindowEx(
        0,                              // 扩展风格
        wc.lpszClassName,                     // 窗口类名
        g_qfw.GetWindowsTitle(),                // 窗口标题
        WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,            // 窗口样式
        CW_USEDEFAULT, CW_USEDEFAULT,   // 初始位置
        1500, 900,   // 初始大小
        NULL,                           // 父窗口
        NULL,                           // 菜单
        hInstance,                      // 应用程序实例句柄
        NULL                            // 其他参数
    );


    if (hwnd == NULL) {
        return 0;
    }


    // 显示窗口
    ShowWindow(hwnd, nCmdShow);


    QFUSE::BindPanelWindow(hwnd);
    QWMange* pbuff = (QWMange*)QFUSE::GetQWManger();
    //pbuff->SetBKBrush(NULL);
    pbuff->SetBKPicture(L"./picture.jpg");
    qct = (QCtrlToolbox*)QFUSE::CreateQWND("QFEDR_CtrlToolBox", QRect(10, 10, 300, 50));
    qct->SetFileButtonEvent(OpenBTClickEvent,0);
    qct->SetFileButtonEvent(SavaBTClickEvent, 1);
    

    QFUSE::AttachCtrl(qct);
    
    QButton2* mouseBt = (QButton2*)QFUSE::CreateQWND("QFPButton", QRect(10, 10, 100, 50));
    mouseBt->SetClickEvent(BTClickEvent);
    mouseBt->SetText(L"鼠标");
    mouseBt->LoadPicture(L"mouse28x28.png");
    mouseBt->SetFontSize(16);
    qct->InsertCtrl(mouseBt, 1);

    tinyxml2::XMLDocument xmlDoc;
    tinyxml2::XMLError result = xmlDoc.LoadFile("./CtrlToolBox.xml");
    if (result == tinyxml2::XML_SUCCESS) {
        // 获取根元素
        tinyxml2::XMLElement* root = xmlDoc.RootElement();
        if (root != nullptr) {
            //判断root的name是否为CtrlToolBox
            std::string Rname = root->Name();
            if ("CtrlToolBox" == Rname) {
                // 遍历子元素
                for (tinyxml2::XMLElement* child = root->FirstChildElement("CTRL"); child != nullptr; child = child->NextSiblingElement("CTRL")) {
                    // 获取元素文本内容
                    const char* text = child->GetText();
                    if (text) {
                        std::cout << "Child content: " << text << std::endl;
                    }

                    // 获取元素的属性
                    const char* name = child->Attribute("name");
                    if (name) {
                        std::cout << "Child name: " << name << std::endl;
                    }
        
                    QButton2* qst = (QButton2*)QFUSE::CreateQWND("QFPButton", QRect(10, 10, 100, 50));
                    std::wstring btN=stringToWString(std::string(name));
                    qst->SetText(btN.c_str());
                    qct->InsertCtrl(qst, 1);
                    qst->SetFontSize(16);
                
                    qst->SetClickEvent(BTClickEvent);
                    const char* createText = child->Attribute("createText");
                    if (createText) {
                        g_ctrl[int(qst->qwm)] = createText;
                        qst->LoadPicture(stringToWString("./"+std::string(createText) + ".png").c_str());
                    }
                }

            }
        }
    }

    //加入编辑窗口
    g_qew = (QEditorWin*)QFUSE::CreateQWND("QFEDR_EditorWin", QRect(10, 10, 500, 400));
    g_qew->SetLButtonEvent(WLDownEvent);//鼠标按下事件
    g_qew->SetPlaceCtrlEvent(WPlaceCtrlEvent);//放在控件事件
    g_qew->SetDeleteCtrlEvent(DeleteCtrlEvent);//删除控件事件
    g_qew->SetSelectCtrlEvent(SelectCtrlEvent);//选中控件事件
    QFUSE::AttachCtrl(g_qew);

    bAttribute ba;
    g_pba = &ba;
    btAttribute bta;
    g_pbta =&bta;
    bt2Attribute bt2a;
    g_pbt2a = &bt2a;
    edAttribute eda;
    g_peda =&eda;
    rfAttribute rfa;
    g_prfa = &rfa;
    picAttribute pica;
    g_pica = &pica;
    pbAttribute pba;
    g_ppba = &pba;
    lcAttribute lca;
    g_plca = &lca;
    tabeAttribute tabea;
    g_ptabea = &tabea;
    catlbcAttribute catlbca;
    g_pcatlbca = &catlbca;

    catltcAttribute catltca;
    g_pcatltca = &catltca;
    qfdAttribute qfda;
    g_pqfda = &qfda;
    stAttribute sta;
    g_psta = &sta;
    qsAttribute pqsa;
    g_pqsa = &pqsa;
    swAttribute pswa;
    g_pswa = &pswa;
    pgrAttribute ppgra;
    g_ppgra = &ppgra;
    cbxAttribute pcbta;
    g_pcbta = &pcbta;
    sltAttribute pslta;
    g_pslta = &pslta;
    //QEdit* qedit = (QEdit*)QFUSE::CreateQWND("QFEdit", QRect(10, 10, 100, 50));
    //   
    //qedit->SetChangeEvent(EditChangeEvent, 100);
    //qct->InsertAttributeCtrl(qedit, 0);

    //QButton* qst = (QButton*)QFUSE::CreateQWND("QFButton", QRect(10, 10, 100, 50));
    ////qst->SetText(L"123456");
    //qct->InsertCtrl(qst,1);
    //QButton* qst2 = (QButton*)QFUSE::CreateQWND("QFButton", QRect(10, 10, 100, 50));
    //qct->InsertCtrl(qst2, 1);
    //QButton* qst3 = (QButton*)QFUSE::CreateQWND("QFButton", QRect(10, 10, 100, 50));
    //qct->InsertCtrl(qst3, 1);
    // 主消息循环
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
