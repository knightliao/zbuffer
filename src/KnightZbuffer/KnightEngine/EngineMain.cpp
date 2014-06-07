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
// Description	:	管理引擎
// Time			:	2008/12/20
// Email		:	KnightLiao@gmail.com	liaoqiqi@cad.zju.edu.cn
//////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "EngineMain.h"


EngineMain::EngineMain()
{
	m_engine = NULL;
	m_render_mode = RENDER_OPENGL;	// 默认用OPENGL
}


EngineMain::~EngineMain()
{
	RenderShutdown();
}


//=====初使化引擎，生成DX或OPENGL
bool EngineMain::InitializeEngine(WinHWND hwd)
{
//	t.print("trace in EngineMain::InitializeEngine\n");

	//---生成引擎---
	switch(m_render_mode){
	case RENDER_OPENGL:
		if(!CreateOpenGLRenderer(&m_engine))
			return false;
		break;
	case RENDER_DIRECTX:
		//----
		break;
	default:
		break;
	}

	//---初使化引擎---
	if(!m_engine->Initialize(0,0,hwd,0))
		return false;

	//---设置背景颜色---
	m_engine->SetClearCol(160.0f/255.0f,160.0f/255.0f,160.0f/255.0f);
	
	return true;
}


//=====旋转模型========================
void EngineMain::RotateObject(double x,double y)
{
//	t.print("trace in EngineMain::RotateObject\n");

	switch(m_render_mode){
	case RENDER_OPENGL:
		m_engine->RotateObject(x,y);
		break;
	case RENDER_DIRECTX:
		//----
		break;
	default:
		break;
	}
}


//=====关闭引擎
void EngineMain::ShutDownEngine()
{
	if(m_engine)
	{
		m_engine->Shutdown();
		delete m_engine;
		m_engine = NULL;
	}
}


//====渲染
void EngineMain::RenderLoop()
{
	if(!m_engine)
		return ;
	m_engine->StartRender(1,1,1);

	m_engine->EndRendering();
}


//=====关闭引擎=====
void EngineMain::RenderShutdown()
{
	delete m_engine;
}


//=====设置视口=====
void EngineMain::SetViewport(int x,int y,unsigned int w,unsigned int h)
{
//	t.print("trace in EngineMain::SetViewport\n");

	switch(m_render_mode){
	case RENDER_OPENGL:
		m_engine->SetViewport(x,y,w,h);
		break;
	case RENDER_DIRECTX:
		//----
		break;
	default:
		break;
	}
}


//=====本实验中OBJ模型的渲染================
bool EngineMain::RenderLoopObj(KnightObj *obj,int render_mode)
{
//	t.print("trace in EngineMain::RenderLoopObj\n");
	
	if(render_mode==1)//directx
		m_render_mode = RENDER_DIRECTX;
	else
		m_render_mode = RENDER_OPENGL;

	switch(m_render_mode){
	case RENDER_OPENGL:
		m_engine->StartRender(1,1,0);	// 设置颜色、深度量
		m_engine->renderInitial();	// 渲染初使设置
		m_engine->createVertexFace(obj);//---填充顶点、面、法线数据---	
		m_engine->EndRendering();	// 结束渲染
		break;
	case RENDER_DIRECTX:
		//----
		break;
	default:
		break;
	}
	
	return true;
}


//======设置引擎中的模型信息
void EngineMain::RegisterObjectInfo(double scale,  Triple<double> &center)
{
//	t.print("trace in EngineMain::RegisterObjectInfo\n");

	switch(m_render_mode){
	case RENDER_OPENGL:
		//---填充顶点、面、法线数据---
		m_engine->RegisterObjectInfo(scale,center);
		break;
	case RENDER_DIRECTX:
		//----
		break;
	default:
		break;
	}
}


//==============主要是把模型放在屏幕的中间的位置========================
void EngineMain::FocusObject()
{
//	t.print("trace in EngineMain::FocusObject\n");

	switch(m_render_mode){
	case RENDER_OPENGL:
		//---填充顶点、面、法线数据---
		m_engine->FocusObject();
		break;
	case RENDER_DIRECTX:
		//----
		break;
	default:
		break;
	}
}


//=========
bool EngineMain::RenderInitialize()
{
	return true;
}


//===启动反走样====================
void EngineMain::AntiAliasing(bool anti)
{
	m_engine->AntiAliasing(anti);
}


//===放大放小模型==================
void EngineMain::ZoomObject(double z_delta)
{
	m_engine->ZoomObject(z_delta * 0.001);
}


/*

KnightEngineInterface* EngineMain::GetEngine()
{
return m_engine;
}*/