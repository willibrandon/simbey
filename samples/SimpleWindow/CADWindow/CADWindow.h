#pragma once

#include "Library\Core\BaseUnknown.h"
#include "Library\Window\BaseWindow.h"
#include "Library\GraphCtrl.h"
#include "Library\CADCtrl.h"

class CCADWindow :
	public CBaseUnknown,
	public CBaseWindow,
	public IGraphClient,
	public ICADHost
{
protected:
	HINSTANCE m_hInstance;

	CGraphCtrl m_graph;
	HWND m_hwndStatus;
	RECT m_rcStatus;
	BOOL m_fFullStatus;

	VertexMap m_mapVertices;
	LineMap m_mapLines;
	PolygonMap m_mapPolygons;
	CCADCtrl* m_pCAD;

public:
	IMP_BASE_UNKNOWN

	BEGIN_UNK_MAP
		UNK_INTERFACE(IOleWindow)
		UNK_INTERFACE(IBaseWindow)
	END_UNK_MAP

	BEGIN_WM_MAP
		HANDLE_WM(WM_CREATE, OnCreate)
		HANDLE_WM(WM_PAINT, OnPaint)
		HANDLE_WM(WM_ERASEBKGND, OnEraseBackground)
		HANDLE_WM(WM_SIZE, OnSize)
		HANDLE_WM(WM_CLOSE, OnClose)
		HANDLE_ANY(m_graph.OnMessage)
	END_WM_MAP

	CCADWindow (HINSTANCE hInstance);
	virtual ~CCADWindow ();

	static HRESULT Register (HINSTANCE hInstance);
	static HRESULT Unregister (HINSTANCE hInstance);

	HRESULT Initialize (INT nWidth, INT nHeight, INT nCmdShow);

	// IGraphClient
	virtual VOID onGraphPaint (IGrapher* lpGraph);
	virtual VOID onGraphMouseMove (KEYS dwKeys, FLOAT x, FLOAT y);
	virtual VOID onGraphLBtnDown (KEYS dwKeys, FLOAT x, FLOAT y);
	virtual VOID onGraphLBtnUp (KEYS dwKeys, FLOAT x, FLOAT y);
	virtual BOOL onGraphRBtnDown (KEYS dwKeys, FLOAT x, FLOAT y);
	virtual VOID onGraphRBtnUp (KEYS dwKeys, FLOAT x, FLOAT y);
	virtual VOID onGraphLBtnDbl (KEYS dwKeys, FLOAT x, FLOAT y);
	virtual VOID onGraphRBtnDbl (KEYS dwKeys, FLOAT x, FLOAT y);
	virtual VOID onGraphViewChanged (BOOL fZoomChanged);
	virtual BOOL onGraphKeyDown (WPARAM iKey);
	virtual BOOL onGraphKeyUp (WPARAM iKey);
	virtual BOOL onGraphChar (WPARAM iKey);
	virtual BOOL onGraphWheel (SHORT sDistance, FLOAT x, FLOAT y);
	virtual HRESULT onGraphGetAcc (IAccessible** ppAccessible);

	// IGraphContainer
	virtual VOID WINAPI OnScaleChanged (FLOAT fScale);
	virtual VOID WINAPI OnGridSpacingChanged (INT iSpacing);
	virtual HRESULT WINAPI SetFocus (__in IGrapher* pGraphCtrl);
	virtual HRESULT WINAPI InvalidateContainer (__in IGrapher* pGraphCtrl);
	virtual HRESULT WINAPI ScreenToWindowless (__in IGrapher* pGraphCtrl, __inout PPOINT ppt);
	virtual BOOL WINAPI CaptureMouse (__in IGrapher* pGraphCtrl, BOOL fCapture);
	virtual VOID WINAPI DrawDib24 (HDC hdcDest, LONG x, LONG y, HDC hdcSrc, const BYTE* pcDIB24, LONG xSize, LONG ySize, LONG lPitch);

	// ICADHost
	virtual BOOL OnSplitLine (const CAD_LINE* pcLine, const FPOINT& fpSplit, ICADLine* pSplit, __deref_out ICADLine** ppNewSplit);
	virtual BOOL OnCreateLine (DWORD idPolygon, DWORD idNewLine, __deref_out ICADLine** ppLine);
	virtual BOOL OnCreatePolygon (DWORD idPolygon, __deref_out ICADPolygon** ppPolygon);

	virtual VOID OnBeginDrawing (VOID);
	virtual VOID OnDrawLineStatus (FLOAT rLength);
	virtual VOID OnEndDrawing (VOID);

protected:
	virtual HINSTANCE GetInstance (VOID);
	virtual VOID OnFinalDestroy (HWND hwnd);

	virtual HRESULT FinalizeAndShow (DWORD dwStyle, INT nCmdShow);

	VOID ConfigureStatusBar (VOID);
	VOID UpdateStatusBarMode (VOID);

	DECL_WM_HANDLER(OnCreate);
	DECL_WM_HANDLER(OnPaint);
	DECL_WM_HANDLER(OnEraseBackground);
	DECL_WM_HANDLER(OnSize);
	DECL_WM_HANDLER(OnClose);
};
