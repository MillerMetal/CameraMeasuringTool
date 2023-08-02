// CShowImage.cpp : implementation file
//

#include "pch.h"
//#include "SampleMFCOpenCV.h"
//#include "afxdialogex.h"
#include "CShowImage.h"


// CShowImage dialog

IMPLEMENT_DYNAMIC(CShowImage, CDialogEx)

CShowImage::CShowImage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SHOW_IMAGE, pParent)
{

}

CShowImage::~CShowImage()
{
}

void CShowImage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShowImage, CDialogEx)
END_MESSAGE_MAP()


// CShowImage message handlers
