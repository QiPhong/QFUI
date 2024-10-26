#pragma once
#include "QFCtrlBase.h"
#include <list>
#include <cmath>

class QPopDialogBase : public QCtrlBaseD2DIPD
{
public:
    //弹窗被切换会调用这个方法
    virtual int Inverted(QPOINTER ptr);
    QPopDialogBase(HDC winddc, const GRect& rf);
    //依附的对象
    static QWND* m_attchObj;
};




#define __QPOPDIALOG__
class QPopDialog : public QPopDialogBase, public QSCWnd
{
	float m_titleHeight = 0;
	std::wstring m_titleText = L"PopDialog";
	unsigned int m_titleColor1 = 0xff0000ff;
	unsigned int m_titleColor2 = 0xff0000ff;
	unsigned char m_titleColorDirect = 0;//标题颜色渐变方向，0为水平，1为垂直

	//绘制内容的偏移
	float m_offsetX = 0;
	float m_offsetY = 0;
    static HCURSOR m_hCursorBuff;
public:
	QPopDialog(HDC winddc, const GRect& rf);
    virtual ~QPopDialog();
	virtual int Draw(HDC sdc = NULL)override;
	void AttachCtrl(QWND* w);       //关联一个控件
	virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
	virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //重新创建
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
    virtual int LButtonDBLClick(int x, int y, WPARAM wParam)override;//鼠标左键弹起消息，
    virtual int LButtonUp(int x, int y, WPARAM wParam)override;//鼠标左键双击消息，
    virtual int RButtonDown(int x, int y, WPARAM wParam)override;//鼠标右键按下消息，
    virtual int RButtonUp(int x, int y, WPARAM wParam)override;//鼠标右键弹起消息，
    virtual int RButtonDBLClick(int x, int y, WPARAM wParam)override;//鼠标右键双击消息，
    virtual int MButtonDown(int x, int y, WPARAM wParam)override;//鼠标中键按下消息，
    virtual int MButtonUp(int x, int y, WPARAM wParam)override;//鼠标中键弹起消息，
    virtual int MButtonDBLClick(int x, int y, WPARAM wParam)override;//鼠标中键双击消息，
    virtual int MouseWheel(int x, int y, WPARAM wParam)override;//鼠标滚轮转到
    virtual int MouseMoveOut()override;                         //鼠标移出
    virtual int KeyChar(WPARAM wParam, LPARAM lParam)override;//键盘键入一个字符
    virtual int KeyIMEChar(WPARAM wParam, LPARAM lParam)override;//输入法发送一个字符过来
    virtual int KeyDown(WPARAM wParam, LPARAM lParam)override;//键盘按下某键
    virtual int KeyUp(WPARAM wParam, LPARAM lParam)override;//键盘弹起某键
    virtual void killFocus()override;
    //弹窗被切换会调用这个方法
    virtual int Inverted(QPOINTER ptr)override;
    
protected:
    //**************************消息处理
//鼠标消息
    inline int MouseMessage(UINT uMsg, WPARAM wParam, int x, int y);
    inline int MoseMsg(QWND* qwnd, const POINT& pt, UINT uMsg, WPARAM wParam);//
    //键盘消息处理
    inline void KeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);//
    //*********************************
    QWND* FocusCtrl = NULL;           //焦点控件，主要响应键盘的
    QWND* mouseFocusCtrl = NULL;           //鼠标焦点控件
    QWND* mouseSeleFocusCtrl = NULL;           //鼠标选中焦点控件
    QWND* mouseCapture = NULL;         //鼠标捕获控件
};

class PopListObj
{
public:
    //颜色方向,默认和0为水平渐变，1为垂直渐变
    unsigned char colorD=0;
    //背景颜色1
    unsigned int color1 = 0xffffffff;
    //背景颜色2
    unsigned int color2 = 0xffffffff;
    //显示的内容
    std::wstring text;
    //字体颜色1
    unsigned int fcolor1 = 0xff000000;
    //字体颜色2
    unsigned int fcolor2 = 0xff000000;
    //字体颜色方向,默认和0为水平渐变，1为垂直渐变
    unsigned char fcolorD=0;

};
typedef int (*QPOPLISTSELECTINTEMEVENT)(QPOINTER,int);
class QPopList : public  QPopDialogBase
{
protected:


    static HCURSOR m_hCursorBuff;

    //当前鼠标的位置X
    float m_cureentX = -1;
    //当前鼠标的位置Y
    float m_cureentY = -1;


public:
    //绘制内容的偏移X
    std::atomic <float*> m_offsetX = 0;
    //绘制内容的偏移Y
    std::atomic <float*> m_offsetY = 0;
    //操作list的锁
    std::atomic <std::shared_mutex*> m_LK_list;
    //存储list的内容
    std::atomic <std::list<PopListObj>*> m_list;
    //是否打开下拉列表
    std::atomic <bool*> m_isOpen;

    //每一个列表对象的高度
    float m_itemHeight = 32;
    //当前鼠标所在项目索引
    int m_index = -1;
    //被选项目被单击时回调
    QPOPLISTSELECTINTEMEVENT m_selectItem = nullptr;


    QPopList(HDC winddc, const GRect& rf);
    virtual ~QPopList();
    virtual int Draw(HDC sdc = NULL)override;
    virtual  QDWORD TransferMessage(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam)override;
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //重新创建
    virtual int MouseMove(int x, int y, WPARAM wParam)override;//鼠标移动消息
    virtual int MouseMoveOut();                         //鼠标移出
    virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
    virtual int MouseWheel(int x, int y, WPARAM wParam);//鼠标滚轮转到
    //弹窗被切换会调用这个方法
    virtual int Inverted(QPOINTER ptr);
};
