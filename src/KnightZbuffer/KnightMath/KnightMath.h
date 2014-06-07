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
// Description	:	数学函数
// Time			:	2008/12/20
// Email		:	KnightLiao@gmail.com	liaoqiqi@cad.zju.edu.cn
//////////////////////////////////////////////////////////////////////////////////


#ifndef _KNIGHT_MATH_H_
#define _KNIGHT_MATH_H_

#include <math.h>

//-----平方函数------------------------------
template <typename T>
inline T square(const T& x) {return x*x;}

//-----求最大最小函数-------------------------
//---能在上界为3[n/2]的复杂度上 求出最大最小值---
template<typename T>
inline T callWithMax(const T&a ,const T& b)
{
	return (a>b ? a : b);
}
template<typename T>
inline T callWithMin(const T&a ,const T& b)
{
	return (a<b ? a : b);
}

inline int Round(const double &x){
	return (int)(x+0.5);
}

// complex:3[n/2]
template<typename T>
bool min_max(T a[],int n,T b[])
{
	T max,min;
	bool even=false;// the default is odd

	if(n/2==0)
		even=true;	// change to even  

	if(even==false){	// if is odd
		min = max = a[0];	//	calculate the min and max default value 
		int mid = (n-1)/2;	// circle times
		for(int i=1;i<mid+1;i++){	// i must below (n-1)/2+1 , circle mid times
			if(max<callWithMax(a[i],a[n-i]))
				max = callWithMax(a[i],a[n-i]);	// update the max 
			if(min>callWithMin(a[i],a[n-i]))
				min = callWithMin(a[i],a[n-i]);		// update the min 
		}
	}
	else{			//  if is even
		if(a[0]<=a[1]){
			min = a[0];
			max = a[1];
		}
		else{
			min = a[1];
			max = a[0];
		}
		int mid = (n-2)/2;	// circle times
		for(int i=2;i<2+mid;i++){	// i must below (n-2)/2+2
			if(max<callWithMax(a[i],a[n-i-1]))
				max = callWithMax(a[i],a[n-i-1]);	// update the max 
			if(min>callWithMin(a[i],a[n-i-1]))
				min = callWithMin(a[i],a[n-i-1]);		// update the min 
		}
	}		
	b[0] = max;	
	b[1] = min;

	return true;
}

#endif