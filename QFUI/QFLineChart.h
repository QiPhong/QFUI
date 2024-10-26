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
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
    virtual int MouseMoveOut();                         //����Ƴ�
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //���´���
    virtual void Reconstruct(HDC sdc)override;                                          //���´���


private:
    std::mutex m_drawLock;
    int m_Status = 0;
    HDC m_curveDC = NULL;                 //��������dc
    HBITMAP m_curveHB = NULL;              //

    GBrush* m_bkBrush = NULL;//������ˢ
    bool m_isUpdateBK = true;




    //****************************************************
    //��������
    Gdiplus::StringAlignment sAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    Gdiplus::StringAlignment slAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    std::wstring m_fontName = L"����";
    INT m_fontSize = 8;
    //ͼ���������С����������
    std::wstring m_legFontName = L"����";
    INT m_legFontSize = 8;


    //***************************************************
    //***************************************************
    //����ͼ������
    
    float m_XAxis_TW = 50.0;//x��̶��ַ��߶�
    float m_YAxis_TW = 80.0;//y��̶��ַ����
    float m_XAxis_SW = 10.0;//x��̶ȿ��
    float m_YAxis_SW = 10.0;//y��̶ȿ��

    double m_XAxis_MaxV = 30.0;//x������ֵ
    double m_YAxis_MaxV = 2.0;//y������ֵ
    double m_XAxis_MinV = -10.0;//x�����Сֵ
    double m_YAxis_MinV = -2.0;//y�����Сֵ
    double m_LegendW = 50.0;//ͼ�����
    double m_lineWidth = 1.0;
    union 
    {
        long long value=0;
        struct {
            long long isHHCurveLeg : 1;//�Ƿ����ز���ʾ������ͼ��
            long long isCloseTracking : 1;//�Ƿ�رո��٣��رպ�û�и����ȸ�������ȡ����ֵ


        }uct;


    }m_Attribute;

    //***************************************************
    //***************************************************
    //����ͼ������
    std::vector<QCurve> m_curve_data;
    //��ǰ�������
    int m_mousex=-1, m_mousey=-1;
private:
    std::vector<GPointF> PointTransfor(const std::vector<GPointF>& data);
    std::vector<GPointF> PointTransfor(const GPointF* data,int len);
    double PointXTransfor(double x);//��ͼ������ת��Ϊ��������
    double PointYTransfor(double y);
    double TransforPointX(double x);//�ӻ�������ת��Ϊͼ������
    double TransforPointY(double y);
    inline double GetXAxisLength();
    inline double GetXAxisBegin();//ȡX�Ὺʼ����
    inline double GetXAxisEnd();//ȡX���������
    inline double GetYAxisLength();
    inline double GetYAxisBegin();//ȡY�Ὺʼ����
    inline double GetYAxisEnd();//ȡY���������
    void InsertPoint(std::vector<GPointF>& vpt, const GPointF& ppt);

public:
    //����ӿ�
    void SetFontName(const wchar_t* name);//������������������
    void SetFontSize(INT size);//���������������С
    void SetTextAlgn(const Gdiplus::StringAlignment& align);
    void SetTextAlgn(const Gdiplus::StringAlignment& align, const Gdiplus::StringAlignment& lalign);
    void SetTextAllgn(const Gdiplus::StringAlignment& lalign);
    void SetLegFontName(const wchar_t* name);//����ͼ����������
    void SetLegFontSize(INT size);//����ͼ�������С
    void SetXAxisStrHeight(double h);
    void SetYAxisStrWidth(double w);
    void SetXAxisScaleWidth(double w);
    void SetYAxisScaleWidth(double w);
    void SetLineWidth(double w);

    void SetXAxisRange(double m,double M);//����x�᷶Χ
    void SetYAxisRange(double m, double M);//����y�᷶Χ
    void SetLegWidth(double w);//����ͼ����Χ
    void SetAttribute(long long v);
    int AddCurve(GPointF* pt,int len,unsigned long color=0xff000000,const wchar_t* name=NULL);
    int FindCurve(const wchar_t* name,int start =0);//�������ߣ����ֺͿ�ʼ���ҵ������������ҵ����������Ҳ�������-1
    void SetCurveColor(int index, unsigned long color);
    void SetCurveName(int index, const wchar_t* name);
    void SetCurveVisible(int index, bool isVisible);
    void CurveInsertPoint(int index, const GPointF& pt);
    void RemoveCurve(int index);
    void SetBackground(GBrush* brush);


    private:
        std::mutex m_mutex_CtrlCurve;//�������ߵ���
};
