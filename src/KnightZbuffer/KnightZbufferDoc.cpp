// KnightZbufferDoc.cpp : CKnightZbufferDoc 类的实现
//

#include "stdafx.h"
#include "KnightZbuffer.h"

#include "KnightZbufferDoc.h"

//---
#include "KnightEngine/KnightOpenGL.h"
//---

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CKnightZbufferDoc

IMPLEMENT_DYNCREATE(CKnightZbufferDoc, CDocument)

BEGIN_MESSAGE_MAP(CKnightZbufferDoc, CDocument)
END_MESSAGE_MAP()


// CKnightZbufferDoc 构造/析构

CKnightZbufferDoc::CKnightZbufferDoc()
{
	// TODO: 在此添加一次性构造代码
	//-----因为m_mesh不是全局变量 所以必须初使化  m_loadobj是类，有构造函数，不用在这里初使化
	m_mesh = new KnightMesh;
}

CKnightZbufferDoc::~CKnightZbufferDoc()
{
	delete m_mesh;	// 删除对象,清除内存----
	m_mesh = NULL;
}

BOOL CKnightZbufferDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}

// CKnightZbufferDoc 序列化

void CKnightZbufferDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

// CKnightZbufferDoc 诊断

#ifdef _DEBUG
void CKnightZbufferDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CKnightZbufferDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CKnightZbufferDoc 命令


//=====载入模型=====
bool CKnightZbufferDoc::LoadMesh(const char * file_path,EngineMain *rmain)
{
//	t.print("trace in CKnightZbufferDoc::LoadMesh\n");

	if(!m_mesh->LoadMesh(file_path)){
		return false;
	}
	//---rmian为引擎控制，将模型的大小、中心信息记录在引擎里-----
	rmain->RegisterObjectInfo(m_mesh->getScale(),m_mesh->getCenter());
	//---利用引擎把模型放在视口中-----
	rmain->FocusObject();

	return true;
}
