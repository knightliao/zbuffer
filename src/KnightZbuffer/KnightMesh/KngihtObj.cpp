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
//					可处理多边形面片
// Time			:	2008/12/20
// Email		:	KnightLiao@gmail.com	liaoqiqi@cad.zju.edu.cn
//////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "KnightObj.h"
#include "../KnightGlobal/KnightGlobal.h"
#include "../KnightDebug/KnightDebug.h"
#include "../KnightMath/KnightMath.h"


//=====第一遍扫描OBJ文件 统计顶点数目 面数目
bool KnightObj::ReloadObjFile(const char* file_path, int& vertex_number,int& surface_number) {

//	t.print("trace in KnightObj::ReloadObjFile\n");

	FILE *pFile;
	//---带有安全机制的打开函数---
	if(fopen_s(&pFile,file_path,"r"))
		return false;
	vertex_number = surface_number = 0;
	//---假设一行最多有65535个字符
	static const int MAXLEN = 65535;
	static char line[MAXLEN];
	//---设置标志位 倒数第一位为0,如果被修改,则说明行缓冲不够,出错---
	line[MAXLEN-2] = 0;
	while(fgets(line,MAXLEN,pFile)){
		//---如果 一行超过65535 则说明出错
		if(line[MAXLEN-2]){
			fclose(pFile);
			return false;
		}
		switch(*line){
			case 'f':	// 面信息
				//---一行一个面片---三角形面片也是多边形面片
				surface_number++ ;
				break;
			case 'v':	// 顶点信息 v vt纹理  vn法线
				// 每一行代表一个顶点
				if(*(line+1)==' ')
					++vertex_number;
				break;
			default :
				break;
		}
	}
	fclose(pFile);
	return vertex_number && surface_number;
}


//---辅助函数1:去掉控制字符,包括空格(0x20)---
inline const char* ridOfCtrl(const char* s) {
	while (*s && (*s<0x21 || *s>0x7e)) ++s;
	return s;
}


//----辅助函数2:读取面的一个顶点信息---
inline const char* getVerInfo(const char* is, char* os) {
	const char *old=is=ridOfCtrl(is);
	//---把非控制符读进来,不包括空格---
	while (*is>0x20 && *is<0x7f) 
		*(os++)=*(is++); 
	*os=0;
	return old==is ? 0 : is;
}


//---辅助函数3:读取出第一个顶点的顶点索引信息---
inline unsigned int getVerIndex(const char* s) {
	//---去掉可能出现的+号---
	if (*(s=ridOfCtrl(s))=='+') 
		++s;
	unsigned int ret=0;
	while (*s>='0' && *s<='9') 
		ret=ret*10+*(s++)-'0';
	return ret;
}


//=====读多边形面片=====
bool KnightObj::ReadPolygonal(const char* file_path, int& vertex_number,int& surface_number) {

	//---设置指针---
	Triple<double>* vertex_ptr = &m_vertex[0];	 
	ObjFace* surface_ptr = &m_surface[0];	// 面片指针

	FILE *pFile;
	if(fopen_s(&pFile,file_path,"r"))
		return false;
	static const int MAXLEN = 65536;
	static char line[MAXLEN];
	vertex_number = surface_number = 0;
	line[MAXLEN-2] = 0;
	while(fgets(line,MAXLEN,pFile)){
		if(line[MAXLEN-2]){
			fclose(pFile);
			return false;
		}
		static char tmp[MAXLEN];
		const char *p;
		switch(*line){
		case 'v':	
			//---顶点
			if(*(line+1) == ' '){	
				//---读入顶点的坐标---
				sscanf_s(line+1,"%lf %lf %lf", &vertex_ptr->x, &vertex_ptr->y, &vertex_ptr->z);
				//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++ 		
//				t_stream << vertex_ptr->x << " " << vertex_ptr->y << " "<<vertex_ptr->z << std::endl;
				//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
				++vertex_ptr;
				++vertex_number;
			}		
			break;
		case 'f':
			//---表面---
			p=line+1;
			//---把所有顶点索引压入栈---
			while(p=getVerInfo(p,tmp)){
				surface_ptr->vertexIndex.push_back(getVerIndex(tmp)-1);//---注意,在这里就减1了
			}
			//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
//			for(int j=0;j<(int)surface_ptr->vertexIndex.size();++j)
//				t_stream << surface_ptr->vertexIndex[j] << " " ; 
//			t_stream << " end " << std::endl;
			//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
			++surface_ptr;
			++surface_number;
			break;
		default:
			break;
		}
	}
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	t_stream << "surface num: " << surface_number << "\nvertex num: " << vertex_number << std::endl;
	//t_stream.offFile_stream();
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	fclose(pFile);
	return true;
}


//====设置模型属性信息=====
// 中心点 模型大小 面的法线信息
void KnightObj::SetProperties() {
	m_center.x = m_center.y = m_center.z = 0.0 ;
	Triple<double> min_xyz = m_vertex[0],max_xyz=m_vertex[0];
	for(int i=0; i< (int)m_vertex.size(); ++i){
		m_center += m_vertex[i];
		min_xyz.x = min(min_xyz.x,m_vertex[i].x);
		min_xyz.y = min(min_xyz.y,m_vertex[i].y);
		min_xyz.z = min(min_xyz.z,m_vertex[i].z);
		max_xyz.x = max(max_xyz.x,m_vertex[i].x);
		max_xyz.y = max(max_xyz.y,m_vertex[i].y);
		max_xyz.z = max(max_xyz.z,m_vertex[i].z);
	}
	//---中心位置---
	m_center/=(float)m_vertex.size();
	//---法线量---
	m_normal.resize(m_surface.size());
	//---模型的大小----
	m_scale = (max_xyz-min_xyz).VectorLen();
	
	//---求面的法线信息---
	for(int i=0;i<(int)m_surface.size();++i){
		m_normal[i] = (m_vertex[m_surface[i].vertexIndex[1]] - m_vertex[m_surface[i].vertexIndex[0]])*
					(m_vertex[m_surface[i].vertexIndex[2]]-m_vertex[m_surface[i].vertexIndex[0]]);
		m_normal[i] /= m_normal[i].VectorLen();
	}
	
}
