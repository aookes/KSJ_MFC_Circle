#pragma once
#include "afxdialogex.h"


// CRadiusDlg 대화 상자

class CRadiusDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRadiusDlg)

public:
	CRadiusDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CRadiusDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RADIUS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();    // ★ 다이얼로그 초기화
	afx_msg void OnOK();            // ★ OK 버튼 눌렀을 때 처리

	DECLARE_MESSAGE_MAP()

public:
	int m_radius;   // ★ 우리가 사용할 반지름 값

	// 다이얼로그 점용 / 두께용 구분 플래그
	enum { MODE_POINT = 0, MODE_THICKNESS = 1 };
	int m_mode;     // 기본값은 생성자에서 점(MODE_POINT)으로 설정
};
