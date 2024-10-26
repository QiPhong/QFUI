#pragma once
#include "QFCtrlBase.h"
#include "QFCtrlListener.h"
#include <windows.h>
#include <string>


#define ____QFBUTTON_TYPEDEF
#ifdef ____QFBUTTON_TYPEDEF

#endif


class QButton :public QCtrlBase
{
    QFBUTTONCLICK m_clickEvent = NULL;
public:
    QButton(HDC winddc, const GRect& rf);
    virtual ~QButton();


    virtual int Draw(HDC sdc = NULL)override;
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息

    //按钮的属性设置
    void SetText(const wchar_t* t);
    const wchar_t* GetText();
    void SetFontName(const wchar_t* name);
    void SetFontSize(INT size);
    void SetTextAlgn(const Gdiplus::StringAlignment& align);
    void SetTextAlgn(const Gdiplus::StringAlignment& align, const Gdiplus::StringAlignment& lalign);
    void SetTextAllgn(const Gdiplus::StringAlignment& lalign);
    virtual int MouseMoveOut()override;
    void SetClickEvent(QFBUTTONCLICK fun);

private:
 
    int m_Status = 0;
    inline void ChangeStatus(int satus);
    GColor color1;
    GColor color2;
    GColor color3;
    GColor color4;



    //****************************************************
    //字体属性
    std::wstring m_text = L"按钮1";
    Gdiplus::StringAlignment sAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    Gdiplus::StringAlignment slAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    std::wstring m_fontName = L"黑体";
    INT m_fontSize = 16;
    //***************************************************



};


