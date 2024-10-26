#pragma once
#include "QFUIWND.h"
#ifndef __QFREPORTFORM_H
#define __QFREPORTFORM_H
#include "QFCtrlBase.h"
#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <iterator> 
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




//列表头单元
class QReportFormTitleUnit
{
public:
    //列表文本
    std::wstring text = L"列名";
    //表头的四个颜色，给渐变色使用
    GColor color1;
    GColor color2;
    GColor color3;
    GColor color4;
    //宽度
    int width = 50;
    QReportFormTitleUnit();
    QReportFormTitleUnit(const QReportFormTitleUnit& rft);
    void operator=(const QReportFormTitleUnit& rft);
};
//列表头
class QReportFormTitle
{
public:
    //表头高度
    int height = 32;
    std::vector<QReportFormTitleUnit> unit;


};
//记录单元的画刷描述
#define QREPORTFORMCONTEXT_SOLIDE                   0x0000//描述使用实心画刷
#define QREPORTFORMCONTEXT_GRADUAL_V                0x0001//线性垂直
#define QREPORTFORMCONTEXT_GRADUAL_H                0x0002//线性水平
#define QREPORTFORMCONTEXT_GRADUAL_LTRB             0x0003//线性左上右下
#define QREPORTFORMCONTEXT_GRADUAL_RTLB             0x0004//线性右上左下

class QReportFormBrushContext
{
public:
    unsigned char A = 0xff;
    unsigned char R = 0xC0;
    unsigned char G = 0xC0;
    unsigned char B = 0xC0;
    unsigned char A2 = 0;
    unsigned char R2 = 0;
    unsigned char G2 = 0;
    unsigned char B2 = 0;
    INT W = 80;  //矩形宽度
    int F = 0;//上边定义的值
    bool operator<(const QReportFormBrushContext& tc)const;
};


//记录单元
class QRecorderUnit
{
public:
    std::wstring text;
    QReportFormBrushContext bContext;
    std::shared_ptr<GBrush> pGBrush = NULL;


};


//记录
class QRecorder
{
public:
    QRecorder() {}
    QRecorder(size_t count) { unit = std::vector<QRecorderUnit>(count); }

    std::vector<QRecorderUnit> unit;
    //画刷工具箱
    std::map<QReportFormBrushContext, std::shared_ptr<GBrush>> m_BToolBox;
    //设置单元属性
    void SetUnitText(int index, std::wstring t);
    void SetUnitBackgroundColor(int index, QReportFormBrushContext context, INT height);
    inline GBrush* CreaterGBrush(const QReportFormBrushContext& rf, INT height);
    inline int InsertUint(int index);


};

class QReportForm :public QCtrlBase
{
    //表头数据
    QReportFormTitle m_Title;
    //报表内容
    std::vector<QRecorder> m_data;
    //记录的高度
    INT height = 40;
    //水平滚动位置
    int RollPointHorizontal = 0;
    //垂直滚动位置
    int RollPointVertical = 0;

    //列表行列增删锁
    std::shared_mutex m_adlock;


    bool m_LbtIsDown = false;
    int m_LDownPosY = -1;//鼠标按下时y坐标跟表格最顶端的位置


    //高亮显示属性
    int selectItemIndex = -1;//高亮索引
    GColor SItemColor;

public:
    bool m_isDrag = false;//鼠标按下是否可拖动列表上下滑动
    inline int InsertRecorder(const std::wstring& w = L"", int index = -1);//插入索引0为插入到最前面，超出记录最大数无效
    inline int RemoveRecorder(int index);
    inline int SelectItemIndex();
    /**
     * ============================================================================
     * 函数名：SetRecorderText
     * 作用：设置项目文本
     * 参数1：行索引
     * 参数2：列索引
     * 参数3：欲设置的文本
     * 返回值：返回1正常，其他为失败代码
     * ============================================================================
    */
    inline int SetRecorderText(int iRd, int iUnit, std::wstring context);
    inline int GetRecorderCount()const;                                                             //获取记录总数
    inline int GetTitleCount()const;                                                                //获取字段总数
    /**
     * ============================================================================
     * SetRecorderBK
     * 作用：设置项目的背景
     * 参数1：行索引
     * 参数2：列索引，-1为整行设置一直画刷
     * 参数3：欲设置的背景画刷
     * 返回值：返回1正常，其他为失败代码
     * ============================================================================
    */
    inline int SetRecorderBK(int iRd, int iUnit, const QReportFormBrushContext& context);



    QReportForm(HDC winddc, const GRect& rf);
    virtual ~QReportForm();


    virtual int Draw(HDC sdc = NULL)override;


    //属性设置
    void SetFontName(const wchar_t* name);
    void SetFontSize(INT size);
    void SetTextAlgn(const Gdiplus::StringAlignment& align);
    void SetTextAlgn(const Gdiplus::StringAlignment& align, const Gdiplus::StringAlignment& lalign);
    void SetTextAllgn(const Gdiplus::StringAlignment& lalign);
    void SetBKBrush(GBrush* pgb);
    const GBrush* GetBKBrush()const;

    void InsertTitle(const std::wstring& w = L"", INT wdth = 80, int index = -1);

    int SetTitleHeight(int h);

    const wchar_t* GetTitleText(int index); //获取字段文本
    int GetTitleWidth(int index);//获取字段宽度

    int RemovTitle(int index);
    int SetTitle(const std::wstring& w , INT wdth , int index);


    const wchar_t* GetCellText(int iRd, int iUnit);
public:


    //鼠标消息处理
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息，
    virtual int LButtonDBLClick(int x, int y, WPARAM wParam)override;//鼠标左键弹起消息，
    virtual int LButtonUp(int x, int y, WPARAM wParam)override;//鼠标左键双击消息，

    virtual int RButtonDown(int x, int y, WPARAM wParam)override;//鼠标右键按下消息，
    virtual int RButtonUp(int x, int y, WPARAM wParam)override;//鼠标右键弹起消息，
    virtual int RButtonDBLClick(int x, int y, WPARAM wParam)override;//鼠标右键双击消息，

    virtual int MButtonDown(int x, int y, WPARAM wParam)override;//鼠标中键按下消息，
    virtual int MButtonUp(int x, int y, WPARAM wParam)override;//鼠标中键弹起消息，
    virtual int MButtonDBLClick(int x, int y, WPARAM wParam)override;//鼠标中键双击消息，

    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int MouseWheel(int x, int y, WPARAM wParam)override;




private:
    GBrush* m_bkBrush = NULL;




    //****************************************************
    //字体属性
    Gdiplus::StringAlignment sAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    Gdiplus::StringAlignment slAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    std::wstring m_fontName = L"Arial";
    INT m_fontSize = 12;

    //***************************************************



};

    #undef __QFREPORTFORM_TYPEDEF






#endif
