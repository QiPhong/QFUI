#pragma once
#include "QFCtrlBase.h"
#include <chrono>
#include <string>
#include "QFButton.h"
//
#define __QPBUTTON__
class QPButton : public QCtrlBaseD2DIPD
{
    float __margin = 5;//��ť���ߵ���߾�ľ���
    float __ShrinkageRatio = 1.01;//��������
    std::chrono::steady_clock::time_point __startTime;//��ʼ����ʱ��
    std::chrono::steady_clock::time_point __checkPathTime;
    //����Ƿ��ڰ�ť��
    ID2D1PathGeometry* pBTPathGeometry = nullptr;//��ǰ����
    ID2D1GeometrySink* pBTSink = nullptr;

    std::wstring __text = L"Button";
    unsigned int __BKColor = 0xff0000ff;

    QFBUTTONCLICK m_clickEvent = NULL;//�¼���������

    //ͼƬ���ַ�ʽ:0Ϊ��ߣ�1Ϊ�ұߣ�2Ϊ�ϱߣ�3Ϊ�±ߣ�4Ϊ����
    unsigned char __picType = 0;

    ID2D1Bitmap* pBitmap = nullptr;    //��ǩͼ��
    std::wstring mapPath;//ͼ��·��
    float __xRadius = 20.0,__yRadius =20.0;//��ťԲ�ǰ뾶
    int m_Status = 0;
    


public:
    QPButton(HDC winddc, const GRect& rf);
    ~QPButton();
    virtual int Draw(HDC sdc = NULL)override;
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int MouseMoveOut()override;

public:
    //�û��ӿ�
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

