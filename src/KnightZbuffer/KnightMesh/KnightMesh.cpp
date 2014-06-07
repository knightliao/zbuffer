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
// Description	:	Mesh构造模块 包含了软件实现/硬件实现 绘制 的封装
// Time			:	
//--------------------------------------------------------


#include "stdafx.h"
#include "KnightMesh.h"
#include "../KnightGlobal/KnightGlobal.h"
#include "../KnightDebug/KnightDebug.h"


KnightMesh::~KnightMesh()
{
	m_obj.ClearVerSur();//清空数据，不要占用内存
	delete m_scanline;
	m_scanline = NULL;
}

//---导入OBJ模型函数--------------------------------------------------------
bool KnightMesh::LoadMesh(const char* file_path) {
	
//	t.print("trace in KnightMesh::LoadMesh");

	//---清空顶点、面缓存数据---
	m_obj.ClearVerSur();

	int vertexNum,surfaceNum;
	//---第一遍扫描统计知道OBJ文件的顶点、面数量。
	if(!m_obj.ReloadObjFile(file_path,vertexNum,surfaceNum)){
		return false;
	}

	//---重新设置顶点、面的VECTOR大小
	m_obj.ResetVerSur(vertexNum,surfaceNum);//---这里如果把Vector设置错误(surface最容易设置错误)
											//则要读多边形面片时就会出现内存溢出
	//---读面片---
	int vn,sn;
	bool ret=m_obj.ReadPolygonal(file_path,vn,sn);
	if(vn!=vertexNum || sn!=surfaceNum || !ret){
		return false;
	}

	//---设置OBJ模型的信息-----------
	m_obj.SetProperties();

	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
//	t_stream.print_stream("trace in KnightMesh::LoadMesh");
//	t_stream << "vertexNum:" << vertexNum << "  " <<"surfaceNum:"<< surfaceNum << std::endl;
	//t_stream.offFile_stream();
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	m_draw_soft = true;		// 软件绘制是否成功-------

	return true;
}


//-----硬件绘制函数-----------------------------------------------
void KnightMesh::DrawHardwareImpl(EngineMain *rMain,int render_mode){

	rMain->RenderLoopObj(&m_obj,render_mode);
}


//-----软件模拟绘制函数----------------------------------------------
void KnightMesh::DrawSoftwareImpl(){

	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	//t_stream.off_stream();
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	int vertex_num = m_obj.VertexNumber();
	Triple<double>* vertex = &(m_obj.getVertex())[0];
	int face_num = m_obj.SurfaceNumber();
	ObjFace *surface = &(m_obj.getFace())[0];
	Triple<double> *normal = &(m_obj.getNormal())[0];
	
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
/*	std::vector<Triple<double> > ver = m_obj.getVertex();
	for(int i=0;i<(int)ver.size();++i)
		t_stream << ver[i].x << " " << ver[i].y << " " << ver[i].z << std::endl;
	std::vector<ObjFace> face = m_obj.getFace();
	for(int i=0;i<(int)face.size();++i){
		for(int j=0;j<(int)surface[i].vertexIndex.size();++j){
			t_stream << surface[i].vertexIndex[j] << " ";
		}
		t_stream << " " << std::endl;
	}
*/	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	
	if(m_draw_soft == false)	// 如果第一次软件绘制失败,那么就不要再次绘制-------
		return;
	
	//---软件绘制前的初使设置-------------
	m_scanline->InitialZbuffer(vertex_num,vertex,face_num
		,surface,normal);

	//---软件绘制算法----------------
	if(!m_scanline->StartAlgorithm())
		m_draw_soft = false;	// 绘制失败,停止绘制...
}


//-----绘制函数-------------------------------------------------------
void KnightMesh::Draw(EngineMain *rMain,int render_mode) {

	//---如果m_obj还未读入内存,则不要画----
	if(!m_obj.IsValid())
		return;

	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
//	t_stream << " in draw "<< render_mode << std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	switch(render_mode){
		case 0:
			// soft
			m_draw_mode = DRAW_SOFTWARE;
			break;
		case 1:
			// directx
			m_draw_mode = DRAW_HARDWARE;
			break;
		case 2:
			// opengl
			m_draw_mode = DRAW_HARDWARE;
			break;
		default:
			break;
	}

	switch (m_draw_mode) {
	case DRAW_HARDWARE: DrawHardwareImpl(rMain,render_mode);
		break;
	case DRAW_SOFTWARE: DrawSoftwareImpl();
		break;
	default:
		break;
	}
}


//---得到顶点的数目
int KnightMesh::getVertexNum()
{
	if(m_obj.IsValid())
		return m_obj.VertexNumber();
	return 0;
}


//---得到面的数目
int KnightMesh::getFaceNum()
{
	if(m_obj.IsValid())
		return m_obj.SurfaceNumber();
	return 0;
}