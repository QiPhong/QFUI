#pragma once
#ifndef __QFCTRLLISTENER_H
#define __QFCTRLLISTENER_H
#include "QFUIWND.h"
//��ť������
class QButtonListener
{
public:
    virtual void BTClick(QWND* w);
};

class QLButtonDownListener
{
public:
    virtual void operator()(QWND* w) ;

};
class QLSizeChangeListener
{
public:
    virtual void operator()(QWND* w, long width, long height) ;

};
typedef void(*QFBUTTONCLICK)(void*);

























#endif
