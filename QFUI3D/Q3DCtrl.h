#pragma once
#include "QFCtrlBase3D.h"

class Q3DCtrl :public QFCtrlBase3D
{
	ID3DXFont* m_pFont = NULL;    //����COM�ӿ�
	LPDIRECT3DVERTEXBUFFER9		m_pVertexBuffer = NULL;    //���㻺��������
	LPDIRECT3DINDEXBUFFER9		m_pIndexBuffer = NULL;    // �����������
public:
	Q3DCtrl(HDC winddc, const GRect& rf);
	~Q3DCtrl();
    virtual int Draw(HDC sdc = NULL)override;                                    //���Ƶ�ָ��DC






private:
	HRESULT Init();





};

