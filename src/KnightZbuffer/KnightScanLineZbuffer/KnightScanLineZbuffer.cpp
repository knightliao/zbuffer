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
// Description	:	扫描线Zbuffer实现
//						可以处理任意多面体 只可以处理凸多边形 
//						有利用OpenGL部分功能	
//						模拟了光线明亮的处理,使不同面具有不同的明暗值,以便于清晰的显示出棱边来
// Time			:	2008/12/20
// Email		:	KnightLiao@gmail.com	liaoqiqi@cad.zju.edu.cn
//////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "KnightScanLineZbuffer.h"
#include "../KnightGlobal/KnightGlobal.h"
#include "../KnightDebug/KnightDebug.h"


//---初使化,构造 活化表(活化多边形/活化边对)   桶(多边形桶/边桶)   帧缓存---------------
KnightScanLineZbuffer::KnightScanLineZbuffer()
{
	m_activeList = new KnightActiveList;	// 活化表
	m_bucket = new KnightBucket;	// 桶
	m_frameBuffer = new KnightFrameBuffer<Triple<unsigned char> >; // frame buffer
}


//---类的初使化
//---顶点数 顶点数据指针 面数 面数据指针 法线数据指针-----------------------------------
void KnightScanLineZbuffer::InitialZbuffer( int vetex_num,Triple<double> *vertex,
					   int face_num, ObjFace *surface , Triple<double> *xx_normal)
{
	m_vertex_num = vetex_num;	// 顶点数
	m_surface_num = face_num;	// 面数
	m_vertex = vertex;			// 顶点数据
	m_surface = surface;		// 面数据
	m_normal_x = xx_normal;		// 原法线数据
	
	//---法线数据的备份,主要是用于旋转时法线数据要变化,但又不想改变原法线数据-----
	m_normal.clear();
	m_normal.resize(m_surface_num);
	for(int i=0;i<m_surface_num;++i){
		m_normal[i].x = xx_normal[i].x;
		m_normal[i].y = xx_normal[i].y;
		m_normal[i].z = xx_normal[i].z;
	}

//	m_zBuffer = new KnightZBuffer;	// zbuffer

	//---颜色数据,主要用于面的着色---
	m_color.clear();
	m_color.resize(m_surface_num);	
}


//---析构函数-----------------------------------------------------------------
KnightScanLineZbuffer::~KnightScanLineZbuffer()
{
	//---清空数据---
	m_normal.clear();
	m_color.clear();

	//---与new操作对应
	if(m_activeList){
		delete m_activeList;
		m_activeList = NULL;
	}
	if(m_bucket){
		delete m_bucket;
		m_bucket = NULL;
	}
	if(m_frameBuffer){
		delete m_frameBuffer;
		m_frameBuffer = NULL;
	}
}


//---初使化边桶 多边形桶
//---把顶点数据读进来后 把数据写进桶里面-----------------------------------------------------
//---Y桶值一律统一取整-----------------
void KnightScanLineZbuffer::setBucket()
{
	int height = m_viewport[3];	// 宽度
	int width = m_viewport[2];	// 高度

	//---reset 桶
	m_bucket->Reset(height+1);
	//---reset 活化边表
	//m_activeList->Reset(m_surface_num+1);
	m_activeList->Reset();

	//---对每个多边表进行处理,加入Y多边形Y桶------------------
	for(int i=0;i<(int)m_surface_num;++i){

		//---求该多边形的Y变化量------------------------
		double min_y,max_y ;
		min_y = max_y = m_projectionVertex[m_surface[i].vertexIndex[0]].y;	// 第一个顶点的Y值

		//---求该多边形的最大Y值和最小值---
		for(int j =0;j<(int)m_surface[i].vertexIndex.size()-1;++j){
			min_y = min(min_y, m_projectionVertex[m_surface[i].vertexIndex[j+1]].y);
			max_y = max(max_y, m_projectionVertex[m_surface[i].vertexIndex[j+1]].y);
		}
		//---裁剪掉在视窗外的多边形,加速绘制过程------
		if((max_y)<0 || (min_y)>=height)	
			continue;

		//---循环每条边,加入边Y桶---------------------------------
		int delta_y;		// Y的变化量
		double edge_max_y,edge_min_y;	// 边的最大Y值和最小Y值
		double delta_x;		// X的变化量
		//double edge_min_x,edge_max_x;
		double edge_upper;	// 上端点的X值
		ObjFace *objFacePtr = &m_surface[i];	// 面指针

		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
		//t_scanLine << "face "<< i << " : " << objFacePtr->vertexIndex.size() << std::endl;
		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

		//---边循环开始-------------
		for(int j =0;j<(int)objFacePtr->vertexIndex.size();++j){

			//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
			//t_scanLine << objFacePtr->vertexIndex[j]  << std::endl;
			//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
			
			//---最后一个顶点与第一个顶点也会组成一条边---
			int next = j+1;
			if(next >=(int)objFacePtr->vertexIndex.size())
				next = 0;

			//----裁剪---,加速-----
			bool is_y_maxY = true;
			double zz = 1.0;	// 边的深度信息
			if(m_projectionVertex[objFacePtr->vertexIndex[j]].y > m_projectionVertex[objFacePtr->vertexIndex[next]].y){
				edge_max_y = m_projectionVertex[objFacePtr->vertexIndex[j]].y;
				edge_min_y =m_projectionVertex[objFacePtr->vertexIndex[next]].y;
				zz = m_projectionVertex[objFacePtr->vertexIndex[j]].z;		// 该边的Z值
				edge_upper = m_projectionVertex[objFacePtr->vertexIndex[j]].x;	//取上端点(Y值较大的顶点)的X值
				is_y_maxY = true;
			}
			else{
				edge_max_y = m_projectionVertex[objFacePtr->vertexIndex[next]].y;
				edge_min_y = m_projectionVertex[objFacePtr->vertexIndex[j]].y;
				zz = m_projectionVertex[objFacePtr->vertexIndex[next]].z;
				edge_upper = m_projectionVertex[objFacePtr->vertexIndex[next]].x;
				is_y_maxY = false;
			}

/*	
			if(m_projectionVertex[objFacePtr->vertexIndex[j]].x > m_projectionVertex[objFacePtr->vertexIndex[next]].x){
				zz = m_projectionVertex[objFacePtr->vertexIndex[j]].z;
			}
			else{
				zz = m_projectionVertex[objFacePtr->vertexIndex[next]].z;
			}
*/
			//edge_max_y = max(m_projectionVertex[objFacePtr->vertexIndex[j]].y,m_projectionVertex[objFacePtr->vertexIndex[j+1]].y);
			//edge_min_y = min(m_projectionVertex[objFacePtr->vertexIndex[j]].y,m_projectionVertex[objFacePtr->vertexIndex[j+1]].y);
			
			//---边的Y值超出屏幕大小--------没有处理成功。。-------------
			if(edge_max_y >= height){
				
				//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
				//t_scanLine << " @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << edge_max_y << " " << i << "  " << height <<  " " << width <<std::endl;
				//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

				double tmp_yy = (edge_max_y - height + 1);
				double tmp_yyy,tmp_xxx;
				if(is_y_maxY){
					tmp_yyy = m_projectionVertex[objFacePtr->vertexIndex[j]].y - m_projectionVertex[objFacePtr->vertexIndex[next]].y;
					tmp_xxx = m_projectionVertex[objFacePtr->vertexIndex[j]].x - m_projectionVertex[objFacePtr->vertexIndex[next]].x;
				}
				else{
					tmp_yyy	= m_projectionVertex[objFacePtr->vertexIndex[next]].y - m_projectionVertex[objFacePtr->vertexIndex[j]].y;
					tmp_yyy	= m_projectionVertex[objFacePtr->vertexIndex[next]].x - m_projectionVertex[objFacePtr->vertexIndex[j]].x;
				}
				double tmp_xx = (tmp_yy * tmp_xxx)/tmp_yyy;
				double tmp_delta_zx = m_normal[i].x / m_normal[i].z;
				tmp_delta_zx *= (-1.0);
				double tmp_delta_zy = m_normal[i].y / m_normal[i].z;
				double tmp_delta_z = tmp_xx * tmp_delta_zx - tmp_delta_zy * tmp_yy;
				if(is_y_maxY){
					zz = m_projectionVertex[objFacePtr->vertexIndex[j]].z  - tmp_delta_z;
					edge_upper = m_projectionVertex[objFacePtr->vertexIndex[j]].x - tmp_xx;
				}
				else{
					zz = m_projectionVertex[objFacePtr->vertexIndex[next]].z  - tmp_delta_z;
					edge_upper = m_projectionVertex[objFacePtr->vertexIndex[next]].x - tmp_xx;
				}
				
				//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
				t_scanLine << edge_upper <<  "  " <<  zz << std::endl;
				//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

				edge_max_y = height-1;
			}
			if(edge_min_y < 0){

				//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
				//t_scanLine << " $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << edge_min_y << " " << i << "  " << "0" << " " << width << std::endl;
				//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

				double tmp_yy = (edge_min_y - 0.0);
				double tmp_yyy,tmp_xxx;
				if(is_y_maxY){
					tmp_yyy = m_projectionVertex[objFacePtr->vertexIndex[next]].y - m_projectionVertex[objFacePtr->vertexIndex[j]].y;
					tmp_xxx = m_projectionVertex[objFacePtr->vertexIndex[next]].x - m_projectionVertex[objFacePtr->vertexIndex[j]].x;
				}
				else{
					tmp_yyy	= m_projectionVertex[objFacePtr->vertexIndex[j]].y - m_projectionVertex[objFacePtr->vertexIndex[next]].y;
					tmp_yyy	= m_projectionVertex[objFacePtr->vertexIndex[j]].x - m_projectionVertex[objFacePtr->vertexIndex[next]].x;
				}
				double tmp_xx = (tmp_yy * tmp_xxx)/tmp_yyy;
				double tmp_delta_zx = m_normal[i].x / m_normal[i].z;
				tmp_delta_zx *= (-1.0);
				double tmp_delta_zy = m_normal[i].y / m_normal[i].z;
				double tmp_delta_z = tmp_xx * tmp_delta_zx - tmp_delta_zy * tmp_yy;
				if(is_y_maxY){
					zz = m_projectionVertex[objFacePtr->vertexIndex[next]].z  - tmp_delta_z;
					edge_upper = m_projectionVertex[objFacePtr->vertexIndex[next]].x - tmp_xx;
				}
				else{
					zz = m_projectionVertex[objFacePtr->vertexIndex[j]].z  - tmp_delta_z;
					edge_upper = m_projectionVertex[objFacePtr->vertexIndex[j]].x - tmp_xx;
				}

				//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
				//t_scanLine << edge_upper <<  "  " << zz << std::endl;
				//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

				edge_min_y = 0;
			}
			//---该边的y的变化量---------------------------------------
			//---对于Y的量,一律统一取整..
			//---因此,Y的变化应该是分别取整后,再相减-------------------------
			delta_y = (int)edge_max_y - (int)edge_min_y;

			//---x的变化量
			//edge_min_x= min(m_projectionVertex[objFacePtr->vertexIndex[j]].x,m_projectionVertex[objFacePtr->vertexIndex[next]].x);
			//edge_max_x = max(m_projectionVertex[objFacePtr->vertexIndex[j]].x,m_projectionVertex[objFacePtr->vertexIndex[next]].x); 
			//delta_x = edge_max_x - edge_min_x;
			//---上端点的X值为 edge_max_x

			//---不处理 水平线,略过水平线，不存储---------------------
			if(delta_y<1e-5){	// 这里其实就是delta_y=0,
				continue;
				//delta_x = 0;
				//delta_y = 0;
			}
			else{	// 非水平线 要求出X的变化量	
				delta_x = (m_projectionVertex[objFacePtr->vertexIndex[j]].x-m_projectionVertex[objFacePtr->vertexIndex[next]].x) / 
					((int)m_projectionVertex[objFacePtr->vertexIndex[j]].y - (int)m_projectionVertex[objFacePtr->vertexIndex[next]].y);
				delta_x = (-1)*delta_x;
			}
			
			//---把边加入边Y桶---------------
			int tmp_y = (int)(edge_max_y);	// edge_max_y就是You桶值

			//---上端点X值edge_upper    X的变化量delta_x    
			//---Y的变化量delta_y   Y桶tmp_y   Z深度值zz
			m_bucket->AddEdge(edge_upper,delta_x,delta_y,i,tmp_y,zz);

			//++objFacePtr;		//---下一条边
		}
		//---上面是对多边形的每条边进行处理---------------------------

		//---重新计算法线,因为出现了旋转---------------------------------
		m_normal[i] = (m_projectionVertex[m_surface[i].vertexIndex[1]] - m_projectionVertex[m_surface[i].vertexIndex[0]])*
			(m_projectionVertex[m_surface[i].vertexIndex[2]]-m_projectionVertex[m_surface[i].vertexIndex[0]]);
		m_normal[i] /= m_normal[i].VectorLen();
	
		//---求面的颜色,根据 模型视点矩阵与原法线的量相乘 来模拟光线-------------------------------------
		double factor = m_modelview[2]*m_normal_x[i].x + m_modelview[6]*m_normal_x[i].y + m_modelview[10]*m_normal_x[i].z;
		if(factor<0)
			factor = -factor;
		Triple<unsigned char> color;
		color.x = (unsigned char)Round(94.0f*factor);
		color.y = (unsigned char)Round(225.0f*factor);
		color.z = (unsigned char)Round(30.0f*factor);
		m_color[i] = color;		// 第i个多边形的颜色光亮值

		//---把多边形放入多边形Y桶------------------------------------------
		//---多边形的Y变化量  裁剪后的max_y min_y
		if(max_y >=height)
			max_y = height-1;
		if(min_y<0)
			min_y = 0;

		delta_y = (int)max_y - (int)min_y;	// Y的变化量 要先取整后相减
		int tmp_y = (int)(max_y);	// Y桶值

		//---新法线m_normal[i]  Y的变化量delta_y  颜色明亮度color  Y桶tmp_y
		m_bucket->AddPolygon(m_normal[i],i,delta_y,color,tmp_y);
	}
	
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	//m_bucket->printBucketPloy();
	//m_bucket->printBucketEdge();
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
}


//---初使化帧缓存 Zbuffer缓冲---------------------------------
void KnightScanLineZbuffer::InitialBuffers()
{
	m_zBuffer.ResizeBuffer(m_viewport[2]+1);	// 清空Zbuffer的数值，并重新设置Zbuffer 大小
	m_frameBuffer->ResizeBuffer(m_viewport[2],m_viewport[3]);	// 清空帧缓存的值，并重新设置大小frame buffer size
	m_frameBuffer->Memset(0xA0);		// 黑色屏幕
}


//---把该多边形与扫描线的交点边对放入边活化表中-----------------------------------
//---num为扫描线  poly_num为多边形编号  para为参数 a b c
//--func 为0 表示是当遇到新的多边形时 的情形
//--func 为1 表示是当边消失时 新加入了边对的情形，该情形要把delta_ylr都减一
//--err_code 为返回的错误代码-----
bool KnightScanLineZbuffer::PutEdgeToAcitive(int num,int poly_num,Triple<double> &para,int func,int &err_code)
{
	
	//---返回与多边形相交的数组-------------------
	std::vector<Edge> edge;	// 返回的相交边表
	int ret_num = m_bucket->HaveEdge(num,poly_num,edge);
	//---如果找到的边的数量不是偶数个,则出错
	if(ret_num%2 != 0){
		AfxMessageBox("error in function: KnightScanLineZbuffer::PutEdgeToAcitive ",MB_YESNO|MB_ICONQUESTION);
		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
		//t_scanLine << "error in function: KnightScanLineZbuffer::PutEdgeToAcitive ret_num%2 != 0" <<std::endl;
		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
		err_code = 1;	// 出错代码---
		return false;
	}

	//---边表两两配对---------------------------------------
	Edge *edge_left = NULL;		// 边对的左边
	Edge *edge_right = NULL;	// 边对的右边
	
	for(int i=0;i<(int)edge.size();i+=2){	// i+=2 两两成对

		//---配置边表信息                                    
		//---判断左边和右边
		edge_left = &edge[i+1];
		edge_right = &edge[i];
		if(edge_right->num != edge_left->num){
			AfxMessageBox("error in function: KnightScanLineZbuffer::PutEdgeToAcitive ",MB_YESNO|MB_ICONQUESTION);
			//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
			//t_scanLine << "error in function: KnightScanLineZbuffer::PutEdgeToAcitive " <<std::endl;
			//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
			err_code = 2;
			return false;
		}
		if(edge_left->x + edge_left->delta_x  > edge_right->x + edge_right->delta_x){
			edge_left = &edge[i];
			edge_right = &edge[i+1];
		}
		//---edge_left一定指向左边的边

		EdgeActive edge_active;
		edge_active.xl = edge_left->x;	// xl
		edge_active.delta_xl = edge_left->delta_x;	// delta xl
		edge_active.delta_yl = edge_left->delta_y;	// delta yl
		edge_active.zl = edge_left->z;	// 左侧边交点的象素中心处多边形的深度值*****
		
		//如果平面是XOY平面
		if(para.z==0){
			edge_active.delta_zx = 0;
			edge_active.delta_zy = 0;
		}
		else{
			edge_active.delta_zx = -(para.x)/(para.z);
			edge_active.delta_zy = (para.y)/(para.z);
		}

		edge_active.xr = edge_right->x;
		edge_active.delta_yr = edge_right->delta_y;
		edge_active.delta_xr = edge_right->delta_x;
		
		edge_active.num = edge_right->num;

		//---当发现新的多边形时,加入新边的情形
		if(func==1){
			--edge_active.delta_yr;
			--edge_active.delta_yl;
		}	

		//---把该 边对 加入 活性边对表
		//---poly_num为多边形的编号  edge_active为要加入的边对
		m_activeList->AddActiveEdge(poly_num,edge_active);
	}

	err_code = 0;	// 无错
	return true;
}


//---检查多边形的Y桶，如果有新的多边形涉及该扫描线，则把它放入多边形活化表中
//---当加入多边形之后 也要把该多边形与当前扫描线相交的边对 加入到活化边对表-----------------
void KnightScanLineZbuffer::CheckPolygonBucket_PutItToActive(int num,int &err_code)
{
	//---得到多边形表
	std::vector<KnightPolygon> poly ;	// 多边形链表
	if(m_bucket->HaveBucketPoly(num,poly)==-1){
		err_code = 0;	// 没有相交的多边形,无错
		return ;
	}
	
	//---把多边形放入活化表-----------------------
	for(int i=0;i<(int)poly.size();++i){

		// 如果多边形的数据就在一条直线内,不加入活化多边形,也不画出来
		if(poly[i].delta_y == 0)
			continue;		
		m_activeList->AddActivePoly(poly[i]);
		int poly_num = poly[i].num;	// 多边形编号

		//---把该多边形与扫描线的交点边对放入边活化表
		//--num为扫描线 poly_num多边形编号 
		//--normal为法线数据 0为功能选择档 err_code为错误代码
		if(!PutEdgeToAcitive(num,poly_num,m_normal[poly_num],0,err_code)){
			return;
		}
	}	

	err_code = 0;	// 无错误
	return;
}


//**************************************************************************************************************************
//=====处理每一条扫扫描线============================================================================
//--num为扫描线 error_code为错误代码---
void KnightScanLineZbuffer::DealWithActiveEdge(int num,int &error_code)
{
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	//t_scanLine << "trace in DealWithActiveEdge\n" ;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	//---得到活化边对表---
	std::list<EdgeActive> * edge_activeList;
	edge_activeList = m_activeList->getEdgeActive();
	
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	//m_activeList->printEdge();
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	//---目前没有活化边
	if(edge_activeList->size()==0){
		error_code = 0;		// 无错
		return ;
	}
	
	//---处理每一个边对----------------------------------------------	
	double zx;
	Iterator it = edge_activeList->begin();
	int cout_tmp = (int)edge_activeList->size();	// 要处理的次数
	for(int i=0; i<cout_tmp;){

		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
		//t_scanLine << " 正在检查属于 " << (*it).num << "多边形的边" << std::endl;
		//m_activeList->printEdgeInfo((*it).num);
		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

		//---对xl<=x<=xr的像素进行处理--------------
		zx = (*it).zl;
		for(int x=(int)(*it).xl+1; x<=(int)(*it).xr; x++){
			
			zx += (*it).delta_zx;
			//---与Zbuffer的值进行对比
			if(zx < m_zBuffer[x]){		// 取最小值
				m_zBuffer[x] = zx;
				//---设置帧缓存 第num行，第x列 , 设置成第(*it).num个多边形的颜色
				*m_frameBuffer->Pixel(num,x) = m_color[(*it).num];
			}
		}
		
		//---更新边对信息
		(*it).delta_yl -= 1;
		(*it).delta_yr -= 1;

		//---边对同时消失
		if((*it).delta_yl<0 && (*it).delta_yr<0){

			//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
			//t_scanLine << "边对同时消失" << std::endl;
			//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

			//---检查该边对所属多边形是否仍应留在活化多边形表中
			int delta_y_tmp = m_activeList->getPolyDeltaY((*it).num);
			if(delta_y_tmp<=0){	// 该多边还有边未处理
				m_activeList->erasePolyActive((*it).num);	// 删除掉它
				m_activeList->eraseEdgeActive(it++);		// 从活化边表删除该边对	
			}
			else{
				//---否则的话，要新增边对
				//--num为扫描线 (*it).num为多边形编号 m_normal为法线信息 
				//-- 1为功能选择,表示要把新增边的Y变化量减一,error_code为错误代码
				if(!PutEdgeToAcitive(num,(*it).num,m_normal[(*it).num],1,error_code))
					return ;
				// 从活化边表删除该边对 it现在指向下一个要处理的边对
				m_activeList->eraseEdgeActive(it++);		
			}
			++i;		// 下一个
			continue;	// 已经处理好该边对 
		}
		else if((*it).delta_yl <0  || (*it).delta_yr<0){
		//---有其中一条边消失了-----

			//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
			//t_scanLine << "有一条边消失" << std::endl ;
			//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

			//---寻找新的边-------------------
			int poly_num = (*it).num;	// 第poly_num号多边形
			//在num----num+delta_y之间寻找新的边
			//---判断一下是左边的delta_y 还是右边的delta_y
			int delta_tmp = (*it).delta_yl;
			if((*it).delta_yl <0)
				delta_tmp = (*it).delta_yr;
	
			//---得到合适的边来配对---
			//--num为扫描线 delta_tmp为往下扫描数量 poly_num为多边形 edge_tmp为得到的新边------------
			Edge edge_tmp ;
			if(!m_bucket->getProperEdge(num,delta_tmp,poly_num,edge_tmp)){
				AfxMessageBox("error in function: KnightScanLineZbuffer::DealWithActiveEdge 无法处理凹多边形!!!",MB_YESNO|MB_ICONQUESTION);
				//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
				//t_scanLine << "error in function: KnightScanLineZbuffer::DealWithActiveEdge " <<std::endl;
				//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
				error_code = 1;	// 错误代码1
				return;	// 出错，这是不应该出现的情形!!!!!!!!!!!!!!!!!!!
			}

			//---左边消失，右边还在
			if((*it).delta_yl<0 && (*it).delta_yr>0){
				//---重新配置边对-----
				(*it).xl = edge_tmp.x;
				(*it).delta_yl = edge_tmp.delta_y -1 ;
				(*it).delta_xl = edge_tmp.delta_x;

				// 根据平面的连续性,左边消失了,新补齐的边的z值 与原来的值相等
				(*it).zl = edge_tmp.z;	
			}
			else{
				//---左边还在，右边消失
				//---重新配置边对,zl不变,因为左边没有消失-----
				(*it).xr = edge_tmp.x;
				(*it).delta_yr = edge_tmp.delta_y -1 ;
				(*it).delta_xr = edge_tmp.delta_x;
			}
		}

		//----计算下一条扫描线交点-------------更新-------
		(*it).xl = (*it).xl + (*it).delta_xl;
		(*it).xr = (*it).xr + (*it).delta_xr;
		//----是加上delta_zy,而不是减去,书上讲的有错误
		(*it).zl = (*it).zl + (*it).delta_zx * (*it).delta_xl + (*it).delta_zy;
	
		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
		//m_activeList->printEdgeInfo((*it).num);
		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

		++it;	//-----下一个边对
		++i;	// 下一个
	}
	//---上面已经处理好所有的边对信息----------------------------------------------------

	//---活化多边形中的每一个多边形的 delta_y 减1
	m_activeList->PolySubDeltaY();
	
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	//m_activeList->printEdge();
	//m_activeList->printPoly();
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	//---删除掉活性多边形中delta_y小于0的多边形-----------
	m_activeList->PolyUpdate();

	error_code = 0;	// 无错
	return;
}


//----------扫描线主算法框架--------------------------------------------------------------------------------------------------
bool KnightScanLineZbuffer::MainAlgorithm()
{
	int width = m_viewport[2];
	int height = m_viewport[3];

	//-------从上开始往下扫描--------------------------
	for(int i=height-1 ; i>0 ; --i){
		
		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
		//t_scanLine << "扫描线" << i << " *************************************************************" << std::endl;
		//m_activeList->printEdge();
		//m_activeList->printPoly();
		//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

		//---reset zbuffer-------------
		for(int j=0;j<width; ++j)	
  			m_zBuffer[j] = 1;		// 设置成最小的值
		
		int err_code = 0;

		//---检查多边形Y桶，如果有新的多边形涉及扫描线，则把它放入多边形活化表中-----
		//---如果有新的多边形加入多边形活化表，则要把该多边形在OXY平面上的投影和扫描线相交的边对加入边活化表中
		CheckPolygonBucket_PutItToActive(i, err_code);
		if(err_code!=0)
			return false;

		//---取出边对来处理------------------------------------------------
		//---如果有些边在这条扫描线处结束了，而其所在的多边形仍在多边形活化表内，则可从边的Y桶中找到该多边形在OXY平面上
		//---的投影和扫描线相交的新的边或边对，修改或加到边的活化表中去。
		DealWithActiveEdge(i,err_code);	
		if(err_code !=0 )
			return false;	//出错
	}

	return true;
}


//---图形绘制函数-------------------------------------------
void KnightScanLineZbuffer::DrawLine()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0,m_viewport[2],0.0,m_viewport[3]);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(0,0);
	glDrawPixels(m_viewport[2],m_viewport[3],GL_RGB,GL_UNSIGNED_BYTE,m_frameBuffer->Pixel(0,0));
	glPopMatrix();
}

//==============================================================================================================================
//====扫描线Zbuffer算法========================================================================================================
bool KnightScanLineZbuffer::StartAlgorithm()
{
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
//	t_scanLine.on_stream();
//	t_stream.on_stream();
	t_scanLine.setPrecision(10);
	t_stream.setPrecision(10);
	t_scanLine.off_stream();	// 关闭所有调试
	t_stream.off_stream();		// 关闭所有调试
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	//---设置透视投影，得到视口、模型、投影矩阵----
	SetScene();
	//---把模型投影到屏幕上-----------
	ProjectScene();
	//---设置好 多边形Y桶 Y桶 活化边桶 活化多边表桶
	setBucket();

	//===============开始算法==========================================
	//---设置好 帧缓存、Zbuffer
	InitialBuffers();
	//---主算法
	if(!MainAlgorithm())
		return false;	// 出错
	//---draw
	DrawLine();

	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
	//for(int i= 0;i<m_surface_num;++i)
	//	t_stream << m_normal[i].x  << " " << m_normal[i].y << " " << m_normal[i].z <<std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	return true;
}
//===============================================================================================================================


//------------------------------------------------------------------------
void KnightScanLineZbuffer::SetScene(){
	//---清空颜色缓存--------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//---得到视口
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//---透视投影
	gluPerspective(60.0, m_viewport[2] / (double)m_viewport[3], 1e-2, 1e6);
	//---得到模型变换矩阵
	glGetDoublev(GL_MODELVIEW_MATRIX,m_modelview);
	//---得到投影矩阵
	glGetDoublev(GL_PROJECTION_MATRIX,m_projection);
}

//---生成投影坐标----------------------------------------
void KnightScanLineZbuffer::ProjectScene()
{	
	// 生成投影坐标
	m_projectionVertex.resize(m_vertex_num);
	for(int i=0;i<(int)m_vertex_num;++i){
		gluProject(m_vertex[i].x, m_vertex[i].y, m_vertex[i].z, m_modelview, m_projection, m_viewport,
			&m_projectionVertex[i].x,&m_projectionVertex[i].y,&m_projectionVertex[i].z);
	}
/*
	//---重新计算法线
	for(int i=0;i<m_surface_num;++i){
		m_normal[i] = (m_projectionVertex[m_surface[i].vertexIndex[1]] - m_projectionVertex[m_surface[i].vertexIndex[0]])*
			(m_projectionVertex[m_surface[i].vertexIndex[2]]-m_projectionVertex[m_surface[i].vertexIndex[0]]);
		m_normal[i] /= m_normal[i].VectorLen();
	}
	*/
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
/*	t_stream << "888888888888888888888888888888888888888888888 " << std::endl;
	for(int i=0;i<m_surface_num;++i){
		t_stream << "face " << i << " size: " << m_surface[i].vertexIndex.size() << "*******************" << std::endl;
		for(int j=0;j<(int)m_surface[i].vertexIndex.size();++j){
			t_stream << m_surface[i].vertexIndex[j] << " " << std::endl;
			t_stream << m_projectionVertex[m_surface[i].vertexIndex[j]].x << " "
				<< m_projectionVertex[m_surface[i].vertexIndex[j]].y << " "
				<< m_projectionVertex[m_surface[i].vertexIndex[j]].z << " " << std::endl;
		}
		t_stream << " " << std::endl;
	}
*/	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
}