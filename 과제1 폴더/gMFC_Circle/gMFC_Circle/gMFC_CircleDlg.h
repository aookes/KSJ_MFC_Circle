
// gMFC_CircleDlg.h: 헤더 파일
//

#pragma once
#include "CRadiusDlg.h"
#define WM_RANDOM_STEP   (WM_APP + 1)


// CgMFCCircleDlg 대화 상자
class CgMFCCircleDlg : public CDialogEx
{
// 생성입니다.
public:
	CgMFCCircleDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GMFC_CIRCLE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnReset();

	// 랜덤 이동 버튼 + 메시지 핸들러
	afx_msg void OnBnClickedBtnRandom();
	afx_msg LRESULT OnRandomStep(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	int   m_pointRadius;   // 점(검은 원) 반지름
	CPoint m_points[3];    // 점 위치
	int    m_pointCount;   // 찍힌 점 개수 (0~3)

	// 원 관련
	bool   m_hasCircle;       // 세 점 원 존재 여부
	CPoint m_circleCenter;    // 원 중심 좌표
	double m_circleRadius;    // 원 반지름 (픽셀)
	int    m_circleThickness; // 원 선 두께

	// 드래그 관련
	int    m_dragIndex;       // 드래그 중인 점 인덱스 (-1이면 없음)

	// 랜덤 이동 상태 플래그
	bool   m_isRandomRunning;

	// 헬퍼 함수
private:
	void DrawScene(CDC* pDC);
	void DrawPointCircle(CDC* pDC, const CPoint& pt, int r);
	void DrawCircle(CDC* pDC, const CPoint& center, double radius, int thickness);
	void RecalcCircle();                 // 세 점으로 원 계산
	int  HitTestPoint(const CPoint& pt); // 클릭한 점 인덱스 찾기
	
	struct RandomState					// 랜덤 이동용 구조체 + 쓰레드 함수
	{
		CPoint pts[3];
	};

	static UINT RandomMoveThreadProc(LPVOID pParam);
};
