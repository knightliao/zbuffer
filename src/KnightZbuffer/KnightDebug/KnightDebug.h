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
// Description	:	测试模块
// Time			:	2008/12/10
// Email		:	KnightLiao@gmail.com	liaoqiqi@cad.zju.edu.cn
//////////////////////////////////////////////////////////////////////////////////


//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++


#ifndef KNIGHT_DEBUG_H
#define KNIGHT_DEBUG_H


#include <iostream>
#include <fstream>


class Trace {
public:

	//-------------------------------以下采用C语言流的方式------------------------------------------
	//---采用屏幕输出---
	Trace() {m_noisy = 0; m_f = stdout;}
	
	//---采用C语言文件流来输出---
	Trace(FILE *ff) { m_noisy = 0; m_f= ff;}

	//---采用C语言文件流打印方式---
	void print(char* s)
	{ 
		if(!m_stream_cplus && m_noisy)
			fprintf(m_f,"%s",s);
	}

	//---采用C语言文件流:是否有输出
	void on() { m_noisy = 1;}
	void off() {m_noisy = 0;}

	//-----C语言文件流的关闭方式-----
	void offFile(){
		if(!m_stream_cplus){
			m_noisy = 0;
			fclose(m_f);
		}
	}
	
	//------------------------------以下采用C++流或C流的方式来实现------------------------------------
	//-----C++或C实现,根据func来选择功能---------
	Trace(bool func,const char *filename){
		//---C++实现方式
		if(func==true){
			m_stream_cplus = true;	// 采用C++流
			m_noisy_cplus = 0;		// 默认不输出
			m_log.open(filename);		// 打开指定文件
		}
		//--- C语言方式
		else{
			m_stream_cplus = false;	// 采用C流
			m_noisy = 0;				// 默认不输出
			fopen_s(&m_f,filename,"w");	// 打开指定文件
		}
	}

	//---设置C++流的浮点数输出精度---------
	bool setPrecision(int num){
		if(m_log && num>0){
			m_log.precision(num);
			return true;
		}
		else
			return false;
	}

	//---采用C++流,是否有输出---------------------
	void on_stream(){m_noisy_cplus = 1;}
	void off_stream(){m_noisy_cplus = 0;}

	//---模板方法,C++流的输出,不推荐使用----
	template<typename U>
	void print_stream(U u){
		if(m_stream_cplus && m_noisy_cplus)
			m_log << u << std::endl;
	}

	//---关闭C++文件流----------
	void offFile_stream(){
		if(m_stream_cplus){
			m_noisy_cplus = 0;
			m_log.close();
		}
	}

	//---模板方法,功能强大的<<C++流输出方法,可以处理任意基本数据类型----------------------
	//---强烈推荐使用--------------
	//---注意：
	//		目前还不支持 m_stream << std::endl;的做法------
	template<typename T>
	std::ostream & operator<< (T t){

		if(m_stream_cplus && m_noisy_cplus){
			m_log << t ;
			return m_log;
		}
		return std::cout;
	}

private:
	bool m_stream_cplus;	// 是否使用C++流
	int m_noisy;		// C流是否有输出 
	int m_noisy_cplus;	// C++流是否输出
	FILE* m_f;			// C流方件
	std::ofstream m_log;	// C++流文件
};

#endif