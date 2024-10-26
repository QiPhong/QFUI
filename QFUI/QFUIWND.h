#pragma once
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <dwrite.h>
#pragma comment(lib, "dwrite")
#include <wincodec.h> // 用于图片解码

#pragma comment(lib, "windowscodecs.lib")
#include <Windows.h>
#include<windowsx.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")

#include <thread>
#include<algorithm>

#include <string>
#include "QFMsg.h"
#include <shared_mutex>
#include "QFdef.h"
typedef Gdiplus::Image GImage;
typedef Gdiplus::TextureBrush GTextureBrush;
typedef Gdiplus::Brush GBrush;
typedef Gdiplus::Rect GRect;
typedef Gdiplus::Size GSize;
typedef Gdiplus::Point GPoint;
typedef Gdiplus::PointF GPointF;
typedef Gdiplus::Color GColor;
typedef Gdiplus::SolidBrush GSolidBrush;
typedef Gdiplus::Font GFont;
typedef Gdiplus::RectF GRectF;
typedef Gdiplus::StringFormat GStringFormat;
typedef Gdiplus::Pen GPen;
typedef Gdiplus::ARGB GARGB;


/**
 * ==============================================================================================================================
 * 奇风窗口和控件的基类
 * 创建时间：2023年11月13日
 *
 *
 *
 *
 *
 * ==============================================================================================================================
*/


typedef Gdiplus::Brush GBrush;
typedef Gdiplus::Rect GRect;
typedef Gdiplus::Size GSize;
typedef Gdiplus::Point GPoint;
typedef Gdiplus::PointF GPointF;
typedef Gdiplus::Color GColor;
typedef Gdiplus::SolidBrush GSolidBrush;
typedef Gdiplus::Font GFont;
typedef Gdiplus::RectF GRectF;
typedef Gdiplus::StringFormat GStringFormat;
typedef Gdiplus::Pen GPen;
typedef Gdiplus::Image GImage;
typedef Gdiplus::REAL GREAL;
std::string GetCurrentModuleAddr();
bool isFileExists(const std::string& dllPath);

int ReconstructDC(HDC& dc, HDC& sDC, HBITMAP& hb, const GRect& rf);

//窗口属性
union QWNDAttribute{
    unsigned long long AData=0;
    struct {
        unsigned long long isQHostCtrl : 1;//是否宿主控件
        unsigned long long isContainerCtrl : 1;//是否容器控件，控件里面还存储其他控件
        unsigned long long isLayout : 1;//是否布局容器
        unsigned long long HostCtrlMouse : 1;//QHostCtrl给与鼠标信息

    }Attribute;

};

class QWND
{
protected:
    HDC m_dc;//虚拟dc
    HDC m_sdc;//上一层的dc
    GRect m_rect;
    HBITMAP m_hBitmap;
    HCURSOR m_hCursor = NULL;
    QWND* m_Parent = NULL;
    std::string m_name;
    QWNDAttribute __WNDAttribute;
    //std::shared_mutex m_draw_lk;
   
public:
    bool m_isVisual = true;
    bool m_isEnable = true;
    HWND m_HostHwnd = NULL;//宿主窗口句柄,绘制在那个窗口上要指定下句柄，否则有些功能失效,句柄和m_Parent要存在一个
     HCURSOR SetCursor(HCURSOR hc) ;
    HCURSOR GetCursor();
    void LoadThisCtrlCursor() ;
    virtual void Refresh();


    /**
     * 第一个参数，要展示的dc。如果想展示在窗口上则是改窗口的dc，如果是内存dc也可
     * 第二个参数，这个控件或者窗口要展示的大小或者位置
    */
    QWND(HDC winddc, const GRect& rf);
    /**
     * 参数一，要展示的dc。如果想展示在窗口上则是改窗口的dc，如果是内存dc也可
     * 参数二，展示位置x坐标
     * 参数三，展示位置y坐标
     * 参数四，展示宽度
     * 参数五，展示高度
    */
    QWND(IN HDC winddc,
        IN INT x,
        IN INT y,
        IN INT width,
        IN INT height);
    virtual ~QWND();

    virtual int Draw(HDC sdc = NULL);                                    //绘制到指定DC
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //重新创建
    virtual void Reconstruct(HDC sdc);                                          //重新创建
    virtual void SetLocation(const GPoint& point);                               //设置位置
    virtual void SetSize(const GSize& size, HDC dc = NULL);                         //设置大小(会重新构建控件)
    //是否包含某点
    virtual BOOL Contains(IN INT x,
        IN INT y) const;
    virtual BOOL Contains(IN const GPoint& pt) const;
    virtual BOOL Contains(IN GRect& rect) const;
    virtual void Redraw() ;

    ///////////////////////////////////////////////////////////////////////////
    //消息处理
        //鼠标左右键回复控制,
#define QFUI_CTRL_CAPTURE_MOUSE 1 //在鼠标按下的事件返回这个值能捕获鼠标，前提是没有控件在捕获
#define QFUI_CTRL_ACQUIRE_KEYBOARD 2 //在鼠标按下的事件返回这个值能获得键盘焦点



    virtual int MsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);//原始消息

    virtual int LButtonDown(int x, int y, WPARAM wParam);//鼠标左键按下消息，
    virtual int LButtonDBLClick(int x, int y, WPARAM wParam);//鼠标左键弹起消息，
    virtual int LButtonUp(int x, int y, WPARAM wParam);//鼠标左键双击消息，

    virtual int RButtonDown(int x, int y, WPARAM wParam);//鼠标右键按下消息，
    virtual int RButtonUp(int x, int y, WPARAM wParam);//鼠标右键弹起消息，
    virtual int RButtonDBLClick(int x, int y, WPARAM wParam);//鼠标右键双击消息，

    virtual int MButtonDown(int x, int y, WPARAM wParam);//鼠标中键按下消息，
    virtual int MButtonUp(int x, int y, WPARAM wParam);//鼠标中键弹起消息，
    virtual int MButtonDBLClick(int x, int y, WPARAM wParam);//鼠标中键双击消息，
    virtual int MouseWheel(int x, int y, WPARAM wParam);//鼠标滚轮转到

    virtual int MouseMove(int x, int y, WPARAM wParam);//鼠标移动消息
    virtual int MouseMoveOut();                         //鼠标移出

    virtual int KeyChar(WPARAM wParam, LPARAM lParam);//键盘键入一个字符
    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam);//输入法发送一个字符过来
    virtual int KeyDown(WPARAM wParam, LPARAM lParam);//键盘按下某键
    virtual int KeyUp(WPARAM wParam, LPARAM lParam);//键盘弹起某键


    ///////////////////////////////////////////////////////////////////////////
    //失去焦点
    virtual void killFocus() {};


    /*
    *传递消息
    * 返回值和参数为自定义
    * 参数一：消息ID
    * 参数二：左参数
    * 参数三：有参数
    * 参数四：附加参数
    */
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
    void SetQWNDName(const char* name);
    const char* GetQWNDName();
    GRect GetRect();
    unsigned long long GetAttribute();//获取属性值
    virtual int GetSubCtrlCount();//取子控件数
    virtual QWND* GetSubCtrl(int index);//取子控件
   
private:
    //创建自己的DC
    inline HDC QCreateDC(HDC winddc);
    //释放DC
    inline void ReleaseDC();


protected:
    virtual void UpdateWND(QDWORD para=1);



};



