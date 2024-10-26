#pragma once
#include "QFUIWND.h"
#include <map>
/**
* ֮���QFUI�ؼ�������������࣬�޸�Ҳ����������͡�QWN�ཫ�ṩ���ⲿʹ��
* 
*/
class QFUIBase :public QWND
{
	//�����Զ���������
	std::shared_mutex raLK;
	std::map<long long, long long> __ReflAttr;//װ���Զ�������

public:
	QFUIBase(HDC winddc, const GRect& rf);
	~QFUIBase();


public:
	//�û��ӿ�
	
	int RecordAttribute(long long key, long long value);//��¼һ������
	int FindAttribute(long long key, long long* pvalue);//��ѯ����
	int RemoveAttribute(long long key);//�Ƴ�һ������
};

