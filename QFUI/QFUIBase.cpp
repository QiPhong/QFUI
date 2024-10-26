#include "QFUIBase.h"

QFUIBase::QFUIBase(HDC winddc, const GRect& rf):QWND(winddc, rf)
{
}

QFUIBase::~QFUIBase()
{
}

int QFUIBase::RecordAttribute(long long key, long long value)
{
    std::unique_lock<std::shared_mutex> lk(raLK);

    __ReflAttr[key] = value;
    return 1;
}

int QFUIBase::FindAttribute(long long key, long long* pvalue)
{
    std::shared_lock<std::shared_mutex> lk(raLK);
    auto it = __ReflAttr.find(key);
    if (__ReflAttr.end() == it) {
    
        return -1;
    }
    if (pvalue) {
        *pvalue = it->second;
    }

    return 1;
}

int QFUIBase::RemoveAttribute(long long key)
{
    std::unique_lock<std::shared_mutex> lk(raLK);
    auto it = __ReflAttr.find(key);
    if (__ReflAttr.end() == it) {

        return -1;
    }

    __ReflAttr.erase(it);
}



extern "C"
{
    __declspec(dllexport) int QFUIBRecordAttribute(QFUIBase* p ,long long key, long long value)
    {
    
        return p->RecordAttribute(key, value);
    }

    __declspec(dllexport) int QFUIBFindAttribute(QFUIBase* p, long long key, long long* pvalue)
    {

        return p->FindAttribute(key, pvalue);
    }

    __declspec(dllexport) int QFUIBRemoveAttribute(QFUIBase* p, long long key)
    {
        return p->RemoveAttribute(key);
    }
}
