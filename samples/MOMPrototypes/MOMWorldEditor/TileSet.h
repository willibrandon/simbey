#pragma once

#include "Library\Core\RStrMap.h"
#include "Published\SIF.h"

interface IJSONValue;
interface IJSONObject;
interface IJSONArray;

class CSmoothingSystem;
class CTileRules;
class CTileSet;

class CTile
{
private:
	CTileSet* m_pTileSet;
	RSTRING m_rstrKey;
	union
	{
		ISimbeyInterchangeSprite* m_pSprite;
		ISimbeyInterchangeAnimator* m_pAnimator;
	};
	bool m_fIsSprite;

public:
	CTile (CTileSet* pTileSet, RSTRING rstrKey, ISimbeyInterchangeSprite* pSprite);
	CTile (CTileSet* pTileSet, RSTRING rstrKey, ISimbeyInterchangeAnimator* pAnimator);
	~CTile ();

	inline CTileSet* GetTileSet (VOID) { return m_pTileSet; }
	inline RSTRING GetKey (VOID) { return m_rstrKey; }

	HRESULT CreateSprite (__deref_out ISimbeyInterchangeSprite** ppSprite);
};

class CTileSet
{
public:
	RSTRING m_rstrName;
	TRStrMap<TArray<CTile*>*> m_mapTiles;

public:
	CTileSet (RSTRING rstrName);
	~CTileSet ();

	inline RSTRING GetName (VOID) { return m_rstrName; }

	HRESULT AddVariant (RSTRING rstrKey, ISimbeyInterchangeSprite* pSprite);
	HRESULT AddVariant (RSTRING rstrKey, ISimbeyInterchangeAnimator* pAnimator);

	HRESULT FindFromKey (RSTRING rstrKey, __deref_out TArray<CTile*>** ppTiles);

private:
	HRESULT EnsureTilesArray (RSTRING rstrKey, __deref_out TArray<CTile*>** ppTiles);
};

struct MAPTILE
{
	CTile* pTile;
	RSTRING rstrFeature;
	IJSONObject* pData;
	IJSONObject* pCity;
	IJSONObject* pStack;
};

class CPlaceItem
{
public:
	CTileRules* m_pTileRules;

	RSTRING m_rstrTile;
	WCHAR m_wzKey[12];
	CTile* m_pTile;

	INT m_xTile;
	INT m_x, m_y;	// Adjusted tile coordinates

public:
	static HRESULT CreatePlaceItem (CTileRules* pTileRules, MAPTILE* pWorld, INT xWorld, INT yWorld, INT x, INT y, __deref_out CPlaceItem** ppItem);

public:
	CPlaceItem (CTileRules* pTileRules, CTile* pTile, INT xTile, INT x, INT y);
	~CPlaceItem ();

	bool IsSameTile (RSTRING rstrTile);
	bool IsBorderTile (RSTRING rstrTile);
	bool IsSpecialTile (RSTRING rstrTile);

	VOID SetTileOnly (RSTRING rstrTile);
	HRESULT GetTransitionTile (__in_opt RSTRING rstrTile, __out RSTRING* prstrTile);
	HRESULT SetNewKey (TRStrMap<CTileSet*>* pmapTileSets, PCWSTR pcwzKey);

private:
	HRESULT SmoothTile (CTileSet* pTileSet, PWSTR pwzKey, TArray<CTile*>** ppTiles);
	HRESULT SetAltTile (TRStrMap<CTileSet*>* pmapTileSets, PCWSTR pcwzKey);
};

class CMapPainter
{
private:
	CTileRules* m_pTileRules;
	MAPTILE* m_pWorld;
	INT m_xWorld, m_yWorld;
	TArray<CPlaceItem*> m_aAffected;

public:
	CMapPainter (CTileRules* pTileRules, MAPTILE* pWorld, INT xWorld, INT yWorld);
	~CMapPainter ();

	HRESULT PaintTile (INT xTile, INT yTile, RSTRING rstrTile);
	HRESULT CheckTransitions (INT xTile, INT yTile, RSTRING rstrTile);
	HRESULT Commit (TRStrMap<CTileSet*>* pmapTileSets, __out_opt TArray<POINT>* paTilesChanged);

private:
	VOID GetTileKey (MAPTILE* pWorld, CPlaceItem* pItem, TArray<CPlaceItem*>& aItems, PWSTR pwzKey);

	static CPlaceItem* FindItem (TArray<CPlaceItem*>& aItems, INT xTile, INT yTile);
};
