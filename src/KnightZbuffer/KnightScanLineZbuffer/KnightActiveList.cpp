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
// Description	:	活性链表
// Time			:	2008/12/20
// Email		:	KnightLiao@gmail.com	liaoqiqi@cad.zju.edu.cn
//////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "KnightActiveList.h"
#include "../KnightGlobal/KnightGlobal.h"
#include "../KnightDebug/KnightDebug.h"


//---释放内存
KnightActiveList::~KnightActiveList(){
	m_edgeActive.clear(); 
	m_polygonActive.clear();
}


//---构造---
KnightActiveList::KnightActiveList(){
	m_edgeActive.clear(); 
	m_polygonActive.clear();
}


/*
//---活化边表、活化多边形表 重新设置大小
void KnightActiveList::Reset(int width){
	m_edgeActive.resize(width * 2);
	m_polygonActive.resize(width);
}
*/


//---把多边形插入活化多边表中
void KnightActiveList::AddActivePoly(KnightPolygon &poly)
{	
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	t_scanLine << "KnightActiveList::AddActivePoly. Add poly to Active poly list , it's " << poly.num << std::endl;
	t_scanLine << "a,b,c,d: " <<  poly.a << " " << poly.b << " " << poly.c << " " 
		<< " delta_y " << poly.delta_y 
	//	<< " color " << (int)poly.color.x << " " << (int)poly.color.y << " " << (int)poly.color.z 
		<< std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	m_polygonActive.insert(m_polygonActive.end(),poly);
}


//---把边插入活化边表中-----------------
void KnightActiveList::AddActiveEdge(int poly_num,EdgeActive &edge_active)
{	
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	t_scanLine << "KnightActiveList::AddActiveEdge. Add edge to Active Edge List, it belongs to " << poly_num << " poly, info is:" << std::endl;
	t_scanLine << "xl: " << edge_active.xl << " delta_xl: " << edge_active.delta_xl
		<< " delta_yl " << edge_active.delta_yl 
		<< " xr: " << edge_active.xr << " delta_xr: " << edge_active.delta_xr 
		<< " delta_yr " << edge_active.delta_yr 
		<< " zl,zx,zy: " << edge_active.zl << " " << edge_active.delta_zx << " " << edge_active.delta_zy << std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	m_edgeActive.insert(m_edgeActive.end(),edge_active);
}


//---得到活化边表
std::list<EdgeActive> * KnightActiveList::getEdgeActive(){
	return &m_edgeActive;
}


//---删除掉活化边
void KnightActiveList::eraseEdgeActive(std::list<EdgeActive>::iterator it)
{
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	t_scanLine << "KnightActiveList::eraseEdgeActive. edge which belongs to " << (*it).num  << std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	m_edgeActive.erase(it);
}


//---得到指定活化多边形的delta_y
int KnightActiveList::getPolyDeltaY(int num){
	IteratorPoly it;
	for(it=m_polygonActive.begin();it!=m_polygonActive.end();++it){
		if((*it).num==num){
			return (*it).delta_y;
		}
	}
	return 0;
}


//---删除掉指定活化多边形
void KnightActiveList::erasePolyActive(int num)
{	
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	t_scanLine << "KnightActiveList::erasePolyActive. poly which is " << num << std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	IteratorPoly it;
	for(it=m_polygonActive.begin();it!=m_polygonActive.end();++it){
		if((*it).num==num){
			m_polygonActive.erase((it));
			return ;
		}
	}
}


//---对 活化多边形 的每个delta_y减一
void KnightActiveList::PolySubDeltaY()
{
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	t_scanLine << "KnightActiveList::PolySubDeltaY. ";
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	IteratorPoly it;
	for(it=m_polygonActive.begin();it!=m_polygonActive.end();++it)
		--(*it).delta_y;
}


//---把Y变化量小于1的每个多边形删掉-----------
void KnightActiveList::PolyUpdate(){
	IteratorPoly it;
	for(it=m_polygonActive.begin();it!=m_polygonActive.end();){
		if((*it).delta_y<0){
				
			//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
			t_scanLine << "KnightActiveList::PolyUpdate。 delete poly " << (*it).num << std::endl;
			//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

			m_polygonActive.erase(it++);	// 删除该多边形,注意写法---
		}
		else{
			++it;
		}
	}
}


//---打印出 活化边 中的信息----------------------
void KnightActiveList::printEdge()
{
	if(m_edgeActive.size()==0)
		return ;

	Iterator pos;

	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	t_scanLine << "KnightActiveList::printEdge. Active Edge have: " << " ";
	for(pos = m_edgeActive.begin();pos!=m_edgeActive.end();++pos){
		t_scanLine << (*pos).num <<  " ";
	}
	t_scanLine<< " "<<std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
}


//---打印出 活化多边形 中的信息-----
void KnightActiveList::printPoly()
{
	if(m_polygonActive.size()==0)
		return ;

	typedef std::list<KnightPolygon>::iterator Iterator_poly;
	Iterator_poly pos;

	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	t_scanLine << "KnightActiveList::printPoly. Active Poly have: ";
	for(pos = m_polygonActive.begin();pos!=m_polygonActive.end();++pos){
		t_scanLine << (*pos).num <<  " delta_y " << (*pos).delta_y  << std::endl;
	}
	t_scanLine<< " "<<std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
}


//*****打印活化边表的信息*************************
void KnightActiveList::printEdgeInfo(int num)
{
	if(m_edgeActive.size()==0)
		return ;

	Iterator pos;

	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	t_scanLine << "KnightActiveList::printEdgeInfo. Active Edge info is : " << std::endl ;
	for(pos = m_edgeActive.begin();pos!=m_edgeActive.end();++pos){
		if((*pos).num !=num)
			continue;
		t_scanLine << "属于" << (*pos).num << "多边形" ;
		t_scanLine << "xl: " << (*pos).xl << " delta_xl: " << (*pos).delta_xl << " delta_yl:" << (*pos).delta_yl << 
			" xr: " << (*pos).xr << " delta_xr: " << (*pos).delta_xr << " delta_yr: " << (*pos).delta_yr <<
			" zl,zx,zy: " << (*pos).zl << " "<< (*pos).delta_zx << " " << (*pos).delta_zy << std::endl;
	}
	t_scanLine<< " "<<std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
}
