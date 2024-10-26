#pragma once
#include "QFUIWND.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = nullptr;
    }
}

class QFCtrlBase3D :public QWND
{

protected:
    LPDIRECT3DDEVICE9					m_pd3dDevice = NULL; //Direct3D�豸����

    bool isChange = true;
public:
    QFCtrlBase3D(HDC winddc, const GRect& rf);
    virtual ~QFCtrlBase3D();
    virtual void Redraw() { isChange = true; };

    virtual int Draw(HDC sdc = NULL)override;                                    //���Ƶ�ָ��DC
    virtual void Reconstruct(HDC sdc, const GRect& rect)override;                         //���´���
    virtual void Reconstruct(HDC sdc)override;                                          //���´���
    virtual void SetLocation(const GPoint& point)override;                               //����λ��
    virtual void SetSize(const GSize& size, HDC dc = NULL)override;                         //���ô�С(�����¹����ؼ�)
    virtual void Refresh() { isChange = true; };
private:
    HRESULT InitDirect3D();
    

};

