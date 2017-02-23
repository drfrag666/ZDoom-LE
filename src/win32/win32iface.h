/*
** win32iface.h
**
**---------------------------------------------------------------------------
** Copyright 1998-2008 Randy Heit
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/

#ifndef __WIN32IFACE_H
#define __WIN32IFACE_H

#ifndef DIRECTDRAW_VERSION
#define DIRECTDRAW_VERSION 0x0300
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ddraw.h>

#include "hardware.h"

#define SAFE_RELEASE(x)		{ if (x != NULL) { x->Release(); x = NULL; } }

EXTERN_CVAR (Bool, vid_vsync)

extern HANDLE FPSLimitEvent;

struct FSoftwareRenderer;

class Win32Video : public IVideo
{
 public:
	Win32Video (int parm);
	~Win32Video ();

	void InitDDraw();

	EDisplayType GetDisplayType () { return DISPLAY_Both; }
	void SetWindowedScale (float scale);

	DFrameBuffer *CreateFrameBuffer (int width, int height, bool fs, DFrameBuffer *old);

	void StartModeIterator (int bits, bool fs);
	bool NextMode (int *width, int *height, bool *letterbox);

	bool GoFullscreen (bool yes);
	void BlankForGDI ();

	void DumpAdapters ();

 private:
	struct ModeInfo
	{
		ModeInfo (int inX, int inY, int inBits, int inRealY, int inDoubling)
			: next (NULL),
			  width (inX),
			  height (inY),
			  bits (inBits),
			  realheight (inRealY),
			  doubling (inDoubling)
		{}

		ModeInfo *next;
		int width, height, bits;
		int realheight;
		int doubling;
	} *m_Modes;

	ModeInfo *m_IteratorMode;
	int m_IteratorBits;
	bool m_IteratorFS;
	bool m_IsFullscreen;
	UINT m_Adapter;

	void AddMode (int x, int y, int bits, int baseHeight, int doubling);
	void FreeModes ();

	static HRESULT WINAPI EnumDDModesCB (LPDDSURFACEDESC desc, void *modes);
	void AddLowResModes ();
	void AddLetterboxModes ();
	void ScaleModes (int doubling);

	friend class DDrawFB;
};

class BaseWinFB : public DFrameBuffer
{
	DECLARE_ABSTRACT_CLASS(BaseWinFB, DFrameBuffer)
public:
	BaseWinFB (int width, int height) : DFrameBuffer (width, height), Windowed (true) {}

	bool IsFullscreen () { return !Windowed; }
	virtual void Blank () = 0;
	virtual bool PaintToWindow () = 0;
	virtual HRESULT GetHR () = 0;
	virtual void ScaleCoordsFromWindow(SWORD &x, SWORD &y);

protected:
	virtual bool CreateResources () = 0;
	virtual void ReleaseResources () = 0;
    virtual int GetTrueHeight() { return GetHeight(); }

	bool Windowed;

	friend int I_PlayMovie (const char *name);
	friend class Win32Video;

	BaseWinFB() {}
};

class DDrawFB : public BaseWinFB
{
	DECLARE_CLASS(DDrawFB, BaseWinFB)
public:
	DDrawFB (int width, int height, bool fullscreen);
	~DDrawFB ();

	bool IsValid ();
	bool Lock (bool buffer);
	void Unlock ();
	void ForceBuffering (bool force);
	void Update ();
	PalEntry *GetPalette ();
	void GetFlashedPalette (PalEntry pal[256]);
	void UpdatePalette ();
	bool SetGamma (float gamma);
	bool SetFlash (PalEntry rgb, int amount);
	void GetFlash (PalEntry &rgb, int &amount);
	int GetPageCount ();
	int QueryNewPalette ();
	void PaletteChanged ();
	void SetVSync (bool vsync);
	void NewRefreshRate();
	HRESULT GetHR ();
	bool Is8BitMode();
    virtual int GetTrueHeight() { return TrueHeight; }

	void Blank ();
	bool PaintToWindow ();

private:
	enum LockSurfRes { NoGood, Good, GoodWasLost };
	
	bool CreateResources ();
	void ReleaseResources ();
	bool CreateSurfacesAttached ();
	bool CreateSurfacesComplex ();
	bool CreateBlitterSource ();
	LockSurfRes LockSurf (LPRECT lockrect, LPDIRECTDRAWSURFACE surf);
	void RebuildColorTable ();
	void MaybeCreatePalette ();
	bool AddBackBuf (LPDIRECTDRAWSURFACE *surface, int num);
	HRESULT AttemptRestore ();

	HRESULT LastHR;
	BYTE GammaTable[3][256];
	PalEntry SourcePalette[256];
	PALETTEENTRY PalEntries[256];
	DWORD FlipFlags;

	LPDIRECTDRAWPALETTE Palette;
	LPDIRECTDRAWSURFACE PrimarySurf;
	LPDIRECTDRAWSURFACE BackSurf;
	LPDIRECTDRAWSURFACE BackSurf2;
	LPDIRECTDRAWSURFACE BlitSurf;
	LPDIRECTDRAWSURFACE LockingSurf;
	LPDIRECTDRAWCLIPPER Clipper;
	HPALETTE GDIPalette;
	DWORD ClipSize;
	PalEntry Flash;
	int FlashAmount;
	int BufferCount;
	int BufferPitch;
	int TrueHeight;
	int PixelDoubling;
	float Gamma;

	bool NeedGammaUpdate;
	bool NeedPalUpdate;
	bool NeedResRecreate;
	bool PaletteChangeExpected;
	bool MustBuffer;		// The screen is not 8-bit, or there is no backbuffer
	bool BufferingNow;		// Most recent Lock was buffered
	bool WasBuffering;		// Second most recent Lock was buffered
	bool Write8bit;
	bool UpdatePending;		// On final unlock, call Update()
	bool UseBlitter;		// Use blitter to copy from sys mem to video mem
	bool UsePfx;

	DDrawFB() {}
};


#if 0
#define STARTLOG		do { if (!dbg) dbg = fopen ("e:/vid.log", "w"); } while(0)
#define STOPLOG			do { if (dbg) { fclose (dbg); dbg=NULL; } } while(0)
#define LOG(x)			do { if (dbg) { fprintf (dbg, x); fflush (dbg); } } while(0)
#define LOG1(x,y)		do { if (dbg) { fprintf (dbg, x, y); fflush (dbg); } } while(0)
#define LOG2(x,y,z)		do { if (dbg) { fprintf (dbg, x, y, z); fflush (dbg); } } while(0)
#define LOG3(x,y,z,zz)	do { if (dbg) { fprintf (dbg, x, y, z, zz); fflush (dbg); } } while(0)
#define LOG4(x,y,z,a,b)	do { if (dbg) { fprintf (dbg, x, y, z, a, b); fflush (dbg); } } while(0)
#define LOG5(x,y,z,a,b,c) do { if (dbg) { fprintf (dbg, x, y, z, a, b, c); fflush (dbg); } } while(0)
extern FILE *dbg;
#define VID_FILE_DEBUG	1
#elif _DEBUG && 0
#define STARTLOG
#define STOPLOG
#define LOG(x)			{ OutputDebugString(x); }
#define LOG1(x,y)		{ char poo[1024]; mysnprintf(poo, countof(poo), x, y); OutputDebugString(poo); }
#define LOG2(x,y,z)		{ char poo[1024]; mysnprintf(poo, countof(poo), x, y, z); OutputDebugString(poo); }
#define LOG3(x,y,z,zz)	{ char poo[1024]; mysnprintf(poo, countof(poo), x, y, z, zz); OutputDebugString(poo); }
#define LOG4(x,y,z,a,b)	{ char poo[1024]; mysnprintf(poo, countof(poo), x, y, z, a, b); OutputDebugString(poo); }
#define LOG5(x,y,z,a,b,c) { char poo[1024]; mysnprintf(poo, countof(poo), x, y, z, a, b, c); OutputDebugString(poo); }
#else
#define STARTLOG
#define STOPLOG
#define LOG(x)
#define LOG1(x,y)
#define LOG2(x,y,z)
#define LOG3(x,y,z,zz)
#define LOG4(x,y,z,a,b)
#define LOG5(x,y,z,a,b,c)
#endif

#endif // __WIN32IFACE_H
