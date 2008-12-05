/* 
 *	Copyright (C) 2007-2009 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#pragma once

#include "GPUState.h"
#include "GSRasterizer.h"
#include "GSAlignedClass.h"

class GPUDrawScanline : public GSAlignedClass<16>, public IDrawScanline
{
	union ScanlineSelector
	{
		struct
		{
			DWORD iip:1; // 0
			DWORD me:1; // 1
			DWORD abe:1; // 2
			DWORD abr:2; // 3
			DWORD tge:1; // 5
			DWORD tme:1; // 6
			DWORD twin:1; // 7
			DWORD tlu:1; // 8
			DWORD dtd:1; // 9
			DWORD ltf:1; // 10
			// DWORD dte:1: // 11
		};

		struct
		{
			DWORD _pad1:1; // 0
			DWORD rfb:2; // 1
			DWORD _pad2:2; // 3
			DWORD tfx:2; // 5
		};

		DWORD dw;

		operator DWORD() {return dw & 0xff;}
	};
	
	__declspec(align(16)) struct ScanlineEnvironment
	{
		GPULocalMemory* mem;

		const void* tex;
		const WORD* clut;

		GSVector4i u[2];
		GSVector4i v[2];

		GSVector4i a;
		GSVector4i md; // similar to gs fba

		GSVector4i ds, dt, dst8;
		GSVector4i dr, dg, db, dc8;
	};

	ScanlineSelector m_sel;
	ScanlineEnvironment m_slenv;

	DrawScanlinePtr m_ds[2048], m_dsf;

	void Init();

	void DrawScanlineT(int top, int left, int right, const Vertex& v);

	template<DWORD sel>
	void DrawScanlineExT(int top, int left, int right, const Vertex& v);

	__forceinline void SampleTexture(int pixels, DWORD ltf, DWORD tlu, DWORD twin, GSVector4i& test, const GSVector4i& s, const GSVector4i& t, GSVector4i* c);
	__forceinline void ColorTFX(DWORD tfx, const GSVector4i& r, const GSVector4i& g, const GSVector4i& b, GSVector4i* c);
	__forceinline void AlphaBlend(UINT32 abr, UINT32 tme, const GSVector4i& d, GSVector4i* c);
	__forceinline void WriteFrame(WORD* RESTRICT fb, const GSVector4i& test, const GSVector4i* c, int pixels);

protected:
	GPUState* m_state;
	int m_filter;
	int m_dither;

public:
	GPUDrawScanline(GPUState* state);
	virtual ~GPUDrawScanline();

	void SetOptions(int filter, int dither);

	// IDrawScanline

	void SetupDraw(Vertex* vertices, int count, const void* texture);
	void SetupScanline(const Vertex& dv);
	void DrawScanline(int top, int left, int right, const Vertex& v);
	void FillRect(const GSVector4i& r, const Vertex& v);
	DrawScanlinePtr GetDrawScanlinePtr();
};
