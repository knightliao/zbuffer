// KnightZbufferDoc.h : CKnightZbufferDoc 类的接口
//


#pragma once

#include "KnightMesh/KnightMesh.h"

class CKnightZbufferDoc : public CDocument
{
protected: // 仅从序列化创建
	CKnightZbufferDoc();
	DECLARE_DYNCREATE(CKnightZbufferDoc)

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CKnightZbufferDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()



public:
	//=====载入模型=====
	bool LoadMesh(const char * file_path,EngineMain *rmain);
	bool IsMeshValid(){return m_mesh->IsValid();}
public:
	//=====模型=====
	KnightMesh *m_mesh;
};


