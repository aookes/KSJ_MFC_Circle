// CRadiusDlg.cpp: 구현 파일
//

#include "pch.h"
#include "gMFC_Circle.h"
#include "afxdialogex.h"
#include "CRadiusDlg.h"


// CRadiusDlg 대화 상자

IMPLEMENT_DYNAMIC(CRadiusDlg, CDialogEx)

CRadiusDlg::CRadiusDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RADIUS_DLG, pParent)
	, m_radius(8)
	, m_mode(MODE_POINT)
{

}

CRadiusDlg::~CRadiusDlg()
{
}

void CRadiusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_radius);
}


BEGIN_MESSAGE_MAP(CRadiusDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CRadiusDlg::OnOK)
END_MESSAGE_MAP()

BOOL CRadiusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 기본값을 에디트에 찍어두기
	SetDlgItemInt(IDC_EDIT_RADIUS, m_radius);

	if (CWnd* pLabel = GetDlgItem(IDC_STATIC))
	{
		if (m_mode == MODE_POINT)
		{
			pLabel->SetWindowText(_T("점 반지름을 입력하세요."));
		}
		else if (m_mode == MODE_THICKNESS)
		{
			pLabel->SetWindowText(_T("그리는 원의 두께를 입력하세요."));
		}
	}

	return TRUE;
}

void CRadiusDlg::OnOK()
{
	// 에디트 → m_radius 로 값 가져오기
	UpdateData(TRUE);

	if (m_radius <= 0)
	{
		AfxMessageBox(_T("반지름은 1 이상이어야 합니다."));
		return; // 잘못된 값이면 다이얼로그 닫지 않음
	}

	CDialogEx::OnOK(); // 정상일 때만 닫기
}

// CRadiusDlg 메시지 처리기
