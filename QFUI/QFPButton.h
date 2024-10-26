#pragma once
#include "QFCtrlBase.h"
#include <chrono>
#include <string>
#include "QFButton.h"
//
#define __QPBUTTON__
class QPButton : public QCtrlBaseD2DIPD
{
    float __margin = 5;//按钮各边到外边距的距离
    float __ShrinkageRatio = 1.01;//放缩比例
    std::chrono::steady_clock::time_point __startTime;//开始动画时间
    std::chrono::steady_clock::time_point __checkPathTime;
    //鼠标是否在按钮上
    ID2D1PathGeometry* pBTPathGeometry = nullptr;//当前轮廓
    ID2D1GeometrySink* pBTSink = nullptr;

    std::wstring __text = L"Button";
    unsigned int __BKColor = 0xff0000ff;

    QFBUTTONCLICK m_clickEvent = NULL;//事件触发函数

    //图片布局方式:0为左边，1为右边，2为上边，3为下边，4为拉伸
    unsigned char __picType = 0;

    ID2D1Bitmap* pBitmap = nullptr;    //标签图标
    std::wstring mapPath;//图标路径
    float __xRadius = 20.0,__yRadius =20.0;//按钮圆角半径
    int m_Status = 0;
    


public:
    QPButton(HDC winddc, const GRect& rf);
    ~QPButton();
    virtual int Draw(HDC sdc = NULL)override;
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int MouseMoveOut()override;

public:
    //用户接口
    const wchar_t* GetText();
    void SetText(const wchar_t* t);
    void SetClickEvent(QFBUTTONCLICK fun);
    float SetMargin(float margin);
    unsigned int SetBKColor(unsigned int color);
    unsigned char SetPicType(unsigned char type);
    int LoadPicture(const wchar_t* path);
    const wchar_t* GetPicturePath()const;
    float SetXRadius(float rx);
    float SetYRadius(float ry);

};

