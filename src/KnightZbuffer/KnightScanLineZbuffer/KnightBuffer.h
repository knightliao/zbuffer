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
// Description	:	Zbuffer缓存 和 帧缓存
// Time			:	2008/12/20
// Email		:	KnightLiao@gmail.com	liaoqiqi@cad.zju.edu.cn
//////////////////////////////////////////////////////////////////////////////////


#ifndef _KNIGHT_BUFFER_H_
#define _KNIGHT_BUFFER_H_


//===帧缓存类==========================================
template <typename T>
class KnightFrameBuffer{
public:

	KnightFrameBuffer():m_width(0),m_height(0){}

	//----更新高度和宽度，并重新设置其大小
	void ResizeBuffer(int width,int height){
		if(m_width!=width || m_height!=height){
			m_width = width;
			m_height = height;
			m_effectWidth = (m_width * sizeof(T)+3)/4*4;	// 有效的宽度
			m_buffer.clear();
			m_buffer.resize(m_effectWidth * m_height);	// 帧缓存大小
		}
	}

	//---初使化帧缓存----
	void Memset(int value){
		memset(&m_buffer[0],value,m_buffer.size());
	}

	//---返回要写入的帧缓存位置-----
	T* Pixel(int row,int col){
		if(row>=0 && row<m_height && col>=0 && col < m_width)
			return ((T*)(&m_buffer[0]+m_effectWidth*row))+col;
		else{		
			return &m_dummy;	// 返回未知错误
		}
	}
private:
	std::vector<char> m_buffer;	// 帧缓存
	T m_dummy;	// tmp
	int m_width,m_height;	// 宽度/高度
	int m_effectWidth;	// 有效的宽度
	KnightFrameBuffer(const KnightFrameBuffer&);	// 禁用
	KnightFrameBuffer& operator=(const KnightFrameBuffer &);	// 禁用
};


//===Zbuffer缓存========================================================
class KnightZBuffer{
public:

	KnightZBuffer(){}

	//---写Zbuffer
	double& operator[](int index){
		if((index >=0) &&  (index < (int)m_Zbuffer.size()))
			return m_Zbuffer[index];
		else
			return m_dummy;
	}

	//---重新设置大小
	void ResizeBuffer(int n){
		m_Zbuffer.clear();
		m_Zbuffer.resize(n);
	}

private:
	std::vector<double> m_Zbuffer;
	double m_dummy;
	// 禁用以下函数
	KnightZBuffer(const KnightZBuffer&);
	KnightZBuffer& operator=(const KnightZBuffer&);
};

#endif