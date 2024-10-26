#include "QFGraph3D.h"
#include <triangle/triangle.h>
#pragma comment(lib, "triangle/Libtriangle.lib")
#include <format>
#include "QFGraph3D.h"
struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};
struct TrianglePoint {
	double x, y;
};
// ����ת��Ϊ���ǿ�������ʽ
void convertPointsToTriangleInput(const std::vector<TrianglePoint>& points, triangulateio& input) {
	input.numberofpoints = points.size();
	input.pointlist = new double[input.numberofpoints * 2];
	for (int i = 0; i < input.numberofpoints; ++i) {
		input.pointlist[2 * i] = points[i].x;
		input.pointlist[2 * i + 1] = points[i].y;
	}
}
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)  //FVF�����ʽ

QGraph3D::QGraph3D(HDC winddc, const GRect& rf) :QFCtrlBase3D(winddc, rf)
{
	Init();
	m_common.attr.isVisualAS = true;
}

QGraph3D::~QGraph3D()
{

    SafeRelease(&m_pFont);
	SafeRelease(&m_pConeMesh);
	SafeRelease(&m_Sphere);
	SafeRelease(&m_pXAxisMesh);
	SafeRelease(&m_pYAxisMesh);
	SafeRelease(&m_pZAxisMesh);

	for (auto& it : m_xScal) {
		SafeRelease(&it);
	}
	for (auto& it : m_yScal) {
		SafeRelease(&it);
	}
	for (auto& it : m_zScal) {
		SafeRelease(&it);
	}
	for (auto& it : m_Curve) {
		SafeRelease(&it.pDynamicVB);
		delete it.link;
		it.link = NULL;
	}

}

int QGraph3D::Draw(HDC sdc)
{
	if (!isRender(60, true)) {
		QWND::Draw(sdc);
		return 0;
	}
	int ret = 1;
	BitBlt(m_dc, 0, 0, m_rect.Width, m_rect.Height, sdc, m_rect.X, m_rect.Y, SRCCOPY);
	HDC rDC = NULL;
	// ������ȾĿ��Ϊ�ڴ� DC
	IDirect3DSurface9* pRenderTarget = NULL;
	
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);

	//����һ�����Σ����ڻ�ȡ�����ھ���
	m_pd3dDevice->BeginScene();                     // ��ʼ����
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	D3DXMATRIX matView; //����һ������
	D3DXVECTOR3 vEye(00.0f, 0.0f, m_visualHeigh);  //�������λ��
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //�۲���λ��
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//���ϵ�����
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //�����ȡ���任����
	m_pd3dDevice->SetTransform(D3DTS_VIEW, &matView); //Ӧ��ȡ���任����

	D3DXMATRIX matProj; //����һ������
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 5000.0f); //����ͶӰ�任����
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);  //����ͶӰ�任����

	
	D3DVIEWPORT9 vp; //ʵ����һ��D3DVIEWPORT9�ṹ�壬Ȼ��������������������ֵ�Ϳ�����
	vp.X = 0;		//��ʾ�ӿ�����ڴ��ڵ�X����
	vp.Y = 0;		//�ӿ���ԶԴ��ڵ�Y����
	vp.Width = m_rect.Width;	//�ӿڵĿ��
	vp.Height = m_rect.Height; //�ӿڵĸ߶�
	vp.MinZ = 0.0f; //�ӿ�����Ȼ����е���С���ֵ
	vp.MaxZ = 1.0f;	//�ӿ�����Ȼ����е�������ֵ
	m_pd3dDevice->SetViewport(&vp); //�ӿڵ�����
	UpdateWorldMatrix();
	//����������
	m_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	//CUSTOMVERTEX axes[] =
	//{
	//	{0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 0)},  // x��: ��ɫ
	//	{m_xLength * 4, 0.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 0)},

	//	{0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0)},  // y��: ��ɫ
	//	{0.0f, m_yLength * 4, 0.0f, D3DCOLOR_XRGB(0, 255, 0)},

	//	{0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 255)},  // z��: ��ɫ
	//	{0.0f, 0.0f, m_zLength * 4, D3DCOLOR_XRGB(0, 0, 255)},
	//};


	//m_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 3, axes, sizeof(CUSTOMVERTEX));
	if(m_common.attr.isVisualAS){
	
		if (m_isASChange ) {
			auto now = std::chrono::high_resolution_clock::now();
			auto dta = std::chrono::duration_cast<std::chrono::milliseconds>(now- m_ASCUpdateTime).count();
			if (dta > 500) {
				//�Ƿ�ԭ�е���Դ
				{
					SafeRelease(&m_pConeMesh);
					SafeRelease(&m_Sphere);
					SafeRelease(&m_pXAxisMesh);
					SafeRelease(&m_pYAxisMesh);
					SafeRelease(&m_pZAxisMesh);

					for (auto& it : m_xScal) {
						SafeRelease(&it);
					}
					m_xScal.clear();
					
					for (auto& it : m_yScal) {
						SafeRelease(&it);
					}
					m_yScal.clear();
					for (auto& it : m_zScal) {
						SafeRelease(&it);
					}
					m_zScal.clear();
				}
				//���´�����Դ
				{
				/*	std::unique_lock<std::mutex> lk(m_AsChange_Lock, std::try_to_lock);
					if (lk.owns_lock()) {*/
						initScale();
					//}
					
				}


				m_isASChange = false;
			}
			
		}
		else {
			RenderCylinderZ(m_pd3dDevice, m_xLength, m_zSW, m_zLength);
			RenderCylinderX(m_pd3dDevice, m_yLength, m_xSW, m_xLength);
			RenderCylinderY(m_pd3dDevice, m_xLength, m_ySW, m_yLength);
			RenderScalStrX(m_pd3dDevice, m_xLength, m_yLength, m_xSW);
			RenderScalStrY(m_pd3dDevice, m_yLength, m_xLength, m_ySW);
			RenderScalStrZ(m_pd3dDevice, m_zLength, m_xLength, m_zSW);
			
		}



		
	}
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//��Ⱦ����
	for (auto& it : m_Curve) {
		// ���ö��㻺�����Ͷ����ʽ
		m_pd3dDevice->SetStreamSource(0, it.pDynamicVB, 0, sizeof(CUSTOMVERTEX));
		m_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, it.count);//���������D3DPT_TRIANGLESTRIP 
	
	}






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
	isRender(60, false);
	return ret;
}

int QGraph3D::LButtonDown(int x, int y, WPARAM wParam)
{
	m_IsDragging = true;
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	return 1;
}

int QGraph3D::LButtonUp(int x, int y, WPARAM wParam)
{
	m_IsDragging = false;

	return 0;
}

int QGraph3D::MouseMove(int x, int y, WPARAM wParam)
{
	if (m_IsDragging)
	{


		float deltaX = (x - m_LastMousePos.x) * 0.01f;
		float deltaY = (y - m_LastMousePos.y) * 0.01f;

		m_RotationX -= deltaY;
		m_RotationY -= deltaX;

		m_LastMousePos = POINT(x,y);
	}
	return 0;
}
int QGraph3D::MouseWheel(int x, int y, WPARAM wParam)
{
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	m_visualHeigh += zDelta*0.05;

	double c = abs(m_visualHeigh / 2);
	INT w=min(m_rect.Width, m_rect.Height);

	m_xMin = -c;
	m_xMax = c;
	m_yMin = -c;
	m_yMax = c;
	m_zMin = -c;
	m_zMax = c;
	m_xLength = 1.5 * c;
	m_yLength = 1.5 * c;
	m_zLength = 1.5 * c;
	m_AxisPointRadius = c * 0.01;
	m_AxisRadius = m_AxisPointRadius / 2;
	m_AxisFontSize = m_AxisPointRadius * 5;
	m_xSW = m_AxisFontSize * 4;
	m_ySW = m_xSW;
	m_zSW = m_xSW;

	m_ASCUpdateTime = std::chrono::high_resolution_clock::now();
	m_isASChange = true;
	
	return 0;
}
void QGraph3D::UpdateWorldMatrix()
{
	D3DXMATRIX rotationX, rotationY;
	D3DXMatrixRotationX(&rotationX, m_RotationX);
	D3DXMatrixRotationY(&rotationY, m_RotationY);
	m_matWorld = rotationX * rotationY;
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
}
int QGraph3D::AddCurve(float* xRange, float* yRange, DEFINEDOMAIN dd, DEFINEDOMAINYVALUE ddy, DEFINEDOMAINXVALUE ddx, GETZVALUE getZ, int numVerticesX, int numVerticesY)
{
	if (xRange[0] > xRange[1])return -1;
	if (yRange[0] > yRange[1])return -2;
	if (!dd)return -3;
	if (!ddy)return -4;
	if (!ddx)return -5;
	if (!getZ)return -6;
	if (numVerticesX<1 || numVerticesY<1 )return -7;
	float x_min = xRange[0]; // x ����Сֵ
	float x_max = xRange[1];  // x �����ֵ
	float y_min = yRange[0]; // y ����Сֵ
	float y_max = yRange[1];  // y �����ֵ

	float vta = max(x_max- x_min, y_max - y_min);


	if (m_visualHeigh < vta) {
		m_visualHeigh = vta;

		double c = abs(m_visualHeigh / 2);
		m_xMin = -c;
		m_xMax = c;
		m_yMin = -c;
		m_yMax = c;
		m_zMin = -c;
		m_zMax = c;
		m_xLength = 2 * c;
		m_yLength = 2 * c;
		m_zLength = 2 * c;
		m_AxisPointRadius = c * 0.01;
		m_AxisRadius = m_AxisPointRadius / 2;
		m_AxisFontSize = m_AxisPointRadius * 5;
		m_xSW = m_AxisFontSize * 5;
		m_ySW = m_xSW;
		m_zSW = m_xSW;

		m_ASCUpdateTime = std::chrono::high_resolution_clock::now();
		m_isASChange = true;
	
	
	}






	// ���㲽��
	float deltaX = (x_max - x_min) / (numVerticesX - 1);
	float deltaY = (y_max - y_min) / (numVerticesY - 1);
	std::vector<TrianglePoint> points;
	int numVY = numVerticesY;
	int numVX = numVerticesX;

	/*      numVY = 3;
		  numVX = 3;*/
	for (int j = 0; j < numVY - 1; ++j) {
		for (int i = 0; i < numVX - 1; ++i) {
			float x1 = x_min + i * deltaX;
			float y1 = y_min + j * deltaY;
			float x2 = x_min + (i + 1) * deltaX;
			float y2 = y_min + j * deltaY;
			float x3 = x_min + i * deltaX;
			float y3 = y_min + (j + 1) * deltaY;
			float x4 = x_min + (i + 1) * deltaX;
			float y4 = y_min + (j + 1) * deltaY;

			TrianglePoint p[4] = { x1,y1,x2,y2,x3,y3 , x4,y4 };
			for (int k = -1; ++k < 4;) {
				if (dd(p[k].x, p[k].y)) {
					points.push_back(p[k]);
				}
			}

			//�������
			float y[1024] = { 0 };
			int ct = ddy(x1, y);
			for (int k = -1; ++k < ct;) {
				if (y[k] > y1 && y[k] < y4) {
					points.push_back({ x1,y[k] });
				}
			}

			ct = ddy(x2, y);
			for (int k = -1; ++k < ct;) {
				if (y[k] > y1 && y[k] < y4) {
					points.push_back({ x2,y[k] });
				}
			}
			float x[1024] = { 0 };
			ct = ddx(y1, x);
			for (int k = -1; ++k < ct;) {
				if (x[k] > x1 && x[k] < x2) {
					points.push_back({ x[k],y1 });
				}
			}
			ct = ddx(y4, x);
			for (int k = -1; ++k < ct;) {
				if (x[k] > x1 && x[k] < x2) {
					points.push_back({ x[k],y4 });
				}
			}
		}
	}
	// ��ʼ��Triangle������������ṹ��
	triangulateio input, output, vorout;

	// ��ʼ������ṹ��
	memset(&input, 0, sizeof(input));
	convertPointsToTriangleInput(points, input);

	// ��ʼ������ṹ��
	memset(&output, 0, sizeof(output));
	memset(&vorout, 0, sizeof(vorout));
	// ִ��Delaunay�����ʷ�
	char options[] = "zQ"; // "z" - remove all Steiner points, "Q" - quiet mode
	triangulate(options, &input, &output, &vorout);
	
	Q3DCurve curve;
	curve.color1 = 0xFF000000;
	curve.color2 = 0xFFFFFFFF;
	curve.dd=dd;
	curve.ddx = ddx;
	curve.ddy = ddy;
	curve.getZ = getZ;
	curve.numVerticesX = numVerticesX;
	curve.numVerticesY = numVerticesY;
	curve.x1 = xRange[0];
	curve.x2 = xRange[1];
	curve.y1 = yRange[0];
	curve.y2 = yRange[1];
	int verticesNum = output.numberoftriangles*3;
	LPDIRECT3DVERTEXBUFFER9 pDynamicVB;
	// ������̬���㻺����
	m_pd3dDevice->CreateVertexBuffer(verticesNum * sizeof(CUSTOMVERTEX),
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		D3DFVF_XYZ | D3DFVF_DIFFUSE,
		D3DPOOL_DEFAULT, &pDynamicVB, NULL);
	curve.link = new QCSLINK[verticesNum];
	
	curve.pDynamicVB = pDynamicVB;
	int countx = 0;
	int index = 0;
	int li = -1;
	float maxZ = 0.0f;
	float minZ = -1.0f;
	VOID* pVertices;
	pDynamicVB->Lock(0, numVerticesX * numVerticesY * sizeof(CUSTOMVERTEX), (void**)&pVertices, 0);
	for (int i = 0; i < output.numberoftriangles; ++i) {
		D3DXVECTOR3 tp[3];
		bool isErr = false;
		for (int j = 0; j < 3; ++j) {
			int pointIndex = output.trianglelist[3 * i + j];

			double x = output.pointlist[2 * pointIndex];
			double y = output.pointlist[2 * pointIndex + 1];
			float z = 0.0;
			if (!getZ(x, y, &z)) {
				isErr = true;
				break;
			}
			tp[j].x = x;
			tp[j].y = y;
			tp[j].z = z;
		}
		if (!isErr) {
			++countx;
		/*	int dr = ((curve.color1 >> 16) & 0xff)-((curve.color2 >> 16) & 0xff);*/
			int r =0;
			int g = 0;
			int b = 0;
			float m = max(abs(tp[0].z), max(abs(tp[1].z), abs(tp[2].z)));
			float mi= min(abs(tp[0].z), min(abs(tp[1].z), abs(tp[2].z)));
			if (maxZ < m) {
				maxZ = m;
			}
			if (minZ > mi || minZ < 0) {
				minZ = mi;
			}

			((CUSTOMVERTEX*)pVertices)[index++] = { tp[0].x, tp[0].y, tp[0].z, D3DCOLOR_XRGB(r,  g,  b) };
			((CUSTOMVERTEX*)pVertices)[index++] = { tp[1].x, tp[1].y, tp[1].z, D3DCOLOR_XRGB(r,  g,  b) };
			((CUSTOMVERTEX*)pVertices)[index++] = { tp[2].x, tp[2].y, tp[2].z, D3DCOLOR_XRGB(r,  g,  b) };
			curve.link[++li] = { D3DXVECTOR3(tp[0].x, tp[0].y, tp[0].z) , D3DXVECTOR3(tp[1].x, tp[1].y, tp[1].z) };
			curve.link[++li] = { D3DXVECTOR3(tp[1].x, tp[1].y, tp[1].z) , D3DXVECTOR3(tp[2].x, tp[2].y, tp[2].z) };
			curve.link[++li] = { D3DXVECTOR3(tp[2].x, tp[2].y, tp[2].z) , D3DXVECTOR3(tp[0].x, tp[0].y, tp[0].z) };
		}
	}
	curve.count = index - 1;
	float dtaM = maxZ - minZ;
	int dr = ((curve.color1 >> 16) & 0xff)-((curve.color2 >> 16) & 0xff);
	int dg = ((curve.color1 >> 8) & 0xff) - ((curve.color2 >> 8) & 0xff);
	int db = (curve.color1& 0xff) - (curve.color2  & 0xff);

	for (int i = -1; ++i < index;) {
		float z=abs(((CUSTOMVERTEX*)pVertices)[i].z);
		int r = ((curve.color1 >> 16) & 0xff)-z/dtaM * dr ;
		int g = ((curve.color1 >> 8) & 0xff)-z / dtaM * dg ;
		int b = (curve.color1 & 0xff)-z / dtaM * db ;
		((CUSTOMVERTEX*)pVertices)[i].color = D3DCOLOR_XRGB(r, g, b);
	}


	// �������㻺����
	pDynamicVB->Unlock();
	m_Curve.push_back(curve);





	// �ͷŷ�����ڴ�
	delete[] input.pointlist;
	delete[] output.pointlist;
	delete[] output.trianglelist;
	delete[] output.triangleattributelist;
	delete[] output.edgemarkerlist;
	delete[] output.holelist;
	delete[] output.neighborlist;
	delete[] output.normlist;
	delete[] output.pointattributelist;
	delete[] output.pointmarkerlist;
	delete[] output.trianglearealist;


	return 1;
}
void QGraph3D::SetVisualHeigh(float height)
{
	m_visualHeigh = height;
}
void QGraph3D::IsVisivalAxis(bool isVisival)
{
	m_common.attr.isVisualAS = isVisival;
}
HRESULT QGraph3D::Init()
{
	//��������
	if (FAILED(D3DXCreateFont(m_pd3dDevice, 36, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("΢���ź�"), &m_pFont)))
		return E_FAIL;
	
	initScale();
	D3DXMatrixIdentity(&m_matWorld);                  // ��λ���������
	//TEST();
	return S_OK;
}
void QGraph3D::initScale()
{
	HDC hdc = CreateCompatibleDC(NULL);
	HFONT hNewFont = NULL;
	HFONT hOldFont = NULL;

	hNewFont = CreateFontW(
		10,
		12,
		0,
		0,
		FW_BOLD,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"����");

	hOldFont = (HFONT)SelectObject(hdc, hNewFont);

	//����x�����̶�ģ��
	int count = m_xLength / 2 / m_xSW;
	for (int i = -1; ++i <= count;) {
		ID3DXMesh* p = NULL;
		std::wstring txt = std::format(L"{:.2f}", (m_xMax)*m_xSW * i / m_xLength * 2);
		D3DXCreateTextW(
			m_pd3dDevice,
			hdc,
			txt.c_str(),
			0.001f,
			0.01f,
			&p,
			NULL,
			NULL);
		m_xScal.push_back(p);
	}
	//����x�ĸ��̶�ģ��
	for (int i = 0; ++i <= count;) {
		ID3DXMesh* p = NULL;
		std::wstring txt = std::format(L"{:.2f}", (m_xMin)*m_xSW * i / m_xLength * 2);
		D3DXCreateTextW(
			m_pd3dDevice,
			hdc,
			txt.c_str(),
			0.001f,
			0.01f,
			&p,
			NULL,
			NULL);
		m_xScal.push_back(p);
	}

	D3DXCreateCylinder(
		m_pd3dDevice,   // D3D �豸����
		m_AxisRadius,           // Բ����ĵ���뾶
		m_AxisRadius,           // Բ����Ķ���뾶
		m_xLength,           // Բ����ĸ߶�
		20,             // �������Ƭ����
		20,             // Բ����ĸ߶���Ƭ����
		&m_pXAxisMesh,         // ���ڱ���Բ�����������ݵı���
		NULL            // ����Ҫ���������������Ϣ
	);

	D3DXCreateCylinder(
		m_pd3dDevice,   // D3D �豸����
		m_AxisRadius * 4,           // Բ����ĵ���뾶
		0.001f,           // Բ����Ķ���뾶
		m_AxisRadius * 8,           // Բ����ĸ߶�
		20,             // �������Ƭ����
		20,             // Բ����ĸ߶���Ƭ����
		&m_pConeMesh,         // ���ڱ���Բ�����������ݵı���
		NULL            // ����Ҫ���������������Ϣ
	);

	//����y�����̶�ģ��
	count = m_yLength / 2 / m_ySW;
	for (int i = -1; ++i <= count;) {
		ID3DXMesh* p = NULL;
		std::wstring txt = std::format(L"{:.2f}", (m_yMax)*m_ySW * i / m_yLength * 2);
		D3DXCreateTextW(
			m_pd3dDevice,
			hdc,
			txt.c_str(),
			0.001f,
			0.01f,
			&p,
			NULL,
			NULL);
		m_yScal.push_back(p);
	}
	//����y�ĸ��̶�ģ��
	for (int i = 0; ++i <= count;) {
		ID3DXMesh* p = NULL;
		std::wstring txt = std::format(L"{:.2f}", (m_yMin)*m_ySW * i / m_yLength * 2);
		D3DXCreateTextW(
			m_pd3dDevice,
			hdc,
			txt.c_str(),
			0.001f,
			0.01f,
			&p,
			NULL,
			NULL);
		m_yScal.push_back(p);
	}

	D3DXCreateCylinder(
		m_pd3dDevice,   // D3D �豸����
		m_AxisRadius,           // Բ����ĵ���뾶
		m_AxisRadius,           // Բ����Ķ���뾶
		m_yLength,           // Բ����ĸ߶�
		20,             // �������Ƭ����
		20,             // Բ����ĸ߶���Ƭ����
		&m_pYAxisMesh,         // ���ڱ���Բ�����������ݵı���
		NULL            // ����Ҫ���������������Ϣ
	);
	//����z�����̶�ģ��
	count = m_zLength / 2 / m_zSW;
	for (int i = -1; ++i <= count;) {
		ID3DXMesh* p = NULL;
		std::wstring txt = std::format(L"{:.2f}", (m_zMax)*m_zSW * i / m_zLength * 2);
		D3DXCreateTextW(
			m_pd3dDevice,
			hdc,
			txt.c_str(),
			0.001f,
			0.01f,
			&p,
			NULL,
			NULL);
		m_zScal.push_back(p);
	}
	//����z�ĸ��̶�ģ��
	for (int i = 0; ++i <= count;) {
		ID3DXMesh* p = NULL;
		std::wstring txt = std::format(L"{:.2f}", (m_zMin)*m_zSW * i / m_zLength * 2);
		D3DXCreateTextW(
			m_pd3dDevice,
			hdc,
			txt.c_str(),
			0.001f,
			0.01f,
			&p,
			NULL,
			NULL);
		m_zScal.push_back(p);
	}
	D3DXCreateCylinder(
		m_pd3dDevice,   // D3D �豸����
		m_AxisRadius,           // Բ����ĵ���뾶
		m_AxisRadius,           // Բ����Ķ���뾶
		m_zLength,           // Բ����ĸ߶�
		20,             // �������Ƭ����
		20,             // Բ����ĸ߶���Ƭ����
		&m_pZAxisMesh,         // ���ڱ���Բ�����������ݵı���
		NULL            // ����Ҫ���������������Ϣ
	);

	D3DXCreateSphere(
		m_pd3dDevice,   // D3D �豸����
		m_AxisPointRadius,           // ����İ뾶
		20,             // ������Ƭ�������ߣ�
		20,             // ������Ƭ����γ�ߣ�
		&m_Sphere,         // ���ڱ��������������ݵı���
		NULL            // ����Ҫ���������������Ϣ
	);

	SelectObject(hdc, hOldFont);
	DeleteObject(hNewFont);
	DeleteDC(hdc);
	

}
bool QGraph3D::isRender(int pfs, bool isbegin)
{
	std::chrono::milliseconds frame_time(static_cast<int>(1000.0 / pfs));;
	std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();//��ǰʱ��
	static std::chrono::steady_clock::time_point startFps = std::chrono::high_resolution_clock::now();//��һ֡ʱ��
	std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startFps);
	if (isbegin) {
		if (frame_time > duration) {
			return false;
		}

		return true;
	}
	else {
		startFps = std::chrono::high_resolution_clock::now();
	}

	return true;
}

void QGraph3D::RenderCylinderZ(LPDIRECT3DDEVICE9 pd3dDevice, double XaxisLenth = 100.0f, double Sw = 30.0f, double ZaxisLenth = 100.0f)
{
	D3DMATERIAL9 mtrl, olMtrl;
	pd3dDevice->GetMaterial(&olMtrl);
	ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Ambient.r = mtrl.Diffuse.r = 0.0f;
	mtrl.Ambient.g = mtrl.Diffuse.g = 0.0f;
	mtrl.Ambient.b = mtrl.Diffuse.b = 1.0f;
	mtrl.Ambient.a = mtrl.Diffuse.a = 1.0f;
	pd3dDevice->SetMaterial(&mtrl);

	D3DLIGHT9 light,ollight;
	D3DXVECTOR3 dir(0.0f, 1.0f, 1.0f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = color * 0.6f;
	light.Diffuse = color;
	light.Specular = color * 0.6f;
	light.Direction = dir;
	pd3dDevice->GetLight(0, &ollight);
	pd3dDevice->SetLight(0, &light);
	pd3dDevice->LightEnable(0, TRUE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	D3DXMATRIX wm, olVM, Rx, Ry, Rz, worldAs;
	D3DXMatrixIdentity(&worldAs);                  // ��λ���������
	pd3dDevice->GetTransform(D3DTS_WORLD, &olVM);


	D3DXMatrixRotationX(&Rx, D3DX_PI * 0.0f);    // ��X����ת
	D3DXMatrixRotationY(&Ry, D3DX_PI * -0.0f);    // ��y����ת
	D3DXMatrixRotationZ(&Rz, D3DX_PI * -0.0f);    // ��z����ת
	D3DXMatrixTranslation(&wm, 0.0f, 0.0f, -ZaxisLenth / 2);
	D3DXMATRIX amWorld;
	worldAs = Rx * Ry * Rz * m_matWorld;
	D3DXMatrixTranslation(&wm, XaxisLenth, 0.0f, ZaxisLenth / 2);
	amWorld = Rx * Ry * Rz * m_matWorld;



	pd3dDevice->SetTransform(D3DTS_WORLD, &amWorld);
	m_pZAxisMesh->DrawSubset(0);

	pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
	for (double i = 0; i < ZaxisLenth / 2; i += Sw) {
		m_Sphere->DrawSubset(0);
		D3DXMatrixTranslation(&wm, 0.0f, 0.0f, Sw);
		worldAs = wm * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
	}
	D3DXMatrixTranslation(&wm, 0.0f, 0.0f, ZaxisLenth / 2);
	worldAs = wm * amWorld;
	pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);

	m_pConeMesh->DrawSubset(0);
	worldAs = amWorld;

	for (double i = -Sw; i > -ZaxisLenth / 2; i -= Sw) {
		D3DXMatrixTranslation(&wm, 0.0f, 0.0f, -Sw);
		worldAs = wm * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
		m_Sphere->DrawSubset(0);


	}
	pd3dDevice->SetLight(0, &ollight);
	pd3dDevice->LightEnable(0, FALSE);

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	pd3dDevice->SetMaterial(&olMtrl);
	pd3dDevice->SetTransform(D3DTS_WORLD, &olVM);
}

void QGraph3D::RenderCylinderX(LPDIRECT3DDEVICE9 pd3dDevice, double YaxisLenth = 100.0f, double Sw = 30.0f, double XaxisLenth = 100.0f)
{
	D3DMATERIAL9 mtrl, olMtrl;
	pd3dDevice->GetMaterial(&olMtrl);
	ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Ambient.r = mtrl.Diffuse.r = 1.0f;
	mtrl.Ambient.g = mtrl.Diffuse.g = 0.0f;
	mtrl.Ambient.b = mtrl.Diffuse.b = 0.0f;
	mtrl.Ambient.a = mtrl.Diffuse.a = 1.0f;
	pd3dDevice->SetMaterial(&mtrl);

	D3DLIGHT9 light;
	D3DXVECTOR3 dir(0.0f, 1.0f, 1.0f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = color * 0.6f;
	light.Diffuse = color;
	light.Specular = color * 0.6f;
	light.Direction = dir;
	pd3dDevice->SetLight(0, &light);
	pd3dDevice->LightEnable(0, TRUE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	D3DXMATRIX wm, olVM, Rx, Ry, Rz, worldAs;
	D3DXMatrixIdentity(&worldAs);                  // ��λ���������
	pd3dDevice->GetTransform(D3DTS_WORLD, &olVM);


	D3DXMatrixRotationX(&Rx, D3DX_PI * 0.0f);    // ��X����ת
	D3DXMatrixRotationY(&Ry, D3DX_PI * 0.5f);    // ��y����ת
	D3DXMatrixRotationZ(&Rz, D3DX_PI * -0.0f);    // ��z����ת
	D3DXMATRIX amWorld;
	worldAs = Rx * Ry * Rz * m_matWorld;
	amWorld = Rx * Ry * Rz * m_matWorld;

	pd3dDevice->SetTransform(D3DTS_WORLD, &amWorld);
	m_pXAxisMesh->DrawSubset(0);

	D3DXMatrixRotationY(&Ry, D3DX_PI * -0.5f);    // ��y����ת
	worldAs = Ry * worldAs;
	pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);



	for (double i = 0; i < XaxisLenth / 2; i += Sw) {
		m_Sphere->DrawSubset(0);
		D3DXMatrixTranslation(&wm, Sw, 0.0f, 0.0f);
		worldAs = wm * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
	}

	D3DXMatrixTranslation(&wm, 0.0f, 0.0f, YaxisLenth / 2);
	worldAs = wm * amWorld;
	pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);

	m_pConeMesh->DrawSubset(0);


	D3DXMatrixRotationY(&Ry, D3DX_PI * 0.5f);    // ��y����ת
	worldAs = Ry * amWorld;
	pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
	for (double i = -Sw; i > -XaxisLenth / 2; i -= Sw) {

		D3DXMatrixTranslation(&wm, Sw, 0.0f, 0.0f);
		worldAs = wm * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
		m_Sphere->DrawSubset(0);
	}



	pd3dDevice->LightEnable(0, FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	pd3dDevice->SetMaterial(&olMtrl);
	pd3dDevice->SetTransform(D3DTS_WORLD, &olVM);
}
void QGraph3D::RenderCylinderY(LPDIRECT3DDEVICE9 pd3dDevice, double XaxisLenth = 100.0f, double Sw = 30.0f, double YaxisLenth = 100.0f)
{

	double fontSize = 5.0f;
	D3DMATERIAL9 mtrl, olMtrl;
	pd3dDevice->GetMaterial(&olMtrl);
	ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Ambient.r = mtrl.Diffuse.r = 0.0f;
	mtrl.Ambient.g = mtrl.Diffuse.g = 01.0f;
	mtrl.Ambient.b = mtrl.Diffuse.b = 0.0f;
	mtrl.Ambient.a = mtrl.Diffuse.a = 1.0f;
	pd3dDevice->SetMaterial(&mtrl);

	D3DLIGHT9 light;
	D3DXVECTOR3 dir(0.0f, 1.0f, 1.0f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = color * 0.6f;
	light.Diffuse = color;
	light.Specular = color * 0.6f;
	light.Direction = dir;
	pd3dDevice->SetLight(0, &light);
	pd3dDevice->LightEnable(0, TRUE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	D3DXMATRIX wm, olVM, Rx, Ry, Rz, worldAs;
	D3DXMatrixIdentity(&worldAs);                  // ��λ���������

	pd3dDevice->GetTransform(D3DTS_WORLD, &olVM);


	D3DXMatrixRotationX(&Rx, D3DX_PI * -0.5f);    // ��X����ת
	D3DXMatrixRotationY(&Ry, D3DX_PI * -0.0f);    // ��y����ת
	D3DXMatrixRotationZ(&Rz, D3DX_PI * -0.0f);    // ��z����ת
	//D3DXMatrixTranslation(&wm, XaxisLenth, 0.0f, 0.0f);

	D3DXMATRIX amWorld;
	worldAs = Rx * Ry * Rz * m_matWorld;

	amWorld = Rx * Ry * Rz * m_matWorld;
	pd3dDevice->SetTransform(D3DTS_WORLD, &amWorld);
	m_pYAxisMesh->DrawSubset(0);
	D3DXMatrixRotationX(&Rx, D3DX_PI * 0.5f);    // ��X����ת
	worldAs = Rx * worldAs;
	pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);



	for (double i = 0; i < YaxisLenth / 2; i += Sw) {
		m_Sphere->DrawSubset(0);
		D3DXMatrixTranslation(&wm, 0.0f, Sw, 0.0f);

		worldAs = wm * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
	}

	D3DXMatrixTranslation(&wm, 0.0f, 0.0f, YaxisLenth / 2);
	worldAs = wm * amWorld;
	pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);

	m_pConeMesh->DrawSubset(0);


	D3DXMatrixRotationX(&Rx, D3DX_PI * -0.5f);    // ��X����ת
	worldAs = Rx * amWorld;


	for (double i = -Sw; i > -YaxisLenth / 2; i -= Sw) {
		D3DXMatrixTranslation(&wm, 0.0f, Sw, 0.0f);
		worldAs = wm * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
		m_Sphere->DrawSubset(0);

	}

	pd3dDevice->LightEnable(0, FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	pd3dDevice->SetMaterial(&olMtrl);
	pd3dDevice->SetTransform(D3DTS_WORLD, &olVM);
}
void QGraph3D::RenderScalStrX(LPDIRECT3DDEVICE9 pd3dDevice, double xLength = 100.0f, double yLength = 100.0f, double SW = 30.0f)
{

	double fontSize = m_AxisFontSize;
	D3DMATERIAL9 mtrl, olMtrl;
	pd3dDevice->GetMaterial(&olMtrl);
	ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Ambient.r = mtrl.Diffuse.r = 0.5f;
	mtrl.Ambient.g = mtrl.Diffuse.g = 0.0f;
	mtrl.Ambient.b = mtrl.Diffuse.b = 0.0f;
	mtrl.Ambient.a = mtrl.Diffuse.a = 1.0f;
	pd3dDevice->SetMaterial(&mtrl);
	D3DLIGHT9 light;
	D3DXVECTOR3 dir(1.0f, 0.0f, 1.0f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = color * 0.6f;
	light.Diffuse = color;
	light.Specular = color * 0.6f;
	light.Direction = dir;
	pd3dDevice->SetLight(0, &light);
	pd3dDevice->LightEnable(0, TRUE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);


	D3DXMATRIX wm, olVM, Rx, Ry, Rz, worldAs;
	D3DXMatrixIdentity(&worldAs);                  // ��λ���������

	pd3dDevice->GetTransform(D3DTS_WORLD, &olVM);

	float len = 0.0f, oldL = 0.0;

	for (auto it : m_xScal) {
		CalculateMeshLength(it, oldL);
		if (oldL > len)len = oldL;
	}


	D3DXMatrixRotationX(&Rx, D3DX_PI * 0.0f);    // ��X����ת
	D3DXMatrixRotationY(&Ry, D3DX_PI * 0.0f);    // ��y����ת
	D3DXMatrixRotationZ(&Rz, D3DX_PI * -0.0f);    // ��z����ת
	D3DXMatrixTranslation(&wm, 0.0f, m_AxisRadius * 3, 0.0f);



	D3DXMATRIX scaleMatrix;
	D3DXMatrixScaling(&scaleMatrix, fontSize, fontSize, fontSize); // 
	//worldAs = scaleMatrix* worldAs;
	worldAs = scaleMatrix * wm * Rx * Ry * Rz * m_matWorld;
	pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
	D3DXMatrixRotationZ(&Rz, D3DX_PI * -0.5f);    // ��z����ת
	D3DXMatrixRotationY(&Ry, D3DX_PI * -1.0f);
	worldAs = Ry * Rz * worldAs;
	D3DXMATRIX bufwd = worldAs;

	for (int i = -1; ++i * SW < xLength / 2;)
	{
		D3DXMATRIX wm;

		worldAs = Rx * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
		m_xScal[i]->DrawSubset(0);

		D3DXMatrixTranslation(&wm, 0.0f, SW / fontSize, 0.0f);
		worldAs = wm * worldAs;

		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);

	}
	worldAs = bufwd;
	int count = xLength / 2 / SW;
	for (int i = -1; ++i < count;)
	{
		D3DXMATRIX wm;
		//D3DXMatrixRotationX(&Rx, D3DX_PI * 1.0f);
		D3DXMatrixTranslation(&wm, 0.0f, -SW / fontSize, 0.0f);
		worldAs = wm * worldAs;
		//worldAs = Rx * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
		m_xScal[i + count + 1]->DrawSubset(0);


	}

	// ���ö����ʽ
	pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	pd3dDevice->LightEnable(0, FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);

	pd3dDevice->SetTransform(D3DTS_WORLD, &olVM);
	pd3dDevice->SetMaterial(&olMtrl);
}
void QGraph3D::RenderScalStrY(LPDIRECT3DDEVICE9 pd3dDevice, double yLength = 100.0f, double xLength = 100.0f, double SW = 30.0f)
{

	double fontSize = m_AxisFontSize;
	D3DMATERIAL9 mtrl, olMtrl;
	pd3dDevice->GetMaterial(&olMtrl);
	ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Ambient.r = mtrl.Diffuse.r = 0.0f;
	mtrl.Ambient.g = mtrl.Diffuse.g = 0.5f;
	mtrl.Ambient.b = mtrl.Diffuse.b = 0.0f;
	mtrl.Ambient.a = mtrl.Diffuse.a = 1.0f;
	pd3dDevice->SetMaterial(&mtrl);

	D3DLIGHT9 light;
	D3DXVECTOR3 dir(0.0f, 1.0f, 0.0f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = color * 0.6f;
	light.Diffuse = color;
	light.Specular = color * 0.6f;
	light.Direction = dir;
	pd3dDevice->SetLight(0, &light);
	pd3dDevice->LightEnable(0, TRUE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);


	D3DXMATRIX wm, olVM, Rx, Ry, Rz, worldAs;
	D3DXMatrixIdentity(&worldAs);                  // ��λ���������
	pd3dDevice->GetTransform(D3DTS_WORLD, &olVM);

	D3DXMatrixRotationX(&Rx, D3DX_PI * 0.0f);    // ��X����ת
	D3DXMatrixRotationY(&Ry, D3DX_PI * 1.0f);    // ��y����ת
	D3DXMatrixRotationZ(&Rz, D3DX_PI * -0.0f);    // ��z����ת

	D3DXMatrixTranslation(&wm, m_AxisRadius * 3, 0.0f, 0.0f);
	//worldAs *= wm;

	D3DXMATRIX scaleMatrix;
	D3DXMatrixScaling(&scaleMatrix, fontSize, fontSize, fontSize); // 
	//worldAs *= scaleMatrix;
	worldAs = scaleMatrix * wm * Rx * Ry * Rz * m_matWorld;
	D3DXMATRIX bufwd = worldAs;
	pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);



	for (int i = -1; ++i * SW < yLength / 2;)
	{
		D3DXMATRIX wm;

		m_yScal[i]->DrawSubset(0);

		D3DXMatrixTranslation(&wm, 0.0f, SW / fontSize, 0.0f);
		worldAs = wm * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);

	}


	worldAs = bufwd;

	int count = yLength / 2 / SW;
	for (int i = -1; ++i < count;)
	{
		D3DXMATRIX wm;
		D3DXMatrixTranslation(&wm, 0.0f, -SW / fontSize, 0.0f);
		worldAs = wm * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
		m_yScal[i + count + 1]->DrawSubset(0);



	}

	// ���ö����ʽ
	pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	pd3dDevice->LightEnable(0, FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);

	pd3dDevice->SetTransform(D3DTS_WORLD, &olVM);
	pd3dDevice->SetMaterial(&olMtrl);
}
void QGraph3D::RenderScalStrZ(LPDIRECT3DDEVICE9 pd3dDevice, double zLength = 100.0f, double xLength = 100.0f, double SW = 30.0f)
{

	double fontSize = m_AxisFontSize;
	D3DMATERIAL9 mtrl, olMtrl;
	pd3dDevice->GetMaterial(&olMtrl);
	ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Ambient.r = mtrl.Diffuse.r = 0.0f;
	mtrl.Ambient.g = mtrl.Diffuse.g = 0.0f;
	mtrl.Ambient.b = mtrl.Diffuse.b = 0.5f;
	mtrl.Ambient.a = mtrl.Diffuse.a = 1.0f;
	pd3dDevice->SetMaterial(&mtrl);
	D3DLIGHT9 light;
	D3DXVECTOR3 dir(1.0f, 0.0f, 1.0f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = color * 0.6f;
	light.Diffuse = color;
	light.Specular = color * 0.6f;
	light.Direction = dir;
	pd3dDevice->SetLight(0, &light);
	pd3dDevice->LightEnable(0, TRUE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);


	D3DXMATRIX wm, olVM, Rx, Ry, Rz, worldAs;
	D3DXMatrixIdentity(&worldAs);                  // ��λ���������
	//D3DXMatrixTranslation(&worldAs, 0.0f, 0.0f, 0.0f);
	pd3dDevice->GetTransform(D3DTS_WORLD, &olVM);

	D3DXMatrixRotationX(&Rx, D3DX_PI * 0.0f);    // ��X����ת
	D3DXMatrixRotationY(&Ry, D3DX_PI * -0.00f);    // ��y����ת
	D3DXMatrixRotationZ(&Rz, D3DX_PI * -0.00f);    // ��z����ת


	D3DXMatrixTranslation(&wm, m_AxisRadius * 3, 0.0f, 0.0f);



	D3DXMATRIX scaleMatrix;
	D3DXMatrixScaling(&scaleMatrix, fontSize, fontSize, fontSize); // 
	//worldAs *= scaleMatrix;
	worldAs = scaleMatrix * wm * Rx * Ry * Rz * m_matWorld;
	D3DXMATRIX bufwd = worldAs;

	pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);


	//pd3dxTextMesh->DrawSubset(0);
	for (int i = -1; ++i * SW < zLength / 2;)
	{
		D3DXMATRIX wm;
		D3DXMatrixRotationX(&Rx, D3DX_PI * 0.5f);    // ��X����ת
		worldAs = Rx * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
		m_zScal[i]->DrawSubset(0);
		D3DXMatrixRotationX(&Rx, D3DX_PI * -0.5f);    // ��X����ת

		D3DXMatrixTranslation(&wm, 0.0, 0.0, SW / fontSize);
		worldAs = wm * Rx * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);

	}

	worldAs = bufwd;
	int count = zLength / 2 / SW;
	for (int i = -1; ++i < count;)
	{
		D3DXMATRIX wm;
		D3DXMatrixRotationX(&Rx, D3DX_PI * 0.5f);    // ��X����ת
		D3DXMatrixTranslation(&wm, 0.0, 0.0, -SW / fontSize);
		worldAs = wm * worldAs;
		worldAs = Rx * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);
		m_zScal[i + count + 1]->DrawSubset(0);
		D3DXMatrixRotationX(&Rx, D3DX_PI * -0.5f);    // ��X����ת

		worldAs = Rx * worldAs;
		pd3dDevice->SetTransform(D3DTS_WORLD, &worldAs);

	}


	// ���ö����ʽ
	pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	pd3dDevice->LightEnable(0, FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);

	pd3dDevice->SetTransform(D3DTS_WORLD, &olVM);
	pd3dDevice->SetMaterial(&olMtrl);
}
void QGraph3D::CalculateMeshLength(ID3DXMesh* mesh, float& length) {
	if (!mesh) return;

	// ��ȡ��������
	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
	mesh->GetDeclaration(decl);

	// �������㻺����
	void* pVertices = nullptr;
	mesh->LockVertexBuffer(D3DLOCK_READONLY, &pVertices);

	// ��ȡ���������
	DWORD numVertices = mesh->GetNumVertices();

	// ������������
	D3DXVECTOR3 minPt(FLT_MAX, FLT_MAX, FLT_MAX);
	D3DXVECTOR3 maxPt(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	BYTE* pByte = static_cast<BYTE*>(pVertices);
	for (DWORD i = 0; i < numVertices; ++i) {
		D3DXVECTOR3* pPos = reinterpret_cast<D3DXVECTOR3*>(pByte);

		// ������С������
		if (pPos->x < minPt.x) minPt.x = pPos->x;
		if (pPos->y < minPt.y) minPt.y = pPos->y;
		if (pPos->z < minPt.z) minPt.z = pPos->z;

		if (pPos->x > maxPt.x) maxPt.x = pPos->x;
		if (pPos->y > maxPt.y) maxPt.y = pPos->y;
		if (pPos->z > maxPt.z) maxPt.z = pPos->z;

		pByte += mesh->GetNumBytesPerVertex();
	}

	// �������㻺����
	mesh->UnlockVertexBuffer();

	// ���㳤��
	D3DXVECTOR3 diff = maxPt - minPt;
	length = D3DXVec3Length(&diff);
}

//z=sinf()

bool DefineDomain(float x, float y)
{
	return true;
	//�����ж϶�����
	return 16 - x * x - y * y >= -0.0001;
}
//ȡ������Yֵ
int DefineDomainYValue(float x, float* yarr)
{
	yarr[0] = 4.0;
	yarr[1] = -4.0;
	return 2;

	float y1 = sqrtf(16 - x * x);
	yarr[0] = y1;
	if (!y1)return 1;
	yarr[1] = -y1;
	return 2;
}
//ȡ������Xֵ
int DefineDomainXValue(float y, float* xarr)
{
	xarr[0] = 4.0;
	xarr[1] = -4.0;
	return 2;
	float x1 = sqrtf(16 - y * y);
	xarr[0] = x1;
	if (!x1)return 1;
	xarr[1] = -x1;

	return 2;
}
bool GetZValue(float x, float y, float* z)
{
	if (!DefineDomain(x, y))return FALSE;

	*z = sin(x) * sin(y);
	return true;
	//z = sinf(pow(x*x+y*y,0.5));
	*z = pow(abs(16 - x * x - y * y), 0.5);
	//z = 0;
	return true;
}


void QGraph3D::TEST()
{
	//float x[2] = { -4,4 };
	//float y[2] = { -4,4 };

	//AddCurve(x,y, DefineDomain, DefineDomainYValue, DefineDomainXValue, GetZValue,100,100);
}


extern "C"
__declspec(dllexport) int QG3DAddCurve(QGraph3D* qg3d, float* xRange, float* yRange, 
	DEFINEDOMAIN dd, DEFINEDOMAINYVALUE ddy, DEFINEDOMAINXVALUE ddx, GETZVALUE getZ, 
	int numVerticesX, int numVerticesY)
{
	return qg3d->AddCurve(xRange, yRange, dd, ddy, ddx, getZ, numVerticesX, numVerticesY);
}
extern "C"
__declspec(dllexport) void  QG3DSetVisualHeigh(QGraph3D * qg3d, float height)
{
	return qg3d->SetVisualHeigh(height);
}
extern "C"
__declspec(dllexport) void  QG3DIsVisivalAxis(QGraph3D * qg3d, bool isVisival)
{
	return qg3d->IsVisivalAxis(isVisival);
}




















