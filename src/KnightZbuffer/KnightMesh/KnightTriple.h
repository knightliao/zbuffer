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
// Description	:	Triple 3元结构
//					采用模板来实现
// Time			:	2008/12/20
// Email		:	KnightLiao@gmail.com	liaoqiqi@cad.zju.edu.cn
//////////////////////////////////////////////////////////////////////////////////


#ifndef _KNIGHT_TRIPLE_H_
#define _KNIGHT_TRIPLE_H_

#include "../KnightMath/KnightMath.h"	// 数学库

//-----三点坐标运算-------------------------
template <typename T>  
struct Triple {
	T x, y, z;

	Triple() {}
	Triple(T ax, T ay, T az) : x(ax), y(ay), z(az) {}
	
	//-----矢量加法----------------------
	Triple<T>& operator+=(const Triple<T>& rhs) {	
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	//-----矢量减法-----------------------
	Triple<T> operator-(const Triple<T>& rhs) {
		return Triple<T>(x - rhs.x, y - rhs.y, z - rhs.z);
	}
	
	//-----坐标外积公式----------------
	Triple<T> operator*(const Triple<T>& rhs) {
		return Triple<T>(y * rhs.z - z * rhs.y,
						 z * rhs.x - x * rhs.z,
                         x * rhs.y - y * rhs.x);
	}

	//-----矢量除以指定常数-------------------
	Triple<T>& operator/=(const T& rhs) {
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}

	//------矢量的长度---------------------------
	T VectorLen() const {return sqrt(square(x) + square(y) + square(z));}
};

#endif  