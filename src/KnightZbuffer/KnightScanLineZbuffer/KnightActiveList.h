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


#ifndef  _KNIGHT_ACTIVE_H_
#define  _KNGIHT_ACTIVE_H_


#include "Algorithm.h"
#include <list>


typedef std::list<KnightPolygon>::iterator IteratorPoly;

class KnightActiveList{
public:
	//---释放内存
	~KnightActiveList();
	KnightActiveList();
	
	//---活化边表、活化多边形表 清空
	void Reset(){
		m_edgeActive.clear();
		m_polygonActive.clear();
	}
	
	//---把多边形插入活化多边表中
	void AddActivePoly(KnightPolygon &poly);
	
	//---把边插入活化边表中-----------------
	void AddActiveEdge(int poly_num,EdgeActive &edge_active);

	//---得到活化边表
	std::list<EdgeActive>  *getEdgeActive();

	//---删除掉活化边
	void eraseEdgeActive(std::list<EdgeActive>::iterator it);

	//---得到指定活化多边形的delta_y
	int getPolyDeltaY(int num);

	//---删除掉指定活化多边形
	void erasePolyActive(int num);

	//---对活化多边形的每个delta_y减一
	void PolySubDeltaY();

	//---把Y变化量小于1的每个多边形删掉-----------
	void PolyUpdate();

	//---打印出 活化边 中的信息----------------------
	void printEdge();

	//---打印出 活化多边形 中的信息-----
	void printPoly();

	//*****打印活化边表的信息*************************
	void printEdgeInfo(int num);

private:
	std::list<EdgeActive> m_edgeActive;		// 活化边表
	std::list<KnightPolygon> m_polygonActive;	// 活化多边形表
};

#endif