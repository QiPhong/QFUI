#pragma once
#include "QFCtrlBase.h"
#include <windows.h>
#include <string>
#include <locale>
#include <codecvt>
#include <chrono>
#include "QFKeyValueTransform.h"
#include <thread>

#define __QFEDIT_TYPEDEF
#ifdef __QFEDIT_TYPEDEF

#endif


typedef int (*QEDIT_CHANGE_EVENT)(int, int, int);//参数1：自身指针；参数2：字符串指针；参数3：设置时要求调用时带的
struct QEdit_Change_Event
{
    QEDIT_CHANGE_EVENT fun = nullptr;
    int addParam = 0;
    bool isEnable = false;

};




class QEdit :public QCtrlBase
{
private:
    QEdit_Change_Event __ChangeEvent;
    virtual int Draw(HDC sdc = NULL)override;
    Gdiplus::ImageAttributes imageAttr;//图片属性
    GImage* m_pBKImage = NULL;    //背景图片
    inline void __LoadPicture(const std::wstring& path);

    GBrush* m_bkBrush = NULL; //背景画刷
    GColor m_bodyColo;


public:
    QEdit(HDC winddc, const GRect& rf);
    virtual ~QEdit();
    inline void SetBKImageTransparency(unsigned char apha);
    void LoadPicture(const std::wstring& path);
    virtual void Redraw() { isChange = true; };

    void SetBKBrush(GBrush* brush);
    //失去焦点
    virtual void killFocus();

    ///////////////////////////////////////////////////////////////////////////
    //消息处理
    // virtual int MsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);//原始消息

    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息，
    // virtual int LButtonDBLClick(int x,int y, WPARAM wParam);//鼠标左键弹起消息，
    // virtual int LButtonUp(int x,int y, WPARAM wParam);//鼠标左键双击消息，

    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam)override;//输入法输入一个字符
    virtual int KeyChar(WPARAM wParam, LPARAM lParam)override;//输入法输入一个字符
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int KeyDown(WPARAM wParam, LPARAM lParam)override;//键盘按下某键


    ///////////////////////////////////////////////////////////////////////////
    //*********************字体属性设置*******************************
    void setText(const wchar_t* text);
    void setText(const char* text);
    inline const std::wstring& getText()const;
    inline void setFontName(const wchar_t* fName);
    inline void setFontSize(INT size);
    void setFontBrush(const GBrush& brush);

    //***************************************************************



    //********************************************************************************************
private:
    //********************************************************************************************
    //字体属性
    std::wstring m_text = L"";
    Gdiplus::StringAlignment slAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    std::wstring m_fontName = L"Arial";
    INT m_fontSize = 12;
    GBrush* m_fontBrush = NULL;
    int __FontHeight = 0;//字体高度，不要设置，设置也没有用，给系统记录的
    Gdiplus::REAL __drawStartpos = 0.0f;//开始绘制文字的位置
    GStringFormat* m_gsf = NULL;
    //********************************************************************************************

    //**************************插入符参数*********************************************************
    Gdiplus::REAL m_CaretPos = 0.0f;
    int m_CaretIndex = 0;//插入符在第几个字符后面
    unsigned char m_CaretState = 0;//插入符状态0为不显示，1为显示
    unsigned int m_CaretPeriod = 500;//插入符周期，单位毫秒
    std::chrono::time_point<std::chrono::high_resolution_clock> m_CaretUpdateTime;//更新时间， std::chrono::high_resolution_clock::now()
    bool m_isFocus = false;
    inline void __upDateCare_up();//右移更新光标
    inline void __upDateCare_down();//左移更新光标

    //********************************************************************************************


    //内部字符串按键处理***************************************************************************
    inline void __KeyInput(WPARAM wParam, LPARAM lParam);
    inline void __AddChar(wchar_t wc);//增加一个字符
    inline void __RemoveChar();//移除一个字符
    inline int __TrailingCharacter(GREAL wf);//获得当前指定长度内最后一个字符的位置
    inline void __UpdateCarePos();



    //*******************************************************************************************
public:
    void SetChangeEvent(QEDIT_CHANGE_EVENT even,int param);
    
};

