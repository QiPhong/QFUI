#pragma once
#include "QFCtrlBase3D.h"

class Q3DCtrl :public QFCtrlBase3D
{
	ID3DXFont* m_pFont = NULL;    //字体COM接口
	LPDIRECT3DVERTEXBUFFER9		m_pVertexBuffer = NULL;    //顶点缓冲区对象
	LPDIRECT3DINDEXBUFFER9		m_pIndexBuffer = NULL;    // 索引缓存对象
public:
	Q3DCtrl(HDC winddc, const GRect& rf);
	~Q3DCtrl();
    virtual int Draw(HDC sdc = NULL)override;                                    //绘制到指定DC






private:
	HRESULT Init();





};

