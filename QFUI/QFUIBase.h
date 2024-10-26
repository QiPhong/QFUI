#pragma once
#include "QFUIWND.h"
#include <map>
/**
* 之后的QFUI控件都将基于这个类，修改也基于这个类型。QWN类将提供给外部使用
* 
*/
class QFUIBase :public QWND
{
	//操作自定义属性锁
	std::shared_mutex raLK;
	std::map<long long, long long> __ReflAttr;//装载自定义属性

public:
	QFUIBase(HDC winddc, const GRect& rf);
	~QFUIBase();


public:
	//用户接口
	
	int RecordAttribute(long long key, long long value);//记录一条属性
	int FindAttribute(long long key, long long* pvalue);//查询属性
	int RemoveAttribute(long long key);//移除一条属性
};

