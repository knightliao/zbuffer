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
// Description	:	Mesh构造模块
//					松耦合实现方式 Mesh为整体框架 包括OBJ模型
// Time			:	2008/12/20
// Email		:	KnightLiao@gmail.com	liaoqiqi@cad.zju.edu.cn
//////////////////////////////////////////////////////////////////////////////////


#ifndef _KNIGHT_MESH_H_
#define _KNIGHT_MESH_H_


#include "KnightObj.h"
#include "../KnightEngine/EngineMain.h"
#include "../KnightScanLineZbuffer/KnightScanLineZbuffer.h"


class KnightMesh {
public:
	//-----绘制方式-----
	//---硬件或软件模拟
	enum {DRAW_HARDWARE, DRAW_SOFTWARE};

	//-----构造函数,复制构造,赋值操作-----
	KnightMesh() {
		DrawMode(DRAW_HARDWARE);
		m_scanline = new KnightScanLineZbuffer;
		m_draw_soft = false;
	}
	
//	KnightMesh(const KnightMesh& rhs);
	
	~KnightMesh();
	
	KnightMesh& operator=(const KnightMesh& rhs);

	//-----导入mesh-----
	bool LoadMesh(const char* file_path);

	//-----设置绘制方式-----
	void DrawMode(int mode) {m_draw_mode = mode;}

	//-----返回绘制方式-----
	int DrawMode() const {return m_draw_mode;}

	//-----绘制函数-----
	void Draw(EngineMain *,int render_mode);

	//---得到顶点数---
	int getVertexNum();

	//---得到面数---
	int getFaceNum();

	Triple<double>&  getCenter(){return m_obj.Center();}	// 物体的中心坐标

	double getScale(){return m_obj.Scale();}		// 模型的尺寸

	bool IsValid(){return m_obj.IsValid();}	//	Mesh是否有效

private:
	//-----内部绘制函数-----
	//---硬件 软件 绘制函数-----
	void DrawHardwareImpl(EngineMain *,int render_mode);

	void DrawSoftwareImpl();

	int m_draw_mode;	// 绘制方式:硬或软
	bool m_draw_soft;	// 软件绘制是否成功
	KnightObj m_obj;		// OBJ模型
	KnightScanLineZbuffer *m_scanline;	// 软件扫描线Zbuffer算法类
};

#endif