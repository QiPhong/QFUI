#pragma once
#include "QFMsg.h"
#include <vector>
#include <mutex>
#include <shared_mutex>
/**
* ========================================================================================================================================
*��Ϣģ��
* 
* ========================================================================================================================================
*/

class QMsgModule
{
public:
	/**
	* ���ã���Ϣ����
	* ����ֵ���Ѵ�����true����������false
	* �ĸ�����������QF��Ϣϵͳ�Ĳ���
	*/
	virtual bool MsgProcess(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam, QDWORD* ret = NULL)=0;


};
template <typename T, typename LOCK>
class QMsgGroupLK :public QMsgModule
{
	std::vector<T*> __ctrl;
	LOCK* __lk;
	// ��̬���ԣ���� LOCK �Ƿ��� std::mutex �� std::shared_mutex
	static_assert(std::is_same<LOCK, std::mutex>::value || std::is_same<LOCK, std::shared_mutex>::value,
		"Template parameters LOCK can only be std::mutex or std::shared_mutex. (ģ�����LOCKֻ����std::mutex �� std::shared_mutex)");
public:
	void AddCtrlArr(T* ctrl);
	virtual bool MsgProcess(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam, QDWORD*ret=NULL)override;
	QMsgGroupLK(LOCK* lk);

};
template <typename T>
class QMsgGroup :public QMsgModule
{
	std::vector<T*> __ctrl;
public:
	void AddCtrlArr(T* ctrl);
	virtual bool MsgProcess(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam, QDWORD* ret = NULL)override;

};







#include "QFUIWND.h"

template<typename MutexType>
using LockType = typename std::conditional<
	std::is_same<MutexType, std::shared_mutex>::value,
	std::shared_lock<MutexType>,
	std::unique_lock<MutexType>>::type;

template<typename MutexType>
class LockGuard {
public:
	explicit LockGuard(MutexType& mutex) : lock(mutex) {}

private:
	LockType<MutexType> lock;
};

template<typename T>
void __RADIO_ALL_NSELE(T& ctrl, QDWORD LParam, QDWORD RParam, QDWORD AddParam)
{
	for (auto& it : ctrl) {
		for (auto& itt : *it) {
			itt->TransferMessage(QM_RADIO_NSELE, LParam, RParam, AddParam);
		}

	}

}


template<typename T, typename LOCK>
bool QMsgGroupLK<T, LOCK>::MsgProcess(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam, QDWORD* ret)
{
	switch (msgID)
	{
	case QM_RADIO_ALL_NSELE: {
		LockGuard<LOCK> lk(*__lk);
		__RADIO_ALL_NSELE(__ctrl, LParam, RParam, AddParam);
		if (NULL != ret)*ret = 0;
		return true;
	}

	default:
		break;
	}



	return false;
}

template<typename T, typename LOCK>
QMsgGroupLK<T, LOCK>::QMsgGroupLK(LOCK* lk)
{
	__lk = lk;
}

template<typename T>
void QMsgGroup<T>::AddCtrlArr(T* ctrl)
{
	__ctrl.push_back(ctrl);
}

template<typename T>
bool QMsgGroup<T>::MsgProcess(QDWORD msgID, QDWORD LParam, QDWORD RParam, QDWORD AddParam, QDWORD* ret)
{
	switch (msgID)
	{
	case QM_RADIO_ALL_NSELE: {
		__RADIO_ALL_NSELE(__ctrl, LParam, RParam, AddParam);
		if (NULL != ret)*ret = 0;
		return true;
	}

	default:
		break;
	}



	return false;
}

template<typename T, typename LOCK>
void QMsgGroupLK<T, LOCK>::AddCtrlArr(T* ctrl)
{
	__ctrl.push_back(ctrl);
}