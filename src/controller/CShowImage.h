#pragma once
#include "afxdialogex.h"


// CShowImage dialog

class CShowImage : public CDialogEx
{
	DECLARE_DYNAMIC(CShowImage)

public:
	CShowImage(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CShowImage();

// Dialog Data
	enum { IDD = IDD_SHOW_IMAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
