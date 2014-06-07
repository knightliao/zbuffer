/////////////////////////////////////////////////////////////////////////////////
//       ___          ___                     ___          ___
//      /__/|        /__/\       ___         /  /\        /__/\         ___
//     |  |:|        \  \:\     /  /\       /  /:/_       \  \:\       /  /\
//     |  |:|         \  \:\   /  /:/      /  /:/ /\       \__\:\     /  /:/
//   __|  |:|     _____\__\:\ /__/::\     /  /:/_/::\  ___ /  /::\   /  /:/
//  /__/\_|:|____/__/::::::::\\__\/\:\__ /__/:/__\/\:\/__/\  /:/\:\ /  /::\
//  \  \:\/:::::/\  \:\~~\~~\/   \  \:\/\\  \:\ /~~/:/\  \:\/:/__\//__/:/\:\
//   \  \::/~~~~  \  \:\  ~~~     \__\::/ \  \:\  /:/  \  \::/     \__\/  \:\
//    \  \:\       \  \:\         /__/:/   \  \:\/:/    \  \:\          \  \:\
//     \  \:\       \  \:\        \__\/     \  \::/      \  \:\          \__\/
//      \__\/        \__\/                   \__\/        \__\/
//
//
// author		:	KNIGHT
// Description	:	物理引擎接口
// Time			:	2008/12/20
// Email		:	KnightLiao@gmail.com	liaoqiqi@cad.zju.edu.cn
//////////////////////////////////////////////////////////////////////////////////


#ifndef _KNIGHT_ENGINEINTERFACE_H_
#define _KNIGHT_ENGINEINTERFACE_H_


#include "KnightDefine.h"
#include "../KnightGlobal/KnightGlobal.h"	// 调试类
#include "../KnightMesh/KnightTriple.h"
#include "../KnightMesh/KnightObj.h"


class KnightEngineInterface
{
public:

	KnightEngineInterface() : m_screenWidth(0),
		m_screenHeight(0), m_near(0), m_far(0) { }
	virtual ~KnightEngineInterface() {}	// 虚拟的析构函数，可以保证派生类在析构时可以调用正确的析构函数

	//---初使化---
	virtual bool Initialize(int w, int h,WinHWND mainWin, bool fullScreen) = 0;	

	//---计时函数---
	virtual void OneTimeInit() = 0;

	//---清除渲染系统--- 
	virtual void Shutdown() = 0;

	//---将后台缓存颜色设置为指定颜色---
	virtual void SetClearCol(float r, float g, float b) = 0;

	//---启动新场景---
	virtual void StartRender(bool bColor, bool bDepth,bool bStencil) = 0;
	
	virtual void ClearBuffers(bool bColor, bool bDepth,
		bool bStencil) = 0;

	//---结束新场景---
	virtual void EndRendering() = 0;

	//---立体投影---
	virtual void CalculateProjMatrix(float fov, float n, float f) = 0;

	//---正交投影---
	virtual void CalculateOrthoMatrix(float n, float f) = 0;

	//---创建静态顶点缓存---
	virtual int CreateStaticBuffer(VertexType, PrimType,
		int totalVerts, int totalIndices,
		int stride, void **data, unsigned int *indices,
		int *staticId) = 0;

	//---将缓存内容显示在屏幕上
	virtual int Render(int staticId) = 0;

	//---旋转
	virtual void RotateObject(double x,double y)=0;

	//---视口位置 
	virtual void SetViewport(int x,int y,unsigned int w,unsigned int h)=0;

	virtual bool createVertexFace(KnightObj* obj)=0;
	virtual bool renderInitial()=0;
	virtual void RegisterObjectInfo(double scale,  Triple<double> &center) = 0;
	virtual void FocusObject() =0;
	virtual void AntiAliasing(bool anti) = 0;
	virtual void ZoomObject(double)=0;


protected:	// 保护，可以是派生类的成员变量
	int m_screenWidth;	// 屏幕宽度
	int m_screenHeight;	// 屏幕高度
	bool m_fullscreen;	// 是否全屏

	WinHWND m_mainHandle;//窗口句柄

	float m_near;	// 投影矩阵的近距离值
	float m_far;	// 远距离值

	double m_z_delta;	// z变量
	Triple<double> m_center;	//中心位置 
	double m_scale;	//大小
};

#endif
