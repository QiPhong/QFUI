#pragma once
#include "QFCtrlBase.h"
#include <list>
#include <cmath>

class QPopDialogBase : public QCtrlBaseD2DIPD
{
public:
    //�������л�������������
    virtual int Inverted(QPOINTER ptr);
    QPopDialogBase(HDC winddc, const GRect& rf);
    //�����Ķ���
    static QWND* m_attchObj;
};




#define __QPOPDIALOG__
class QPopDialog : public QPopDialogBase, public QSCWnd
{
	float m_titleHeight = 0;
	std::wstring m_titleText = L"PopDialog";
	unsigned int m_titleColor1 = 0xff0000ff;
	unsigned int m_titleColor2 = 0xff0000ff;
	unsigned char m_titleColorDirect = 0;//������ɫ���䷽��0Ϊˮƽ��1Ϊ��ֱ

	//�������ݵ�ƫ��
	float m_offsetX = 0;
	float m_offsetY = 0;
    static HCURSOR m_hCursorBuff;
public:
	QPopDialog(HDC winddc, const GRect& rf);
    virtual ~QPopDialog();
	virtual int Draw(HDC sdc = NULL)override;
	void AttachCtrl(QWND* w);       //����һ���ؼ�
	virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
	virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //���´���
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
    virtual int LButtonDBLClick(int x, int y, WPARAM wParam)override;//������������Ϣ��
    virtual int LButtonUp(int x, int y, WPARAM wParam)override;//������˫����Ϣ��
    virtual int RButtonDown(int x, int y, WPARAM wParam)override;//����Ҽ�������Ϣ��
    virtual int RButtonUp(int x, int y, WPARAM wParam)override;//����Ҽ�������Ϣ��
    virtual int RButtonDBLClick(int x, int y, WPARAM wParam)override;//����Ҽ�˫����Ϣ��
    virtual int MButtonDown(int x, int y, WPARAM wParam)override;//����м�������Ϣ��
    virtual int MButtonUp(int x, int y, WPARAM wParam)override;//����м�������Ϣ��
    virtual int MButtonDBLClick(int x, int y, WPARAM wParam)override;//����м�˫����Ϣ��
    virtual int MouseWheel(int x, int y, WPARAM wParam)override;//������ת��
    virtual int MouseMoveOut()override;                         //����Ƴ�
    virtual int KeyChar(WPARAM wParam, LPARAM lParam)override;//���̼���һ���ַ�
    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam)override;//���뷨����һ���ַ�����
    virtual int KeyDown(WPARAM wParam, LPARAM lParam)override;//���̰���ĳ��
    virtual int KeyUp(WPARAM wParam, LPARAM lParam)override;//���̵���ĳ��
    virtual void killFocus()override;
    //�������л�������������
    virtual int Inverted(QPOINTER ptr)override;
    
protected:
    //**************************��Ϣ����
//�����Ϣ
    inline int MouseMessage(UINT uMsg, WPARAM wParam, int x, int y);
    inline int MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//
    //������Ϣ����
    inline void KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);//
    //*********************************
    QWND* FocusCtrl = NULL;           //����ؼ�����Ҫ��Ӧ���̵�
    QWND* mouseFocusCtrl = NULL;           //��꽹��ؼ�
    QWND* mouseSeleFocusCtrl = NULL;           //���ѡ�н���ؼ�
    QWND* mouseCapture = NULL;         //��겶��ؼ�
};

class PopListObj
{
public:
    //��ɫ����,Ĭ�Ϻ�0Ϊˮƽ���䣬1Ϊ��ֱ����
    unsigned char colorD=0;
    //������ɫ1
    unsigned int color1 = 0xffffffff;
    //������ɫ2
    unsigned int color2 = 0xffffffff;
    //��ʾ������
    std::wstring text;
    //������ɫ1
    unsigned int fcolor1 = 0xff000000;
    //������ɫ2
    unsigned int fcolor2 = 0xff000000;
    //������ɫ����,Ĭ�Ϻ�0Ϊˮƽ���䣬1Ϊ��ֱ����
    unsigned char fcolorD=0;

};
typedef int (*QPOPLISTSELECTINTEMEVENT)(QPOINTER,int);
class QPopList : public  QPopDialogBase
{
protected:


    static HCURSOR m_hCursorBuff;

    //��ǰ����λ��X
    float m_cureentX = -1;
    //��ǰ����λ��Y
    float m_cureentY = -1;


public:
    //�������ݵ�ƫ��X
    std::atomic <float*> m_offsetX = 0;
    //�������ݵ�ƫ��Y
    std::atomic <float*> m_offsetY = 0;
    //����list����
    std::atomic <std::shared_mutex*> m_LK_list;
    //�洢list������
    std::atomic <std::list<PopListObj>*> m_list;
    //�Ƿ�������б�
    std::atomic <bool*> m_isOpen;

    //ÿһ���б����ĸ߶�
    float m_itemHeight = 32;
    //��ǰ���������Ŀ����
    int m_index = -1;
    //��ѡ��Ŀ������ʱ�ص�
    QPOPLISTSELECTINTEMEVENT m_selectItem = nullptr;


    QPopList(HDC winddc, const GRect& rf);
    virtual ~QPopList();
    virtual int Draw(HDC sdc = NULL)override;
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //���´���
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//����ƶ���Ϣ
    virtual int MouseMoveOut();                         //����Ƴ�
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
    virtual int MouseWheel(int x, int y, WPARAM wParam);//������ת��
    //�������л�������������
    virtual int Inverted(QPOINTER ptr);
};
