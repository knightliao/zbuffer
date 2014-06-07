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


#ifndef _KNIGHT_ALGORITHM_H_
#define _KNIGHT_ALGORITHM_H_


#include "../KnightMath/KnightMath.h"		// 自定义数学库
#include "../KnightMesh/KnightObj.h"		// obj模型定义
#include "../KnightMesh/KnightTriple.h"		// 3元数组
#include <list>


//---多边形表、活化多边形表--------------------
struct KnightPolygon{
	double a,b,c;	// 多边形方程
//	double d;
	int delta_y;	// 穿过的扫描线数
//	Triple<unsigned char> color;	// 颜色
	unsigned int num;	// 多边形编号
//	KnightPolygon *next;	// 链表
};

//---边表---------------------
struct Edge{
	double delta_x;	// x变化量
	int delta_y;	// y变化量
	unsigned int num;	//多边形编号
	double x;	// 边的上端点X坐标的值
	double z;	// 最高顶点的深度值,这在算法书上没有提到这个
//	Edge *next;		// 链表
};

//---活化边表-------------
struct EdgeActive{
	double xl,delta_xl;
	double xr,delta_xr;
	double zl,delta_zx,delta_zy;
	unsigned int num;
	int delta_yl,delta_yr;
};

typedef std::list<EdgeActive>::iterator Iterator;

#endif