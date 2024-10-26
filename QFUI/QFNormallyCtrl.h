#pragma once
/**
* ========================================================================================================================================
* 常用控件：
* 1、QGroup(分组框)
* 2、QRadio（单选框）
* 3、QDrawer (抽屉控件)
* 4、QHostCtrl（宿主控件）
* 5、QHLayout（水平布局器）
* 6、QVLayout （垂直布局器）
* 7、QStaticText （静态文本）
* 8、QSwitch （开关控件）
* 9、QButton2
* ========================================================================================================================================
*/
#include "QFCtrlBase.h"
#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include "QFTabs.h"
#include <map>
#include <set>
#include <cairo.h>


class QDSubkey
{
public:
    std::wstring name = L"子项目";
    float width = 0;//名字字符绘制所需的宽度
    float height = 0;//名字字符绘制所需的高度
    bool isChange = true;
    int color = 0xff000000;
    int bcolor = 0x80ffffff;
};



class Category
{
public:
    std::vector<QDSubkey> subkey;
    std::wstring name=L"项目";
    //名字字符绘制所需的宽度
    float width = 0;
    //名字字符绘制所需的高度
    float height = 0;
    bool isChange = true;
    int color=0xff000000;
    int bcolor = 0x80ffffff;
    bool isWithdrawn = false;
};

typedef int (*QDRAWERSELECT)(int,int);
#define __QDRAWER__
class QDrawer :public QWND
{
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
    HDC m_pageDC = NULL;                 //函数绘制dc
    HBITMAP m_pageHB = NULL;              //
    ID2D1Factory* m_pD2DFactory;
    ID2D1DCRenderTarget* m_pRenderTarget;
    IDWriteFactory* pDWriteFactory = nullptr;
    IDWriteTextFormat* pTextFormat = nullptr;
    //ID2D1Layer* pLayer = NULL;//创建一个图层
    IWICImagingFactory* pWICFactory = NULL;
    bool isChange = true;
    GBrush* m_bkBrush = NULL;//背景画刷
    std::wstring m_fontName = L"Arial";
    float m_fontSize = 16.0f;
    float m_startPos = 0.0f;
    std::shared_mutex m_lock;
    std::vector<Category> m_data;
    QDRAWERSELECT __selectFUN = NULL;

    //鼠标位置
    int __mouseX = -1, __mouseY = -1;

public:
    QDrawer(HDC winddc, const GRect& rf);
    virtual ~QDrawer();
    virtual void Redraw() { isChange = true; };
    virtual void Refresh() { isChange = true; };
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
    virtual int LButtonUp(int x, int y, WPARAM wParam)override;//鼠标左键弹起消息
    virtual int MouseMove(int x, int y, WPARAM wParam)override;
    virtual int Draw(HDC sdc = NULL)override;
    virtual int MouseWheel(int x, int y, WPARAM wParam);//鼠标滚轮转到
    virtual int LButtonDBLClick(int x, int y, WPARAM wParam);//鼠标左键弹起消息
    virtual int MouseMoveOut();
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //重新创建
private:
    void Test();

public:
    //用户接口
    void SetFontName(const wchar_t* name);
    void SetFontSize(float size);
    int InsertDrawer(int index);
    int InsertSubObject(int iDrawer,int iObj);
    int RemoveSubObject(int iDrawer, int iObj);
    int RemoveDrawer(int index);
    int GetDrawerCount();
    int GetSubObjectCount(int index);
    int SetDrawerName(int index,const wchar_t* name);
    int SetDrawerFontColor(int index,int color);
    int SetDrawerBKColor(int index, int color);
    int OpenDrawer(int index);
    int CloseDrawer(int index);
    int SetSubObjectName(int iDrawer, int iObj, const wchar_t* name);
    int SetSubObjectFontColor(int iDrawer, int iObj, int color);
    int SetSubObjectBKColor(int iDrawer, int iObj, int color);
    int SetSelectEvent(QDRAWERSELECT fun);

    int GetSubObjectName(int iDrawer, int iObj, wchar_t* pName, int bl);
    int GetDrawerName(int iDrawer, wchar_t* pName, int bl);
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
};

#define __QHOSTCTRL__


class QHostCtrl :public QWND
{
    std::mutex m_lock;


    //鼠标样式
    static HCURSOR m_hCursorWE ;
    static HCURSOR m_hCursorNS;
    static HCURSOR m_hCursorARROW;
    static HCURSOR m_hCursorSIZEALL;

    QWND* FocusCtrl = nullptr;
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
    HDC m_pageDC = NULL;                 //函数绘制dc
    HBITMAP m_pageHB = NULL;              //
    ID2D1Factory* m_pD2DFactory = NULL;
    ID2D1DCRenderTarget* m_pRenderTarget = NULL;
    IDWriteFactory* pDWriteFactory = nullptr;
    IDWriteTextFormat* pTextFormat = nullptr;

    int m_MouseStartX = -1, m_MouseStartY = -1;
    int m_MouseCurrentX = -1, m_MouseCurrentY = -1;
    //ID2D1Layer* pLayer = NULL;//创建一个图层
    IWICImagingFactory* pWICFactory = NULL;
    bool isChange = true;
    bool isDown = false;
    bool isrDown = false;
    bool isSelect = false;//当前控件是否被选中
    struct {
        int flag = -1;//0为上边，1为右边，2为下边，3为左边，4为中间
        float dta = -1;//按下时距离另一边的距离
        float dta2= -1;//备用距离

    }__DownData;
    std::wstring m_fontName = L"Arial";
    float m_fontSize = 16.0f;

    //内部控件
    QWND* m_boardCtrl = NULL;

    std::map<std::string, std::string> __pairArr;

public:
    
    QHostCtrl(HDC winddc, const GRect& rf);
    virtual ~QHostCtrl();
    virtual void Redraw() { isChange = true; };
    virtual void Refresh() { isChange = true; };
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
    virtual int LButtonUp(int x, int y, WPARAM wParam)override;//鼠标左键弹起消息
    


    virtual int RButtonDown(int x, int y, WPARAM wParam)override;//鼠标右键按下消息
    virtual int RButtonUp(int x, int y, WPARAM wParam)override;//鼠标右键弹起消息

    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //重新创建
    virtual void Reconstruct(HDC sdc)override;                         //重新创建
    virtual int MouseMove(int x, int y, WPARAM wParam)override;
    virtual int Draw(HDC sdc = NULL)override;
    virtual void killFocus()override;
    virtual int MouseMoveOut()override;
    virtual int MouseWheel(int x, int y, WPARAM wParam)override;//鼠标滚轮转到
   
    virtual int KeyChar(WPARAM wParam, LPARAM lParam);//键盘键入一个字符
    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam);//输入法发送一个字符过来
    virtual int KeyDown(WPARAM wParam, LPARAM lParam);//键盘按下某键
    virtual int KeyUp(WPARAM wParam, LPARAM lParam);//键盘弹起某键
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
public:
    //外部接口
    int SetParasiticCtrl(QWND* ctrl, QWND** oldCtrl = nullptr);//返回-1 为控件不能直接为宿主控件
    QWND* GetParasiticCtrl();
    
private:
    void Test();
    
};




class QLayoutObj 
{
public:
    QWND* pWnd = NULL;
    unsigned int weight = 0;//权重，0为保存宽度，大于0的时候加入权重按比例

};
#define __QHLAYOUT__
//水平布局器
class QHLayout :public QLayout
{

    bool isChange = true;
    std::vector<QLayoutObj> m_QWNDs;
    std::shared_mutex m_QWND_lk;
    std::shared_mutex m_Draw_lk;
public:
    QHLayout(HDC winddc, const GRect& rf);
    virtual void Redraw() ;
    virtual void Refresh() ;
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //重新创建
    virtual void Reconstruct(HDC sdc);                                          //重新创建
    virtual ~QHLayout();
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
    virtual int LButtonDBLClick(int x, int y, WPARAM wParam);//鼠标左键弹起消息，
    virtual int LButtonUp(int x, int y, WPARAM wParam);//鼠标左键双击消息，

    virtual int RButtonDown(int x, int y, WPARAM wParam);//鼠标右键按下消息，
    virtual int RButtonUp(int x, int y, WPARAM wParam);//鼠标右键弹起消息，
    virtual int RButtonDBLClick(int x, int y, WPARAM wParam);//鼠标右键双击消息，

    virtual int MButtonDown(int x, int y, WPARAM wParam);//鼠标中键按下消息，
    virtual int MButtonUp(int x, int y, WPARAM wParam);//鼠标中键弹起消息，
    virtual int MButtonDBLClick(int x, int y, WPARAM wParam);//鼠标中键双击消息，
    virtual int MouseWheel(int x, int y, WPARAM wParam);//鼠标滚轮转到

    virtual int MouseMoveOut();                         //鼠标移出

    virtual int KeyChar(WPARAM wParam, LPARAM lParam);//键盘键入一个字符
    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam);//输入法发送一个字符过来
    virtual int KeyDown(WPARAM wParam, LPARAM lParam);//键盘按下某键
    virtual int KeyUp(WPARAM wParam, LPARAM lParam);//键盘弹起某键
    virtual void killFocus()override;
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
    virtual void SetLocation(const GPoint& point);                               //设置位置
    virtual void SetSize(const GSize& size, HDC dc = NULL);                         //设置大小(会重新构建控件)
    virtual int Type()override { return 1; };//水平布局返回1，垂直布局返回2
private:
    //**************************消息处理
//鼠标消息
    inline int MouseMessage(UINT uMsg, WPARAM wParam, int x, int y);
    inline int MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//
    //键盘消息处理
    inline void KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);//
    //*********************************
    inline void CtrlReconstruct();//更新控件矩形，没上锁，自行上锁使用
    double m_mousex = -1, m_mousey = -1;
    QWND* FocusCtrl = NULL;           //焦点控件，主要响应键盘的
    QWND* mouseFocusCtrl = NULL;           //鼠标焦点控件
    QWND* mouseSeleFocusCtrl = NULL;           //鼠标选中焦点控件
    QWND* mouseCapture = NULL;         //鼠标捕获控件
    
private:
    //测试
    void Test();

public:
    //用户接口
    int InsertCtrl(QWND* ctrl, unsigned int weight=0,int index=-1);//插入控件：weight：控件比例权重；index：插入的位置：-1为尾插
    int RemoveCtrl(int index);//移除控件
    int GetCtrlCount();//取控件数
    QWND* GetCtrl(int index);
    unsigned int GetCtrlWeight(int index);
};

#define __QVLAYOUT__
//垂直布局器
class QVLayout :public QLayout
{

    bool isChange = true;
    std::vector<QLayoutObj> m_QWNDs;
    std::shared_mutex m_QWND_lk;
    std::shared_mutex m_Draw_lk;
public:
    QVLayout(HDC winddc, const GRect& rf);
    virtual void Redraw() override;
    virtual void Refresh() override;
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //重新创建
    virtual void Reconstruct(HDC sdc);                                          //重新创建
    virtual ~QVLayout();
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
    virtual int LButtonDBLClick(int x, int y, WPARAM wParam);//鼠标左键弹起消息，
    virtual int LButtonUp(int x, int y, WPARAM wParam);//鼠标左键双击消息，

    virtual int RButtonDown(int x, int y, WPARAM wParam);//鼠标右键按下消息，
    virtual int RButtonUp(int x, int y, WPARAM wParam);//鼠标右键弹起消息，
    virtual int RButtonDBLClick(int x, int y, WPARAM wParam);//鼠标右键双击消息，

    virtual int MButtonDown(int x, int y, WPARAM wParam);//鼠标中键按下消息，
    virtual int MButtonUp(int x, int y, WPARAM wParam);//鼠标中键弹起消息，
    virtual int MButtonDBLClick(int x, int y, WPARAM wParam);//鼠标中键双击消息，
    virtual int MouseWheel(int x, int y, WPARAM wParam);//鼠标滚轮转到

    virtual int MouseMoveOut();                         //鼠标移出
   

    virtual int KeyChar(WPARAM wParam, LPARAM lParam);//键盘键入一个字符
    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam);//输入法发送一个字符过来
    virtual int KeyDown(WPARAM wParam, LPARAM lParam);//键盘按下某键
    virtual int KeyUp(WPARAM wParam, LPARAM lParam);//键盘弹起某键
    virtual void killFocus()override;
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
    virtual void SetLocation(const GPoint& point);                               //设置位置
    virtual void SetSize(const GSize& size, HDC dc = NULL);                         //设置大小(会重新构建控件)
    virtual int Type()override { return 2; };//水平布局返回1，垂直布局返回2
private:
    //**************************消息处理
//鼠标消息
    inline int MouseMessage(UINT uMsg, WPARAM wParam, int x, int y);
    inline int MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//
    //键盘消息处理
    inline void KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);//
    //*********************************

    inline void CtrlReconstruct();//更新控件矩形，没上锁，自行上锁使用

    double m_mousex = -1, m_mousey = -1;
    QWND* FocusCtrl = NULL;           //焦点控件，主要响应键盘的
    QWND* mouseFocusCtrl = NULL;           //鼠标焦点控件
    QWND* mouseSeleFocusCtrl = NULL;           //鼠标选中焦点控件
    QWND* mouseCapture = NULL;         //鼠标捕获控件
private:
    //测试
    void Test();

public:
    //用户接口
    int InsertCtrl(QWND* ctrl, unsigned int weight = 0, int index = -1);//插入控件：weight：控件比例权重；index：插入的位置：-1为尾插
    int RemoveCtrl(int index);//移除控件
    int GetCtrlCount();//取控件数
    QWND* GetCtrl(int index);
    unsigned int GetCtrlWeight(int index);
};

#define __QTLAYOUT__
//平铺布局器
class QTLayout :public QLayout
{

    bool isChange = true;
    QWND* pWnd = nullptr;
    std::shared_mutex m_Draw_lk;
public:
    QTLayout(HDC winddc, const GRect& rf);
    virtual void Redraw() override;
    virtual void Refresh() override;
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //重新创建
    virtual void Reconstruct(HDC sdc);                                          //重新创建
    virtual ~QTLayout();
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
    virtual int LButtonDBLClick(int x, int y, WPARAM wParam);//鼠标左键弹起消息，
    virtual int LButtonUp(int x, int y, WPARAM wParam);//鼠标左键双击消息，

    virtual int RButtonDown(int x, int y, WPARAM wParam);//鼠标右键按下消息，
    virtual int RButtonUp(int x, int y, WPARAM wParam);//鼠标右键弹起消息，
    virtual int RButtonDBLClick(int x, int y, WPARAM wParam);//鼠标右键双击消息，

    virtual int MButtonDown(int x, int y, WPARAM wParam);//鼠标中键按下消息，
    virtual int MButtonUp(int x, int y, WPARAM wParam);//鼠标中键弹起消息，
    virtual int MButtonDBLClick(int x, int y, WPARAM wParam);//鼠标中键双击消息，
    virtual int MouseWheel(int x, int y, WPARAM wParam);//鼠标滚轮转到

    virtual int MouseMoveOut();                         //鼠标移出


    virtual int KeyChar(WPARAM wParam, LPARAM lParam);//键盘键入一个字符
    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam);//输入法发送一个字符过来
    virtual int KeyDown(WPARAM wParam, LPARAM lParam);//键盘按下某键
    virtual int KeyUp(WPARAM wParam, LPARAM lParam);//键盘弹起某键
    virtual void killFocus()override;
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
    virtual void SetLocation(const GPoint& point);                               //设置位置
    virtual void SetSize(const GSize& size, HDC dc = NULL);                         //设置大小(会重新构建控件)
    virtual int Type()override { return 3; };//水平布局返回1，垂直布局返回2，平铺布局返回3
private:
    //**************************消息处理
//鼠标消息
    inline int MouseMessage(UINT uMsg, WPARAM wParam, int x, int y);
    inline int MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//
    //键盘消息处理
    inline void KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);//
    //*********************************

    inline void CtrlReconstruct();//更新控件矩形，没上锁，自行上锁使用


private:
    //测试
    void Test();

public:
    //用户接口
    int SetCtrl(QWND* ctrl);//插入控件：weight：控件比例权重；
    int RemoveCtrl();//移除控件
    QWND* GetCtrl();
};



#define __QSTATICTEXT__
class QStaticText :public QWND
{
    std::shared_mutex m_dlk;
    //inline void QCreateDC(HDC winddc);
    //inline void ReleaseDC();
    ID2D1Factory* m_pD2DFactory;
    ID2D1DCRenderTarget* m_pRenderTarget;
    IDWriteFactory* pDWriteFactory = nullptr;
    IDWriteTextFormat* pTextFormat = nullptr;
    ID2D1Layer* pLayer = NULL;//创建一个图层
    IWICImagingFactory* pWICFactory = NULL;

    std::wstring m_fontName = L"黑体"; //字体名称
    double m_fontSize = 16.0f;//字体大小
    
    HCURSOR m_hCursorBuff = NULL;
    bool isChange = true;
    unsigned int m_fontColor = 0xff000000;
    unsigned int m_bkColor = 0xffd0d0d0;
    std::wstring m_text = L"Static";
public:
    QStaticText(HDC winddc, const GRect& rf);
    virtual ~QStaticText();
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Redraw() override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //重新创建

    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;

public:
    //用户接口
    void SetText(const wchar_t* t);
    void SetFontName(const wchar_t* t);
    void SetFontSize(double size);
    void SetFontColor(unsigned int color);
    void SetBKColor(unsigned int color);
    const wchar_t* GetText();

};



//----------------------------------------------------------------------------------------------------------------------------------------------控件工具箱
#define __QCTRLTOOLBOX__
#include "QFButton.h"
#include "QFPButton.h"
class QCtrlToolbox :public QCtrlBaseD2D
{
    
protected:
    std::shared_mutex m_drawLK;
    QVLayout* __qvl = nullptr;
    QVLayout* m_ctrlqvl = nullptr;
    QVLayout* m_attrlqvl = nullptr;
    QVLayout* m_fileqvl = nullptr;
    QTabs* m_pTable = nullptr;
    unsigned int m_bkColor = 0xff000000;
    //文件页内的控件
    std::vector<QWND*> m_mangerCtrl;
    QPButton* m_OpenFile = nullptr;
    QPButton* m_SeveFile = nullptr;

public:
    QCtrlToolbox(HDC winddc, const GRect& rf);
    virtual ~QCtrlToolbox();
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Redraw() override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //重新创建

    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息

    virtual int LButtonDBLClick(int x, int y, WPARAM wParam);//鼠标左键弹起消息，
    virtual int LButtonUp(int x, int y, WPARAM wParam);//鼠标左键双击消息，

    virtual int RButtonDown(int x, int y, WPARAM wParam);//鼠标右键按下消息，
    virtual int RButtonUp(int x, int y, WPARAM wParam);//鼠标右键弹起消息，
    virtual int RButtonDBLClick(int x, int y, WPARAM wParam);//鼠标右键双击消息，

    virtual int MButtonDown(int x, int y, WPARAM wParam);//鼠标中键按下消息，
    virtual int MButtonUp(int x, int y, WPARAM wParam);//鼠标中键弹起消息，
    virtual int MButtonDBLClick(int x, int y, WPARAM wParam);//鼠标中键双击消息，
    virtual int MouseWheel(int x, int y, WPARAM wParam);//鼠标滚轮转到

    virtual int MouseMoveOut();                         //鼠标移出
    virtual int KeyChar(WPARAM wParam, LPARAM lParam);//键盘键入一个字符
    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam);//输入法发送一个字符过来
    virtual int KeyDown(WPARAM wParam, LPARAM lParam);//键盘按下某键
    virtual int KeyUp(WPARAM wParam, LPARAM lParam);//键盘弹起某键
public:
    //用户接口
    int InsertCtrl(QWND* ctrl, unsigned int weight = 0, int index = -1);//插入控件：weight：控件比例权重；index：插入的位置：-1为尾插
    int RemoveCtrl(int index);//移除控件
    int GetCtrlCount();//取控件数
    int InsertAttributeCtrl(QWND* w,int index=-1);       //关联一个属性控件
    int RemoveAttributeCtrl(int index);       //移除一个属性控件
    void ClearAttributeCtrl();       //移除一个属性控件
    void SetFileButtonEvent(QFBUTTONCLICK en, int index = 0);

private:
    // 父控件大小发送改变事件
    void ParentCtrlSizeChangeEvent(int w, int h);
};

//----------------------------------------------------------------------------------------------------------------------------------------------画板窗口

#define __QEDITORWIN__
typedef int (*QEDITORWIN_LBDOWN)(int,int ,int);
typedef int (*QEDITORWIN_PLACE_CTRL)(int, int, int,int,int);
typedef void (*QEDITORWIN_DELETE_CTRL)(int);
typedef void (*QEDITORWIN_SELECT_CTRL)(int);

class QEditorWin :public QCtrlBaseD2D
{
    QEDITORWIN_LBDOWN __qLBDown = nullptr;
    QEDITORWIN_PLACE_CTRL __qPCtrl = nullptr;
    QEDITORWIN_DELETE_CTRL __qDeleteCtrl = nullptr;
    QEDITORWIN_SELECT_CTRL __qSelectCtrl = nullptr;
protected:
    std::shared_mutex m_drawLK;
    std::shared_mutex m_ctrlLK;
    std::vector<QWND*> m_QWNDs;
    bool m_sizeChaneg = false;
   
    int m_MouseStartX = -1, m_MouseStartY = -1;
    int m_MouseCurrentX = -1, m_MouseCurrentY = -1;
    static HCURSOR m_hCursorWE;
    static HCURSOR m_hCursorNS;
    static HCURSOR m_hCursorARROW;
    static HCURSOR m_hCursorSIZEALL;
    QWND* m_selectCtrl = nullptr;//被选中的控件
    QWND* m_selectCtrl_h = nullptr;//选中控件的寄生控件
    QWND* FocusCtrl = nullptr;
    QWND* mouseFocusCtrl = NULL;           //鼠标焦点控件
    QWND* mouseSeleFocusCtrl = NULL;           //鼠标选中焦点控件
    QWND* mouseCapture = NULL;         //鼠标捕获控件
    bool isDown = false;
    bool isSelect = false;//当前控件是否被选中
    struct {
        int flag = -1;//0为上边，1为右边，2为下边，3为左边，4为中间,5绘制矩形
        float dta = -1;//按下时距离另一边的距离
        float dta2 = -1;//备用距离

    }__DownData;
    unsigned int m_bkColor = 0xeeffffff;
    unsigned int m_TitleBKColor = 0xffc0c0c0;
    int m_titleW = 24;
    HDC m_clientDC = NULL;                 //函数绘制dc
    HBITMAP m_clientHB = NULL;              //
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
public:

    QEditorWin(HDC winddc, const GRect& rf);
    virtual ~QEditorWin();
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Redraw() override;
    virtual void Refresh() override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //重新创建

    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息

    virtual int LButtonDBLClick(int x, int y, WPARAM wParam);//鼠标左键弹起消息，
    virtual int LButtonUp(int x, int y, WPARAM wParam);//鼠标左键双击消息，

    virtual int RButtonDown(int x, int y, WPARAM wParam);//鼠标右键按下消息，
    virtual int RButtonUp(int x, int y, WPARAM wParam);//鼠标右键弹起消息，
    virtual int RButtonDBLClick(int x, int y, WPARAM wParam);//鼠标右键双击消息，

    virtual int MButtonDown(int x, int y, WPARAM wParam);//鼠标中键按下消息，
    virtual int MButtonUp(int x, int y, WPARAM wParam);//鼠标中键弹起消息，
    virtual int MButtonDBLClick(int x, int y, WPARAM wParam);//鼠标中键双击消息，
    virtual int MouseWheel(int x, int y, WPARAM wParam);//鼠标滚轮转到

    virtual int MouseMoveOut();                         //鼠标移出
    QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
    virtual int KeyUp(WPARAM wParam, LPARAM lParam);//键盘弹起某键
private:
    //**************************消息处理
//鼠标消息
    inline int MouseMessage(UINT uMsg, WPARAM wParam, int x, int y);
    inline int MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//


public:
    //用户接口
    int SetLButtonEvent(QEDITORWIN_LBDOWN ent);
    void AttachCtrl(QWND* w);       //关联一个控件
    void RemoveCtrl(QWND* w);       //移除一个控件


    int SetPlaceCtrlEvent(QEDITORWIN_PLACE_CTRL ent);
    int SetDeleteCtrlEvent(QEDITORWIN_DELETE_CTRL ent);
    int SetSelectCtrlEvent(QEDITORWIN_SELECT_CTRL ent);
};
#define __QSWITCH__
typedef int (*QSWITCH_STATE_CHANGE)(int);

class QSwitch :public QCtrlBaseD2DIPD
{
    //状态改变回调函数

    QSWITCH_STATE_CHANGE __State_ChangeEvent = nullptr;

    unsigned char __state=0;
    bool __isInctrl =false;//鼠标是否在控件上
    std::chrono::steady_clock::time_point __startT;
    bool __isMutex = false;

    BOOL __Contains(IN INT x, IN INT y)const;
public:
    QSwitch(HDC winddc, const GRect& rf);
    ~QSwitch();
    virtual int Draw(HDC sdc = NULL)override;
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
    virtual BOOL Contains(IN const GPoint& pt)const override;
    //是否包含某点
    virtual BOOL Contains(IN INT x,IN INT y) const override;
    virtual int MouseMoveOut();
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam);
public:
    //用户接口
    int SetStateChangeEvent(QSWITCH_STATE_CHANGE ev);
    int GetState()const;
    void SetState(int state);
    void IsMutex(bool im);



};

#define __QSLIDER__
typedef int (*QSLIDER_VALUE_CHANGE)(int,float);
class QSlider :public QCtrlBaseD2DIPD
{
    unsigned char __direction = 0;//控件方向，0为水平，1为垂直
    float __railRatio = 0.3;
    bool __isLDown = false;
    int __lDownPX = -1, __lDownPY = -1;
    unsigned int __railColor = 0xff00f0f0;
    unsigned int __BKColor = 0xffffffff;
    bool __isInBt = false;

  
    float __minValue = 0.0;//最小值
    float __maxValue = 100.0;//最大值
    float __currenValue = __minValue;//当前值

    std::shared_mutex __SegmLK;//分段操作锁
    bool __isSegm = false;//是否分段，如果分段需要设置以下的分段值
    std::set<float> __SegmArr;//分段的数值
    QSLIDER_VALUE_CHANGE __ValueChangeEvent = nullptr;//数值发送改变事件


  

public:
    QSlider(HDC winddc, const GRect& rf);
    ~QSlider();
    virtual int Draw(HDC sdc = NULL)override;
    virtual int MouseMoveOut();
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
    virtual int LButtonUp(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
private:
    void __ChangeCurrenValue(float v);



public:

    void SetMinValue(float v);
    void SetMaxValue(float v);
    void SetCurrentValue(float v);
    unsigned int SetRailColor(unsigned int color);
    float SetRailRatio(float ato);
    void IsSegmentation(bool b);
    int ClearSegmArr();
    int AddSegmVuale(float v);
    int GetSegmValue(float* arr,int bufL);
    void SetDirection(unsigned char d);
    int RemoveSegmVuale(float v);
    QDWORD SetValueChangeEvent(QSLIDER_VALUE_CHANGE ev);

    unsigned int SetColor(unsigned int color);

};

//进度条
#define __QPROGRESS__

class QProgress :public QCtrlBaseD2DIPD
{
    unsigned char __direction = 0;//控件方向，0为水平，1为垂直
    unsigned int __style = 1;//0:直条进度条，1为圆环进度条
    float __currentValue = 60;//当前进度条的位置
    unsigned int __Color = 0xff00f000;

    unsigned char __decimals = 0;//小数位数

    cairo_surface_t* __surface = nullptr;//绘图表面
    cairo_t* __cr = nullptr;//绘图环境
    unsigned int __ringcolor1 = 0xffffffff;
    unsigned int __ringcolor2 = 0xff00ff00;
    int __currentRW = 1000;//当前圆环的取色缓冲背板的宽度
    int __currentRH = 1000;//当前圆环的取色缓冲背板的高度
    float __thickness = 50;//圆环的宽度

    //初始化圆环颜色线程
    std::thread __threadRColor;
    bool __isThreadRefresh = false;
    void __threadRingColor();
    void __threadDraw_gradient_ring(double centerX, double centerY, double radius, double thickness);
    std::mutex __lockRColor;

public:
    QProgress(HDC winddc, const GRect& rf);
    ~QProgress();
    virtual int Draw(HDC sdc = NULL)override;
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //重新创建

private:
    void DrawRingStyle();
    void DrawSLStyle();

 


public:
    //用户接口
    //设置进度条方向
    void SetDirection(unsigned char d);
    //设置进度条样式
    void SetStyle(unsigned int style);
    //获得进度条样式
    unsigned int GetStyle();
    //设置当前值
    void SetCurrentValue(float v);
    //设置线型进度条的颜色
    void SetLineTypeProgressColor(unsigned int color);
    //设置小数显示的位数
    void SetDecimalPlaces(unsigned char dp);
    //设置环形进度条的颜色
    void SetRingProgressColor(unsigned int color1,unsigned int color2);
    //设置圆环宽度
    void SetRingProgressThickness(float thickness);


};

//颜色框
#define __QCOLORBOX__
#include "QFPopDialog.h"
typedef int (*QCOLORBOX_CHANGE)(QPOINTER,unsigned int);
class QColorBox :public QCtrlBaseD2DIPD
{
    unsigned int m_color=0xffa00000;//当前颜色
    QCOLORBOX_CHANGE m_colorChange = nullptr;

protected:
    class QColorPopDialog :public QPopDialog
    {
    public:
        QColorPopDialog(HDC winddc, const GRect& rf);
        ~QColorPopDialog();
        virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;

    };
    static QColorPopDialog* m_cpDialog;
    static QSlider* m_redQslider;
    static QSlider* m_greeQslider;
    static QSlider* m_blueQslider;
    static QSlider* m_alphaQslider;
    static std::mutex createLK;
    static int __currentCount;
    int m_popDialogHeight = 90;
    int m_popDialogWidth = 120;
    inline void __ReconstructDialog(HDC dc);
public:
    QColorBox(HDC winddc, const GRect& rf);
    ~QColorBox();
    virtual int Draw(HDC sdc = NULL)override;
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息


public:
    friend static int sliderVChange(QPOINTER ct,float v);


public:
    //用户接口
    unsigned int GetColor();
    unsigned int SetColor(unsigned int color);
    QCOLORBOX_CHANGE SetColorChangeEvent(QCOLORBOX_CHANGE ev);
    void SetPopDialogHeight(int h);
    void SetPopDialogWidth(int w);

};
int sliderVChange(QPOINTER ct, float v);

//下拉列表框
#define __QSELECT__
typedef int (*QSELECT_SELECTCHANGE)(QPOINTER,int);
class QSelect :public QCtrlBaseD2DIPD
{
protected:
    class QSelectPopDialog :public QPopList
    {
    public:
        QSelectPopDialog(HDC winddc, const GRect& rf);
        ~QSelectPopDialog();


    };
    static QSelectPopDialog* m_spdialog;
    static std::mutex createLK;
    static int __currentCount;
    int m_popDialogHeight = 90;
    int m_popDialogWidth = 120;
    //绘制内容的偏移
    float m_offsetX = 0;
    //绘制内容的偏移Y
    float m_offsetY = 0;
    std::list<PopListObj> m_list;
    std::shared_mutex m_LK_list;

    //背景颜色
    unsigned int m_bkColor = 0xB3b0b0b0;
    //项目高度
    float m_itemHeight = 32;
    //当前显示项目
    //std::wstring m_text;
    PopListObj* m_currentItem =nullptr;
    int m_index = -1;
    //下拉按钮占比
    float m_btWato = 0.3;
    //是否打开下拉列表
    bool m_isOpen=false;

    //项目字体名称
    std::wstring m_itemFName = L"黑体"; 

    //项目字体大小
    double m_itemFSize = 10.0f;

    QSELECT_SELECTCHANGE m_selectEvent = nullptr;

public:
    QSelect(HDC winddc, const GRect& rf);
    ~QSelect();
    virtual int Draw(HDC sdc = NULL)override;
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
    virtual void Reconstruct(HDC sdc, const GRect& rect);                         //重新创建

public:
    //内部组件响应函数
    friend int SelsctChange(QPOINTER ct, int v);

public:
    //用户接口

    //设置弹框高度
    void SetPopDialogHeight(int h);
    //设置弹框宽度
    void SetPopDialogWidth(int w);
    //插入一个项目
    int InsertItem(const wchar_t* text, int index = -1);
    //移除一个项目
    int RemoveItem(int index);
    //设置下拉按钮的宽度比例
    void SetButtonRatio(float rat);
    //设置项目高度
    void SetItemHeight(float h);
    //设置项目字体名称
    void SetItemFontName(const wchar_t* nm);
    //设置项目字体大小
    void SetItemFontSize(float size);
    //获取选中的项目文本
    const wchar_t* GetSelectedText();
    //获取项目文本
    const wchar_t* GetItemText(int index);
    //设置指定项目背景颜色
    void SetItemBKColor(int index, unsigned int c1, unsigned int c2, unsigned char d = 0);
    //设置指定项目字体颜色
    void SetItemFontColor(int index, unsigned int c1, unsigned int c2, unsigned char d = 0);
    //设置项目被选择时回调函数
    QSELECT_SELECTCHANGE SetSelectChangeEvent(QSELECT_SELECTCHANGE ev);
    //获取项目数
    size_t GetItemCount();
    //设置当前选中项目
    int SetCurrentItem(int index);
    //获取当前选项索引
    int GetIndex();
    //获取Item字体颜色
    //参数1：索引
    //参数2：保存方向的指针
    //参数3和4：保存颜色的指针
    int GetItemFontColor(int index, unsigned char* dire, unsigned int* pc1, unsigned int* pc2);
    //获取Item背景颜色
    //参数1：索引
    //参数2：保存方向的指针
    //参数3和4：保存颜色的指针
    int GetItemBKColor(int index, unsigned char* dire, unsigned int* pc1, unsigned int* pc2);
};
int SelsctChange(QPOINTER ct, int v);
























