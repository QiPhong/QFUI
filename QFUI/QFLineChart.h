#pragma once
#include "QFCtrlBase.h"
#include "QFCtrlListener.h"
#include <windows.h>
#include <string>
#include <vector>
#include <mutex>



class QCurve
{
public:
    std::vector<GPointF> srcData;
    std::vector<GPointF> data;
    GARGB color;
    bool isVisible = true;
    std::wstring name=L"curve";
};


class QLineChart :public QCtrlBase
{
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
public:
    QLineChart(HDC winddc, const GRect& rf);
    virtual ~QLineChart();


    virtual int Draw(HDC sdc = NULL)override;
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
    virtual int MouseMoveOut();                         //鼠标移出
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //重新创建
    virtual void Reconstruct(HDC sdc)override;                                          //重新创建


private:
    std::mutex m_drawLock;
    int m_Status = 0;
    HDC m_curveDC = NULL;                 //函数绘制dc
    HBITMAP m_curveHB = NULL;              //

    GBrush* m_bkBrush = NULL;//背景画刷
    bool m_isUpdateBK = true;




    //****************************************************
    //字体属性
    Gdiplus::StringAlignment sAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    Gdiplus::StringAlignment slAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    std::wstring m_fontName = L"黑体";
    INT m_fontSize = 8;
    //图例的字体大小和字体名字
    std::wstring m_legFontName = L"黑体";
    INT m_legFontSize = 8;


    //***************************************************
    //***************************************************
    //线形图的属性
    
    float m_XAxis_TW = 50.0;//x轴刻度字符高度
    float m_YAxis_TW = 80.0;//y轴刻度字符宽度
    float m_XAxis_SW = 10.0;//x轴刻度宽度
    float m_YAxis_SW = 10.0;//y轴刻度宽度

    double m_XAxis_MaxV = 30.0;//x轴的最大值
    double m_YAxis_MaxV = 2.0;//y轴的最大值
    double m_XAxis_MinV = -10.0;//x轴的最小值
    double m_YAxis_MinV = -2.0;//y轴的最小值
    double m_LegendW = 50.0;//图例宽度
    double m_lineWidth = 1.0;
    union 
    {
        long long value=0;
        struct {
            long long isHHCurveLeg : 1;//是否隐藏不显示的曲线图例
            long long isCloseTracking : 1;//是否关闭跟踪，关闭后没有辅助先跟踪鼠标获取曲线值


        }uct;


    }m_Attribute;

    //***************************************************
    //***************************************************
    //线形图的属性
    std::vector<QCurve> m_curve_data;
    //当前鼠标坐标
    int m_mousex=-1, m_mousey=-1;
private:
    std::vector<GPointF> PointTransfor(const std::vector<GPointF>& data);
    std::vector<GPointF> PointTransfor(const GPointF* data,int len);
    double PointXTransfor(double x);//从图像坐标转换为画板坐标
    double PointYTransfor(double y);
    double TransforPointX(double x);//从画板坐标转换为图像坐标
    double TransforPointY(double y);
    inline double GetXAxisLength();
    inline double GetXAxisBegin();//取X轴开始坐标
    inline double GetXAxisEnd();//取X轴结束坐标
    inline double GetYAxisLength();
    inline double GetYAxisBegin();//取Y轴开始坐标
    inline double GetYAxisEnd();//取Y轴结束坐标
    void InsertPoint(std::vector<GPointF>& vpt, const GPointF& ppt);

public:
    //对外接口
    void SetFontName(const wchar_t* name);//设置坐标轴字体名称
    void SetFontSize(INT size);//设置坐标轴字体大小
    void SetTextAlgn(const Gdiplus::StringAlignment& align);
    void SetTextAlgn(const Gdiplus::StringAlignment& align, const Gdiplus::StringAlignment& lalign);
    void SetTextAllgn(const Gdiplus::StringAlignment& lalign);
    void SetLegFontName(const wchar_t* name);//设置图例字体名称
    void SetLegFontSize(INT size);//设置图例字体大小
    void SetXAxisStrHeight(double h);
    void SetYAxisStrWidth(double w);
    void SetXAxisScaleWidth(double w);
    void SetYAxisScaleWidth(double w);
    void SetLineWidth(double w);

    void SetXAxisRange(double m,double M);//设置x轴范围
    void SetYAxisRange(double m, double M);//设置y轴范围
    void SetLegWidth(double w);//设置图例范围
    void SetAttribute(long long v);
    int AddCurve(GPointF* pt,int len,unsigned long color=0xff000000,const wchar_t* name=NULL);
    int FindCurve(const wchar_t* name,int start =0);//查找曲线，名字和开始查找的索引，返回找到的索引，找不到返回-1
    void SetCurveColor(int index, unsigned long color);
    void SetCurveName(int index, const wchar_t* name);
    void SetCurveVisible(int index, bool isVisible);
    void CurveInsertPoint(int index, const GPointF& pt);
    void RemoveCurve(int index);
    void SetBackground(GBrush* brush);


    private:
        std::mutex m_mutex_CtrlCurve;//操作曲线的锁
};
