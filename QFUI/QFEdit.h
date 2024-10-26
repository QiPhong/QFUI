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


typedef int (*QEDIT_CHANGE_EVENT)(int, int, int);//����1������ָ�룻����2���ַ���ָ�룻����3������ʱҪ�����ʱ����
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
    Gdiplus::ImageAttributes imageAttr;//ͼƬ����
    GImage* m_pBKImage = NULL;    //����ͼƬ
    inline void __LoadPicture(const std::wstring& path);

    GBrush* m_bkBrush = NULL; //������ˢ
    GColor m_bodyColo;


public:
    QEdit(HDC winddc, const GRect& rf);
    virtual ~QEdit();
    inline void SetBKImageTransparency(unsigned char apha);
    void LoadPicture(const std::wstring& path);
    virtual void Redraw() { isChange = true; };

    void SetBKBrush(GBrush* brush);
    //ʧȥ����
    virtual void killFocus();

    ///////////////////////////////////////////////////////////////////////////
    //��Ϣ����
    // virtual int MsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);//ԭʼ��Ϣ

    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ��
    // virtual int LButtonDBLClick(int x,int y, WPARAM wParam);//������������Ϣ��
    // virtual int LButtonUp(int x,int y, WPARAM wParam);//������˫����Ϣ��

    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam)override;//���뷨����һ���ַ�
    virtual int KeyChar(WPARAM wParam, LPARAM lParam)override;//���뷨����һ���ַ�
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int KeyDown(WPARAM wParam, LPARAM lParam)override;//���̰���ĳ��


    ///////////////////////////////////////////////////////////////////////////
    //*********************������������*******************************
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
    //��������
    std::wstring m_text = L"";
    Gdiplus::StringAlignment slAlign = Gdiplus::StringAlignment::StringAlignmentCenter;
    std::wstring m_fontName = L"Arial";
    INT m_fontSize = 12;
    GBrush* m_fontBrush = NULL;
    int __FontHeight = 0;//����߶ȣ���Ҫ���ã�����Ҳû���ã���ϵͳ��¼��
    Gdiplus::REAL __drawStartpos = 0.0f;//��ʼ�������ֵ�λ��
    GStringFormat* m_gsf = NULL;
    //********************************************************************************************

    //**************************���������*********************************************************
    Gdiplus::REAL m_CaretPos = 0.0f;
    int m_CaretIndex = 0;//������ڵڼ����ַ�����
    unsigned char m_CaretState = 0;//�����״̬0Ϊ����ʾ��1Ϊ��ʾ
    unsigned int m_CaretPeriod = 500;//��������ڣ���λ����
    std::chrono::time_point<std::chrono::high_resolution_clock> m_CaretUpdateTime;//����ʱ�䣬 std::chrono::high_resolution_clock::now()
    bool m_isFocus = false;
    inline void __upDateCare_up();//���Ƹ��¹��
    inline void __upDateCare_down();//���Ƹ��¹��

    //********************************************************************************************


    //�ڲ��ַ�����������***************************************************************************
    inline void __KeyInput(WPARAM wParam, LPARAM lParam);
    inline void __AddChar(wchar_t wc);//����һ���ַ�
    inline void __RemoveChar();//�Ƴ�һ���ַ�
    inline int __TrailingCharacter(GREAL wf);//��õ�ǰָ�����������һ���ַ���λ��
    inline void __UpdateCarePos();



    //*******************************************************************************************
public:
    void SetChangeEvent(QEDIT_CHANGE_EVENT even,int param);
    
};

