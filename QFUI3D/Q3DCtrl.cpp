#include "Q3DCtrl.h"
#include <chrono>
struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)  //FVF�����ʽ
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
	// ������ȾĿ��Ϊ�ڴ� DC
	IDirect3DSurface9* pRenderTarget = NULL;

	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮һ������������
	//--------------------------------------------------------------------------------------
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 214, 158), 1.0f, 0);

	//����һ�����Σ����ڻ�ȡ�����ھ���

		
	
	m_pd3dDevice->BeginScene();                     // ��ʼ����

	D3DXMATRIX matWorld, Rx, Ry, Rz;
	D3DXMatrixIdentity(&matWorld);                  // ��λ���������
	auto now = std::chrono::high_resolution_clock::now();
	double dt = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() / 1000.0f;
	D3DXMatrixRotationX(&Rx, D3DX_PI * (dt));    // ��X����ת
	D3DXMatrixRotationY(&Ry, D3DX_PI * 0.25f);    // ��Y����ת
	D3DXMatrixRotationZ(&Rz, D3DX_PI * 0.25f);   // ��Z����ת
	matWorld = Rx * Ry * Rz * matWorld;             // �õ����յ���Ͼ���
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);  //��������任����

	D3DXMATRIX matView; //����һ������
	D3DXVECTOR3 vEye(0.0f, 0.0f, -200.0f);  //�������λ��
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //�۲���λ��
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//���ϵ�����
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //�����ȡ���任����
	m_pd3dDevice->SetTransform(D3DTS_VIEW, &matView); //Ӧ��ȡ���任����

	D3DXMATRIX matProj; //����һ������
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 1000.0f); //����ͶӰ�任����
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);  //����ͶӰ�任����

	//--------------------------------------------------------------------------------------
	//���Ĵ�任֮�ġ����ӿڱ任������
	//--------------------------------------------------------------------------------------
	D3DVIEWPORT9 vp; //ʵ����һ��D3DVIEWPORT9�ṹ�壬Ȼ��������������������ֵ�Ϳ�����
	vp.X = 0;		//��ʾ�ӿ�����ڴ��ڵ�X����
	vp.Y = 0;		//�ӿ���ԶԴ��ڵ�Y����
	vp.Width = m_rect.Width;	//�ӿڵĿ��
	vp.Height = m_rect.Height; //�ӿڵĸ߶�
	vp.MinZ = 0.0f; //�ӿ�����Ȼ����е���С���ֵ
	vp.MaxZ = 1.0f;	//�ӿ�����Ȼ����е�������ֵ
	m_pd3dDevice->SetViewport(&vp); //�ӿڵ�����





	m_pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));//�Ѱ����ļ�������Ϣ�Ķ��㻺�����Ⱦ��ˮ�������
	m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);//ָ������ʹ�õ������ʽ�ĺ�����
	m_pd3dDevice->SetIndices(m_pIndexBuffer);//������������
	m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 20, 0, 30);//��������������϶��㻺�����ͼ��

	m_pd3dDevice->EndScene();                       // ��������

	m_pd3dDevice->Present(NULL, NULL, NULL, NULL);  // ��ת����ʾ
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
	//��������
	if (FAILED(D3DXCreateFont(m_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("΢���ź�"), &m_pFont)))
		return E_FAIL;
	//�������㻺��
	if (FAILED(m_pd3dDevice->CreateVertexBuffer(8 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &m_pVertexBuffer, NULL)))
	{
		return E_FAIL;
	}
	// ������������
	if (FAILED(m_pd3dDevice->CreateIndexBuffer(36 * sizeof(WORD), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIndexBuffer, NULL)))
	{
		return E_FAIL;

	}

	//�������ݵ����ã�
	CUSTOMVERTEX Vertices[] =
	{
		// ��һ���棨���棩���ĸ����㣬��ɫΪ��ɫ
		{ -20.0f, 20.0f, -20.0f,  D3DCOLOR_XRGB(0xff, 0x00, 0x00) },//0
		{ 20.0f, 20.0f, -20.0f,   D3DCOLOR_XRGB(0xff, 0x00, 0x00) },//1
		{ 20.0f, -20.0f, -20.0f,    D3DCOLOR_XRGB(0xff, 0x00, 0x00) },//2
		{ -20.0f, -20.0f, -20.0f,   D3DCOLOR_XRGB(0xff, 0x00, 0x00) },//3

		// �ڶ����棨���棩���ĸ����㣬��ɫΪ��ɫ
		{ -20.0f, 20.0f, 20.0f, D3DCOLOR_XRGB(0x00, 0xff, 0x00) },//4
		{ -20.0f, -20.0f, 20.0f,  D3DCOLOR_XRGB(0x00, 0xff, 0x00) },//5
		{ 20.0f, -20.0f, 20.0f,   D3DCOLOR_XRGB(0x00, 0xff, 0x00) },//6
		{ 20.0f, 20.0f, 20.0f,  D3DCOLOR_XRGB(0x00, 0xff, 0x00) },//7

		// ����
		{ -20.0f, 20.0f, 20.0f,  D3DCOLOR_XRGB(0xff, 0xaa, 0x00) },//4
		{ 20.0f, 20.0f, 20.0f, D3DCOLOR_XRGB(0xff, 0xaa, 0x00) },//1
		{ 20.0f, 20.0f, -20.0f,  D3DCOLOR_XRGB(0xff, 0xaa, 0x00) },//2
		{ -20.0f, 20.0f, -20.0f,   D3DCOLOR_XRGB(0xff, 0xaa, 0x00) },//5
		//�Ҳࣺ + +-�� + --�� + -+�� + ++
		{ 20.0f, -20.0f, -20.0f,   D3DCOLOR_XRGB(0xff, 0x00, 0xbb) },//5
		{ 20.0f, 20.0f, -20.0f,  D3DCOLOR_XRGB(0xff, 0x00, 0xbb) },//6
		{ 20.0f, 20.0f, 20.0f,   D3DCOLOR_XRGB(0xff, 0x00, 0xbb) },//3
		{ 20.0f, -20.0f, 20.0f,    D3DCOLOR_XRGB(0xff, 0x00, 0xbb) },//2
		//���棺+ - -��+ - +��- - +��- - -
		{ -20.0f, -20.0f, -20.0f,    D3DCOLOR_XRGB(0x88, 0x55, 0x22) },//6
		{ 20.0f, -20.0f, -20.0f,   D3DCOLOR_XRGB(0x88, 0x55, 0x22) },//3
		{ 20.0f, -20.0f, 20.0f,  D3DCOLOR_XRGB(0x88, 0x55, 0x22) },//0
		{ -20.0f, -20.0f, 20.0f,   D3DCOLOR_XRGB(0x88, 0x55, 0x22) },//7
	};
	//��䶥�㻺��
	VOID* pVertices;
	if (FAILED(m_pVertexBuffer->Lock(0, sizeof(Vertices), (void**)&pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, Vertices, sizeof(Vertices));
	m_pVertexBuffer->Unlock();

	// �����������
	WORD* pIndices = NULL;

	m_pIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);

	// ����
	pIndices[0] = 0, pIndices[1] = 1, pIndices[2] = 2;
	pIndices[3] = 0, pIndices[4] = 2, pIndices[5] = 3;
	// ����
	pIndices[6] = 8, pIndices[7] = 9, pIndices[8] = 10;
	pIndices[9] = 8, pIndices[10] = 10, pIndices[11] = 11;
	// �����
	pIndices[12] = 0, pIndices[13] = 3, pIndices[14] = 5;
	pIndices[15] = 0, pIndices[16] = 5, pIndices[17] = 4;
	// �Ҳ���
	pIndices[18] = 12, pIndices[19] = 13, pIndices[20] = 14;
	pIndices[21] = 12, pIndices[22] = 14, pIndices[23] = 15;
	// ����
	pIndices[24] = 16, pIndices[25] = 17, pIndices[26] = 18;
	pIndices[27] = 16, pIndices[28] = 18, pIndices[29] = 19;
	// ����
	pIndices[30] = 4, pIndices[31] = 5, pIndices[32] = 6;
	pIndices[33] = 4, pIndices[34] = 6, pIndices[35] = 7;

	m_pIndexBuffer->Unlock();

	return S_OK;
}
