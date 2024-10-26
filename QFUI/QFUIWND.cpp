
#include "QFUIWND.h"
#include "QFUI.h"

std::string GetCurrentModuleAddr()
{

    HMODULE hCaller = NULL;
    char szModuleName[MAX_PATH];
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)(&GetCurrentModuleAddr), &hCaller);//GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
    GetModuleFileNameA(hCaller, szModuleName, _countof(szModuleName));

    std::string strDllPathBuff = szModuleName;

    auto nPathTruncLoc = strDllPathBuff.find_last_of('\\');
    strDllPathBuff = strDllPathBuff.substr(0, nPathTruncLoc + 1);
    return strDllPathBuff;
}
bool isFileExists(const std::string& dllPath)
{
    
    DWORD attributes = GetFileAttributesA(dllPath.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
   
}
int ReconstructDC(HDC& dc, HDC& sDC, HBITMAP& hb, const GRect& rf)
{

    HBITMAP hb1 = hb;
    hb = CreateCompatibleBitmap(sDC, rf.Width, rf.Height);
    SelectObject(dc, hb);
    DeleteObject(hb1);
        
  
    return 1;
}
QWND::QWND(HDC winddc, const GRect& rf)
{
    m_sdc = winddc;
    rf.GetBounds(&m_rect);//1这两个顺序不要弄反
    m_dc = QCreateDC(m_sdc);//2这两个顺序不要弄反

    m_hCursor = LoadCursor(NULL, IDC_ARROW);
    __WNDAttribute.AData = 0;

}

QWND::QWND(IN HDC winddc,
    IN INT x,
    IN INT y,
    IN INT width,
    IN INT height)
{
    m_sdc = winddc;
    m_rect.X = x;
    m_rect.Y = y;
    m_rect.Width = width;
    m_rect.Height = height;



    m_dc = QCreateDC(m_sdc);

}
QWND::~QWND()
{
    ReleaseDC();
}
unsigned long long QWND::GetAttribute()
{
    return __WNDAttribute.AData;
}
int QWND::GetSubCtrlCount()
{
    if(__WNDAttribute.Attribute.isContainerCtrl)
        return -1;
}
QWND* QWND::GetSubCtrl(int index)
{
    if (__WNDAttribute.Attribute.isContainerCtrl)
        return nullptr;
    
    
    return 0;
}
inline HDC QWND::QCreateDC(HDC winddc)
{
    HDC hdc = CreateCompatibleDC(winddc);

    m_hBitmap = CreateCompatibleBitmap(winddc, m_rect.Width, m_rect.Height);
    SelectObject(hdc, m_hBitmap);
    return hdc;

}
inline void QWND::ReleaseDC()
{
    DeleteObject(m_hBitmap);
    m_hBitmap = NULL;
    DeleteDC(m_dc);
    m_dc = NULL;


}

void QWND::UpdateWND(QDWORD para)
{
    if (!m_Parent)return;
    m_Parent->TransferMessage(QM_UPDATE_CTRL,1,0,0);
}


HCURSOR QWND::GetCursor()
{
    return m_hCursor; 

}

int QWND::Draw(HDC sdc)                                    //绘制到指定DC
{
    if (!m_isVisual)return 0;
    if (!sdc) {
        sdc = m_sdc;
    }
    else {
        m_sdc = sdc;
    }
    //std::unique_lock<std::shared_mutex> lk(m_draw_lk);
    StretchBlt(sdc, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height,
        m_dc,0,0, m_rect.Width, m_rect.Height, SRCCOPY);
    //BitBlt(sdc, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height, m_dc, 0, 0, SRCCOPY);
    return 1;
}
void QWND::Reconstruct(HDC sdc, const GRect& rect)                         //重新创建
{
    //ReleaseDC();
    m_rect.X = rect.X;
    m_rect.Y = rect.Y;
    if (rect.Width == m_rect.Width && m_rect.Height == rect.Height)return;
    //std::unique_lock<std::shared_mutex> lk(m_draw_lk);
    rect.GetBounds(&m_rect);
    //m_dc = QCreateDC(sdc);
    Reconstruct(sdc);


}
void QWND::Reconstruct(HDC sdc)                                          //重新创建
{
   
    //ReleaseDC();
    if (NULL == sdc) {
        sdc = GetDC(m_HostHwnd);
        HBITMAP hb = m_hBitmap;
        m_hBitmap = CreateCompatibleBitmap(sdc, m_rect.Width, m_rect.Height);
        SelectObject(m_dc, m_hBitmap);
        DeleteObject(hb);
        //m_dc = QCreateDC(sdc);
        DeleteDC(sdc);
    }
    else {
        HBITMAP hb = m_hBitmap;
        m_hBitmap = CreateCompatibleBitmap(sdc, m_rect.Width, m_rect.Height);
        SelectObject(m_dc, m_hBitmap);
        DeleteObject(hb);
        //m_dc = QCreateDC(sdc);
    }
  

}
void QWND::SetLocation(const GPoint& point)                             //设置位置
{
    m_rect.X = point.X;
    m_rect.Y = point.Y;

}
void QWND::SetSize(const GSize& size, HDC dc)                        //设置大小(会重新构建控件)
{
    m_rect.Width = size.Width;
    m_rect.Height = size.Height;
    if (!dc) {
        dc = m_sdc;

    }
    Reconstruct(dc);


}

BOOL QWND::Contains(IN INT x,
    IN INT y) const
{

    return m_rect.Contains(x, y);


}
BOOL QWND::Contains(IN const GPoint& pt) const
{

    return m_rect.Contains(pt);

}
BOOL QWND::Contains(IN GRect& rect) const
{
    return m_rect.Contains(rect);
}

int QWND::MsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // switch(uMsg){
    //     case WM_SETCURSOR:{
    //         ::SetCursor(m_hCursor);

    //         break;
    //     }


    // }

    return 0;
}

int QWND::LButtonDown(int x, int y, WPARAM wParam)
{

    return 0;
}
int QWND::LButtonDBLClick(int x, int y, WPARAM wParam)
{

    return 0;
}
int QWND::LButtonUp(int x, int y, WPARAM wParam)
{


    return 0;
}

int QWND::RButtonDown(int x, int y, WPARAM wParam)
{


    return 0;
}
int QWND::RButtonUp(int x, int y, WPARAM wParam)
{


    return 0;
}
int QWND::RButtonDBLClick(int x, int y, WPARAM wParam)
{


    return 0;
}

int QWND::MButtonDown(int x, int y, WPARAM wParam)
{


    return 0;
}
int QWND::MButtonUp(int x, int y, WPARAM wParam)
{


    return 0;
}
int QWND::MButtonDBLClick(int x, int y, WPARAM wParam)
{


    return 0;
}
int QWND::MouseWheel(int x, int y, WPARAM wParam)
{

    return 0;
}


int QWND::MouseMove(int x, int y, WPARAM wParam)
{


    return 0;
}

int QWND::MouseMoveOut()
{
    return 0;
}

int QWND::KeyChar(WPARAM wParam, LPARAM lParam)
{


    return 0;
}
int QWND::KeyIMEChar(WPARAM wParam, LPARAM lParam)
{

    return 0;
}
int QWND::KeyDown(WPARAM wParam, LPARAM lParam)
{


    return 0;
}
int QWND::KeyUp(WPARAM wParam, LPARAM lParam)
{


    return 0;
}
QDWORD QWND::TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{
    if (m_Parent) {
        switch (msgID) {//拦截一些传给父控件的
        case QM_QHOST_LDOWN:

         return m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);
        case QM_QHOST_RDOWN:
            return m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);

        }
    }
    switch (msgID) {
    case QM_GETATTRIBUTE:
        return __WNDAttribute.AData;
    case QM_GET_SUB_CTRL: {
        if(m_Parent)
            return m_Parent->TransferMessage(msgID, LParam, RParam, AddParam);
        else
            return QMSGDEF_RET;
    }
    case QM_TO_CLIENT_XY: {
        QDWORD* plp = (QDWORD*)LParam;
        QDWORD lp = *plp;
        int x = lp >> 32;
        int y = lp & 0xffffffff;
        x += m_rect.X;
        y += m_rect.Y;
        *plp = (QDWORD(x)<<32) | (QDWORD(y)&0xffffffff);
        if (m_Parent)return m_Parent->TransferMessage(QM_TO_CLIENT_XY,QDWORD(plp),0,0);
        return 1;

    }return QMSGDEF_RET;
    case QM_GET_CLIENT_SIZE: {
        return m_Parent->TransferMessage(QM_GET_CLIENT_SIZE, LParam, RParam, AddParam);
        return 1;

    }return QMSGDEF_RET;
    case QM_SET_POPDIALOG:
    {
        if(m_Parent)return m_Parent->TransferMessage(QM_SET_POPDIALOG, LParam, RParam, AddParam);
    }return QMSGDEF_RET;
    case QM_GET_POPDIALOG: {

        
        if (m_Parent)return m_Parent->TransferMessage(QM_GET_POPDIALOG, LParam, RParam, AddParam);

    }
    case QM_POINT_IN_CTRL: {


        if (Contains(LParam, RParam)) {

            return (DWORD)this;
        }

    }
    }
   


    return QMSGDEF_RET;
}
GRect QWND::GetRect()
{

    return m_rect;
}
void QWND::SetQWNDName(const char* name)
{
    m_name = name;
}
const char* QWND::GetQWNDName()
{
    return m_name.c_str();
}
HCURSOR QWND::SetCursor(HCURSOR hc)
{
    HCURSOR h = m_hCursor;
    m_hCursor = hc;

    return h;

}
void QWND::LoadThisCtrlCursor()
{
    if (m_HostHwnd) {
        SetClassLongPtr(m_HostHwnd, GCLP_HCURSOR, (LONG_PTR)m_hCursor);
        ::SetCursor(m_hCursor);

    }
    else if (m_Parent) {
        SetClassLongPtr(m_Parent->m_HostHwnd, GCLP_HCURSOR, (LONG_PTR)m_hCursor);
        ::SetCursor(m_hCursor);

    }

}

void  QWND::Refresh()
{


}
void QWND::Redraw()
{

}

extern "C" {
    __declspec(dllexport) void SetQWNDName(QWND* qwm, const char* name)
    {
        return qwm->SetQWNDName(name);

    }

    __declspec(dllexport) QDWORD TransferMessage(QWND* qwm, QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
    {
    
        return qwm->TransferMessage(msgID, LParam, RParam, AddParam);
    }
    __declspec(dllexport) const char* GetQWNDName(QWND* qwm)
    {
        return qwm->GetQWNDName();

    }
}






#include "pch.h"