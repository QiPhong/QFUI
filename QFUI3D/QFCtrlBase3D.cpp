
#include "QFCtrlBase3D.h"

QFCtrlBase3D::QFCtrlBase3D(HDC winddc, const GRect& rf) :QWND(winddc, rf)
{
	InitDirect3D();
}

QFCtrlBase3D::~QFCtrlBase3D()
{
	SafeRelease(&m_pd3dDevice);
}

int QFCtrlBase3D::Draw(HDC sdc)
{
	//****************************************
	return QWND::Draw(sdc);
}

void QFCtrlBase3D::Reconstruct(HDC sdc, const GRect& rect)
{
    QWND::Reconstruct(sdc, rect);
	
}

void QFCtrlBase3D::Reconstruct(HDC sdc)
{
	QWND::Reconstruct(sdc);
}

void QFCtrlBase3D::SetLocation(const GPoint& point)
{
	QWND::SetLocation(point);
}

void QFCtrlBase3D::SetSize(const GSize& size, HDC dc)
{
	QWND::SetSize(size, dc);
}

HRESULT QFCtrlBase3D::InitDirect3D()
{
	LPDIRECT3D9  pD3D = NULL; //Direct3D接口对象的创建
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION))) //初始化Direct3D接口对象，并进行DirectX版本协商
		return E_FAIL;

	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之二,取信息】：获取硬件设备信息
	//--------------------------------------------------------------------------------------
	D3DCAPS9 caps; int vp = 0;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		return E_FAIL;
	}
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;   //支持硬件顶点运算，我们就采用硬件顶点运算，妥妥的
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING; //不支持硬件顶点运算，无奈只好采用软件顶点运算

	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之三，填内容】：填充D3DPRESENT_PARAMETERS结构体
	//--------------------------------------------------------------------------------------
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = m_rect.Width;
	d3dpp.BackBufferHeight = m_rect.Height;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = NULL;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;//开启设备上下文
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		NULL, vp, &d3dpp, &m_pd3dDevice)))
		return E_FAIL;
	SafeRelease(&pD3D);

	//if (!(S_OK == Init())) return E_FAIL;     //调用一次Objects_Init，进行渲染资源的初始化

	// 设置渲染状态
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);      //关闭光照
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //开启背面消隐




	return S_OK;
}


