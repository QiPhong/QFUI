#pragma once
#include "QFCtrlBase.h"
#include <vector>
#include <string>
#include <shared_mutex>

#define CATLBarChart_Value_Type double
#define CBCVT CATLBarChart_Value_Type
struct CATLBC 
{
    int color=0xff000000;
    CBCVT v=0;
    std::wstring text;
    int tcolor = 0xffffffff;
};




class QCATLBarChart :public QCtrlBase
{
    std::shared_mutex m_lock;               //���ݲ�����
    HDC m_curveDC = NULL;                 //��������dc
    HBITMAP m_curveHB = NULL;              //
    inline void QCreateDC(HDC winddc);
    inline void ReleaseDC();
    //��ǰ�������
    int m_mousex = -1, m_mousey = -1;
    //����Ƿ���
    bool __isDown = false;
    int __lastXPt = -1;
public:
    QCATLBarChart(HDC winddc, const GRect& rf);
    virtual ~QCATLBarChart();

    virtual int Draw(HDC sdc = NULL)override;
    virtual void Redraw() { m_ischange = true; isChange = true;};
    virtual void Refresh() { m_ischange = true; isChange = true;};

    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
    virtual int LButtonUp(int x, int y, WPARAM wParam)override;//������������Ϣ
    virtual int MouseMoveOut()override;                         //����Ƴ�
    virtual int MouseWheel(int x, int y, WPARAM wParam)override;
    void Reconstruct(HDC sdc, const GRect& rect)override;                         //���´���
  
    void SetFontName(const wchar_t* name);
    void SetFontSize(float size);
    int InsertBarChart(int grouId, int index, CBCVT v, int color, const wchar_t* text = L"", int tcolor = 0xff000000);
    int SetBarChart(int grouId, int index, CBCVT v, int color, const wchar_t* text = L"", int tcolor = 0xff000000);
    int SetBarChartValue(int grouId, int index, CBCVT v);
    int SetBarChartColor(int grouId, int index, unsigned int color);
    int SetBarChartText(int grouId, int index, const wchar_t* text);
    int SetBarChartTextColor(int grouId, int index, int color);
    int InsertGroup(int grouId);
    int RemoveGroup(int grouId);
    int RemoveBarChart(int grouId, int index);

    void SetScaleColor(unsigned int color);
    void SetScaleMax(float max);
    void SetScaleMin(float min);
    void SetScaleWidth(float width);
    void SetScaleHeight(float height);
    void SetBarChartWidth(float width);
    void SetGridWidth(float width);
    void SetLineWidth(float width);
    void SetYAxisStrWidth(float width);
    
    void ClearBarChart();
    int GetGroupCount();
    int GetBarChartCount(int gID);

private:
    //****************************************************
    //��������
    Gdiplus::StringAlignment sAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    Gdiplus::StringAlignment slAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    std::wstring m_fontName = L"����";
    float m_fontSize = 10;
    //***************************************************

    unsigned int m_ScaleColor = 0xff000000;//��������ɫ
    float m_MaxSc = 65535.0;//���̶�
    float m_MinSc = 0.0;//��С�̶�
    float m_startPos = 0; //��״ͼ��ʼλ��
    float m_ScaleWidth = 90;//�̶ȳ��ַ��Ŀ��
    float m_ScaleHeight = 0;//�̶ȳ��ַ��ĸ߶�
    float m_bcWidth = 24;//����Ŀ��
    float m_gridWidth = 32;//�������������
    float m_lineWidth = 1.0;//�߿�
    float m_YAxis_SW = 20.0;//y��̶ȿ��

    std::vector<std::vector<CATLBC>> m_data;
    std::vector<int> m_defaColor = { 0xC0C0C0,0xE16941,0x00FFFF,0x1299FF,0x0061FF,0xFFFF00,0xCD5A6A,0x0F5E38,0x542E08,0x228B22,0xE22B8A,0x572687,0xFF00FF };

    bool m_ischange = true;

    void Test();
    double GetYAxisBegin();
    double GetXAxisEnd();
    double GetXAxisBegin();
    double GetXAxisLength();
    double GetYAxisLength();
    float TransforPointY(float y);
    //���Ժ���
};

struct CTempChart
{
    int color=0xffffffff;
    std::wstring text = L"";
    int tcolor = 0xff000000;
};
struct CTempChartGroup
{
    std::wstring name = L"";
    std::vector<CTempChart> ctc;
    int m_lineCount = 5;    //ÿ�ж��ٸ�

};

class QCATLTemperatureChart :public QCtrlBase
{
    std::shared_mutex m_lock;               //���ݲ�����
        //����Ƿ���
    bool __isDown = false;
    int __lastYPt = -1;
    float m_totalHeight = 0.0f;
    float m_startPos = 0; //
public:
    QCATLTemperatureChart(HDC winddc, const GRect& rf);
    virtual ~QCATLTemperatureChart();
    virtual int Draw(HDC sdc = NULL)override;

    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ


    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ

    virtual int LButtonUp(int x, int y, WPARAM wParam)override;//������������Ϣ

    virtual int MouseWheel(int x, int y, WPARAM wParam)override;
    void Reconstruct(HDC sdc, const GRect& rect)override;                         //���´���

    //�û�ʹ�ýӿ�

    void SetFontName(const wchar_t* name);
    void SetFontSize(float size);
    void SetUnitFontSize(float size);
    void SetUnitWidth(float w);
    void SetUnitIntervalWidth(float w);
    void SetAlignWidth(float w);
    int InsertGroup(int gID);
    int RemoveGroup(int gID);
    int InsertUnit(int gID,int uID,int color,const wchar_t* text=L"",int tcolor=0xff000000);
    int RemoveUnit(int gID, int uID);

    int SetUnit(int gID, int uID, int color, const wchar_t* text = L"", int tcolor = 0xff000000);

    int SetUnitColor(int gID, int uID, int color);

    int SetUnitText(int gID, int uID, const wchar_t* text);


    int SetUnitTextColor(int gID, int uID, int color);

    int SetGroupLineCount(int gID, int count);



    int SetGroupName(int gID, const wchar_t* name);


private:
    //****************************************************
    //��������
    Gdiplus::StringAlignment sAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    Gdiplus::StringAlignment slAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    std::wstring m_fontName = L"����";
    float m_fontSize = 10;//
    float m_ufontSize = 10;//ÿ����Ԫ��������С
    //***************************************************
    std::vector<CTempChartGroup> m_data;
    float m_UnitWidth = 32; //ÿ����Ԫ��Ŀ��
    float m_UnitIntervalWidth = 5; //���ڵ�Ԫ��Ŀ��
    float m_AlignWidth = 10;//ÿ������߽�֮��Ŀ��

    void Test();
   
};