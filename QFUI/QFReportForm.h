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




//�б�ͷ��Ԫ
class QReportFormTitleUnit
{
public:
    //�б��ı�
    std::wstring text = L"����";
    //��ͷ���ĸ���ɫ��������ɫʹ��
    GColor color1;
    GColor color2;
    GColor color3;
    GColor color4;
    //���
    int width = 50;
    QReportFormTitleUnit();
    QReportFormTitleUnit(const QReportFormTitleUnit& rft);
    void operator=(const QReportFormTitleUnit& rft);
};
//�б�ͷ
class QReportFormTitle
{
public:
    //��ͷ�߶�
    int height = 32;
    std::vector<QReportFormTitleUnit> unit;


};
//��¼��Ԫ�Ļ�ˢ����
#define QREPORTFORMCONTEXT_SOLIDE                   0x0000//����ʹ��ʵ�Ļ�ˢ
#define QREPORTFORMCONTEXT_GRADUAL_V                0x0001//���Դ�ֱ
#define QREPORTFORMCONTEXT_GRADUAL_H                0x0002//����ˮƽ
#define QREPORTFORMCONTEXT_GRADUAL_LTRB             0x0003//������������
#define QREPORTFORMCONTEXT_GRADUAL_RTLB             0x0004//������������

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
    INT W = 80;  //���ο��
    int F = 0;//�ϱ߶����ֵ
    bool operator<(const QReportFormBrushContext& tc)const;
};


//��¼��Ԫ
class QRecorderUnit
{
public:
    std::wstring text;
    QReportFormBrushContext bContext;
    std::shared_ptr<GBrush> pGBrush = NULL;


};


//��¼
class QRecorder
{
public:
    QRecorder() {}
    QRecorder(size_t count) { unit = std::vector<QRecorderUnit>(count); }

    std::vector<QRecorderUnit> unit;
    //��ˢ������
    std::map<QReportFormBrushContext, std::shared_ptr<GBrush>> m_BToolBox;
    //���õ�Ԫ����
    void SetUnitText(int index, std::wstring t);
    void SetUnitBackgroundColor(int index, QReportFormBrushContext context, INT height);
    inline GBrush* CreaterGBrush(const QReportFormBrushContext& rf, INT height);
    inline int InsertUint(int index);


};

class QReportForm :public QCtrlBase
{
    //��ͷ����
    QReportFormTitle m_Title;
    //��������
    std::vector<QRecorder> m_data;
    //��¼�ĸ߶�
    INT height = 40;
    //ˮƽ����λ��
    int RollPointHorizontal = 0;
    //��ֱ����λ��
    int RollPointVertical = 0;

    //�б�������ɾ��
    std::shared_mutex m_adlock;


    bool m_LbtIsDown = false;
    int m_LDownPosY = -1;//��갴��ʱy����������˵�λ��


    //������ʾ����
    int selectItemIndex = -1;//��������
    GColor SItemColor;

public:
    bool m_isDrag = false;//��갴���Ƿ���϶��б����»���
    inline int InsertRecorder(const std::wstring& w = L"", int index = -1);//��������0Ϊ���뵽��ǰ�棬������¼�������Ч
    inline int RemoveRecorder(int index);
    inline int SelectItemIndex();
    /**
     * ============================================================================
     * ��������SetRecorderText
     * ���ã�������Ŀ�ı�
     * ����1��������
     * ����2��������
     * ����3�������õ��ı�
     * ����ֵ������1����������Ϊʧ�ܴ���
     * ============================================================================
    */
    inline int SetRecorderText(int iRd, int iUnit, std::wstring context);
    inline int GetRecorderCount()const;                                                             //��ȡ��¼����
    inline int GetTitleCount()const;                                                                //��ȡ�ֶ�����
    /**
     * ============================================================================
     * SetRecorderBK
     * ���ã�������Ŀ�ı���
     * ����1��������
     * ����2����������-1Ϊ��������һֱ��ˢ
     * ����3�������õı�����ˢ
     * ����ֵ������1����������Ϊʧ�ܴ���
     * ============================================================================
    */
    inline int SetRecorderBK(int iRd, int iUnit, const QReportFormBrushContext& context);



    QReportForm(HDC winddc, const GRect& rf);
    virtual ~QReportForm();


    virtual int Draw(HDC sdc = NULL)override;


    //��������
    void SetFontName(const wchar_t* name);
    void SetFontSize(INT size);
    void SetTextAlgn(const Gdiplus::StringAlignment& align);
    void SetTextAlgn(const Gdiplus::StringAlignment& align, const Gdiplus::StringAlignment& lalign);
    void SetTextAllgn(const Gdiplus::StringAlignment& lalign);
    void SetBKBrush(GBrush* pgb);
    const GBrush* GetBKBrush()const;

    void InsertTitle(const std::wstring& w = L"", INT wdth = 80, int index = -1);

    int SetTitleHeight(int h);

    const wchar_t* GetTitleText(int index); //��ȡ�ֶ��ı�
    int GetTitleWidth(int index);//��ȡ�ֶο��

    int RemovTitle(int index);
    int SetTitle(const std::wstring& w , INT wdth , int index);


    const wchar_t* GetCellText(int iRd, int iUnit);
public:


    //�����Ϣ����
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ��
    virtual int LButtonDBLClick(int x, int y, WPARAM wParam)override;//������������Ϣ��
    virtual int LButtonUp(int x, int y, WPARAM wParam)override;//������˫����Ϣ��

    virtual int RButtonDown(int x, int y, WPARAM wParam)override;//����Ҽ�������Ϣ��
    virtual int RButtonUp(int x, int y, WPARAM wParam)override;//����Ҽ�������Ϣ��
    virtual int RButtonDBLClick(int x, int y, WPARAM wParam)override;//����Ҽ�˫����Ϣ��

    virtual int MButtonDown(int x, int y, WPARAM wParam)override;//����м�������Ϣ��
    virtual int MButtonUp(int x, int y, WPARAM wParam)override;//����м�������Ϣ��
    virtual int MButtonDBLClick(int x, int y, WPARAM wParam)override;//����м�˫����Ϣ��

    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int MouseWheel(int x, int y, WPARAM wParam)override;




private:
    GBrush* m_bkBrush = NULL;




    //****************************************************
    //��������
    Gdiplus::StringAlignment sAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    Gdiplus::StringAlignment slAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    std::wstring m_fontName = L"Arial";
    INT m_fontSize = 12;

    //***************************************************



};

    #undef __QFREPORTFORM_TYPEDEF






#endif
