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
// Description	:	桶实现(多边形桶/边桶)
// Time			:	2008/12/20
// Email		:	KnightLiao@gmail.com	liaoqiqi@cad.zju.edu.cn
//////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "KnightBucket.h"
#include "../KnightGlobal/KnightGlobal.h"
#include "../KnightDebug/KnightDebug.h"


//---把边加入 边表---------------------------------------
//=== 最左端点x  相邻扫描线的x变化量  Y的变化量  第num个多边形  第Y桶 深度值zz
bool KnightBucket::AddEdge(double x,double delta_x,int delta_y,unsigned int num,unsigned int y,double zz)
{
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	//t_scanLine << "KnightBucket::AddEdge. insert edge: 扫描线是 " << y << " 属于第 " << num << " 个多边形"  << std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	Edge edge;
	edge.delta_x = delta_x;
	edge.delta_y = delta_y;
	edge.num = num;
	edge.x = x;
	edge.z = zz;
	m_edge[y].push_back(edge);
	
	return true;
}


//---把多边形 加入 多边形桶---------------------------------------
// 平面参数  num扫描线 delta_y穿过的扫描线数量  color颜色 y桶
bool KnightBucket::AddPolygon(Triple<double> para, unsigned int num , int delta_y, 
							  Triple<unsigned char> color, unsigned int y)
{
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	//t_scanLine << "KnightBucket::AddPolygon. insert poly: 扫描线是 " << y << " 属于第 " << num << " 个多边形"  << std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	KnightPolygon polygon;
	polygon.a = para.x;
	polygon.b = para.y;
	polygon.c = para.z;
//	polygon.color = color;
	polygon.delta_y = delta_y;
	polygon.num = num;
	m_polygon[y].push_back(polygon);

	return true;
}


//---如果num桶有多边形的话，则返回poly多边形链表，并返回其个数, 否则返回-1
int KnightBucket::HaveBucketPoly(int num , std::vector<KnightPolygon> &poly)
{
	if(m_polygon[num].size()==0){
		return -1;
	}
	else{
		poly = m_polygon[num];
		
		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
		//t_scanLine << "KnightBucket::HaveBucketPoly. 扫描线 " << num << " 与桶中的多边形有" << m_polygon[num].size() << " 个 " << std::endl;
		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

		return (int)m_polygon[num].size();
	}

}

//---返回第num条扫描线与第poly_num个多边形的交点,返回边表edge , 及其数量
//---在边桶中寻找属于poly_num多边形的边，并返回
int KnightBucket::HaveEdge(int num,int poly_num,std::vector<Edge> & edge)
{
	int countt=0;
	if(m_edge[num].size()==0){
		return -1;
	}
	else{
		for(int i=0;i<(int)m_edge[num].size();++i){
			if(m_edge[num][i].num == poly_num){
				edge.push_back(m_edge[num][i]);
				++countt;
			}	
		}
		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
		//t_scanLine << "KnightBucket::HaveEdge. 扫描线 " << num << "  边桶中的边有" << countt << " 个 " << std::endl;
		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

		return countt;
	}
}


//---得到指定的边 在num----num+delta_yr范围中寻找
//num:扫描线  delta_yr:变化的Y值 poly_num:指定的多边形 edge_tmp:返回的边
bool KnightBucket::getProperEdge(int num,int delta_yr,int poly_num,Edge & edge_tmp)
{
	for(int i=num;i<=(int)(num+delta_yr);++i){
		for(int j=0;j<(int)m_edge[i].size();++j){
			if(m_edge[i][j].num == poly_num){
				edge_tmp = m_edge[i][j];
				return true;
			}
		}
	}
	return false;
}


//---打印出 非空 多边形桶中的信息
void KnightBucket::printBucketPloy()
{
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	for(int i=0;i<(int)m_polygon.size();++i){
		if(m_polygon[i].size() == 0)
			continue;
		t_scanLine << "poly buket " << i  << " : " ;
		for(int j=0;j<(int)m_polygon[i].size();++j)
			t_scanLine << m_polygon[i][j].num <<  " " ;
		t_scanLine << " " <<std::endl;
	}
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
}


//---打印出 非空边桶中边的信息------------------
void KnightBucket::printBucketEdge()
{
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	for(int i=0;i<(int)m_edge.size();++i){
		if(m_edge[i].size() == 0)
			continue;
		t_scanLine << "edge buket " << i <<  " : " ;
		for(int j=0;j<(int)m_edge[i].size();++j)
			t_scanLine << m_edge[i][j].num <<  " " ;
		t_scanLine << " " <<std::endl;
	}
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
}

//---打印指定桶的信息---------------------
void KnightBucket::printBucketEdge(int num)
{
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	t_scanLine << "edge buket " << num <<  " : " <<std::endl;
	for(int j=0;j<(int)m_edge[num].size();++j)
		t_scanLine << m_edge[num][j].num 
		<< " delta_y:" << m_edge[num][j].delta_y << " delta_x: " << m_edge[num][j].delta_x 
		<< " x: " << m_edge[num][j].x << std::endl;
	t_scanLine << " " <<std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
}