// KnightZbufferView.cpp : CKnightZbufferView 类的实现
//

#include "stdafx.h"
#include "KnightZbuffer.h"

#include "KnightZbufferDoc.h"
#include "KnightZbufferView.h"

#include "KnightGlobal\KnightGlobal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CKnightZbufferView

IMPLEMENT_DYNCREATE(CKnightZbufferView, CView)

BEGIN_MESSAGE_MAP(CKnightZbufferView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_OPEN, &CKnightZbufferView::OnFileOpen)
	ON_COMMAND(ID_32773, &CKnightZbufferView::onRenderSoft)
	ON_COMMAND(ID_32775, &CKnightZbufferView::OnRenderHardDirectX)
	ON_COMMAND(ID_32774, &CKnightZbufferView::OnRenderHardOpenGL)
	ON_COMMAND(ID_32777, &CKnightZbufferView::OnantialiasingOn)
	ON_COMMAND(ID_32782, &CKnightZbufferView::OnantialiasingOff)
	ON_COMMAND(ID_32780, &CKnightZbufferView::OnRotateAutomatic)
	ON_COMMAND(ID_32781, &CKnightZbufferView::OnRotateManual)
	ON_UPDATE_COMMAND_UI(ID_32773, &CKnightZbufferView::OnUpdateRenderSoft)
	ON_UPDATE_COMMAND_UI(ID_32775, &CKnightZbufferView::OnUpdateRenderHardDirectX)
	ON_UPDATE_COMMAND_UI(ID_32774, &CKnightZbufferView::OnUpdateRenderHardOpenGL)
	ON_UPDATE_COMMAND_UI(ID_32777, &CKnightZbufferView::OnUpdateantialiasingOn)
	ON_UPDATE_COMMAND_UI(ID_32782, &CKnightZbufferView::OnUpdateantialiasingOff)
	ON_UPDATE_COMMAND_UI(ID_32780, &CKnightZbufferView::OnUpdateRotateAutomatic)
	ON_UPDATE_COMMAND_UI(ID_32781, &CKnightZbufferView::OnUpdateRotateManual)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_32783, &CKnightZbufferView::OnAutoManual)
	ON_UPDATE_COMMAND_UI(ID_32783, &CKnightZbufferView::OnUpdateAutoManual)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CKnightZbufferView 构造/析构


CKnightZbufferView::CKnightZbufferView()
{
	// TODO: 在此处添加构造代码
	m_viewengine = new EngineMain;	// 生成引擎
	m_render_mode = renderSoft;		// 软件绘制
	m_rotate_mode = rotateManualAuto;	// 自动+人工
	m_anti_mode = antialiasingOff;	// 不开反走样
	m_framecount = -1;		// 帧计数
	m_dcptr = NULL;			//
}


CKnightZbufferView::~CKnightZbufferView()
{
	if(m_viewengine){
		delete m_viewengine;	// 删除引擎
		m_viewengine = NULL;
	}
	if(m_dcptr){
		delete m_dcptr;
		m_dcptr = NULL;
	}
}


BOOL CKnightZbufferView::PreCreateWindow(CREATESTRUCT& cs)
{
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}


// CKnightZbufferView 绘制========================================================================================
void CKnightZbufferView::OnDraw(CDC* /*pDC*/)
{
	CKnightZbufferDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	m_viewengine->setColor(160.0f/255.0f,160.0f/255.0f,160.0f/255.0f);	// 设置背景颜色 
	pDoc->m_mesh->Draw(m_viewengine,m_render_mode);	// 开始绘制
	SwapBuffers(wglGetCurrentDC());	// 交换缓存

	if(m_rotate_mode==rotateManual){	// 手动也要显示出状态栏
		char info[256];
		sprintf_s(info,"vertexes: %d , faces: %d .  KnightLiao@gmail.com  http://knightliao.blogspot.com ",
			GetDocument()->m_mesh->getVertexNum(),
			GetDocument()->m_mesh->getFaceNum());
		CStatusBar* barPtr =
			(CStatusBar*)(AfxGetMainWnd()->GetDescendantWindow(AFX_IDW_STATUS_BAR));
		if(barPtr)
			barPtr->SetPaneText(0,info);
	}
}
//======================================================================================================================


// CKnightZbufferView 诊断

#ifdef _DEBUG
void CKnightZbufferView::AssertValid() const
{
	CView::AssertValid();
}

void CKnightZbufferView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKnightZbufferDoc* CKnightZbufferView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKnightZbufferDoc)));
	return (CKnightZbufferDoc*)m_pDocument;
}
#endif //_DEBUG


// CKnightZbufferView 消息处理程序
//------------- 读 OBJ 文件 函数 ------------------------------------------------------------------
void CKnightZbufferView::OnFileOpen()
{
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
//	t.print("trace in  CKnightZbufferView::OnFileOpen()\n");
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	//---支持文件类型---------
	static const char kFileTypes[] = "OBJ files (*.obj)|*.obj|All Files (*.*)|*.*||";
	//---打开文件类型---------
	static CFileDialog FOpenDlg(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST|OFN_ENABLESIZING, (LPCTSTR) kFileTypes);
	if (FOpenDlg.DoModal() == IDOK) {
		if (GetDocument()->LoadMesh((FOpenDlg.GetPathName()),m_viewengine)) {
			//MessageBox(_T("load .obj file successful!"),_T( "success"));
		}
		else{
			MessageBox(_T("Failed to load .obj file!"), _T("Error"));
		}
	}

	//---重新绘制画出---
	RedrawWindow();
}


//=====采用软件绘制==================
void CKnightZbufferView::onRenderSoft()
{
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
//	t_stream.on_stream();
//	t_stream << " in onRenderSoft() " << std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	m_render_mode = renderSoft;
	RedrawWindow();
}

//=====采用DirectX来绘制=================
void CKnightZbufferView::OnRenderHardDirectX()
{
//	m_render_mode = renderHardDirectX;

	MessageBox(_T("还没有添加DirectX引擎代码..敬请期待.."), _T(":)"));

	RedrawWindow();
}


//=====采用OpenGL来绘制=============
void CKnightZbufferView::OnRenderHardOpenGL()
{
	m_render_mode = renderHardOpenGL;
	RedrawWindow();
}


void CKnightZbufferView::OnantialiasingOn()
{
	m_anti_mode = antialiasingOn;
	m_viewengine->AntiAliasing(true);	// 打开反走样
	RedrawWindow();
}


void CKnightZbufferView::OnantialiasingOff()
{
	m_anti_mode = antialiasingOff;
	m_viewengine->AntiAliasing(false);	// 关闭反走样
	RedrawWindow();
}


void CKnightZbufferView::OnAutoManual()
{
	OnRotateAutomatic();	// 自动
	m_rotate_mode = rotateManualAuto;	// 设置模式
	RedrawWindow();
}


void CKnightZbufferView::OnRotateAutomatic()
{
//	t.print("trace in CKnightZbufferView::OnRotateAutomatic\n");

	//---10毫秒绘制一次---
	m_draw_timer = SetTimer(1,10,NULL);	
	//---1000毫秒(1秒)计算FPS并更新状态栏
	m_statistic_timer= SetTimer(2,1000,NULL);
	m_rotate_mode=rotateAutomatic;

	m_dx = (rand()-rand())*1e-5;
	m_dy = (rand()-rand())*1e-5;
	
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
//	t_stream << "M_DX: " << m_dx << " M_DY: " << m_dy << std::endl;
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

	m_framecount = -1;
	RedrawWindow();
}


void CKnightZbufferView::OnRotateManual()
{
//	t.print("trace in CKnightZbufferView::OnRotateManual\n");

	KillTimer(m_draw_timer);
	KillTimer(m_statistic_timer);
	m_rotate_mode = rotateManual;
	RedrawWindow();
}


int CKnightZbufferView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++
//	t.on();
//	t_stream.on_stream();
//	t_scanLine.on_stream();
	//+++++++++++++++++++++++++++++++++++Debug+++++++++++++++++++++++++++++++++++++++++++++

//  t.print("trace in CKnightZbufferView::OnCreate\n");

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	//---显示位置---
	m_dcptr = new CClientDC(this);
	//---初使化引擎---
	m_viewengine->InitializeEngine(*m_dcptr);
	//---默认是自动旋转+人工---
	OnAutoManual();

	return 0;
}


void CKnightZbufferView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

//	CView::OnTimer(nIDEvent);

	//---如果没有导入模型，则不要响应----------
	if(!GetDocument()->IsMeshValid())
		return ;
	switch(nIDEvent){
		case 1:	//---还在不停的绘制---
			if(m_framecount==-1)
				m_framecount =0;
			++m_framecount;
			m_viewengine->RotateObject(m_dx,m_dy);
			//---重新绘制---
			RedrawWindow();
			break;
		case 2:	{//---更新状态栏---
			static char info[256];
			if(m_framecount>=100){
				m_framecount = 100;
			}
			sprintf_s(info,"vertexes: %d , faces: %d , FPS: %d .  KnightLiao@gmail.com  http://knightliao.blogspot.com ",
				GetDocument()->m_mesh->getVertexNum(),
				GetDocument()->m_mesh->getFaceNum(),
				m_framecount);
			m_framecount = -1;
			CStatusBar* barPtr =
				(CStatusBar*)(AfxGetMainWnd()->GetDescendantWindow(AFX_IDW_STATUS_BAR));
			if(barPtr)
				barPtr->SetPaneText(0,info);
			break;
		}
		default:
			break;
	}
}


void CKnightZbufferView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	//---设置视口位置---
	m_viewengine->SetViewport(0,0,cx,cy);
	RedrawWindow();
	// TODO: Add your message handler code here
}


void CKnightZbufferView::OnDestroy()
{
	delete m_dcptr;
	m_dcptr = NULL;
	delete m_viewengine;
	m_viewengine = NULL;

	CView::OnDestroy();
}


void CKnightZbufferView::OnMouseMove(UINT nFlags, CPoint point)
{
	//---如果还没有导入模型，则不要响应该函数-----------
	if(!GetDocument()->IsMeshValid())
		return ;
	//---如果没有人工功能，则也不要响应该函数---------
	if(!(m_rotate_mode==rotateManualAuto || m_rotate_mode==rotateManual))
		return ;
	static CPoint last(-1, -1);
	if (nFlags & MK_LBUTTON) {
		if (last.x < 0 || last.y < 0) last = point;
		m_viewengine->RotateObject(point.x - last.x, point.y - last.y);
		last = point;
		RedrawWindow();
	} else {
		last.x = last.y = -1;
	}

	//CView::OnMouseMove(nFlags, point);
}


BOOL CKnightZbufferView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

//	return CView::OnEraseBkgnd(pDC);		// 该条语句一定要去掉!!!!!
	return true;
}


BOOL CKnightZbufferView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//	return CView::OnMouseWheel(nFlags, zDelta, pt);
	
	//---如果没有导入模型，则不要响应该函数-------------
	if(!GetDocument()->IsMeshValid())
		return false;
	m_viewengine->ZoomObject(zDelta);
	RedrawWindow();
	return true;
}


//-------------------------------------------------------------------

void CKnightZbufferView::OnUpdateRenderSoft(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_render_mode==renderSoft);
}

void CKnightZbufferView::OnUpdateRenderHardDirectX(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_render_mode==renderHardDirectX);
}

void CKnightZbufferView::OnUpdateRenderHardOpenGL(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_render_mode==renderHardOpenGL);
}

void CKnightZbufferView::OnUpdateantialiasingOn(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_anti_mode==antialiasingOn);
}

void CKnightZbufferView::OnUpdateantialiasingOff(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_anti_mode==antialiasingOff);
}

void CKnightZbufferView::OnUpdateRotateAutomatic(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_rotate_mode==rotateAutomatic);
}

void CKnightZbufferView::OnUpdateRotateManual(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_rotate_mode==rotateManual);
}

void CKnightZbufferView::OnUpdateAutoManual(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_rotate_mode==rotateManualAuto);
}

//-------------------------------------------------------------
// CKnightZbufferView 打印

BOOL CKnightZbufferView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CKnightZbufferView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CKnightZbufferView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清除过程
}



