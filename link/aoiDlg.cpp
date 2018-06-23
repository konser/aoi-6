
// aoi_testDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "aoiApp.h"
#include "aoiDlg.h"
#include "afxdialogex.h"

#include <io.h>    
#include <fcntl.h>  
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAoiDlg �Ի���



CAoiDlg::CAoiDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAoiDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAoiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAoiDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CAoiDlg ��Ϣ��������




BOOL CAoiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵������ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ����Ӷ���ĳ�ʼ������

	AllocConsole();
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle, _O_TEXT);
	FILE * hf = _fdopen(hCrt, "w");
	*stdout = *hf;

	m_aoi_ctx = create_aoi_ctx();
	m_countor = 1;
	m_timer_countor = 0;

	GetWindowRect(&m_rt);


	for (int i = 0; i < 50;i++)
	{
		TriggerCtx* ctx = new TriggerCtx();
		ctx->pos = CPoint(rand() % m_rt.right, rand() % m_rt.bottom);
		//ctx->pos = CPoint(m_rt.right / 2, m_rt.bottom /2);
		ctx->dest = CPoint(rand() % m_rt.right, rand() % m_rt.bottom);
		ctx->range = rand() % 30 + 20;
		//ctx->range = 200;
		ctx->trigger = CreateTrigger(ctx->pos, ctx->range);
		m_trigger_list.push_back(ctx);
	}

	for ( int i = 0; i < 500; i++ )
	{
		EntityCtx* ctx = new EntityCtx();
		ctx->pos = CPoint(rand() % m_rt.right, rand() % m_rt.bottom);
		ctx->dest = CPoint(rand() % m_rt.right, rand() % m_rt.bottom);
		ctx->entity = CreateEntity(ctx->pos);
		m_entity_list.push_back(ctx);
	}
	

	SetTimer(1, 50, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CAoiDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
void foreach_entity_callback(int uid, int x, int z, void* ud) {
	CAoiDlg* pDlg = (CAoiDlg*)ud;

	CClientDC dc(pDlg);

	std::map<int, bool>::iterator it = pDlg->m_entity_status.find(uid);
	if ( it != pDlg->m_entity_status.end() )
	{
		bool val = it->second;

		CBrush brush0(RGB(0, 255, 0));
		CBrush brush1(RGB(255, 0, 0));

		if ( val )
			dc.SelectObject(&brush0);
		else
			dc.SelectObject(&brush1);

		dc.Ellipse(x - 5, z - 5, x + 5, z + 5);
	}
	else{
		CBrush brush0(RGB(255, 0, 0));
		dc.SelectObject(&brush0);
		dc.Ellipse(x - 5, z - 5, x + 5, z + 5);
	}
}

void foreach_trigger_callback(int uid, int x, int z, int range, void* ud) {
	CClientDC* dc = (CClientDC*)ud;
	dc->Ellipse(x - 5, z - 5, x + 5, z + 5);

	dc->MoveTo(x - range, z - range);
	dc->LineTo(x + range, z - range);

	dc->MoveTo(x - range, z + range);
	dc->LineTo(x + range, z + range);

	dc->MoveTo(x - range, z - range);
	dc->LineTo(x - range, z + range);

	dc->MoveTo(x + range, z - range);
	dc->LineTo(x + range, z + range);
}

// �����Ի���������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAoiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

	CClientDC dc(this);

	CBrush brush0(RGB(255, 0, 0));
	dc.SelectObject(&brush0);
	foreach_aoi_entity(m_aoi_ctx, foreach_entity_callback, this);

	CBrush brush1(RGB(0, 0, 0));
	dc.SelectObject(&brush1);
	foreach_aoi_trigger(m_aoi_ctx, foreach_trigger_callback, &dc);
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAoiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void OnEntityEnter(int self, int other, void* ud) {
	//printf("entity:%d enter:%d\n", self, other);
	CAoiDlg* pDlg = (CAoiDlg*)ud;
	pDlg->m_trigger_status[other] = true;
	pDlg->m_entity_status[self] = true;
}

void OnEntityLeave(int self, int other, void* ud) {
	//printf("entity:%d leave:%d\n", self, other);
	CAoiDlg* pDlg = (CAoiDlg*)ud;
	pDlg->m_trigger_status[other] = false;
	pDlg->m_entity_status[self] = false;
}

void OnTriggerEnter(int self, int other, void* ud) {
	CAoiDlg* pDlg = (CAoiDlg*)ud;
	//printf("trigger:%d enter:%d\n", self, other);
	pDlg->m_entity_status[other] = true;
}

void OnTriggerLeave(int self, int other, void* ud) {
	CAoiDlg* pDlg = (CAoiDlg*)ud;
	//printf("trigger:%d leave:%d\n", self, other);
	pDlg->m_entity_status[other] = false;
}

struct aoi_object* CAoiDlg::CreateEntity(CPoint& point)
{
	int id = m_countor++;
	struct aoi_object* object = create_aoi_object(m_aoi_ctx, id);
	create_entity(m_aoi_ctx, object, point.x, point.y, OnEntityEnter, OnEntityLeave, ( void* )this);
	m_map[id] = object;
	return object;
}

struct aoi_object* CAoiDlg::CreateTrigger(CPoint& point,int range)
{
	int id = m_countor++;
	struct aoi_object* object = create_aoi_object(m_aoi_ctx, id);
	create_trigger(m_aoi_ctx, object, point.x, point.y, range, OnTriggerEnter, OnTriggerLeave, ( void* )this);
	m_map[id] = object;
	return object;
}

void CAoiDlg::UpdateTrigger()
{
	for (size_t i = 0; i < m_trigger_list.size();i++)
	{
		TriggerCtx* ctx = m_trigger_list[i];
		float dt = sqrt((ctx->dest.x - ctx->pos.x) * (ctx->dest.x - ctx->pos.x) + (ctx->dest.y - ctx->pos.y) * (ctx->dest.y - ctx->pos.y));
		if (dt <= 5)
		{
			ctx->dest = CPoint(rand() % m_rt.right, rand() % m_rt.bottom);
		}
		else {
			RECT rt;
			rt.left = ctx->pos.x - ctx->range - 10;
			rt.top = ctx->pos.y - ctx->range - 10;

			float vt = 50;
			float ratio = (vt * 0.1f) / dt;
			ctx->pos.x = ctx->pos.x + (ctx->dest.x - ctx->pos.x) * ratio;
			ctx->pos.y = ctx->pos.y + (ctx->dest.y - ctx->pos.y) * ratio;

			rt.right = ctx->pos.x + ctx->range + 10;
			rt.bottom = ctx->pos.y + ctx->range + 10;

			InvalidateRect(&rt);

			move_trigger(m_aoi_ctx, ctx->trigger, ctx->pos.x, ctx->pos.y, ( void* )this);
		}
	}
}

void CAoiDlg::UpdateEntity()
{
	for ( size_t i = 0; i < m_entity_list.size(); i++ )
	{
		EntityCtx* ctx = m_entity_list[i];
		float dt = sqrt(( ctx->dest.x - ctx->pos.x ) * ( ctx->dest.x - ctx->pos.x ) + ( ctx->dest.y - ctx->pos.y ) * ( ctx->dest.y - ctx->pos.y ));
		if ( dt <= 5 )
		{
			ctx->dest = CPoint(rand() % m_rt.right, rand() % m_rt.bottom);
		}
		else {
			RECT rt;
			rt.left = ctx->pos.x  - 10;
			rt.top = ctx->pos.y - 10;

			float vt = 50;
			float ratio = ( vt * 0.1f ) / dt;
			ctx->pos.x = ctx->pos.x + ( ctx->dest.x - ctx->pos.x ) * ratio;
			ctx->pos.y = ctx->pos.y + ( ctx->dest.y - ctx->pos.y ) * ratio;

			rt.right = ctx->pos.x + 10;
			rt.bottom = ctx->pos.y + 10;

			InvalidateRect(&rt);

			move_entity(m_aoi_ctx, ctx->entity, ctx->pos.x, ctx->pos.y, ( void* )this);
		}
	}
}

void CAoiDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ�������Ϣ������������/�����Ĭ��ֵ

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CAoiDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ�������Ϣ������������/�����Ĭ��ֵ
	CDialogEx::OnRButtonUp(nFlags, point);
	TriggerCtx* ctx = m_trigger_list[0];
	move_trigger(m_aoi_ctx, ctx->trigger, point.x, point.y, ( void* )this);
	Invalidate();
}


void CAoiDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ�������Ϣ������������/�����Ĭ��ֵ

	CDialogEx::OnTimer(nIDEvent);
	
	UpdateTrigger();
	UpdateEntity();
}