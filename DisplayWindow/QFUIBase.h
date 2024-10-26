#pragma once
#include "DLLImportQWND.h"
#include <windows.h>
#include <string>
#define __DefineFun(type,name) static type name
#define __GDefineFun(cname,type,name) type cname::name=NULL
#define __QDefineType(name) name##_Type
#define __MembersDef(name) __DefineFun(__QDefineType(name),name)
#define ____FunTypeDef(fName)  fName##_Type
#define __FunTypeDef(fName) ____FunTypeDef(fName)
#define ____EXTERN_FFUN(flag,name) flag##name
#define __EXTERN_FFUN(flag,name) ____EXTERN_FFUN(flag,name)
typedef int (*__FunTypeDef(QFUIBRecordAttribute))(QWND*,long long,long long);
typedef int (*__FunTypeDef(QFUIBFindAttribute))(QWND*, long long, long long*);
typedef int (*__FunTypeDef(QFUIBRemoveAttribute))(QWND*, long long);
class QFUIBase :public DLLImportQWND
{
	static HMODULE m_hm;
	__MembersDef(QFUIBRecordAttribute);
	__MembersDef(QFUIBFindAttribute);
	__MembersDef(QFUIBRemoveAttribute);
public:
	QFUIBase(HDC winddc, const QRect& rf);
	virtual int init(HMODULE hm);

	int RecordAttribute(long long key, long long value);//��¼һ������
	int FindAttribute(long long key, long long* pvalue);//��ѯ����
	int RemoveAttribute(long long key);//�Ƴ�һ������
};

typedef void (*__FunTypeDef(__EXTERN_FFUN(QCBDD, SetFontSize)))(QWND*, double);
typedef void (*__FunTypeDef(__EXTERN_FFUN(QCBDD, SetFontName)))(QWND*, const wchar_t*);
typedef void (*__FunTypeDef(__EXTERN_FFUN(QCBDD, SetFontColor)))(QWND*, unsigned int);

//�����ڶ��߳�ͬʱˢ�µĿؼ�
class QCtrlBaseD2DIPD :public QFUIBase
{
	static HMODULE m_hm;
	__MembersDef(__EXTERN_FFUN(QCBDD, SetFontSize));
	__MembersDef(__EXTERN_FFUN(QCBDD, SetFontName));
	__MembersDef(__EXTERN_FFUN(QCBDD, SetFontColor));
public:

	QCtrlBaseD2DIPD(HDC winddc, const QRect& rf);
	virtual int init(HMODULE hm);
	virtual void SetFontName(const wchar_t* t);
	virtual void SetFontSize(double size);
	virtual void SetFontColor(unsigned int color);

};

