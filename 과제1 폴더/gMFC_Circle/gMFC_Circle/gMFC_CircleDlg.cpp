
// gMFC_CircleDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "gMFC_Circle.h"
#include "gMFC_CircleDlg.h"
#include "afxdialogex.h"
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CgMFCCircleDlg 대화 상자



CgMFCCircleDlg::CgMFCCircleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GMFC_CIRCLE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pointRadius = 8;  // 기본 반지름 값
	m_pointCount = 0;

	m_hasCircle = false;
	m_circleRadius = 0.0;
	m_circleThickness = 3;

	m_dragIndex = -1;

	m_isRandomRunning = false;
}


void CgMFCCircleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CgMFCCircleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BTN_RESET, &CgMFCCircleDlg::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_RANDOM, &CgMFCCircleDlg::OnBnClickedBtnRandom)   // 랜덤 이동 버튼
	ON_MESSAGE(WM_RANDOM_STEP, &CgMFCCircleDlg::OnRandomStep)              // 쓰레드 → UI 메시지
END_MESSAGE_MAP()


// CgMFCCircleDlg 메시지 처리기

BOOL CgMFCCircleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	srand((unsigned)time(nullptr)); // rand() 시드 설정

	CRadiusDlg radiusDlg;                 // 점 반지름 입력용 팝업
	radiusDlg.m_mode = CRadiusDlg::MODE_POINT;   // 점 모드
	radiusDlg.m_radius = m_pointRadius;
	
	if (radiusDlg.DoModal() == IDOK)
	{
		m_pointRadius = radiusDlg.m_radius;   // 입력값 사용
	}
	else
	{
		m_pointRadius = 8;                    // 취소 시 기본값
	}
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CgMFCCircleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CgMFCCircleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);

		// 전체 배경 흰색으로 칠하고, 우리가 정의한 장면 그리기
		DrawScene(&dc);
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CgMFCCircleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CgMFCCircleDlg::OnEraseBkgnd(CDC* pDC)
{
	// 깜빡임 줄이기용, 배경은 OnPaint에서 처리
	return TRUE;
}

void CgMFCCircleDlg::DrawScene(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);

	// 배경 흰색
	pDC->FillSolidRect(rc, RGB(255, 255, 255));

	// 점(검은 채운 원) 그리기
	for (int i = 0; i < m_pointCount; ++i)
	{
		DrawPointCircle(pDC, m_points[i], m_pointRadius);
	}

	// 세 점 원 그리기
	if (m_hasCircle)
	{
		DrawCircle(pDC, m_circleCenter, m_circleRadius, m_circleThickness);
	}
}

void CgMFCCircleDlg::DrawPointCircle(CDC* pDC, const CPoint& pt, int r)
{
	const int SEG = 36;      // 36각형으로 근사
	CPoint arr[SEG];

	double cx = pt.x;
	double cy = pt.y;
	double step = 2.0 * 3.1415926535 / SEG;

	for (int i = 0; i < SEG; ++i)
	{
		double ang = i * step;
		double x = cx + r * cos(ang);
		double y = cy + r * sin(ang);
		arr[i].x = (LONG)x;
		arr[i].y = (LONG)y;
	}

	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	CBrush brush(RGB(0, 0, 0));

	CPen* pOldPen = pDC->SelectObject(&pen);
	CBrush* pOldBrush = pDC->SelectObject(&brush);

	pDC->Polygon(arr, SEG);

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}

void CgMFCCircleDlg::OnBnClickedBtnReset()
{
	// 찍힌 점 전부 삭제
	m_pointCount = 0;

	m_hasCircle = false;
	m_circleRadius = 0.0;
	m_dragIndex = -1;

	Invalidate(FALSE);   // 화면 다시 그리기 → 점/원 사라짐
}

void CgMFCCircleDlg::DrawCircle(CDC* pDC, const CPoint& center, double radius, int thickness)
{
	const int SEG = 360;
	CPen pen(PS_SOLID, thickness, RGB(0, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&pen);
	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

	double cx = center.x;
	double cy = center.y;
	double step = 2.0 * 3.1415926535 / SEG;

	for (int i = 0; i <= SEG; ++i)
	{
		double ang = i * step;
		double x = cx + radius * cos(ang);
		double y = cy + radius * sin(ang);
		CPoint pt((LONG)x, (LONG)y);

		if (i == 0) pDC->MoveTo(pt);
		else        pDC->LineTo(pt);
	}

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}

void CgMFCCircleDlg::RecalcCircle()
{
	if (m_pointCount < 3)
	{
		m_hasCircle = false;
		return;
	}

	double x1 = m_points[0].x, y1 = m_points[0].y;
	double x2 = m_points[1].x, y2 = m_points[1].y;
	double x3 = m_points[2].x, y3 = m_points[2].y;

	double D = 2.0 * (x1 * (y2 - y3) +
		x2 * (y3 - y1) +
		x3 * (y1 - y2));

	if (fabs(D) < 1e-6)
	{
		// 세 점이 거의 일직선인 경우: 원이 정의 안 됨
		m_hasCircle = false;
		return;
	}

	double x1sq = x1 * x1 + y1 * y1;
	double x2sq = x2 * x2 + y2 * y2;
	double x3sq = x3 * x3 + y3 * y3;

	double cx = (x1sq * (y2 - y3) +
		x2sq * (y3 - y1) +
		x3sq * (y1 - y2)) / D;

	double cy = (x1sq * (x3 - x2) +
		x2sq * (x1 - x3) +
		x3sq * (x2 - x1)) / D;

	m_circleCenter.x = (LONG)(cx + 0.5);
	m_circleCenter.y = (LONG)(cy + 0.5);

	m_circleRadius = sqrt((cx - x1) * (cx - x1) + (cy - y1) * (cy - y1));

	m_hasCircle = true;
}

int CgMFCCircleDlg::HitTestPoint(const CPoint& pt)
{
	for (int i = 0; i < m_pointCount; ++i)
	{
		double dx = (double)pt.x - m_points[i].x;
		double dy = (double)pt.y - m_points[i].y;
		double d = sqrt(dx * dx + dy * dy);

		if (d <= m_pointRadius + 3)  // 살짝 여유
			return i;
	}
	return -1;
}

void CgMFCCircleDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	// 1) 기존 점 클릭인지 먼저 체크 (드래그 시작)
	int hit = HitTestPoint(point);
	if (hit >= 0)
	{
		m_dragIndex = hit;
	}
	else
	{
		// 2) 아직 3개 미만이면 새 점 추가
		if (m_pointCount < 3)
		{
			m_points[m_pointCount] = point;
			++m_pointCount;

			// 3번째 점 찍힌 타이밍에 선 두께 입력 받기 + 원 계산
			if (m_pointCount == 3)
			{
				CRadiusDlg thickDlg;
				thickDlg.m_mode = CRadiusDlg::MODE_THICKNESS;   // 두께 모드
				thickDlg.m_radius = m_circleThickness;            // 기본 두께 값(예: 3)

				if (thickDlg.DoModal() == IDOK)
					m_circleThickness = thickDlg.m_radius;

				RecalcCircle();   // 세 점 기준으로 원 중심/반지름 계산
			}
		}
		// 4번째 이후 클릭은 무시
	}

	Invalidate(FALSE);
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CgMFCCircleDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) && m_dragIndex >= 0)
	{
		// 드래그 중인 점 위치 갱신
		m_points[m_dragIndex] = point;

		// 원 다시 계산
		RecalcCircle();
		Invalidate(FALSE);
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CgMFCCircleDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture() == this)
		ReleaseCapture();

	m_dragIndex = -1;

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CgMFCCircleDlg::OnBnClickedBtnRandom()
{
	// 원이 없는 상태(점이 3개 미만 등)에서는 그냥 무시
	if (!m_hasCircle || m_pointCount < 3)
	{
		AfxMessageBox(_T("먼저 점 3개를 찍어서 원을 만든 뒤 사용하세요."));
		return;
	}

	// 이미 랜덤 이동 중이면 또 시작하지 않음
	if (m_isRandomRunning)
		return;

	m_isRandomRunning = true;

	// 워커 쓰레드 시작
	AfxBeginThread(RandomMoveThreadProc, this);
}

UINT CgMFCCircleDlg::RandomMoveThreadProc(LPVOID pParam)
{
	CgMFCCircleDlg* pDlg = (CgMFCCircleDlg*)pParam;
	if (!pDlg) return 0;

	HWND hWnd = pDlg->m_hWnd;

	for (int i = 0; i < 10; ++i) // 총 10번
	{
		if (!::IsWindow(hWnd)) break;

		// 클라이언트 영역 가져오기
		CRect rc;
		::GetClientRect(hWnd, &rc);

		// 점 3개 랜덤 위치 생성 (여백 조금 주고)
		RandomState* pState = new RandomState;

		for (int k = 0; k < 3; ++k)
		{
			int x = rc.left + 30 + (rand() % max(1, rc.Width() - 60));
			int y = rc.top + 60 + (rand() % max(1, rc.Height() - 80));
			pState->pts[k] = CPoint(x, y);
		}

		// 메인(UI) 쓰레드로 전달
		::PostMessage(hWnd, WM_RANDOM_STEP, (WPARAM)pState, 0);

		// 0.5초 대기 → 초당 2회
		::Sleep(500);
	}

	// 반복 끝
	pDlg->m_isRandomRunning = false;

	return 0;
}

LRESULT CgMFCCircleDlg::OnRandomStep(WPARAM wParam, LPARAM lParam)
{
	RandomState* pState = (RandomState*)wParam;
	if (!pState) return 0;

	// 점 좌표 업데이트
	for (int i = 0; i < 3; ++i)
	{
		m_points[i] = pState->pts[i];
	}
	m_pointCount = 3;   // 항상 3개 유지

	delete pState;      // 메모리 해제

	// 원 다시 계산 + 화면 갱신
	RecalcCircle();
	Invalidate(FALSE);

	return 0;
}