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
#define QF3DG_COMMONATTRIBUTE long long	//公共属性大小 


struct QCSLINK
{
	D3DXVECTOR3 position1;
	D3DXVECTOR3 position2;
};
class Q3DCurve
{
public:
	//参数1：x，参数2：y，返回值是否在定义域内
	DEFINEDOMAIN dd = NULL;
	//参数1：x，参数2为指向float数组的指针，需要足够打的空间，这里默认给1024个；返回值：返回参数2中存储的y值个数
	DEFINEDOMAINYVALUE ddy = NULL;
	//同y
	DEFINEDOMAINYVALUE ddx = NULL;
	//参数1：x的值；参数2：y值；参数3：z的指针；返回值：返回是否能取到z值
	GETZVALUE getZ = NULL;
	DWORD color1;//颜色值1
	DWORD color2;//颜色值2
	LPDIRECT3DVERTEXBUFFER9 pDynamicVB =NULL;
	float x1,x2,y1,y2;
	int numVerticesX = 100; //x 方向的切片数
	int numVerticesY = 100; // y 方向的切片数
	QCSLINK* link = NULL;
	int count = 0;
};


class QGraph3D :public QFCtrlBase3D
{
	ID3DXFont* m_pFont = NULL;    //字体COM接口
	LPD3DXMESH m_pConeMesh = NULL;//坐标轴箭头
	D3DXMATRIX m_matWorld;   //世界矩阵
	LPD3DXMESH m_Sphere = NULL; //坐标轴刻度点
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

	std::mutex m_AsChange_Lock;//坐标轴改变锁
	bool m_isASChange = false;//坐标轴是否有更新
	std::chrono::steady_clock::time_point m_ASCUpdateTime = std::chrono::high_resolution_clock::now();


	union {
		QF3DG_COMMONATTRIBUTE data={0};
		struct {
			QF3DG_COMMONATTRIBUTE isVisualAS:1;//是否可视坐标轴

		}attr;

	} m_common;

	//曲线数据
	std::vector<Q3DCurve> m_Curve;
public:
	QGraph3D(HDC winddc, const GRect& rf);
	~QGraph3D();
	virtual int Draw(HDC sdc = NULL)override;                                    //绘制到指定DC
	virtual int LButtonDown(int x, int y, WPARAM wParam)override;//鼠标左键按下消息
	virtual int LButtonUp(int x, int y, WPARAM wParam)override;//鼠标左键弹起消息
	virtual int MouseMove(int x, int y, WPARAM wParam)override;	//鼠标移动消息
	virtual int MouseWheel(int x, int y, WPARAM wParam);//鼠标滚轮转到
	void UpdateWorldMatrix();
	/**
	* 参数1：x的范围，长度为2，第一个为x的最小值，第二个为x的最大值
	* 参数2：y的范围，同x
	* 参数3：定义域判断回调，传入x，y，返回是否在定义域内
	* 参数4：回调函数指针，取定义域y值，传入指定x值，返回y值个数，第二个参数返回y值
	* 参数5：回调函数指针，取定义域x值，同y
	* 参数6：回调函数指针，传入xy去z的值
	* 参数7：x方向的切片
	* 参数8：y方向的切片
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

