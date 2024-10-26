#pragma once
#include "DLLImportQWND.h"
#include <windows.h>
#include <string>
#include "QFbutton.h"
#include "QFUIBase.h"

#define ____EXTERN_FFUN(flag,name) flag##name
#define __EXTERN_FFUN(flag,name) ____EXTERN_FFUN(flag,name)
/**
* QDrawer ����ؼ�
* 
* 
* 
*/
//�������¼�
//��Ŀ��ѡ��ʱ�¼�����������1��Ϊѡ��ʱ����Ŀ����������2���¼�ѡ��ʱ������Ŀ����
typedef int (*QDRAWERSELECT)(int, int);
typedef void (*QDWSetFontName_Type)(QWND* p, const wchar_t* name);
typedef void (*QDWSetFontSize_Type)(QWND* p, float size);
typedef int (*QDWInsertDrawer_Type)(QWND* p, int index);
typedef int (*QDWInsertSubObject_Type)(QWND* p, int iDrawer, int iObj);
typedef int (*QDWRemoveSubObject_Type)(QWND* p, int iDrawer, int iObj);
typedef int (*QDWRemoveDrawer_Type)(QWND* p, int index);
typedef int (*QDWGetDrawerCount_Type)(QWND* p);
typedef int (*QDWGetSubObjectCount_Type)(QWND* p, int index);
typedef int (*QDWSetDrawerName_Type)(QWND* p, int index, const wchar_t* name);
typedef int (*QDWSetDrawerFontColor_Type)(QWND* p, int index, int color);
typedef int (*QDWSetDrawerBKColor_Type)(QWND* p, int index, int color);
typedef int (*QDWOpenDrawer_Type)(QWND* p, int index);
typedef int (*QDWCloseDrawer_Type)(QWND* p, int index);
typedef int (*QDWSetSubObjectName_Type)(QWND* p, int iDrawer, int iObj, const wchar_t* name);
typedef int (*QDWSetSubObjectFontColor_Type)(QWND* p, int iDrawer, int iObj, int color);
typedef int (*QDWSetSubObjectBKColor_Type)(QWND* p, int iDrawer, int iObj, int color);
typedef int (*QDWSetSelectEvent_Type)(QWND* p, QDRAWERSELECT fun);
typedef int (*QDWGetSubObjectName_Type)(QWND* p, int iDrawer, int iObj, wchar_t* pName, int bl);
typedef int (*QDWGetDrawerName_Type)(QWND* p, int iDrawer, wchar_t* pName, int bl);
#define __DefineFun(type,name) static type name
#define __GDefineFun(cname,type,name) type cname::name=NULL
#define __QDefineType(name) name##_Type
#define __MembersDef(name) __DefineFun(__QDefineType(name),name)
#define __FunTypeDef(fName) fName##_Type
#define __FunTypeDef2(a) __FunTypeDef(a)

#define __QDRAWER__
class QDrawer :public DLLImportQWND
{
    static HMODULE m_hm;
    __MembersDef(QDWSetFontName);
    __MembersDef(QDWSetFontSize);
    __MembersDef(QDWInsertDrawer);
    __MembersDef(QDWInsertSubObject);
    __MembersDef(QDWRemoveSubObject);
    __MembersDef(QDWRemoveDrawer);
    __MembersDef(QDWGetDrawerCount);

    __MembersDef(QDWGetSubObjectCount);
    __MembersDef(QDWSetDrawerName);
    __MembersDef(QDWSetDrawerFontColor);
    __MembersDef(QDWSetDrawerBKColor);
    __MembersDef(QDWOpenDrawer);
    __MembersDef(QDWCloseDrawer);
    __MembersDef(QDWSetSubObjectName);

    __MembersDef(QDWSetSubObjectFontColor);
    __MembersDef(QDWSetSubObjectBKColor);
    __MembersDef(QDWSetSelectEvent);
    __MembersDef(QDWGetSubObjectName);
    __MembersDef(QDWGetDrawerName);

public:
    QDrawer(HDC winddc, const QRect& rf);
    virtual ~QDrawer();
    //�û��ӿ�
    void SetFontName(const wchar_t* name);
    void SetFontSize(float size);
    int InsertDrawer(int index);
    int InsertSubObject(int iDrawer, int iObj);
    int RemoveSubObject(int iDrawer, int iObj);
    int RemoveDrawer(int index);
    int GetDrawerCount();
    int GetSubObjectCount(int index);
    int SetDrawerName(int index, const wchar_t* name);
    int SetDrawerFontColor(int index, int color);
    int SetDrawerBKColor(int index, int color);
    int OpenDrawer(int index);
    int CloseDrawer(int index);
    int SetSubObjectName(int iDrawer, int iObj, const wchar_t* name);
    int SetSubObjectFontColor(int iDrawer, int iObj, int color);
    int SetSubObjectBKColor(int iDrawer, int iObj, int color);
    int SetSelectEvent(QDRAWERSELECT fun);

    std::wstring GetSubObjectName(int iDrawer, int iObj);
    std::wstring GetDrawerName(int iDrawer);

    virtual int init(HMODULE hm);

};




#define __QHOSTCTRL__
typedef int (*QHCSetParasiticCtrl_Type)(QWND* p, QWND* ctrl, QWND** oldCtrl);
typedef QWND* (*__FunTypeDef(QHCGetParasiticCtrl))(QWND* p);

class QHost :public DLLImportQWND
{
    static HMODULE m_hm;
    __MembersDef(QHCSetParasiticCtrl);
    __MembersDef(QHCGetParasiticCtrl);

public:
    QHost(HDC winddc, const QRect& rf);
    virtual ~QHost();
    int SetParasiticCtrl(DLLImportQWND* ctrl, QWND** oldCtrl = nullptr);
    QWND* GetParasiticCtrl();
    int init(HMODULE hm);
    //����-1 Ϊ�ؼ�����ֱ��Ϊ�����ؼ�

};

#define __QHLAYOUT__
typedef int (*QHLInsertCtrl_Type)(QWND* , QWND* , unsigned int , int );
typedef int (*QHLRemoveCtrl_Type)(QWND*, int);
typedef int (*QHLGetCtrlCount_Type)(QWND*);
typedef QWND* (*__FunTypeDef(QHLGetCtrl))(QWND* p,  int index);
typedef unsigned int (*__FunTypeDef(QHLGetCtrlWeight))(QWND* p, int index);
class QHLayout :public DLLImportQWND
{
    static HMODULE m_hm;
    __MembersDef(QHLInsertCtrl);
    __MembersDef(QHLRemoveCtrl);
    __MembersDef(QHLGetCtrlCount);
    __MembersDef(QHLGetCtrl);
    __MembersDef(QHLGetCtrlWeight);

public:
    QHLayout(HDC winddc, const QRect& rf);
    int init(HMODULE hm);
    int InsertCtrl(DLLImportQWND* ctrl, unsigned int weight = 0, int index = -1);//����ؼ���weight���ؼ�����Ȩ�أ�index�������λ�ã�-1Ϊβ��
    int RemoveCtrl(int index);//�Ƴ��ؼ�
    int GetCtrlCount();//ȡ�ؼ���
    QWND* GetCtrl(int index);
    unsigned int GetCtrlWeight(int index);
};
#define __QVLAYOUT__
typedef int (*QVLInsertCtrl_Type)(QWND*, QWND*, unsigned int, int);
typedef int (*QVLRemoveCtrl_Type)(QWND*, int);
typedef int (*QVLGetCtrlCount_Type)(QWND*);
typedef QWND* (*__FunTypeDef(QVLGetCtrl))(QWND* p, int index);
typedef unsigned int (*__FunTypeDef(QVLGetCtrlWeight))(QWND* p, int index);
class QVLayout :public DLLImportQWND
{
    static HMODULE m_hm;
    __MembersDef(QVLInsertCtrl);
    __MembersDef(QVLRemoveCtrl);
    __MembersDef(QVLGetCtrlCount);
    __MembersDef(QVLGetCtrl);
    __MembersDef(QVLGetCtrlWeight);
public:
    QVLayout(HDC winddc, const QRect& rf);
    int init(HMODULE hm);
    int InsertCtrl(DLLImportQWND* ctrl, unsigned int weight = 0, int index = -1);//����ؼ���weight���ؼ�����Ȩ�أ�index�������λ�ã�-1Ϊβ��
    int RemoveCtrl(int index);//�Ƴ��ؼ�
    int GetCtrlCount();//ȡ�ؼ���
    QWND* GetCtrl(int index);
    unsigned int GetCtrlWeight(int index);
};
#define __QTLAYOUT__
typedef int (*__FunTypeDef(QTLSetCtrl))(QWND*, QWND*);
typedef int (*__FunTypeDef(QTLRemoveCtrl))(QWND*);
typedef QWND* (*__FunTypeDef(QTLGetCtrl))(QWND*);

class QTLayout :public DLLImportQWND
{
    static HMODULE m_hm;
    __MembersDef(QTLSetCtrl);
    __MembersDef(QTLRemoveCtrl);
    __MembersDef(QTLGetCtrl);

public:
    QTLayout(HDC winddc, const QRect& rf);
    int init(HMODULE hm);
    int SetCtrl(QWND* ctrl);//����ؼ���weight���ؼ�����Ȩ�أ�
    int RemoveCtrl();//�Ƴ��ؼ�
    QWND* GetCtrl();
};



#define __QSTATICTEXT__
typedef void (*QSTTSetText_Type)(QWND* p, const wchar_t* t);
typedef void (*QSTTSetFontName_Type)(QWND* p, const wchar_t* t);
typedef void (*QSTTSetFontSize_Type)(QWND* p, double);
typedef void (*QSTTSetFontColor_Type)(QWND* p, unsigned int);
typedef void (*QSTTSetBKColor_Type)(QWND* p, unsigned int);
typedef const wchar_t* (*__FunTypeDef(QSTTGetText))(QWND* p);

class QStaticText :public DLLImportQWND
{
    static HMODULE m_hm;
    __MembersDef(QSTTSetText);
    __MembersDef(QSTTSetFontName);
    __MembersDef(QSTTSetFontSize);
    __MembersDef(QSTTSetFontColor);
    __MembersDef(QSTTSetBKColor);
    __MembersDef(QSTTGetText);

public:
    QStaticText(HDC winddc, const QRect& rf);
    int init(HMODULE hm);
    void SetText(const wchar_t* t);
    void SetFontName(const wchar_t* t);
    void SetFontSize(double size);
    void SetFontColor(unsigned int color);
    void SetBKColor(unsigned int color);
    std::wstring GetText();

};


#define __QCTRLTOOLBOX__
typedef int (*QCTBInsertCtrl_Type)(QWND*, QWND*, unsigned int, int);
typedef int (*QCTBRemoveCtrl_Type)(QWND*, int);
typedef int (*QCTBGetCtrlCount_Type)(QWND*);
typedef int (*__FunTypeDef(QCTBInsertAttributeCtrl))(QWND* p, QWND* w, int index);
typedef int (*__FunTypeDef(QCTBRemoveAttributeCtrl))(QWND* p, int index);
typedef void (*__FunTypeDef(QCTBClearAttributeCtrl))(QWND* p);
typedef void (*__FunTypeDef(QCTBSetFileButtonEvent))(QWND* p, QFBUTTONCLICK en, int index);

class QCtrlToolbox :public DLLImportQWND
{
    static HMODULE m_hm;
    __MembersDef(QCTBInsertCtrl);
    __MembersDef(QCTBRemoveCtrl);
    __MembersDef(QCTBGetCtrlCount);
    __MembersDef(QCTBInsertAttributeCtrl);
    __MembersDef(QCTBRemoveAttributeCtrl);
    __MembersDef(QCTBClearAttributeCtrl);
    __MembersDef(QCTBSetFileButtonEvent);

public:
    QCtrlToolbox(HDC winddc, const QRect& rf);
    int init(HMODULE hm);
    int InsertCtrl(DLLImportQWND* ctrl, unsigned int weight = 0, int index = -1);//����ؼ���weight���ؼ�����Ȩ�أ�index�������λ�ã�-1Ϊβ��
    int RemoveCtrl(int index);//�Ƴ��ؼ�
    int GetCtrlCount();//ȡ�ؼ���
    int InsertAttributeCtrl(DLLImportQWND* w, int index = -1);       //����һ�����Կؼ�
    int RemoveAttributeCtrl(int index);       //�Ƴ�һ�����Կؼ�
    void ClearAttributeCtrl();       //�Ƴ�һ�����Կؼ�
    void SetFileButtonEvent(QFBUTTONCLICK en, int index = 0);
};


#define __QEDITORWIN__
typedef int (*QEDITORWIN_LBDOWN)(int, int, int);
typedef int (*QEDITORWIN_PLACE_CTRL)(int, int, int, int, int);
typedef void (*QEDITORWIN_DELETE_CTRL)(int);
typedef void (*QEDITORWIN_SELECT_CTRL)(int);
typedef int (*QEWSetLButtonEvent_Type)(QWND*, QEDITORWIN_LBDOWN);
typedef void (*QEWAttachCtrl_Type)(QWND*, QWND*);
typedef void (*QEWRemoveCtrl_Type)(QWND*, QWND*);
typedef int (*QEWSetPlaceCtrlEvent_Type)(QWND*, QEDITORWIN_PLACE_CTRL);
typedef int (*__FunTypeDef(QEWSetDeleteCtrlEvent))(QWND* p, QEDITORWIN_DELETE_CTRL);
typedef int (*__FunTypeDef(QEWSetSelectCtrlEvent))(QWND* p, QEDITORWIN_DELETE_CTRL);

class QEditorWin :public DLLImportQWND
{
    static HMODULE m_hm;
    __MembersDef(QEWSetLButtonEvent);
    __MembersDef(QEWAttachCtrl);
    __MembersDef(QEWRemoveCtrl);
    __MembersDef(QEWSetPlaceCtrlEvent);
    __MembersDef(QEWSetDeleteCtrlEvent);
    __MembersDef(QEWSetSelectCtrlEvent);
public:
    QEditorWin(HDC winddc, const QRect& rf);
    int init(HMODULE hm);
    int SetLButtonEvent(QEDITORWIN_LBDOWN ent);
    void AttachCtrl(DLLImportQWND* w);       //����һ���ؼ�
    void RemoveCtrl(DLLImportQWND* w);       //�Ƴ�һ���ؼ�
    int SetPlaceCtrlEvent(QEDITORWIN_PLACE_CTRL ent);
    int SetDeleteCtrlEvent(QEDITORWIN_DELETE_CTRL ent);
    int SetSelectCtrlEvent(QEDITORWIN_SELECT_CTRL ent);
};

#define __QSWITCH__
typedef int (*QSWITCH_STATE_CHANGE)(int);
#define __QFLAG QSW
typedef int (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetStateChangeEvent)))(QWND* p, QSWITCH_STATE_CHANGE ev);
typedef int (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, GetState)))(QWND* p);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetState)))(QWND* p,int);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, IsMutex)))(QWND* p, bool);

class QSwitch :public QCtrlBaseD2DIPD
{
    static HMODULE m_hm;
    __MembersDef(__EXTERN_FFUN(QSW, SetStateChangeEvent));
    __MembersDef(__EXTERN_FFUN(QSW, GetState));
    __MembersDef(__EXTERN_FFUN(QSW, SetState));
    __MembersDef(__EXTERN_FFUN(QSW, IsMutex));

public:
    QSwitch(HDC winddc, const QRect& rf);
    int init(HMODULE hm);
    //����״̬�ı�ʱ�Ļص�����
    //typedef int (*QSWITCH_STATE_CHANGE)(int);
    int SetStateChangeEvent(QSWITCH_STATE_CHANGE ev);
    //��õ�ǰ״̬��0Ϊδѡ�У�1Ϊѡ��
    int GetState()const;
    //���õ�ǰ��״̬��0Ϊδѡ�У�1Ϊѡ��
    void SetState(int state);
    //�Ƿ񻥳�
    //���������ͬһ�������ڵ�QSwitchֻ��ͬʱѡ��һ��
    void IsMutex(bool im);
};

#define __QSLIDER__
typedef int (*QSLIDER_VALUE_CHANGE)(int, float);
#define __QFLAG QSL
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetMinValue)))(QWND* p, float);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetMaxValue)))(QWND* p, float);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetCurrentValue)))(QWND* p, float);
typedef unsigned int (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetRailColor)))(QWND* p, unsigned int);
typedef float (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetRailRatio)))(QWND* p, float);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, IsSegmentation)))(QWND* p, bool);
typedef int (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, ClearSegmArr)))(QWND* p);
typedef int (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, AddSegmVuale)))(QWND* p,float);
typedef int (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, RemoveSegmVuale)))(QWND* p, float);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetDirection)))(QWND* p,unsigned char);
typedef QDWORD (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetValueChangeEvent)))(QWND* p, QSLIDER_VALUE_CHANGE);
typedef unsigned int(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetColor)))(QWND* p, unsigned int);
typedef int(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, GetSegmValue)))(QWND* p, float* ,int);

class QSlider :public QCtrlBaseD2DIPD
{
    static HMODULE m_hm;
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetMinValue));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetMaxValue));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetCurrentValue));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetRailColor));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetRailRatio));
    __MembersDef(__EXTERN_FFUN(__QFLAG, IsSegmentation));
    __MembersDef(__EXTERN_FFUN(__QFLAG, ClearSegmArr));
    __MembersDef(__EXTERN_FFUN(__QFLAG, AddSegmVuale));
    __MembersDef(__EXTERN_FFUN(__QFLAG, RemoveSegmVuale));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetDirection));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetValueChangeEvent));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetColor));
    __MembersDef(__EXTERN_FFUN(__QFLAG, GetSegmValue));

public:
    QSlider(HDC winddc, const QRect& rf);
    int init(HMODULE hm);

    //���û������Сֵ
    void SetMinValue(float v);
    //���û�������ֵ
    void SetMaxValue(float v);
    //���û���ĵ�ǰֵ
    void SetCurrentValue(float v);
    //���û���Ĺ����ɫ
    unsigned int SetRailColor(unsigned int color);
    //���û���Ĺ������
    float SetRailRatio(float ato);
    //�Ƿ�ֶ�
    void IsSegmentation(bool b);
    //��շֶ�����
    int ClearSegmArr();
    //���ӷֶ�ֵ
    int AddSegmVuale(float v);
    //�Ƴ��ֶ�ֵ
    int RemoveSegmVuale(float v);
    //���÷���0Ϊˮƽ��1Ϊ��ֱ
    void SetDirection(unsigned char d);
    //���û����ص�����
    //typedef int (*QSLIDER_VALUE_CHANGE)(int, float);
    QDWORD SetValueChangeEvent(QSLIDER_VALUE_CHANGE ev);
    //������ɫ
    unsigned int SetColor(unsigned int color);
    //��õ�ǰ�ֶ�ֵ
    //����1���洢�ֶ�ֵ������ָ��
    //����2���ռ䳤��
    //����ֵ����ǰ�ؼ��ֶ�ֵ������
    int GetSegmValue(float* arr, int bufL);
};


#define __QPROGRESS__

#define __QFLAG QPRO
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetDirection)))(QWND* p, unsigned char);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetStyle)))(QWND* p, unsigned int);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetCurrentValue)))(QWND* p,float);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetLineTypeProgressColor)))(QWND* p, unsigned int);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetDecimalPlaces)))(QWND* p, unsigned char);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetRingProgressColor)))(QWND* p, unsigned int, unsigned int);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetRingProgressThickness)))(QWND* p, float);
typedef unsigned int (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, GetStyle)))(QWND* p);

class QProgress :public QCtrlBaseD2DIPD
{
    static HMODULE m_hm;
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetDirection));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetStyle));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetCurrentValue));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetLineTypeProgressColor));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetDecimalPlaces));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetRingProgressColor));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetRingProgressThickness));
    __MembersDef(__EXTERN_FFUN(__QFLAG, GetStyle));
public:
    QProgress(HDC winddc, const QRect& rf);
    int init(HMODULE hm);
    //���ý���������
    void SetDirection(unsigned char d);
    //���ý�������ʽ
    void SetStyle(unsigned int style);
    //���õ�ǰֵ
    void SetCurrentValue(float v);
    //�������ͽ���������ɫ
    void SetLineTypeProgressColor(unsigned int color);
    //����С����ʾ��λ��
    void SetDecimalPlaces(unsigned char dp);
    //���û��ν���������ɫ
    void SetRingProgressColor(unsigned int color1, unsigned int color2);
    //����Բ�����
    void SetRingProgressThickness(float thickness);
    //��ý�������ʽ
    unsigned int GetStyle();
};


//��ť2
#define __QBUTTON__
#define __EXTERN_FFUN_QB2(name) QB2##name
#define __FunTypeDef_QB2(fName) fName##_Type
//typedef const wchar_t* (*__FunTypeDef(__EXTERN_FFUN(QB2,GetText)))(QWND* p);
typedef const wchar_t* (*__FunTypeDef2(__EXTERN_FFUN_QB2(GetText)))(QWND* p);
typedef void (*__FunTypeDef2(__EXTERN_FFUN_QB2(SetText)))(QWND* p,const wchar_t*);
typedef void (*__FunTypeDef2(__EXTERN_FFUN_QB2(SetClickEvent)))(QWND* p, QFBUTTONCLICK);
typedef float (*__FunTypeDef2(__EXTERN_FFUN_QB2(SetMargin)))(QWND* p, float);
typedef unsigned int (*__FunTypeDef2(__EXTERN_FFUN_QB2(SetBKColor)))(QWND* p, unsigned int);
typedef unsigned char (*__FunTypeDef2(__EXTERN_FFUN_QB2(SetPicType)))(QWND* p, unsigned char);
typedef int (*__FunTypeDef2(__EXTERN_FFUN_QB2(LoadPicture)))(QWND* p, const wchar_t*);
typedef const wchar_t* (*__FunTypeDef2(__EXTERN_FFUN_QB2(GetPicturePath)))(QWND* p);
typedef float (*__FunTypeDef2(__EXTERN_FFUN_QB2(SetXRadius)))(QWND* p, float);
typedef float (*__FunTypeDef2(__EXTERN_FFUN_QB2(SetYRadius)))(QWND* p, float);
class QButton2 :public QCtrlBaseD2DIPD
{
    static HMODULE m_hm;
    __MembersDef(__EXTERN_FFUN_QB2(GetText));
    __MembersDef(__EXTERN_FFUN_QB2(SetText));
    __MembersDef(__EXTERN_FFUN_QB2(SetClickEvent));
    __MembersDef(__EXTERN_FFUN_QB2(SetMargin));
    __MembersDef(__EXTERN_FFUN_QB2(SetBKColor));
    __MembersDef(__EXTERN_FFUN_QB2(SetPicType));
    __MembersDef(__EXTERN_FFUN_QB2(LoadPicture));
    __MembersDef(__EXTERN_FFUN_QB2(GetPicturePath));
    __MembersDef(__EXTERN_FFUN_QB2(SetXRadius));
    __MembersDef(__EXTERN_FFUN_QB2(SetYRadius));
public:
    QButton2(HDC winddc, const QRect& rf);
    //�û��ӿ�
    std::wstring GetText();
    void SetText(const wchar_t* t);
    void SetClickEvent(QFBUTTONCLICK fun);
    float SetMargin(float margin);
    unsigned int SetBKColor(unsigned int color);
    unsigned char SetPicType(unsigned char type);
    int LoadPicture(const wchar_t* path);
    std::wstring GetPicturePath()const;
    float SetXRadius(float rx);
    float SetYRadius(float ry);

    int init(HMODULE hm);
};


//��ɫ���ڿ�
#define __QCOLORBOX__
#define __QFLAG QCOLORB
typedef int (*QCOLORBOX_CHANGE)(QPOINTER, unsigned int);
typedef unsigned int (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, GetColor)))(QWND* p);
typedef unsigned int (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetColor)))(QWND* p, unsigned int);
typedef QCOLORBOX_CHANGE (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetColorChangeEvent)))(QWND* p, QCOLORBOX_CHANGE);
typedef void(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetPopDialogHeight)))(QWND* p, int);
typedef void(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetPopDialogWidth)))(QWND* p, int);
class QColorBox :public QCtrlBaseD2DIPD
{
    static HMODULE m_hm;
    __MembersDef(__EXTERN_FFUN(__QFLAG, GetColor));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetColor));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetColorChangeEvent));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetPopDialogHeight));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetPopDialogWidth));
public:
    QColorBox(HDC winddc, const QRect& rf);
    int init(HMODULE hm);

public:
    //�û��ӿ�
    //ȡ��ǰ��ɫ
    unsigned int GetColor();
    //���õ�ǰ��ɫ����������ǰ����ɫ
    unsigned int SetColor(unsigned int color);
    //������ɫ�ı�ʱ�������¼�����
    QCOLORBOX_CHANGE SetColorChangeEvent(QCOLORBOX_CHANGE ev);
    //���õ���ĸ߶�
    void SetPopDialogHeight(int h);
    //���õ���Ŀ��
    void SetPopDialogWidth(int w);
};

//�����б��
#define __QSELECT__
#define __QFLAG QSELECT
typedef int (*QSELECT_SELECTCHANGE)(QPOINTER, int);
typedef void(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetPopDialogHeight)))(QWND* p, int);
typedef void(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetPopDialogWidth)))(QWND* p, int);
typedef int(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, InsertItem)))(QWND* p, const wchar_t*, int);
typedef int(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, RemoveItem)))(QWND* p,int);
typedef void(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetButtonRatio)))(QWND* p, float);
typedef void(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetItemHeight)))(QWND* p, float);
typedef void(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetItemFontName)))(QWND* p, const wchar_t*);
typedef void(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetItemFontSize)))(QWND* p, float);
typedef const wchar_t* (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, GetSelectedText)))(QWND* p);
typedef const wchar_t* (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, GetItemText)))(QWND* p, int);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetItemBKColor)))(QWND* p, int, unsigned int, unsigned int, unsigned char);
typedef void (*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetItemFontColor)))(QWND* p, int, unsigned int, unsigned int, unsigned char);
typedef QSELECT_SELECTCHANGE(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetSelectChangeEvent)))(QWND* p, QSELECT_SELECTCHANGE);
typedef size_t(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, GetItemCount)))(QWND* p);
typedef int(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, SetCurrentItem)))(QWND* p,int);
typedef int(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, GetIndex)))(QWND* p);
typedef int(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, GetItemFontColor)))(QWND* p, int, unsigned char*, unsigned int*, unsigned int*);
typedef int(*__FunTypeDef2(__EXTERN_FFUN(__QFLAG, GetItemBKColor)))(QWND* p, int, unsigned char*, unsigned int*, unsigned int*);

class QSelect :public QCtrlBaseD2DIPD
{
    static HMODULE m_hm;
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetPopDialogHeight));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetPopDialogWidth));
    __MembersDef(__EXTERN_FFUN(__QFLAG, InsertItem));
    __MembersDef(__EXTERN_FFUN(__QFLAG, RemoveItem));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetButtonRatio));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetItemHeight));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetItemFontName));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetItemFontSize));
    __MembersDef(__EXTERN_FFUN(__QFLAG, GetSelectedText));
    __MembersDef(__EXTERN_FFUN(__QFLAG, GetItemText));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetItemBKColor));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetItemFontColor));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetSelectChangeEvent));
    __MembersDef(__EXTERN_FFUN(__QFLAG, GetItemCount));
    __MembersDef(__EXTERN_FFUN(__QFLAG, SetCurrentItem));
    __MembersDef(__EXTERN_FFUN(__QFLAG, GetIndex));
    __MembersDef(__EXTERN_FFUN(__QFLAG, GetItemFontColor));
    __MembersDef(__EXTERN_FFUN(__QFLAG, GetItemBKColor));

public:
    QSelect(HDC winddc, const QRect& rf);
    int init(HMODULE hm);
public:
    //���õ���߶�
    void SetPopDialogHeight(int h);
    //���õ�����
    void SetPopDialogWidth(int w);
    //����һ����Ŀ����Ŀ�ı��Ͳ����λ��
    int InsertItem(const wchar_t* text, int index = -1);
    //�Ƴ�һ����Ŀ���Ƴ���λ��
    int RemoveItem(int index);
    //����������ť�Ŀ�ȱ������������ʿ����������θı��С��һ���ò���
    void SetButtonRatio(float rat);
    //������Ŀ�߶�
    void SetItemHeight(float h);
    //������Ŀ��������
    void SetItemFontName(const wchar_t* nm);
    //������Ŀ�����С
    void SetItemFontSize(float size);
    //��ȡѡ�е���Ŀ�ı�
    std::wstring GetSelectedText();
    //��ȡ��Ŀ�ı�
    std::wstring GetItemText(int index);
    //����ָ����Ŀ������ɫ��֧�����Խ���
    //��ɫ1��2
    //dΪ0ˮƽ���䣬1Ϊ��ֱ����
    void SetItemBKColor(int index, unsigned int c1, unsigned int c2, unsigned char d = 0);
    //����ָ����Ŀ������ɫ
    //��ɫ1��2
    //dΪ0ˮƽ���䣬1Ϊ��ֱ����
    void SetItemFontColor(int index, unsigned int c1, unsigned int c2, unsigned char d = 0);
    //������Ŀ��ѡ��ʱ�ص�����
    QSELECT_SELECTCHANGE SetSelectChangeEvent(QSELECT_SELECTCHANGE ev);
    //��ȡ��Ŀ��
    size_t GetItemCount();
    //���õ�ǰѡ����Ŀ
    int SetCurrentItem(int index);
    //��ȡ��ǰѡ������
    int GetIndex();
    //��ȡItem������ɫ
    //����1������
    //����2�����淽���ָ��
    //����3��4��������ɫ��ָ��
    int GetItemFontColor(int index, unsigned char* dire, unsigned int* pc1, unsigned int* pc2);
    //��ȡItem������ɫ
    //����1������
    //����2�����淽���ָ��
    //����3��4��������ɫ��ָ��
    int GetItemBKColor(int index, unsigned char* dire, unsigned int* pc1, unsigned int* pc2);
};