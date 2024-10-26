#include "Q3DCtrl.h"
#include <chrono>
struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)  //FVF灵活顶点格式
Q3DCtrl::Q3DCtrl(HDC winddc, const GRect& rf) :QFCtrlBase3D(winddc, rf)
{
	Init();
}

Q3DCtrl::~Q3DCtrl()
{
	SafeRelease(&m_pIndexBuffer);
	SafeRelease(&m_pVertexBuffer);
	SafeRelease(&m_pFont);
}

int Q3DCtrl::Draw(HDC sdc)
{
	int ret = 1;
	BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
	HDC rDC = NULL;
	// 设置渲染目标为内存 DC
	IDirect3DSurface9* pRenderTarget = NULL;

	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之一】：清屏操作
	//--------------------------------------------------------------------------------------
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 214, 158), 1.0f, 0);

	//定义一个矩形，用于获取主窗口矩形

		
	
	m_pd3dDevice->BeginScene();                     // 开始绘制

	D3DXMATRIX matWorld, Rx, Ry, Rz;
	D3DXMatrixIdentity(&matWorld);                  // 单位化世界矩阵
	auto now = std::chrono::high_resolution_clock::now();
	double dt = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() / 1000.0f;
	D3DXMatrixRotationX(&Rx, D3DX_PI * (dt));    // 绕X轴旋转
	D3DXMatrixRotationY(&Ry, D3DX_PI * 0.25f);    // 绕Y轴旋转
	D3DXMatrixRotationZ(&Rz, D3DX_PI * 0.25f);   // 绕Z轴旋转
	matWorld = Rx * Ry * Rz * matWorld;             // 得到最终的组合矩阵
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);  //设置世界变换矩阵

	D3DXMATRIX matView; //定义一个矩阵
	D3DXVECTOR3 vEye(0.0f, 0.0f, -200.0f);  //摄像机的位置
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //观察点的位置
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//向上的向量
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //计算出取景变换矩阵
	m_pd3dDevice->SetTransform(D3DTS_VIEW, &matView); //应用取景变换矩阵

	D3DXMATRIX matProj; //定义一个矩阵
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 1000.0f); //计算投影变换矩阵
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);  //设置投影变换矩阵

	//--------------------------------------------------------------------------------------
	//【四大变换之四】：视口变换的设置
	//--------------------------------------------------------------------------------------
	D3DVIEWPORT9 vp; //实例化一个D3DVIEWPORT9结构体，然后做填空题给各个参数赋值就可以了
	vp.X = 0;		//表示视口相对于窗口的X坐标
	vp.Y = 0;		//视口相对对窗口的Y坐标
	vp.Width = m_rect.Width;	//视口的宽度
	vp.Height = m_rect.Height; //视口的高度
	vp.MinZ = 0.0f; //视口在深度缓存中的最小深度值
	vp.MaxZ = 1.0f;	//视口在深度缓存中的最大深度值
	m_pd3dDevice->SetViewport(&vp); //视口的设置





	m_pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));//把包含的几何体信息的顶点缓存和渲染流水线相关联
	m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);//指定我们使用的灵活顶点格式的宏名称
	m_pd3dDevice->SetIndices(m_pIndexBuffer);//设置索引缓存
	m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 20, 0, 30);//利用索引缓存配合顶点缓存绘制图形

	m_pd3dDevice->EndScene();                       // 结束绘制

	m_pd3dDevice->Present(NULL, NULL, NULL, NULL);  // 翻转与显示
	m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pRenderTarget);

		
	RECT rc;
	SetRect(&rc, 0, 0, m_rect.Width, m_rect.Height);
	pRenderTarget->GetDC(&rDC);
	BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, rDC, 0, 0, SRCCOPY);
		
	pRenderTarget->ReleaseDC(rDC);
	pRenderTarget->Release();




	
	QWND::Draw(sdc);

	return ret;
}

HRESULT Q3DCtrl::Init()
{
	//创建字体
	if (FAILED(D3DXCreateFont(m_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &m_pFont)))
		return E_FAIL;
	//创建顶点缓存
	if (FAILED(m_pd3dDevice->CreateVertexBuffer(8 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &m_pVertexBuffer, NULL)))
	{
		return E_FAIL;
	}
	// 创建索引缓存
	if (FAILED(m_pd3dDevice->CreateIndexBuffer(36 * sizeof(WORD), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIndexBuffer, NULL)))
	{
		return E_FAIL;

	}

	//顶点数据的设置，
	CUSTOMVERTEX Vertices[] =
	{
		// 第一个面（顶面）的四个顶点，颜色为红色
		{ -20.0f, 20.0f, -20.0f,  D3DCOLOR_XRGB(0xff, 0x00, 0x00) },//0
		{ 20.0f, 20.0f, -20.0f,   D3DCOLOR_XRGB(0xff, 0x00, 0x00) },//1
		{ 20.0f, -20.0f, -20.0f,    D3DCOLOR_XRGB(0xff, 0x00, 0x00) },//2
		{ -20.0f, -20.0f, -20.0f,   D3DCOLOR_XRGB(0xff, 0x00, 0x00) },//3

		// 第二个面（底面）的四个顶点，颜色为绿色
		{ -20.0f, 20.0f, 20.0f, D3DCOLOR_XRGB(0x00, 0xff, 0x00) },//4
		{ -20.0f, -20.0f, 20.0f,  D3DCOLOR_XRGB(0x00, 0xff, 0x00) },//5
		{ 20.0f, -20.0f, 20.0f,   D3DCOLOR_XRGB(0x00, 0xff, 0x00) },//6
		{ 20.0f, 20.0f, 20.0f,  D3DCOLOR_XRGB(0x00, 0xff, 0x00) },//7

		// 正面
		{ -20.0f, 20.0f, 20.0f,  D3DCOLOR_XRGB(0xff, 0xaa, 0x00) },//4
		{ 20.0f, 20.0f, 20.0f, D3DCOLOR_XRGB(0xff, 0xaa, 0x00) },//1
		{ 20.0f, 20.0f, -20.0f,  D3DCOLOR_XRGB(0xff, 0xaa, 0x00) },//2
		{ -20.0f, 20.0f, -20.0f,   D3DCOLOR_XRGB(0xff, 0xaa, 0x00) },//5
		//右侧： + +-、 + --、 + -+、 + ++
		{ 20.0f, -20.0f, -20.0f,   D3DCOLOR_XRGB(0xff, 0x00, 0xbb) },//5
		{ 20.0f, 20.0f, -20.0f,  D3DCOLOR_XRGB(0xff, 0x00, 0xbb) },//6
		{ 20.0f, 20.0f, 20.0f,   D3DCOLOR_XRGB(0xff, 0x00, 0xbb) },//3
		{ 20.0f, -20.0f, 20.0f,    D3DCOLOR_XRGB(0xff, 0x00, 0xbb) },//2
		//后面：+ - -、+ - +、- - +、- - -
		{ -20.0f, -20.0f, -20.0f,    D3DCOLOR_XRGB(0x88, 0x55, 0x22) },//6
		{ 20.0f, -20.0f, -20.0f,   D3DCOLOR_XRGB(0x88, 0x55, 0x22) },//3
		{ 20.0f, -20.0f, 20.0f,  D3DCOLOR_XRGB(0x88, 0x55, 0x22) },//0
		{ -20.0f, -20.0f, 20.0f,   D3DCOLOR_XRGB(0x88, 0x55, 0x22) },//7
	};
	//填充顶点缓存
	VOID* pVertices;
	if (FAILED(m_pVertexBuffer->Lock(0, sizeof(Vertices), (void**)&pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, Vertices, sizeof(Vertices));
	m_pVertexBuffer->Unlock();

	// 填充索引数据
	WORD* pIndices = NULL;

	m_pIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);

	// 顶面
	pIndices[0] = 0, pIndices[1] = 1, pIndices[2] = 2;
	pIndices[3] = 0, pIndices[4] = 2, pIndices[5] = 3;
	// 正面
	pIndices[6] = 8, pIndices[7] = 9, pIndices[8] = 10;
	pIndices[9] = 8, pIndices[10] = 10, pIndices[11] = 11;
	// 左侧面
	pIndices[12] = 0, pIndices[13] = 3, pIndices[14] = 5;
	pIndices[15] = 0, pIndices[16] = 5, pIndices[17] = 4;
	// 右侧面
	pIndices[18] = 12, pIndices[19] = 13, pIndices[20] = 14;
	pIndices[21] = 12, pIndices[22] = 14, pIndices[23] = 15;
	// 背面
	pIndices[24] = 16, pIndices[25] = 17, pIndices[26] = 18;
	pIndices[27] = 16, pIndices[28] = 18, pIndices[29] = 19;
	// 底面
	pIndices[30] = 4, pIndices[31] = 5, pIndices[32] = 6;
	pIndices[33] = 4, pIndices[34] = 6, pIndices[35] = 7;

	m_pIndexBuffer->Unlock();

	return S_OK;
}
