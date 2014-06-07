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
// Description	:	OpenGL物理引擎
// Time			:	2008/12/20
// Email		:	KnightLiao@gmail.com	liaoqiqi@cad.zju.edu.cn
//////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "KnightOpenGl.h"


//-=====根据引擎控制 选择OPENGL
bool CreateOpenGLRenderer(KnightEngineInterface **pObj)
{
//	t.print("trace in CreateOpenGLRenderer\n");

	if(!*pObj) 
		*pObj = new KnightOpenGL;
	else 
		return false;

	return true;
}


KnightOpenGL::~KnightOpenGL()
{
	Shutdown();
}


//====初使化设置引擎================
bool KnightOpenGL::Initialize(int w,int h,WinHWND hdc,bool fullScreen) {

//	t.print("trace in KnightOpenGL::Initialize\n");

	//=====设置OPENGL初使化======
	//---定义像素格式描述---
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		::GetDeviceCaps(hdc, BITSPIXEL),	// 得到像素颜色位数 Number of adjacent color bits for each pixel
		0,0,0,0,0,0,0,0,
		0,0,0,0,0, 
		32,
		0,0, 
		PFD_MAIN_PLANE, 
		0, 
		0,0,0
	};
	//---得到像素最佳格式---
	//---match an appropriate pixel format supported by a device context to a given pixel format specification.
	int pixel_format;
	if(!(pixel_format = ChoosePixelFormat(hdc, &pfd))){
		//MessageBox("ChoosePixelFormat failed");
		return false;
	}
	//---设置像素格式---
	//---sets the pixel format of the specified device context to the format specified by the iPixelFormat index.
	SetPixelFormat(hdc, pixel_format, &pfd);
	//---得到OPENGL绘制上下文,得到一个与给定DC兼容的渲染描述表，调用成功返回渲染描述表句柄，不成功返回NULL---
	//---creates a new OpenGL rendering context, which is suitable for drawing on the device referenced by hdc. 
	//---The rendering context has the same pixel format as the device context.
	m_render_contex = wglCreateContext(hdc);
	//---设置当前绘制上下文,渲染描述表当前化,调用成功返回GL_TRUE
	//---makes a specified OpenGL rendering context the calling thread's current rendering context
	wglMakeCurrent(hdc, m_render_contex);

	//=====上面已经设置好OPENGL的渲染描述表，以下是设置OPENGL状态机=====
	setStatus();
	return true;
}


//---设置初使状态-------------
void KnightOpenGL::setStatus()
{
//	t.print("trace in KnightOpenGL::setStatus\n");

	glDisable(GL_DEPTH_TEST);	// 关闭ZBUFFER深度测试*****************************
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);	// 开启双面光照,正面 背面
	glClearColor(160/255,160/255,160/255,1.0f);	// 把屏幕清空成白色 默认
	glShadeModel(GL_FLAT);	// 使用着色模式：平滑着色
	glEnable(GL_LIGHTING);	// 开启光照
	glEnable(GL_LIGHT0);	// 开启光照灯1
	static const float mat_amb_diff[] = {0.0f, 0.0f, 0.0f, 1.0f };	// 设置散射光和环境光反射颜色
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,mat_amb_diff);
	m_z_delta = 0.0;	// 模型放在世界坐标的深度
	glDisable(GL_DEPTH_TEST);	// 禁用深度测试----****
}


//---旋转模型---
void KnightOpenGL::RotateObject(double x,double y)
{
//	t.print("trace in KnightOpenGL::RotateObject\n");

	double modleview_matrix[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modleview_matrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//---先移到原点----
	glTranslated(0.0,0.0,m_z_delta);
	//---旋转---
	glRotated(sqrt((double)(x*x+y*y)),x,y,0.0);
	//---再移回原处---
	glTranslated(0.0,0.0, -m_z_delta);
	glMultMatrixd(modleview_matrix);
}


//=====关闭引擎===============================
void KnightOpenGL::Shutdown()
{
//	t.print("trace in KnightOpenGL::Shutdown\n");

	wglMakeCurrent(NULL, NULL);
	if (m_render_contex != NULL) {
		wglDeleteContext(m_render_contex);
		m_render_contex = NULL;
	}
}


//=====设置清空颜色==========================
void KnightOpenGL::SetClearCol(float r, float g, float b)
{
//	t.print("trace in KnightOpenGL::SetClearCol\n");

	//---设置清空颜色---
	glClearColor(r,g,b,1);
	//---设置屏幕的颜色---
	glClear(GL_COLOR_BUFFER_BIT);
}


//=====开始渲染，设置缓存=====================
void KnightOpenGL::StartRender(bool bColor, bool bDepth,
							   bool bStencil)
{
//	t.print("trace in KnightOpenGL::StartRender\n");

	if(bColor==true)
		glClear(GL_COLOR_BUFFER_BIT);
	if(bDepth==true)
		glClear(GL_DEPTH_BUFFER_BIT);
	if(bStencil)
		glClear(GL_STENCIL_BITS);
}


//=====结束渲染===================
void KnightOpenGL::EndRendering()
{
//	t.print("trace in KnightOpenGL::EndRendering\n");

	glFinish();
}


//===设置视口==============
void KnightOpenGL::SetViewport(int x,int y,unsigned int w,unsigned int h)
{
//	t.print("trace in KnightOpenGL::SetViewport\n");

	glViewport(x,y,w,h);
}


//=====利用顶点绘制信息===========================
bool KnightOpenGL::createVertexFace(KnightObj* obj)
{
//	t.print("trace in KnightOpenGL::createVertexFace\n");

	std::vector<Triple<double> > vertex_ptr = obj->getVertex();
	std::vector<Triple<double> > normal_ptr = obj->getNormal();
	std::vector<ObjFace> face_ptr = obj->getFace();
	
	for(int i=0;i<(int)face_ptr.size();++i){
		glBegin(GL_POLYGON);
		glNormal3d(normal_ptr[i].x,normal_ptr[i].y,normal_ptr[i].z);
		t_stream << normal_ptr[i].x << " " << normal_ptr[i].y << " "<< normal_ptr[i].z << std::endl;
		for(int j=0;j<(int)face_ptr[i].vertexIndex.size();++j){
			glVertex3d(vertex_ptr[face_ptr[i].vertexIndex[j]].x,
				vertex_ptr[face_ptr[i].vertexIndex[j]].y,
				vertex_ptr[face_ptr[i].vertexIndex[j]].z);
			t_stream << vertex_ptr[face_ptr[i].vertexIndex[j]].x << " " << 
				vertex_ptr[face_ptr[i].vertexIndex[j]].y << " " << 
				vertex_ptr[face_ptr[i].vertexIndex[j]].z << std::endl;
		}
		glEnd();
	}

	t_stream.offFile_stream();
	return true;
}


//======绘制前的准备==================
bool KnightOpenGL::renderInitial()
{
//	t.print("trace in KnightOpenGL::renderInitial\n");

	// set 3Dprojection
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0,viewport[2]/(double)viewport[3],1e-2,1e6);
	//----散射的颜色-------------
	const float color[] = {77.0f/255.0f, 91.0f/255.0f, 230.0f/255.0f, 1.0f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
	// 硬件 开启深度测试---------
	glEnable(GL_DEPTH_TEST);
 
	return true;
}


//====根据模型设置引擎信息================
void KnightOpenGL::RegisterObjectInfo(double scale,  Triple<double> &center)
{
	m_scale = scale;
	m_center = center;
}


//=====把模型放置在视口空间中============================
void KnightOpenGL::FocusObject() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(-m_center.x, -m_center.y,-m_center.z - m_scale * 1.5);
	m_z_delta = -m_scale *1.5;
}


//==================是否开启反走样=======================
void KnightOpenGL::AntiAliasing(bool anti){
	if(anti){
		//glEnable(GL_POINT_SMOOTH);
		//glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);
	}
	else{
		//glDisable(GL_POINT_SMOOTH);
		//glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POLYGON_SMOOTH);
	}
}


//=================放大放小模型=================================
void KnightOpenGL::ZoomObject(double z_delta)
{	
	//---不可过大
	if(m_z_delta+z_delta*m_scale > -m_scale*1.1)
		return ;
	//---不可过小
	if(m_z_delta+z_delta*m_scale < -m_scale * 10)
		return ;
	
	//---得到模型视点矩阵==========
	double modelviewMatrix[16];
	glGetDoublev(GL_MODELVIEW_MATRIX,modelviewMatrix);

	//---修改模型视点矩阵
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//---放大放小模型=
	glTranslated(0.0,0.0,z_delta*m_scale);
	//---深度值变化--
	m_z_delta += z_delta*m_scale;
	//---乘以以前的模型视点矩阵---
	glMultMatrixd(modelviewMatrix);
}


//以下本实验没有用到过
//=======================================================================
void KnightOpenGL::ClearBuffers(bool bColor, bool bDepth,
								bool bStencil)
{

}

void KnightOpenGL::CalculateProjMatrix(float fov, float n, float f)
{

}


void KnightOpenGL::CalculateOrthoMatrix(float n, float f)
{
}


int KnightOpenGL::Render(int staticId)
{
	return 3;
}

int KnightOpenGL::CreateStaticBuffer(VertexType, PrimType, int totalVerts, int totalIndices,
									 int stride, void **data, unsigned int *indices, int *staticId)
{
	return 3;
}

void KnightOpenGL::OneTimeInit()
{

}