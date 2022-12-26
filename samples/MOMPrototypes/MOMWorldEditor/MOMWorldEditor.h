#pragma once

#include <SIFRibbon.h>
#include <SIFRibbonItem.h>
#include "Library\Core\BaseUnknown.h"
#include "Library\Core\RStrMap.h"
#include "Library\Window\BaseWindow.h"
#include "Library\MIDIPlayer.h"
#include "Package\SIFPackage.h"
#include "..\Shared\InteractiveSurface.h"
#include "Dir.h"
#include "TileSet.h"

interface IJSONValue;
interface IJSONObject;
interface IJSONArray;

class CSmoothingSystem;
class CTileRules;

class CTerrainGallery :
	public CBaseUnknown,
	public IUICollection
{
private:
	CSIFRibbon* m_pRibbon;
	ISimbeyInterchangeFile* m_pSIF;
	UINT m_idxSelection;

public:
	IMP_BASE_UNKNOWN

	BEGIN_UNK_MAP
		UNK_INTERFACE(IUICollection)
	END_UNK_MAP

public:
	CTerrainGallery (CSIFRibbon* pRibbon, ISimbeyInterchangeFile* pSIF);
	~CTerrainGallery ();

	HRESULT GetSelectedTile (__out RSTRING* prstrTile);
	UINT GetSelection (VOID);
	VOID SetSelection (UINT idxSelection);

	// IUICollection
	virtual HRESULT STDMETHODCALLTYPE GetCount (__out UINT32* pcItems);
	virtual HRESULT STDMETHODCALLTYPE GetItem (UINT32 index, __deref_out_opt IUnknown** item);
	virtual HRESULT STDMETHODCALLTYPE Add (IUnknown* item);
	virtual HRESULT STDMETHODCALLTYPE Insert (UINT32 index, IUnknown* item);
	virtual HRESULT STDMETHODCALLTYPE RemoveAt (UINT32 index);
	virtual HRESULT STDMETHODCALLTYPE Replace (UINT32 indexReplaced, IUnknown* itemReplaceWith);
	virtual HRESULT STDMETHODCALLTYPE Clear (VOID);
};

namespace World
{
	enum Type
	{
		Arcanus,
		Myrror
	};
}

struct CITYTILE
{
	ISimbeyInterchangeSprite* pNormal[17];
	ISimbeyInterchangeSprite* pWalled[17];
};

class CBaseGalleryCommand
{
public:
	CTerrainGallery* m_pGallery;
	MAPTILE* m_pWorld;
	INT m_xWorld, m_yWorld;

public:
	CBaseGalleryCommand (CTerrainGallery* pGallery, MAPTILE* pWorld, INT xWorld, INT yWorld) :
		m_pGallery(pGallery),
		m_pWorld(pWorld),
		m_xWorld(xWorld), m_yWorld(yWorld)
	{
		m_pGallery->AddRef();
	}

	virtual ~CBaseGalleryCommand ()
	{
		m_pGallery->Release();
	}

	virtual HRESULT Execute (class CMOMWorldEditor* pEditor, INT xTile, INT yTile) = 0;
	virtual BOOL ContinuePainting (VOID) { return TRUE; }
};

class CTerrainCommand : public CBaseGalleryCommand
{
public:
	TRStrMap<CTileSet*>* m_pmapTileSets;

public:
	CTerrainCommand (CTerrainGallery* pGallery, MAPTILE* pWorld, INT xWorld, INT yWorld, TRStrMap<CTileSet*>* pmapTileSets) :
		CBaseGalleryCommand(pGallery, pWorld, xWorld, yWorld),
		m_pmapTileSets(pmapTileSets)
	{
	}

	virtual HRESULT Execute (class CMOMWorldEditor* pEditor, INT xTile, INT yTile);
};

class CFeaturesCommand : public CBaseGalleryCommand
{
public:
	CFeaturesCommand (CTerrainGallery* pGallery, MAPTILE* pWorld, INT xWorld, INT yWorld) :
		CBaseGalleryCommand(pGallery, pWorld, xWorld, yWorld)
	{
	}

	virtual HRESULT Execute (class CMOMWorldEditor* pEditor, INT xTile, INT yTile);
};

class CClearFeatureCommand : public CBaseGalleryCommand
{
public:
	CClearFeatureCommand (CTerrainGallery* pGallery, MAPTILE* pWorld, INT xWorld, INT yWorld) :
		CBaseGalleryCommand(pGallery, pWorld, xWorld, yWorld)
	{
	}

	virtual HRESULT Execute (class CMOMWorldEditor* pEditor, INT xTile, INT yTile);
};

class CPlaceCityCommand : public CBaseGalleryCommand
{
public:
	CPlaceCityCommand (CTerrainGallery* pGallery, MAPTILE* pWorld, INT xWorld, INT yWorld) :
		CBaseGalleryCommand(pGallery, pWorld, xWorld, yWorld)
	{
	}

	virtual HRESULT Execute (class CMOMWorldEditor* pEditor, INT xTile, INT yTile);
	virtual BOOL ContinuePainting (VOID) { return FALSE; }
};

class CMOMWorldEditor :
	public CBaseUnknown,
	public CBaseWindow,
	public IRibbonHost,
	public IUICommandHandler,
	public MIDI::INotifyFinished,
	public ILayerInputHandler
{
protected:
	HINSTANCE m_hInstance;

	CSIFRibbon* m_pRibbon;
	CSIFPackage* m_pPackage;
	CInteractiveSurface* m_pSurface;

	MIDI::CPlayer m_player;
	BOOL m_fActive;

	bool m_fKeys[256];

	TRStrMap<CSmoothingSystem*> m_mapSmoothingSystems;
	CTileRules* m_pTileRules;

	TRStrMap<CTileSet*> m_mapArcanus, m_mapMyrror;
	CTerrainGallery* m_pArcanusTerrain;
	CTerrainGallery* m_pMyrrorTerrain;

	TRStrMap<ISimbeyInterchangeSprite*> m_mapFeatures;
	CTerrainGallery* m_pFeatures;
	TArray<CITYTILE> m_aCityTiles;

	INT m_xWorld, m_yWorld;
	MAPTILE* m_pArcanusWorld;
	MAPTILE* m_pMyrrorWorld;
	World::Type m_eType;

	CSIFCanvas* m_pMain;
	CInteractiveLayer* m_pMapTileLayer;
	sysint m_nFeaturesLayer;
	sysint m_nCitiesLayer;
	ISimbeyInterchangeSprite* m_pMouse;

	INT m_xDrag, m_yDrag;
	BOOL m_fDragging;
	BOOL m_fPainting;

	CBaseGalleryCommand* m_pCommand;

public:
	IMP_BASE_UNKNOWN

	BEGIN_UNK_MAP
		UNK_INTERFACE(IOleWindow)
		UNK_INTERFACE(IBaseWindow)
		UNK_INTERFACE(IRibbonHost)
		UNK_INTERFACE(IUIApplication)
		UNK_INTERFACE(IUICommandHandler)
		UNK_INTERFACE(ILayerInputHandler)
	END_UNK_MAP

	BEGIN_WM_MAP
		HANDLE_WM(WM_CREATE, OnCreate)
		HANDLE_WM(WM_PAINT, OnPaint)
		HANDLE_WM(WM_ERASEBKGND, OnEraseBackground)
		HANDLE_WM(WM_SIZE, OnSize)
		HANDLE_WM(WM_CLOSE, OnClose)
		HANDLE_WM(WM_ACTIVATE, OnActivate)
		HANDLE_WM(WM_MOUSEMOVE, m_pSurface->ProcessLayerInput)
		HANDLE_WM(WM_LBUTTONDOWN, m_pSurface->ProcessLayerInput)
		HANDLE_WM(WM_LBUTTONUP, m_pSurface->ProcessLayerInput)
		HANDLE_WM(WM_RBUTTONDOWN, m_pSurface->ProcessLayerInput)
		HANDLE_WM(WM_RBUTTONUP, m_pSurface->ProcessLayerInput)
		HANDLE_WM(WM_KEYDOWN, OnKeyDown)
		HANDLE_WM(WM_KEYUP, OnKeyUp)
		HANDLE_WM(WM_SETCURSOR, OnSetCursor)
		HANDLE_WM(WM_DESTROY, OnDestroy)
	END_WM_MAP

	CMOMWorldEditor (HINSTANCE hInstance);
	virtual ~CMOMWorldEditor ();

	static HRESULT Register (HINSTANCE hInstance);
	static HRESULT Unregister (HINSTANCE hInstance);

	HRESULT Initialize (INT nWidth, INT nHeight, INT nCmdShow);
	VOID Run (VOID);

	// IRibbonHost
	virtual HRESULT WINAPI GetRibbonSettingsKey (__out_ecount(cchMaxKey) PSTR pszKeyName, INT cchMaxKey);
	virtual HRESULT WINAPI GetRibbonSettingsValue (__out_ecount(cchMaxValue) PSTR pszValueName, INT cchMaxValue);
	virtual HRESULT WINAPI GetRibbonResource (__out HMODULE* phModule, __out_ecount(cchMaxResource) PWSTR pwzResource, INT cchMaxResource);
	virtual UINT32 WINAPI TranslateGroupToImage (UINT32 nID);
	virtual UINT32 WINAPI TranslateImageToLargeImage (UINT32 nID);

	// IUIApplication
	virtual HRESULT STDMETHODCALLTYPE OnViewChanged (UINT32 viewId, UI_VIEWTYPE typeID, IUnknown* view, UI_VIEWVERB verb, INT32 uReasonCode);
	virtual HRESULT STDMETHODCALLTYPE OnCreateUICommand (UINT32 commandId, UI_COMMANDTYPE typeID, IUICommandHandler** commandHandler);
	virtual HRESULT STDMETHODCALLTYPE OnDestroyUICommand (UINT32 commandId, UI_COMMANDTYPE typeID, IUICommandHandler* commandHandler);

	// IUICommandHandler
	virtual HRESULT STDMETHODCALLTYPE Execute (UINT32 commandId, UI_EXECUTIONVERB verb, const PROPERTYKEY* key, const PROPVARIANT* currentValue, IUISimplePropertySet* commandExecutionProperties);
	virtual HRESULT STDMETHODCALLTYPE UpdateProperty (UINT32 commandId, REFPROPERTYKEY key, const PROPVARIANT* currentValue, PROPVARIANT* newValue);

protected:
	virtual HINSTANCE GetInstance (VOID);
	virtual VOID OnFinalDestroy (HWND hwnd);

	virtual HRESULT FinalizeAndShow (DWORD dwStyle, INT nCmdShow);

	DECL_WM_HANDLER(OnCreate);
	DECL_WM_HANDLER(OnPaint);
	DECL_WM_HANDLER(OnEraseBackground);
	DECL_WM_HANDLER(OnSize);
	DECL_WM_HANDLER(OnClose);
	DECL_WM_HANDLER(OnActivate);
	DECL_WM_HANDLER(OnKeyDown);
	DECL_WM_HANDLER(OnKeyUp);
	DECL_WM_HANDLER(OnSetCursor);
	DECL_WM_HANDLER(OnDestroy);

	// INotifyFinished
	virtual VOID OnNotifyFinished (MIDI::CPlayer* pPlayer, BOOL fCompleted);

	// ILayerInputHandler
	virtual BOOL ProcessMouseInput (LayerInput::Mouse eType, WPARAM wParam, LPARAM lParam, INT xView, INT yView, LRESULT& lResult);
	virtual BOOL ProcessKeyboardInput (LayerInput::Keyboard eType, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

	VOID OnUpdateFrame (VOID);

	HRESULT OpenSaveMap (UINT32 commandId);
	HRESULT LoadFrom (PCWSTR pcwzFile);
	HRESULT SaveTo (PCWSTR pcwzFile);
	HRESULT PromptForNewMap (VOID);
	HRESULT GenerateRandomWorlds (VOID);

	VOID DeleteWorld (MAPTILE*& pWorld);
	HRESULT ResetWorldTiles (MAPTILE* pWorld, INT xWorld, INT yWorld, TRStrMap<CTileSet*>& mapTiles);

	HRESULT UpdateVisibleTiles (VOID);
	VOID Scroll (INT x, INT y);

	HRESULT SetupMap (INT xWorld, INT yWorld);
	HRESULT LoadWorldFromJSON (TRStrMap<CTileSet*>& mapTileSet, IJSONObject* pMap, PCWSTR pcwzWorld, INT xWorld, INT yWorld, MAPTILE** ppWorld);
	HRESULT SaveWorldToJSON (IJSONObject* pMap, PCWSTR pcwzWorld, INT xWorld, INT yWorld, MAPTILE* pWorld);

	HRESULT ReplaceCommand (CBaseGalleryCommand* pCommand);

public:
	VOID ClearTileData (MAPTILE* pTile);
	HRESULT ClearTile (INT x, INT y, BOOL fActiveWorld);
	HRESULT PlaceSelectedTile (INT x, INT y);
	HRESULT PlaceTile (MAPTILE* pWorld, INT xTile, INT yTile, TRStrMap<CTileSet*>* pmapTileSets, RSTRING rstrTile, BOOL fActiveWorld);

	HRESULT PlaceOrModifyCity (MAPTILE* pWorld, INT xTile, INT yTile);

protected:
	HRESULT LoadPackage (VOID);
	HRESULT LoadSmoothing (IJSONObject* pSmoothing);

	HRESULT LoadFeatures (ISimbeyInterchangeFile* pFeatures);
	HRESULT LoadCityTiles (ISimbeyInterchangeFile* pCityTiles);

	HRESULT LoadTerrain (VOID);
	HRESULT CreateGallery (TRStrMap<CTileSet*>& mapTiles, CTerrainGallery** ppGallery);
	HRESULT SetupMouse (VOID);

	static HRESULT ColorizeLayer (ISimbeyInterchangeFileLayer* pLayer, COLORREF crColorize, ISimbeyInterchangeFile* pStorage, __deref_out ISimbeyInterchangeFileLayer** ppColorized);

	static HRESULT LoadTileSets (CSIFPackage* pPackage, __out TRStrMap<CTileSet*>& mapTiles);
	static HRESULT LoadTileSet (CSIFPackage* pPackage, RSTRING rstrName, __out TRStrMap<CTileSet*>& mapTiles);
	static HRESULT LoadKeyVariants (ISimbeyInterchangeFile* pSIF, CTileSet* pTileSet, RSTRING rstrKey, IJSONArray* pVariants);
};
