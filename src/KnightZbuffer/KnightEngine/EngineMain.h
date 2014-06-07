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
//--------------------------------------------------------


#ifndef _ENGINE_MAIN_H_
#define _ENGINE_MAIN_H_

#include "KnightEngine.h"

class EngineMain {
public:
	enum {RENDER_OPENGL, RENDER_DIRECTX};

	EngineMain();
	~EngineMain();
	bool InitializeEngine(WinHWND hwd);	// 初使化引擎
	void ShutDownEngine();	// 关闭引擎
	bool RenderInitialize();	// 初使化绘制
	void RenderLoop();		// 渲染
	bool RenderLoopObj(KnightObj *obj,int render_mode);	//用于硬件渲染OBJ
	void RenderShutdown();	// 结束绘制

	//---旋转---
	void RotateObject(double x,double y);
	//---设置视口位置---
	void SetViewport(int x,int y,unsigned int w,unsigned int h);
	//---设置引擎中的模型信息
	void RegisterObjectInfo(double scale, Triple<double>& center);
	//---把模型放在视口当中
	void FocusObject();
	void setColor(float r,float g,float b){m_engine->SetClearCol( r, g, b);}
	void AntiAliasing(bool anti);
	void ZoomObject(double z_delta);

private:
	KnightEngineInterface *m_engine;	// 引擎
	int m_render_mode;	// 渲染方式 
};

#endif