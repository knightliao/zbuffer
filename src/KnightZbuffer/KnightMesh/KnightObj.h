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
// Description	:	OBJ构造模块
// Time			:	2008/12/20
// Email		:	KnightLiao@gmail.com	liaoqiqi@cad.zju.edu.cn
//////////////////////////////////////////////////////////////////////////////////


#ifndef _KNIGHT_OBJ_H
#define _KNIGHT_OBJ_H

#include <iostream>
#include <vector>
#include "KnightTriple.h"


//---多边形面的格式---
struct ObjFace{
	std::vector<unsigned int> vertexIndex;
};

class KnightObj {
public:

	//-----构造函数-----
	KnightObj(){ m_center.x=m_center.y=m_center.z =0.0; m_scale = 1;}

	//-----判断OBJ是否有效------     
	bool IsValid() const {return !m_vertex.empty() && !m_surface.empty();}

	//-----------------------------------
	//-----以下是得到模型的值-------
	//-----得到模型的中心点-----
    Triple<double>& Center() {return m_center;}

	//-----得到模型的缩放值------
	double Scale() const {return m_scale;}

	//-----得到模型的顶点数------
	int VertexNumber() const {return (int)m_vertex.size();}

	//-----得到模型的面-----
	int SurfaceNumber() const {return (int)m_surface.size();}

	//-----清空顶点、面缓存数据-----
	void ClearVerSur() {m_vertex.clear();m_surface.clear();m_normal.clear();}

	//-----重新设置顶点、面矢量大小-----
	void ResetVerSur(const int vernum,const int surnum){
		m_vertex.resize(vernum);
		m_surface.resize(surnum);
	}

	//-----读模型函数-----
	bool ReloadObjFile(const char* file_path, int& vertex_number,int& surface_number);
	bool ReadPolygonal(const char* file_path, int& vertex_number,int& surface_number);

	//-----更新模型的属性值-----
	void SetProperties();

	// 得到顶点
	std::vector<Triple<double> > & getVertex(){return m_vertex;}	

	// 得到法线
	std::vector<Triple<double> > & getNormal(){return m_normal;}

	// 得到面
	std::vector<ObjFace> & getFace(){return m_surface;}

private:
	std::vector<Triple<double> > m_vertex;	// 顶点 单精度浮点数
	std::vector<Triple<double> > m_normal;	// 法线 单精度浮点数
	std::vector<ObjFace> m_surface;	// 面
	Triple<double> m_center;	// 物体的中心坐标
	double m_scale;		// 模型的尺寸
};

#endif
