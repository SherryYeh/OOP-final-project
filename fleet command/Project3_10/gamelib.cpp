/*
 * gamelib.cpp: 本檔案儲存支援遊戲相關的class的implementation
 * Copyright (C) 2002-2012 Woei-Kae Chen <wkc@csie.ntut.edu.tw>
 *
 * This file is part of game, a free game development framework for windows.
 *
 * game is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * game is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 * Known Problems:
 *   2005-9-29
 *      1. VC++.net is unable to add new event handlers for game (this is due
 *         to incompatibility of MFC files between VC++6.0 and VC++.net).
 *         Work around: I have added MOUSEMOVE handlers by using VC++6.0. Other
 *           event handlers are likely unnecessary.
 *      2. When ENABLE_GAME_PAUSE is false, it is possible for a minimized
 *           game to continue playing sounds.
 *
 *
 * History:
 *	 1999-09-24 V2.2
 *		1. Support playback of WAVE files with DirectSound.
 *		2. Use OnIdle to control the flow of the game.
 *	 2002-02-23 v3.0 (fullscreen)
 *		1. Support fullscreen mode.
 *		2. Support playback of both MIDI and WAVE files with DirectMusic.
 *		3. Fix surface lost bugs by restoring surfaces.
 *		4. Move CGame and CBall classes into mygame.cpp.
 *		5. Add CAnimation Class.
 *   2002-03-04 V3.1
 *      1. Add CMovingBitmap::ShowBitmap(CMovingBitmap &) to enable
 *         the operation of blitting from one bitmap into another bitmap.
 *		2. Fix a CheckDDError() bug (incorrect number of errors).
 *      3. Revise delay codes for CSpecialEffect::Delay() and
 *		   CSpecialEffect::DelayFromSetCurrentTime().
 *		4. Support ENABLE_AUDIO flag.
 *   2004-03-01 V4.0
 *      1. Enhance CAudio to support pause and resume.
 *   2004-03-08 V4.1
 *      1. Add KeyDown & KeyUp handler and remove auto-repeat for key
 *         down.
 *      2. Fix a surface lost bug due to suspend of windows.
 *      3. The Game Engine is now closer to a framework.
 *   2005-07-28 V4.2
 *      1  GAME_ASSERT is used to enforce correctness of operations for
 *         CMovingBitamp (make sure bitmap is loaded first).
 *      2. Change the constructor of CAudio to eliminate the compiling
 *         error with VC++.net.
 *      3. Make SurfaceID unsigned to eliminate warning with VC++.net.
 *   2005-09-08
 *      1. Fix a bug that handles CLR_INVALID incorrectly during
 *         SetColorKey(), BltBitmapToBack(), and BltBitmapToBitmap().
 *      2. Eliminate the use of CSpecialEffect::Abort() from BltBitmapToBack.
 *         Use GAME_ASSERT instead.
 *   2005-09-20 V4.2Beta1.
 *   2005-09-29 V4.2Beta2.
 *      1. Add MOUSEMOVE Handler for CGame and CGameState.
 *      2. Add _TRACE preprocessor flag for VC++.net.
 *   2006-02-08 V4.2
 *      1. Fix bugs: make CAnimation::Top() and CAnimation::Left() return y and x.
 *      2. Enhance CAnimation to support SetDelayCount(), Reset(), and IsFinalBitmap().
 *      3. Remove CAnimation::GetLocation() and CMovingBitmap::GetLocation().
 *      4. Bitmap coordinate can no longer be set by CMovingBitmap::LoadBitmap();
 *         defauts to (0,0).
 *   2006-09-09 V4.3
 *      1. Rename Move() and Show() as OnMove and OnShow() to emphasize that they are
 *         event driven.
 *      2. Fix bug: audio is now correctly recovered after a sleep or suspension of windows.
 *      3. Support ENABLE_GAME_PAUSE.
 *   2008-02-15 V4.4
 *      1. Add setup project for Visual studio 2005.
 *      2. Support bitmap scaling when ShowBitmap(scale) is called.
 *      3. Add namespace game_framework.
 *      4. Make the class CGame a singleton so that MFC can access it easily.
 *      5. Support loading of bitmap from bmp file.
 *      6. Support ShowInitProgress(percent) to display loading progress.
 *   2010-02-23 V4.5
 *      1. Remove #define INITGUID to work with VS2008
 *   2010-03-23 V4.6
 *      1. Fix bug: when AUDIO device is not available, CGame::OnInit() returned too early.
 *      2. Rewrite CAudio with MCI commands to eliminate dependency with DirectMusic.
 *      3. Supprt MP3 audio playback.
 *   2012-03-21 V4.7
 *      1. Add SetCapture in CGameView::OnLButtonDown() and RelaseCapture in
 *         CGameView::OnLButtonUp() so that Mouse Events (OnLButtonUp and OnMouseMove)
 *         will be called even if the cursor is outside of the window.
 *   2012-05-13 V4.8
 *      1. Change release mode WINVER to 0x500 so that release mode can be correctly complied
 *         with VS 2010 (project->Game Properties->C/C++->Preprocessor Definitions->WINVER=0x500).
 *      2. Add comments to CGameState::ShowInitProgress() - remind students not to copy
 *         	CDDraw::BltBackToPrimary().
 *      3. Fix Ctrl-Q display bug. Add Invalidate() at the end of CGameView::OnKillFocus.
 *      4. Move CAudio header and implementations into new audio.h and audio.cpp files.
 *      5. Use a thread to execute all MCI commands so that when a sound is played, the
 *         main game thread is not slowed down by MCI commands. According to students, the
 *         slow down is more obvious in Win7 64 bit version; in WinXP, the slow down cannot
 *         be observed.
 *      6. Remove UnitTest folder and configurations.
 *      7. Move header and cpp files into Source sub-directory.
 *   2016-02-26 V4.9
 *      1.Fixed program crash in change display mode to fullscreen
*/

//#define	 INITGUID
#include "stdafx.h"
#include "game.h"
#include "MainFrm.h"
#include "Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include <direct.h>
#include <string.h>
#include "audio.h"
#include "gamelib.h"
#include "mygame.h"

namespace game_framework {

	/////////////////////////////////////////////////////////////////////////////
	// CAnimation: 提供動畫的能力
	// 1. 要懂得怎麼呼叫(運用)其各種能力，但是可以不懂下列的程式是什麼意思
	// 2. 自己寫到運用CMovingBitmap的程式時，可以參考下列程式的寫法
	// 3. 如果需要修改或擴充CAnimation的功能時，請用繼承或直接拷貝的方式，最好
	//    不要直接改CAnimation。
	/////////////////////////////////////////////////////////////////////////////

	CAnimation::CAnimation(int count)
	{
		delay_count = count;
		delay_counter = delay_count;
		x = y = bmp_counter = 0;
	}

	void CAnimation::AddBitmap(int IDB_BITMAP, COLORREF colorkey)
	{
		CMovingBitmap add_bmp;
		add_bmp.LoadBitmap(IDB_BITMAP, colorkey);
		bmp.insert(bmp.end(), add_bmp);
		Reset();
	}

	void CAnimation::AddBitmap(char *filename, COLORREF colorkey)
	{
		CMovingBitmap add_bmp;
		add_bmp.LoadBitmap(filename, colorkey);
		bmp.insert(bmp.end(), add_bmp);
		Reset();
	}

	int CAnimation::GetCurrentBitmapNumber()
	{
		return bmp_counter;
	}

	int CAnimation::Height()
	{
		GAME_ASSERT(bmp.size() != 0, "CAnimation: Bitmaps must be loaded first.");
		return bmp_iter->Height();
	}

	bool CAnimation::IsFinalBitmap()
	{
		GAME_ASSERT(bmp.size() != 0, "CAnimation: Bitmaps must be loaded first.");
		return bmp_counter == (bmp.size() - 1);
	}

	int CAnimation::Left()
	{
		GAME_ASSERT(bmp.size() != 0, "CAnimation: Bitmaps must be loaded first.");
		return x;
	}

	void CAnimation::OnMove()
	{
		GAME_ASSERT(bmp.size() != 0, "CAnimation: Bitmaps must be loaded first.");
		if (--delay_counter <= 0) {
			delay_counter = delay_count;
			bmp_iter++;
			bmp_counter++;
			if (bmp_iter == bmp.end()) {
				bmp_iter = bmp.begin();
				bmp_counter = 0;
			}
		}
	}

	void CAnimation::Reset()
	{
		GAME_ASSERT(bmp.size() != 0, "CAnimation: Bitmaps must be loaded first.");
		delay_counter = delay_count;
		bmp_iter = bmp.begin();
		bmp_counter = 0;
	}

	void CAnimation::SetDelayCount(int dc)
	{
		GAME_ASSERT(dc > 0, "CAnimation: Delay count must be >= 1");
		delay_count = dc;
	}

	void CAnimation::SetTopLeft(int nx, int ny)
	{
		x = nx, y = ny;
		bmp_iter->SetTopLeft(x, y);
	}

	void CAnimation::OnShow()
	{
		GAME_ASSERT(bmp.size() != 0, "CAnimation: Bitmaps must be loaded before they are shown.");
		bmp_iter->SetTopLeft(x, y);
		bmp_iter->ShowBitmap();
	}

	int CAnimation::Top()
	{
		GAME_ASSERT(bmp.size() != 0, "CAnimation: Bitmaps must be loaded first.");
		return y;
	}

	int CAnimation::Width()
	{
		GAME_ASSERT(bmp.size() != 0, "CAnimation: Bitmaps must be loaded first.");
		return bmp_iter->Width();
	}

	/////////////////////////////////////////////////////////////////////////////
	// CInteger: 這個class提供顯示整數圖形的能力
	// 1. 要懂得怎麼呼叫(運用)其各種能力，但是可以不懂下列的程式是什麼意思
	// 2. 自己寫到運用CMovingBitmap的程式時，可以參考下列程式的寫法
	/////////////////////////////////////////////////////////////////////////////

	CMovingBitmap CInteger::digit[11];

	CInteger::CInteger(int digits)
		: NUMDIGITS(digits)
	{
		isBmpLoaded = false;
	}

	void CInteger::Add(int x)
	{
		n += x;
	}

	int CInteger::GetInteger()
	{
		return n;
	}

	void CInteger::LoadBitmap()
	{
		//
		// digit[i]為class varibale，所以必須避免重複LoadBitmap
		//
		if (!isBmpLoaded) {
			int d[11] = { IDB_0,IDB_1,IDB_2,IDB_3,IDB_4,IDB_5,IDB_6,IDB_7,IDB_8,IDB_9,IDB_MINUS };
			for (int i = 0; i < 11; i++)
				digit[i].LoadBitmap(d[i], RGB(255, 255, 255));
			isBmpLoaded = true;
		}
	}

	void CInteger::SetInteger(int i)
	{
		n = i;
	}

	void CInteger::SetTopLeft(int nx, int ny)		// 將動畫的左上角座標移至 (x,y)
	{
		x = nx; y = ny;
	}

	void CInteger::ShowBitmap()
	{
		GAME_ASSERT(isBmpLoaded, "CInteger: 請先執行LoadBitmap，然後才能ShowBitmap");
		int nx;		// 待顯示位數的 x 座標
		int MSB;	// 最左邊(含符號)的位數的數值
		if (n >= 0) {
			MSB = n;
			nx = x + digit[0].Width()*(NUMDIGITS - 1);
		}
		else {
			MSB = -n;
			nx = x + digit[0].Width()*NUMDIGITS;
		}
		for (int i = 0; i < NUMDIGITS; i++) {
			int d = MSB % 10;
			MSB /= 10;
			digit[d].SetTopLeft(nx, y);
			digit[d].ShowBitmap();
			nx -= digit[d].Width();
		}
		if (n < 0) { // 如果小於0，則顯示負號
			digit[10].SetTopLeft(nx, y);
			digit[10].ShowBitmap();
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// CMovingBitmap: Moving Bitmap class
	// 這個class提供可以移動的圖形
	// 要懂得怎麼呼叫(運用)其各種能力，但是可以不懂下列的程式是什麼意思
	/////////////////////////////////////////////////////////////////////////////

	CMovingBitmap::CMovingBitmap()
	{
		isBitmapLoaded = false;
	}

	int CMovingBitmap::Height()
	{
		GAME_ASSERT(isBitmapLoaded, "A bitmap must be loaded before Height() is called !!!");
		return location.bottom - location.top;
	}

	int CMovingBitmap::Left()
	{
		GAME_ASSERT(isBitmapLoaded, "A bitmap must be loaded before Left() is called !!!");
		return location.left;
	}

	void CMovingBitmap::LoadBitmap(int IDB_BITMAP, COLORREF color)
	{
		const int nx = 0;
		const int ny = 0;
		GAME_ASSERT(!isBitmapLoaded, "A bitmap has been loaded. You can not load another bitmap !!!");
		CBitmap bitmap;
		BOOL rval = bitmap.LoadBitmap(IDB_BITMAP);
		GAME_ASSERT(rval, "Load bitmap failed !!! Please check bitmap ID (IDB_XXX).");
		BITMAP bitmapSize;
		bitmap.GetBitmap(&bitmapSize);
		location.left = nx; location.top = ny;
		location.right = nx + bitmapSize.bmWidth;
		location.bottom = ny + bitmapSize.bmHeight;
		SurfaceID = CDDraw::RegisterBitmap(IDB_BITMAP, color);
		isBitmapLoaded = true;
	}

	void CMovingBitmap::LoadBitmap(char *filename, COLORREF color)
	{
		const int nx = 0;
		const int ny = 0;
		GAME_ASSERT(!isBitmapLoaded, "A bitmap has been loaded. You can not load another bitmap !!!");
		HBITMAP hbitmap = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (hbitmap == NULL) {
			char error_msg[300];
			sprintf(error_msg, "Loading bitmap from file \"%s\" failed !!!", filename);
			GAME_ASSERT(false, error_msg);
		}
		CBitmap *bmp = CBitmap::FromHandle(hbitmap); // memory will be deleted automatically
		BITMAP bitmapSize;
		bmp->GetBitmap(&bitmapSize);
		location.left = nx; location.top = ny;
		location.right = nx + bitmapSize.bmWidth;
		location.bottom = ny + bitmapSize.bmHeight;
		SurfaceID = CDDraw::RegisterBitmap(filename, color);
		isBitmapLoaded = true;
	}

	void CMovingBitmap::SetTopLeft(int x, int y)
	{
		GAME_ASSERT(isBitmapLoaded, "A bitmap must be loaded before SetTopLeft() is called !!!");
		int dx = location.left - x;
		int dy = location.top - y;
		location.left = x;
		location.top = y;
		location.right -= dx;
		location.bottom -= dy;
	}

	void CMovingBitmap::ShowBitmap()
	{
		GAME_ASSERT(isBitmapLoaded, "A bitmap must be loaded before ShowBitmap() is called !!!");
		CDDraw::BltBitmapToBack(SurfaceID, location.left, location.top);
	}

	void CMovingBitmap::ShowBitmap(double factor)
	{
		GAME_ASSERT(isBitmapLoaded, "A bitmap must be loaded before ShowBitmap() is called !!!");
		CDDraw::BltBitmapToBack(SurfaceID, location.left, location.top, factor);
	}

	void CMovingBitmap::ShowBitmap(CMovingBitmap &bm)
	{
		GAME_ASSERT(isBitmapLoaded, "A bitmap must be loaded before ShowBitmap() is called !!!");
		GAME_ASSERT(bm.isBitmapLoaded, "A bitmap must be loaded before ShowBitmap() is called !!!");
		CDDraw::BltBitmapToBitmap(SurfaceID, bm.SurfaceID, location.left, location.top);
	}

	int CMovingBitmap::Top()
	{
		GAME_ASSERT(isBitmapLoaded, "A bitmap must be loaded before Top() is called !!!");
		return location.top;
	}

	int CMovingBitmap::Width()
	{
		GAME_ASSERT(isBitmapLoaded, "A bitmap must be loaded before Width() is called !!!");
		return location.right - location.left;
	}

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的各種狀態之Base class(是一個abstract class)
	/////////////////////////////////////////////////////////////////////////////

	CGameState::CGameState(CGame *g)
	{
		game = g; 	// 設定game的pointer
	}

	void CGameState::GotoGameState(int state)
	{
		game->SetGameState(state);
	}

	void CGameState::ShowInitProgress(int percent)
	{
		if (!SHOW_LOAD_PROGRESS)
			return;
		const int bar_width = SIZE_X * 2 / 3;
		const int bar_height = SIZE_Y / 20;
		const int x1 = (SIZE_X - bar_width) / 2;
		const int x2 = x1 + bar_width;
		const int y1 = (SIZE_Y - bar_height) / 2;
		const int y2 = y1 + bar_height;
		const int pen_width = bar_height / 8;
		const int progress_x1 = x1 + pen_width;
		const int progress_x2 = progress_x1 + percent * (bar_width - 2 * pen_width) / 100;
		const int progress_x2_end = x2 - pen_width;
		const int progress_y1 = y1 + pen_width;
		const int progress_y2 = y2 - pen_width;

		CDDraw::BltBackColor(DEFAULT_BG_COLOR);		// 將 Back Plain 塗上預設的顏色
		CMovingBitmap loading;						// 貼上loading圖示
		loading.LoadBitmap(IDB_LOADING, RGB(0, 0, 0));
		loading.SetTopLeft((SIZE_X - loading.Width()) / 2, y1 - 2 * loading.Height());
		loading.ShowBitmap();
		//
		// 以下為CDC的用法
		//
		CDC *pDC = CDDraw::GetBackCDC();			// 取得 Back Plain 的 CDC 
		CPen *pp, p(PS_NULL, 0, RGB(0, 0, 0));		// 清除pen
		pp = pDC->SelectObject(&p);

		CBrush *pb, b(RGB(0, 255, 0));				// 畫綠色 progress框
		pb = pDC->SelectObject(&b);
		pDC->Rectangle(x1, y1, x2, y2);

		CBrush b1(DEFAULT_BG_COLOR);				// 畫黑色 progrss中心
		pDC->SelectObject(&b1);
		pDC->Rectangle(progress_x1, progress_y1, progress_x2_end, progress_y2);

		CBrush b2(RGB(255, 255, 0));					// 畫黃色 progrss進度
		pDC->SelectObject(&b2);
		pDC->Rectangle(progress_x1, progress_y1, progress_x2, progress_y2);

		pDC->SelectObject(pp);						// 釋放 pen
		pDC->SelectObject(pb);						// 釋放 brush
		CDDraw::ReleaseBackCDC();					// 放掉 Back Plain 的 CDC
		//
		// 如果是別的地方用到CDC的話，不要抄以下這行，否則螢幕會閃爍
		//
		CDDraw::BltBackToPrimary();					// 將 Back Plain 貼到螢幕
	}

	void CGameState::OnDraw() // Template Method
	{
		OnShow();
	}

	void CGameState::OnCycle() // Template Method
	{
		OnMove();
		OnShow();
	}

	/////////////////////////////////////////////////////////////////////////////
	// CGame: Game Class
	// 這個class是遊戲的facade，是MFC與各個遊戲狀態的橋樑，如果不增加或減少
	// 遊戲狀態的話，可以不用管這個class的介面與實作。
	/////////////////////////////////////////////////////////////////////////////

	CGame CGame::instance;

	CGame::CGame()
		: NUM_GAME_STATES(3)
	{
		running = true;
		suspended = false;
		gameStateTable[GAME_STATE_INIT] = new CGameStateInit(this);
		gameStateTable[GAME_STATE_RUN] = new CGameStateRun(this);
		gameStateTable[GAME_STATE_OVER] = new CGameStateOver(this);
		gameState = NULL;
	}

	CGame::~CGame()
	{
		for (int i = 0; i < NUM_GAME_STATES; i++)
			delete gameStateTable[i];
	}

	CGame *CGame::Instance()
	{
		return &instance;
	}

	bool CGame::IsRunning()
	{
		return running;
	}

	void CGame::OnDraw()
	{
		CDDraw::BltBackColor(DEFAULT_BG_COLOR);	// 將 Back Plain 塗黑
		gameState->OnDraw();					// 顯示遊戲中的每個元素
		if (!running) {
			//
			// 如果在暫停狀態，則顯示Ctrl-Q...
			//
			CMovingBitmap bmp;
			bmp.LoadBitmap(IDB_CONTINUE);
			bmp.SetTopLeft(0, 0);
			bmp.ShowBitmap();
		}
		CDDraw::BltBackToPrimary();				// 將 Back Plain 貼到螢幕
	}

	void  CGame::OnFilePause()
	{
		if (ENABLE_GAME_PAUSE) {
			if (running)
				CAudio::Instance()->Pause();
			else
				CAudio::Instance()->Resume();
			running = !running;
		}
		else {
			CAudio::Instance()->Resume();
			running = true;
		}
	}

	bool CGame::OnIdle()  // 修改功能不要修改OnIdle()，而應修改OnMove()及OnShow()
	{
		if (suspended) {
			running = false;
			suspended = false;
		}
		//
		// 控制遊戲是否暫停
		//
		if (!running)
			return false;
		//
		// 以下是遊戲的主迴圈
		//
		CDDraw::BltBackColor(DEFAULT_BG_COLOR);	// 將 Back Plain 塗上預設的顏色
		gameState->OnCycle();
		CDDraw::BltBackToPrimary();				// 將 Back Plain 貼到螢幕
		//
		// 以下的程式控制遊戲進行的速度，注意事項：
		// 1. 用Debug mode可以檢視每一次迴圈花掉的時間，令此時間為t。
		// 2. 從上次離開OnIdle()至此，時間定為33ms，不可刪除，其時間不可低於t。
		//
		if (SHOW_GAME_CYCLE_TIME)
			TRACE("Ellipse time for the %d th cycle=%d \n", CSpecialEffect::GetCurrentTimeCount(), CSpecialEffect::GetEllipseTime());
		CSpecialEffect::DelayFromSetCurrentTime(GAME_CYCLE_TIME);
		CSpecialEffect::SetCurrentTime();	// 設定離開OnIdle()的時間
		return true;
	}

	void CGame::OnInit()	// OnInit() 只在程式一開始時執行一次
	{
		//
		// 啟動亂數
		//
		srand((unsigned)time(NULL));
		//
		// 開啟DirectX繪圖介面
		//
		CDDraw::Init(SIZE_X, SIZE_Y);							// 設定遊戲解析度
		//
		// 開啟DirectX音效介面
		//
		if (!CAudio::Instance()->Open())						// 開啟音效介面
			AfxMessageBox("Audio Interface Failed (muted)");	// 無音效介面
		//
		// Switch to the first state
		//
		gameState = gameStateTable[GAME_STATE_INIT];
		gameState->OnBeginState();
		CSpecialEffect::SetCurrentTime();
		running = true;
	}

	void CGame::OnInitStates()
	{
		//
		// 呼叫每個狀態的OnInitialUpdate
		//
		for (int i = 0; i < NUM_GAME_STATES; i++)
			gameStateTable[i]->OnInit();
	}

	void CGame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (running)
			if ((nFlags & 0x4000) == 0) // 去除auto repeat
				gameState->OnKeyDown(nChar, nRepCnt, nFlags);
#ifdef _UNITTEST					// invike unit test if _UNITTEST is defined
		void runTest();
		if (nChar == 'T')
			runTest();
#endif
	}

	void CGame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (running)
			gameState->OnKeyUp(nChar, nRepCnt, nFlags);
	}

	void CGame::OnKillFocus()
	{
		CAudio::Instance()->Pause();
		if (ENABLE_GAME_PAUSE)
			running = false;
		else if (CDDraw::IsFullScreen())
			running = false;
	}

	void CGame::OnLButtonDown(UINT nFlags, CPoint point)
	{
		if (running)
			gameState->OnLButtonDown(nFlags, point);
	}

	void CGame::OnRButtonDown(UINT nFlags, CPoint point)
	{
		if (running)
			gameState->OnRButtonDown(nFlags, point);
	}

	void CGame::OnLButtonUp(UINT nFlags, CPoint point)
	{
		if (running)
			gameState->OnLButtonUp(nFlags, point);
	}

	void CGame::OnMouseMove(UINT nFlags, CPoint point)
	{
		if (running)
			gameState->OnMouseMove(nFlags, point);
	}

	void CGame::OnRButtonUp(UINT nFlags, CPoint point)
	{
		if (running)
			gameState->OnRButtonUp(nFlags, point);
	}

	void CGame::OnResume()
	{
		//
		// Note: the resume message is not synchronized with the other messages
		//
	}

	void CGame::OnSetFocus()
	{
		if (!ENABLE_GAME_PAUSE) {
			CAudio::Instance()->Resume();
			running = true;
		}
	}

	void CGame::OnSuspend()
	{
		//
		// Note: the suspend message is not synchronized with the other messages
		//
		suspended = true;
		CAudio::Instance()->SetPowerResume();
	}

	void CGame::SetGameState(int state)
	{
		ASSERT(state >= 0 && state < NUM_GAME_STATES);
		gameState = gameStateTable[state];
		gameState->OnBeginState();
		OnDraw();
		CSpecialEffect::SetCurrentTime();
		running = true;
	}

	/////////////////////////////////////////////////////////////////////////////
	// CSpecialEffect: Specail Effect functions
	// 一般的遊戲並不需直接操作這個物件，因此可以全部略過不看
	/////////////////////////////////////////////////////////////////////////////

	DWORD CSpecialEffect::ctime = 0;
	int   CSpecialEffect::ctimeCount = 0;

	void CSpecialEffect::Delay(DWORD ms)
	{
		DWORD ctime = timeGetTime();
		int waitms;
		waitms = ms - (timeGetTime() - ctime);
		if (waitms > 0)
			Sleep(waitms);
	}

	void CSpecialEffect::DelayFromSetCurrentTime(DWORD ms)
	{
		int waitms;
		waitms = ms - (timeGetTime() - ctime);
		if (waitms > 0)
			Sleep(waitms);
	}

	void CSpecialEffect::SetCurrentTime()
	{
		ctime = timeGetTime();
		ctimeCount++;
	}

	DWORD CSpecialEffect::GetEllipseTime()
	{
		return timeGetTime() - ctime;
	}

	int CSpecialEffect::GetCurrentTimeCount()
	{
		return ctimeCount;
	}

		typedef struct {
			RGNDATAHEADER hdr;
			RECT rgndata[4];
		} CLIPLIST, *LPCLIPLIST;
		CLIPLIST    ClipList;
		RECT rc;
		SetRect(&rc, 0, 0, size_x, size_y);
		ClipList.hdr.dwSize = sizeof(RGNDATAHEADER);
		ClipList.hdr.iType = RDH_RECTANGLES;
		ClipList.hdr.nCount = 1;
		ClipList.hdr.nRgnSize = 0;
		memcpy(&ClipList.hdr.rcBound, &rc, sizeof(RECT));
		memcpy(&ClipList.rgndata, &rc, sizeof(RECT));
		ddrval = lpClipperBack->SetClipList((LPRGNDATA)&ClipList, 0);
		CheckDDFail("SetHWnd FAILED");

		// Create Back (Secondary) Surface
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwHeight = size_y; ddsd.dwWidth = size_x;
		ddrval = lpDD->CreateSurface(&ddsd, &lpDDSBack, NULL);
		CheckDDFail("Can not create back plain");

		// In fullscreen mode, the clipper for primary surafce is fixed to screen size
		ddrval = lpDDSPrimary->SetClipper(lpClipperBack);
		CheckDDFail("SetClipper FAILED");
		ddrval = lpDDSBack->SetClipper(lpClipperBack);
		CheckDDFail("SetClipper FAILED");
		lpClipperBack->Release();

		BltBackColor(RGB(0, 0, 0));
		BltBackToPrimary();
		return true;
	}

	bool CDDraw::CreateSurfaceWindowed()
	{
		ddrval = lpDD->SetCooperativeLevel(AfxGetMainWnd()->m_hWnd, DDSCL_NORMAL);
		CheckDDFail("Can not SetCooperativeLevel ");

		DDSURFACEDESC ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		ddrval = lpDD->CreateSurface(&ddsd, &lpDDSPrimary, NULL);
		CheckDDFail("Create Primary Surface failed");

		// Create clippers for the primary and back surfaces
		ddrval = lpDD->CreateClipper(0, &lpClipperPrimary, NULL);
		CheckDDFail("Create Primay Surface Clipper FAILED");
		ddrval = lpDD->CreateClipper(0, &lpClipperBack, NULL);
		CheckDDFail("Create Back Surface Clipper FAILED");

		ddrval = lpClipperPrimary->SetHWnd(0, AfxGetMainWnd()->m_hWnd);
		CheckDDFail("Primary Surface SetHWnd FAILED");

		typedef struct {
			RGNDATAHEADER hdr;
			RECT rgndata[4];
		} CLIPLIST, *LPCLIPLIST;
		CLIPLIST    ClipList;
		RECT rc;
		SetRect(&rc, 0, 0, size_x, size_y);
		ClipList.hdr.dwSize = sizeof(RGNDATAHEADER);
		ClipList.hdr.iType = RDH_RECTANGLES;
		ClipList.hdr.nCount = 1;
		ClipList.hdr.nRgnSize = 0;
		memcpy(&ClipList.hdr.rcBound, &rc, sizeof(RECT));
		memcpy(&ClipList.rgndata, &rc, sizeof(RECT));
		ddrval = lpClipperBack->SetClipList((LPRGNDATA)&ClipList, 0);
		CheckDDFail("SetHWnd FAILED");

		// Create Back (Secondary) Surface
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwHeight = size_y; ddsd.dwWidth = size_x;
		ddrval = lpDD->CreateSurface(&ddsd, &lpDDSBack, NULL);
		CheckDDFail("Can not create back plain");

		ddrval = lpDDSPrimary->SetClipper(lpClipperPrimary);
		CheckDDFail("SetClipper FAILED");
		lpClipperPrimary->Release();
		ddrval = lpDDSBack->SetClipper(lpClipperBack);
		CheckDDFail("SetClipper FAILED");
		lpClipperBack->Release();

		BltBackColor(RGB(0, 0, 0));
		return true;
	}

	void CDDraw::GetClientRect(CRect &r)
	{
		r = CRect(0, 0, size_x, size_y);
	}

	void CDDraw::Init(int sx, int sy)
	{
		// set target screen size
		size_x = sx, size_y = sy;
		// init lpDD
		LPDIRECTDRAW lpDD0;
		ddrval = DirectDrawCreate(NULL, &lpDD0, NULL);
		CheckDDFail("DDraw create failed");
		ddrval = lpDD0->QueryInterface(IID_IDirectDraw2, (void **)&lpDD);
		CheckDDFail("DDraw surface 2 create failed");
		// init pCView
		POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
		CDocTemplate *doc_t = AfxGetApp()->GetNextDocTemplate(pos);
		pos = doc_t->GetFirstDocPosition();
		CDocument *doc = doc_t->GetNextDoc(pos);
		pos = doc->GetFirstViewPosition();
		pCView = doc->GetNextView(pos);
	}

	bool CDDraw::IsFullScreen()
	{
		return fullscreen;
	}

	void CDDraw::LoadBitmap(int i, int IDB_BITMAP)
	{
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_BITMAP);
		CDC mDC;
		mDC.CreateCompatibleDC(NULL);
		CBitmap* pOldBitmap = mDC.SelectObject(&bitmap);
		BITMAP bitmapSize;
		bitmap.GetBitmap(&bitmapSize);
		DDSURFACEDESC ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		BitmapRect[i].bottom = ddsd.dwHeight = bitmapSize.bmHeight;
		BitmapRect[i].right = ddsd.dwWidth = bitmapSize.bmWidth;
		ddrval = lpDD->CreateSurface(&ddsd, &lpDDS[i], NULL);
		CheckDDFail("Create Bitmap Surface Failed");
		HDC hdc;
		ddrval = lpDDS[i]->GetDC(&hdc);
		CheckDDFail("Get surface HDC failed");
		cdc.Attach(hdc);
		cdc.BitBlt(0, 0, bitmapSize.bmWidth, bitmapSize.bmHeight, &mDC, 0, 0, SRCCOPY);
		cdc.Detach();
		lpDDS[i]->ReleaseDC(hdc);
		// avoid memory leak
		// According to spec, mDC should delete itself automatically.  However,
		// it appears that we have to do it explictly.
		mDC.SelectObject(&pOldBitmap);
		mDC.DeleteDC();
		bitmap.DeleteObject();
	}

	DWORD CDDraw::MatchColorKey(LPDIRECTDRAWSURFACE lpDDSurface, COLORREF color)
	{
		DDSURFACEDESC ddsd;
		HDC hdc;
		HRESULT hres;
		COLORREF rgbT = CLR_INVALID;
		DWORD dw = CLR_INVALID, mask = (DWORD)~0;
		if (lpDDSurface && color != CLR_INVALID) {
			if (lpDDSurface->GetDC(&hdc) == DD_OK) {
				rgbT = GetPixel(hdc, 0, 0);             // save (0,0) pixel value
				SetPixel(hdc, 0, 0, color);             // set our value
				lpDDSurface->ReleaseDC(hdc);
			}
			ddsd.dwSize = sizeof(ddsd);
			while ((hres = lpDDSurface->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING);
			if (hres == DD_OK) {
				dw = *(DWORD *)ddsd.lpSurface;          // get (0,0) data
				if (ddsd.ddpfPixelFormat.dwRGBBitCount < 32)
					mask = (1 << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1;
				dw &= mask;								// mask it to bpp
				lpDDSurface->Unlock(NULL);
			}
			if (lpDDSurface->GetDC(&hdc) == DD_OK) {
				SetPixel(hdc, 0, 0, rgbT);				// restore (0,0) pixel value
				lpDDSurface->ReleaseDC(hdc);
			}
		}
		return dw;
	}

	

	

	void CDDraw::ReleaseSurface()
	{
		if (lpDD)
		{
			for (unsigned i = 0; i < lpDDS.size(); i++)
				if (lpDDS[i]) {
					lpDDS[i]->Release();
					lpDDS[i] = NULL;
				}
			if (lpDDSBack)
			{
				lpDDSBack->Release();
				lpDDSBack = NULL;
			}
			if (lpDDSPrimary)
			{
				lpDDSPrimary->Release();
				lpDDSPrimary = NULL;
			}
		}
	}

	
	

	void CDDraw::SetColorKey(unsigned SurfaceID, COLORREF color)
	{
		if (color != CLR_INVALID) {
			DDCOLORKEY ddck;
			ddck.dwColorSpaceLowValue = MatchColorKey(lpDDS[SurfaceID], color);
			ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
			ddrval = lpDDS[SurfaceID]->SetColorKey(DDCKEY_SRCBLT, &ddck);
			CheckDDFail("Can not Set Color Key");
		}
	}

	bool CDDraw::SetFullScreen(bool isFullScreen)
	{
		fullscreen = isFullScreen;
		return CreateSurface();
	}


}