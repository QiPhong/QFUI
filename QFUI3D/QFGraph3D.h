#pragma once
#include "QFCtrlBase3D.h"
#include <vector>
#include <cmath>
#include <string>
#include <format>
#include <functional>

typedef bool (*DEFINEDOMAIN)(float,float);
typedef int (*DEFINEDOMAINYVALUE)(float, float*);
typedef int (*DEFINEDOMAINXVALUE)(float, float*);
typedef bool (*GETZVALUE)(float, float, float*);
#define QF3DG_COMMONATTRIBUTE long long	//�������Դ�С 


struct QCSLINK
{
	D3DXVECTOR3 position1;
	D3DXVECTOR3 position2;
};
class Q3DCurve
{
public:
	//����1��x������2��y������ֵ�Ƿ��ڶ�������
	DEFINEDOMAIN dd = NULL;
	//����1��x������2Ϊָ��float�����ָ�룬��Ҫ�㹻��Ŀռ䣬����Ĭ�ϸ�1024��������ֵ�����ز���2�д洢��yֵ����
	DEFINEDOMAINYVALUE ddy = NULL;
	//ͬy
	DEFINEDOMAINYVALUE ddx = NULL;
	//����1��x��ֵ������2��yֵ������3��z��ָ�룻����ֵ�������Ƿ���ȡ��zֵ
	GETZVALUE getZ = NULL;
	DWORD color1;//��ɫֵ1
	DWORD color2;//��ɫֵ2
	LPDIRECT3DVERTEXBUFFER9 pDynamicVB =NULL;
	float x1,x2,y1,y2;
	int numVerticesX = 100; //x �������Ƭ��
	int numVerticesY = 100; // y �������Ƭ��
	QCSLINK* link = NULL;
	int count = 0;
};


class QGraph3D :public QFCtrlBase3D
{
	ID3DXFont* m_pFont = NULL;    //����COM�ӿ�
	LPD3DXMESH m_pConeMesh = NULL;//�������ͷ
	D3DXMATRIX m_matWorld;   //�������
	LPD3DXMESH m_Sphere = NULL; //������̶ȵ�
	double m_visualHeigh = 30.0;

	double m_AxisRadius = 0.05;
	double m_AxisPointRadius = 0.1;
	double m_AxisFontSize = 0.5f;


	double m_xMin = -8, m_xMax = 8;
	double m_xLength = 16.0f;
	double m_xSW = 3.0f;

	std::vector<ID3DXMesh*> m_xScal;
	LPD3DXMESH m_pXAxisMesh = NULL;

	double m_yMin = -8, m_yMax = 8;
	double m_yLength =16.0f;
	double m_ySW = 3.0f;
	std::vector<ID3DXMesh*> m_yScal;
	LPD3DXMESH m_pYAxisMesh = NULL;

	double m_zMin = -8, m_zMax = 8;
	double m_zLength = 16.0f;
	double m_zSW = 3.0f;
	std::vector<ID3DXMesh*> m_zScal;
	LPD3DXMESH m_pZAxisMesh = NULL;

	POINT m_LastMousePos;
	bool m_IsDragging = false;
	float m_RotationX = 0.0f;
	float m_RotationY = 0.0f;

	std::mutex m_AsChange_Lock;//������ı���
	bool m_isASChange = false;//�������Ƿ��и���
	std::chrono::steady_clock::time_point m_ASCUpdateTime = std::chrono::high_resolution_clock::now();


	union {
		QF3DG_COMMONATTRIBUTE data={0};
		struct {
			QF3DG_COMMONATTRIBUTE isVisualAS:1;//�Ƿ����������

		}attr;

	} m_common;

	//��������
	std::vector<Q3DCurve> m_Curve;
public:
	QGraph3D(HDC winddc, const GRect& rf);
	~QGraph3D();
	virtual int Draw(HDC sdc = NULL)override;                                    //���Ƶ�ָ��DC
	virtual int LButtonDown(int x, int y, WPARAM wParam)override;//������������Ϣ
	virtual int LButtonUp(int x, int y, WPARAM wParam)override;//������������Ϣ
	virtual int MouseMove(int x, int y, WPARAM wParam)override;	//����ƶ���Ϣ
	virtual int MouseWheel(int x, int y, WPARAM wParam);//������ת��
	void UpdateWorldMatrix();
	/**
	* ����1��x�ķ�Χ������Ϊ2����һ��Ϊx����Сֵ���ڶ���Ϊx�����ֵ
	* ����2��y�ķ�Χ��ͬx
	* ����3���������жϻص�������x��y�������Ƿ��ڶ�������
	* ����4���ص�����ָ�룬ȡ������yֵ������ָ��xֵ������yֵ�������ڶ�����������yֵ
	* ����5���ص�����ָ�룬ȡ������xֵ��ͬy
	* ����6���ص�����ָ�룬����xyȥz��ֵ
	* ����7��x�������Ƭ
	* ����8��y�������Ƭ
	*/
	int AddCurve(float* xRange, float* yRange, DEFINEDOMAIN dd, DEFINEDOMAINYVALUE ddy, DEFINEDOMAINXVALUE ddx,GETZVALUE getZ,int numVerticesX, int numVerticesY);

	void SetVisualHeigh(float height);

	void IsVisivalAxis(bool isVisival);

private:
	HRESULT Init();

	void initScale();

	bool isRender(int pfs, bool isbegin);

	void RenderCylinderZ(LPDIRECT3DDEVICE9 pd3dDevice, double XaxisLenth, double Sw, double ZaxisLenth);

	void RenderCylinderX(LPDIRECT3DDEVICE9 pd3dDevice, double YaxisLenth, double Sw, double XaxisLenth);

	void RenderCylinderY(LPDIRECT3DDEVICE9 pd3dDevice, double XaxisLenth, double Sw, double YaxisLenth);

	void RenderScalStrX(LPDIRECT3DDEVICE9 pd3dDevice, double xLength, double yLength, double SW);

	void RenderScalStrY(LPDIRECT3DDEVICE9 pd3dDevice, double yLength, double xLength, double SW);

	void RenderScalStrZ(LPDIRECT3DDEVICE9 pd3dDevice, double zLength, double xLength, double SW);

	void CalculateMeshLength(ID3DXMesh* mesh, float& length);


	void TEST();

};

