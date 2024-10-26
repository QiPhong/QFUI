#include "QFUI.h"
#include <fstream>

void LoadQFUI3D()
{
    if (NULL == g_hQFUI3D) {
        std::string curPath = GetCurrentModuleAddr();
        if (!isFileExists(curPath + "QFUI3D.dll")) {
            MessageBoxA(0, "找不到QFUI3D.dll", "初始化QFUI3D", MB_OK | MB_ICONERROR);
            return;
        }
        LPCSTR fileName = "d3dx9_43.dll";
        // 获取系统目录路径
        CHAR systemDir[MAX_PATH];
        GetSystemDirectoryA(systemDir, MAX_PATH);
        // 使用 SearchPath 函数查找 DLL 文件
        DWORD result = SearchPathA(systemDir, fileName, NULL, 0, NULL, NULL);
        if (0 == result) {
            result = SearchPathA(NULL, fileName, NULL, 0, NULL, NULL);
            if (0 == result) {
                if (!isFileExists(curPath + fileName)) {
                    MessageBoxA(0, (std::string("找不到")+ fileName).c_str(), "初始化QFUI3D", MB_OK | MB_ICONERROR);
                    return;
                }
                else {
                    // 打开源文件
                    std::ifstream sourceFile(curPath + fileName, std::ios::binary);
                    if (!sourceFile) {
                        return ;
                    }

                    // 创建目标文件
                    std::ofstream destinationFile(fileName, std::ios::binary);
                    if (!destinationFile) {
                        return ;
                    }

                    // 逐个字节复制文件内容
                    destinationFile << sourceFile.rdbuf();

                    // 检查是否复制成功
                    if (!destinationFile) {
                        return ;
                    }
                
                }
            }


        }
        //稳定了就加载3D控件DLL
        g_hQFUI3D = LoadLibraryA((curPath + "QFUI3D.dll").c_str());



    }


}
extern "C" __declspec(dllexport) QWND * CreateQWND(const char* cName, HDC winddc, GRect rf)
{
    //GRect rf(x,y,w,h);
    std::string cn = std::string(cName);
    if ("QFButton" == cn) {

        QWND* p = new QButton(winddc, rf);

        return p;
    }
    else if ("QFReportForm" == cn) {
        QWND* p = new QReportForm(winddc, rf);

        return p;

    }
    else if ("QFPictureBox" == cn) {
        QWND* p = new QPictureBox(winddc, rf);

        return p;
    }
    else if ("QFEdit" == cn) {
        QWND* p = new QEdit(winddc, rf);

        return p;
    }
    else if ("QFPanelDial" == cn) {

    }
    else if ("QFProgressBarCar" == cn) {
        QWND* p = new QProgressBarCar(winddc, rf);

        return p;

    }
    else if ("QFLineChart" == cn) {
        QWND* p = new QLineChart(winddc, rf);

        return p;

    }
    else if ("QFTabs" == cn) {
        QWND* p = new QTabs(winddc, rf);

        return p;

    }
    else if ("QFCATLBarChart" == cn) {
        QWND* p = new QCATLBarChart(winddc, rf);

        return p;

    }
    else if ("QFCATLTempeChart" == cn) {
        QWND* p = new QCATLTemperatureChart(winddc, rf);
        return p;
    }
    else if ("QFDrawer" == cn) {
        
        QWND* p = new QDrawer(winddc, rf);
        return p;
    }
    else if ("QFHostCtrl" == cn) {

        QWND* p = new QHostCtrl(winddc, rf);
        return p;
    }
    else if ("QFHLayout" == cn) {

        QWND* p = new QHLayout(winddc, rf);
        return p;
    }
    else if ("QFVLayout" == cn) {

        QWND* p = new QVLayout(winddc, rf);
        return p;
    }
    else if ("QFTLayout" == cn) {

        QWND* p = new QTLayout(winddc, rf);
        return p;
    }
    else if ("QFStaticText" == cn) {

        QWND* p = new QStaticText(winddc, rf);
        return p;
    }
    else if ("QFSwitch" == cn) {
        QWND* p = new QSwitch(winddc, rf);
        return p;
    
    }
    else if ("QFSlider" == cn) {
        QWND* p = new QSlider(winddc, rf);
        return p;

    }
    else if ("QFProgress" == cn) {
        QWND* p = new QProgress(winddc, rf);
        return p;

    }
    else if ("QFPButton" == cn) {
        QWND* p = new QPButton(winddc, rf);
        return p;

    }
    else if ("QFColorBox" == cn) {
        QWND* p = new QColorBox(winddc, rf);
        return p;

    }
    else if ("QFSelect" == cn) {
        QWND* p = new QSelect(winddc, rf);
        return p;

    }
    
    //3D控件开头
    if ("QF3D" == cn.substr(0, 4)) {
        LoadQFUI3D();
        if (NULL != g_hQFUI3D) {
            typedef QWND* (*CREATEQWND3D)(const char*, HDC, GRect);
            CREATEQWND3D cq3d = (CREATEQWND3D)GetProcAddress(g_hQFUI3D, "CreateQWND3D");
            return cq3d(cName, winddc, rf);
        }

    }
    else if("QFEDR" == cn.substr(0, 5)) {//专门为UI编辑器做的
        if ("QFEDR_CtrlToolBox" == cn) {
            
            return new QCtrlToolbox(winddc, rf);;
        }
        else if ("QFEDR_EditorWin" == cn) {

            return new QEditorWin(winddc, rf);;
        }
    }



    return NULL;
}

extern "C"
__declspec(dllexport) PVOID Get3DProcAddress(const char* funName)
{
    PVOID fun = NULL;
    if (NULL != g_hQFUI3D) {
        fun = GetProcAddress(g_hQFUI3D, funName);
    
    
    }
    return fun;
}




extern "C" __declspec(dllexport) int Draw(QWND * qwm, HDC sdc)
{
    //char str[128];
    //sprintf_s(str,128, "%p,%p", qwm, sdc);
    //MessageBoxA(0, str,"",0);
    return qwm->Draw(sdc);
}
extern "C" __declspec(dllexport) int MsgHandle(QWND * qwm, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return qwm->MsgHandle(uMsg, wParam, lParam);
}
extern "C"
__declspec(dllexport) void IsVisual(QWND * qwm, bool Visual)
{
    qwm->m_isVisual = Visual;
}
extern "C"
__declspec(dllexport) void IsEnable(QWND * qwm, bool Enable)
{
    qwm->m_isEnable = Enable;
}
extern "C"
__declspec(dllexport) void SetHostHwnd(QWND * qwm, HWND HostHwnd)
{
    qwm->m_HostHwnd = HostHwnd;
}
extern "C"
__declspec(dllexport) HCURSOR QWDSetCursor(QWND * qwm, HCURSOR hc)
{
    return qwm->SetCursor(hc);
}

extern "C"
__declspec(dllexport) HCURSOR QWDGetCursor(QWND * qwm)
{
    return qwm->GetCursor();
}

extern "C"
__declspec(dllexport) void LoadThisCtrlCursor(QWND * qwm)
{
    qwm->LoadThisCtrlCursor();
}
extern "C"
__declspec(dllexport) void Refresh(QWND * qwm)
{
    qwm->Refresh();
}

extern "C" __declspec(dllexport) void Reconstruct(QWND * qwm, HDC sdc, const GRect * prect)
{
    qwm->Reconstruct(sdc, *prect);
}
extern "C" __declspec(dllexport) void SetLocation(QWND * qwm, const GPoint * pPoint)
{
    qwm->SetLocation(*pPoint);
}
extern "C" __declspec(dllexport) void SetSize(QWND * qwm, const GSize * size, HDC dc)
{
    qwm->SetSize(*size, dc);
}


extern "C" __declspec(dllexport) BOOL Contains(QWND * qwm, int x, int y)
{
    return qwm->Contains(x, y);
}
extern "C" __declspec(dllexport) void Redraw(QWND * qwm)
{
    qwm->Redraw();
}

extern "C" __declspec(dllexport) int LButtonDown(QWND * qwm, int x, int y, WPARAM wParam)
{
    return qwm->LButtonDown(x, y, wParam);
}
extern "C" __declspec(dllexport) int LButtonUp(QWND * qwm, int x, int y, WPARAM wParam)
{
    return qwm->LButtonUp(x, y, wParam);
}
extern "C" __declspec(dllexport) int LButtonDBLClick(QWND * qwm, int x, int y, WPARAM wParam)
{
    return qwm->LButtonDBLClick(x, y, wParam);
}

extern "C" __declspec(dllexport) int RButtonDown(QWND * qwm, int x, int y, WPARAM wParam)
{
    return qwm->RButtonDown(x, y, wParam);
}
extern "C" __declspec(dllexport) int RButtonUp(QWND * qwm, int x, int y, WPARAM wParam)
{
    return qwm->RButtonUp(x, y, wParam);
}
extern "C" __declspec(dllexport) int RButtonDBLClick(QWND * qwm, int x, int y, WPARAM wParam)
{
    return qwm->RButtonDBLClick(x, y, wParam);
}

extern "C" __declspec(dllexport) int MButtonDown(QWND * qwm, int x, int y, WPARAM wParam)
{
    return qwm->MButtonDown(x, y, wParam);
}
extern "C" __declspec(dllexport) int MButtonUp(QWND * qwm, int x, int y, WPARAM wParam)
{
    return qwm->MButtonUp(x, y, wParam);
}
extern "C" __declspec(dllexport) int MButtonDBLClick(QWND * qwm, int x, int y, WPARAM wParam)
{
    return qwm->MButtonDBLClick(x, y, wParam);
}

extern "C" __declspec(dllexport) int MouseWheel(QWND * qwm, int x, int y, WPARAM wParam)
{
    return qwm->MouseWheel(x, y, wParam);
}
extern "C" __declspec(dllexport) int MouseMove(QWND * qwm, int x, int y, WPARAM wParam)
{
    return qwm->MouseMove(x, y, wParam);
}

extern "C" __declspec(dllexport) int KeyChar(QWND * qwm, WPARAM wParam, LPARAM lParam)
{
    return qwm->KeyChar(wParam, lParam);
}
extern "C" __declspec(dllexport) int KeyIMEChar(QWND * qwm, WPARAM wParam, LPARAM lParam)
{
    return qwm->KeyIMEChar(wParam, lParam);
}
extern "C" __declspec(dllexport) int KeyDown(QWND * qwm, WPARAM wParam, LPARAM lParam)
{
    return qwm->KeyDown(wParam, lParam);
}
extern "C" __declspec(dllexport) int KeyUp(QWND * qwm, WPARAM wParam, LPARAM lParam)
{
    return qwm->KeyUp(wParam, lParam);
}

extern "C" __declspec(dllexport) void killFocus(QWND * qwm)
{
    qwm->killFocus();
}

extern "C" __declspec(dllexport) GRect GetRect(QWND * qwm)
{
    return qwm->GetRect();
}
extern "C"
__declspec(dllexport) void DeleteQWND(QWND * wnd)
{
    delete wnd;
}



__declspec(dllexport) GBrush* QCreateSolidBrush(unsigned char A, unsigned char R, unsigned char G, unsigned char B)
{
    return new GSolidBrush(GColor(A, R, G, B));
}
__declspec(dllexport)void QDeleteBrush(GBrush* brush)
{
    delete brush;
}


__declspec(dllexport)GBrush* QCreateLinearGradientBrush(float x1, float y1, unsigned char A1, unsigned char R1, unsigned char G1, unsigned char B1
    , float x2, float y2, unsigned char A2, unsigned char R2, unsigned char G2, unsigned char B2)
{
    return new Gdiplus::LinearGradientBrush(GPointF(x1, y1), GPointF(x2, y2), GColor(A1, R1, G1, B1), GColor(A2, R2, G2, B2));
}


__declspec(dllexport) GBrush* QCreateTextureBrush(const wchar_t* path)
{
    GImage* image = new GImage(path);
    GTextureBrush* pb = new GTextureBrush(image);
    delete image;
    return pb;
}