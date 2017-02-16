// Tetris.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <vector>
#include <conio.h>
#include <MyTools/CLPublic.h>
#include <MyTools/Character.h>

struct Point
{
	int X;
	int Y;
	Point() : X(0), Y(0){}
	Point(int X_, int Y_) : X(X_), Y(Y_){}

	bool operator == (CONST Point& Point_) CONST
	{
		return this->X == Point_.X && this->Y == Point_.Y;
	}

	VOID SetPointByIndex(_In_ int nIndex)
	{
		X += nIndex % 4;
		Y += nIndex / 4;
	}
};

enum em_Shape
{
	em_Shape_Z,
	em_Shape_Z_Reverse,
	em_Share_L,
	em_Shape_L_Reverse,
	em_Shape_T,
	em_Shape_Square,
	em_Shape_Line,
};

enum em_Dir
{
	em_Dir_Left,
	em_Dir_Top,
	em_Dir_Right,
	em_Dir_Bottom
};

enum em_Result
{
	em_Result_TRUE,
	em_Result_FALSE,
	em_Result_Exception
};

enum em_PrintShape
{
	em_PrintShape_Square,
	em_PrintShape_Empty,
	em_PrintShape_Wall
};

struct TetrisShape
{
	Point				Pos;
	em_Shape			emShape;
	em_Dir				emShapeDir;
	WORD				wColor;
	std::vector<int>	VecShapePoint;
};

CONST int g_nMaxY = 24;
CONST int g_nMaxX = 12;
CONST int g_nMinY = 0;
CONST int g_nMinX = 1;
WORD TetrisShapeArray[g_nMaxY][g_nMaxX];

VOID ShowErr(_In_ LPCWSTR pwszFormat, ...)
{
	va_list		args;
	wchar_t		szBuff[1024] = { 0 };

	va_start(args, pwszFormat);
	_vsnwprintf_s(szBuff, _countof(szBuff) - 1, _TRUNCATE, pwszFormat, args);
	va_end(args);

	::MessageBoxW(NULL, szBuff, L"Error", NULL);
}

CONST std::vector<int>* GetShapeVec(_In_ em_Shape emShape, _In_ em_Dir emDir)
{
	// Dot in Left Bottom Shape
	struct ShapePoint
	{
		em_Shape		emShape;
		em_Dir	emShapeDir;
		std::vector<int> VecPoint;
	};
	
	static CONST std::vector<ShapePoint> VecShapePoint = 
	{
		ShapePoint{ em_Shape_Z, em_Dir_Left, { 8, 9, 13, 14 } }, // Record Index in Two dimensional
		ShapePoint{ em_Shape_Z, em_Dir_Top, { 5, 8, 9, 12 } },
		ShapePoint{ em_Shape_Z, em_Dir_Right, { 8, 9, 13, 14 } },
		ShapePoint{ em_Shape_Z, em_Dir_Bottom, { 5, 8, 9, 12 } },

		ShapePoint{ em_Shape_Z_Reverse, em_Dir_Left, { 9, 10, 12, 13 } },
		ShapePoint{ em_Shape_Z_Reverse, em_Dir_Top, { 4, 8, 9, 13 } },
		ShapePoint{ em_Shape_Z_Reverse, em_Dir_Right, { 9, 10, 12, 13 } },
		ShapePoint{ em_Shape_Z_Reverse, em_Dir_Bottom, { 4, 8, 9, 13 } },

		ShapePoint{ em_Share_L, em_Dir_Left, { 10, 12, 13, 14 } },
		ShapePoint{ em_Share_L, em_Dir_Top, { 4, 8, 9, 13 } },
		ShapePoint{ em_Share_L, em_Dir_Right, { 9, 10, 12, 13 } },
		ShapePoint{ em_Share_L, em_Dir_Bottom, { 4, 8, 9, 13 } },

		ShapePoint{ em_Shape_L_Reverse, em_Dir_Left, { 5, 9, 12, 13 } },
		ShapePoint{ em_Shape_L_Reverse, em_Dir_Top, { 8, 12, 13, 14 } },
		ShapePoint{ em_Shape_L_Reverse, em_Dir_Right, { 4, 5, 8, 12 } },
		ShapePoint{ em_Shape_L_Reverse, em_Dir_Bottom, { 8, 9, 10, 14 } },

		ShapePoint{ em_Shape_T, em_Dir_Left, { 5, 8, 9, 13 } },
		ShapePoint{ em_Shape_T, em_Dir_Top, { 9, 12, 13, 14 } },
		ShapePoint{ em_Shape_T, em_Dir_Right, { 5, 9, 10, 13 } },
		ShapePoint{ em_Shape_T, em_Dir_Bottom, { 8, 9, 10, 13 } },

		ShapePoint{ em_Shape_Square, em_Dir_Left, { 8, 9, 12, 13 } },
		ShapePoint{ em_Shape_Square, em_Dir_Top, { 8, 9, 12, 13 } },
		ShapePoint{ em_Shape_Square, em_Dir_Right, { 8, 9, 12, 13 } },
		ShapePoint{ em_Shape_Square, em_Dir_Bottom, { 8, 9, 12, 13 } },

		ShapePoint{ em_Shape_Line, em_Dir_Left, { 0, 4, 8, 12 } },
		ShapePoint{ em_Shape_Line, em_Dir_Top, { 12, 13, 14, 15 } },
		ShapePoint{ em_Shape_Line, em_Dir_Right, { 0, 4, 8, 12 } },
		ShapePoint{ em_Shape_Line, em_Dir_Bottom, { 12, 13, 14, 15 } },
	};
	
	auto pShapePoint = CLPublic::Vec_find_if_Const(VecShapePoint, [emShape, emDir](CONST ShapePoint& ShapePoint_){ return ShapePoint_.emShape == emShape && ShapePoint_.emShapeDir == emDir; });
	return pShapePoint == nullptr ? nullptr : &pShapePoint->VecPoint;
}

CONST std::vector<int>* GetShapeVec(_In_ CONST TetrisShape& TetrisShape_)
{
	return GetShapeVec(TetrisShape_.emShape, TetrisShape_.emShapeDir);
}

WORD GetColorByShape(_In_ em_Shape emShape_)
{
	struct ShapeColor
	{
		em_Shape	emShape;
		WORD		wColor;
	};

	CONST static std::vector<ShapeColor> VecShapeColor = 
	{
		{ em_Shape_Z, FOREGROUND_BLUE },
		{ em_Shape_Z_Reverse, FOREGROUND_BLUE },
		{ em_Share_L, FOREGROUND_BLUE | FOREGROUND_GREEN },
		{ em_Shape_L_Reverse, FOREGROUND_BLUE | FOREGROUND_GREEN },
		{ em_Shape_T, FOREGROUND_BLUE | FOREGROUND_RED },
		{ em_Shape_Square, FOREGROUND_GREEN | FOREGROUND_RED },
		{ em_Shape_Line, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED },
	};

	auto pShapeColor = CLPublic::Vec_find_if_Const(VecShapeColor, [emShape_](CONST ShapeColor& ShapeColor_){ return ShapeColor_.emShape == emShape_; });
	return pShapeColor == nullptr ? FOREGROUND_RED : pShapeColor->wColor;
}

void PrintSingleShape(_In_ CONST Point& Tar, _In_ WORD wColor, _In_ em_PrintShape emPrintShape)
{
	// Get Console Handle
	HANDLE hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);

	// Set Next Print Color
	::SetConsoleTextAttribute(hConsole, wColor);

	// Set Current Pos to Position
	COORD Pos = { Tar.X * 2, Tar.Y };
	::SetConsoleCursorPosition(hConsole, Pos);

	switch (emPrintShape)
	{
	case em_PrintShape_Square:
		std::wcout << L"¡õ";
		break;
	case em_PrintShape_Empty:
		std::wcout << L" ";
		break;
	case em_PrintShape_Wall:
		std::wcout << L"¡ö";
		break;
	default:
		break;
	}
}

void PrintWall()
{
	// Left Wall
	for (int i = 0; i < 26 ; ++i)
	{
		PrintSingleShape(Point(g_nMinX - 1, i), FOREGROUND_GREEN, em_PrintShape_Wall);
	}

	// Right Wall
	for (int i = 0; i < 26; ++i)
	{
		PrintSingleShape(Point(g_nMaxX + 1, i), FOREGROUND_GREEN, em_PrintShape_Wall);
	}

	// Init
	for (int i = 0; i < g_nMaxY; ++i)
	{
		for (int j = 0; j < g_nMaxX; ++j)
		{
			TetrisShapeArray[i][j] = NULL;
		}
	}
}

void PrintFullShape(_In_ CONST TetrisShape& TetrisShape_, _In_ em_PrintShape emPrintShape)
{
	/*
	|¡ö|¡ö|¡ö|¡ö|
	|¡ö|¡ö|¡ö|¡ö|
	|¡ö|¡ö|¡ö|¡ö|
	|¡ö|¡ö|¡ö|¡ö|
	*/

	// Start for left top Point
	Point Pos = TetrisShape_.Pos;
	// { 12, 13, 14, 15 }
	for (auto nIndex : TetrisShape_.VecShapePoint)
	{
		Pos = TetrisShape_.Pos;
		Pos.SetPointByIndex(nIndex);
		if (Pos.X > g_nMaxX || Pos.X < g_nMinX || Pos.Y < g_nMinY)
			continue;

		PrintSingleShape(Pos, TetrisShape_.wColor, emPrintShape);
	}
}

em_Result CreateTetrisShape(_Out_opt_ TetrisShape& TetrisShape_)
{
	// Show rand location on Top 
	TetrisShape_.Pos = Point(CCharacter::GetRand(g_nMaxX / 2, g_nMinX * 2), g_nMinY - 3);
	TetrisShape_.emShape = static_cast<em_Shape>(CCharacter::GetRand(6, 0));
	TetrisShape_.emShapeDir = static_cast<em_Dir>(CCharacter::GetRand(3, 0));
	TetrisShape_.wColor = GetColorByShape(TetrisShape_.emShape);

	auto pVecPoint = GetShapeVec(TetrisShape_);
	if (pVecPoint == nullptr)
		return em_Result::em_Result_Exception;

	TetrisShape_.VecShapePoint = *pVecPoint;
	return em_Result::em_Result_TRUE;
}

em_Result IsCollision(_In_ CONST TetrisShape& TetrisShape_, _In_ em_Dir emDir)
{
	// Move to Left
	Point NextPoint = TetrisShape_.Pos;
	switch (emDir)
	{
	case em_Dir_Left:
		NextPoint.X -= 1;
		break;
	case em_Dir_Right:
		NextPoint.X += 1;
		break;
	case em_Dir_Bottom:
		NextPoint.Y += 1;
		break;
	default:
		break;
	}

	// Is Exist Pos in VecTetisShape
	Point Pos = NextPoint;
	auto p = CLPublic::Vec_find_if_Const(TetrisShape_.VecShapePoint, [&Pos, NextPoint](int nIndex)
	{
		Pos = NextPoint;
		Pos.SetPointByIndex(nIndex);

		return TetrisShapeArray[Pos.Y - 1][Pos.X - 1] != NULL ? TRUE : FALSE;
	});

	return p != nullptr ? em_Result_TRUE : em_Result_FALSE;
}

BOOL IsInWall(_In_ CONST std::vector<int>& VecShapePoint, _In_ CONST Point& CurPoint)
{
	return CLPublic::Vec_find_if_Const(VecShapePoint, [CurPoint](int nIndex)
	{
		Point Pos = CurPoint;
		Pos.SetPointByIndex(nIndex);
		return Pos.X > g_nMaxX || Pos.X < g_nMinX || Pos.Y > g_nMaxY;
	}) == nullptr ? TRUE : FALSE;
}

em_Result ResetShapeDir(_In_ _Out_ TetrisShape& TetrisShape_, _In_ em_Dir emDir)
{
	Point Pos = TetrisShape_.Pos;
	switch (emDir)
	{
	case em_Dir_Left:
		Pos.X -= 1;
		break;
	case em_Dir_Right:
		Pos.X += 1;
		break;
	case em_Dir_Bottom:
		Pos.Y += 1;
		break;
	default:
		break;
	}

	if (!IsInWall(TetrisShape_.VecShapePoint, Pos))
		return em_Result::em_Result_FALSE;

	em_Result emResult = IsCollision(TetrisShape_, emDir);
	if (emResult == em_Result_Exception)
		return em_Result::em_Result_Exception;
	else if (emResult == em_Result_TRUE)
		return em_Result::em_Result_FALSE;

	// Remove Old Trace
	PrintFullShape(TetrisShape_, em_PrintShape::em_PrintShape_Empty);

	// Move to Dir
	TetrisShape_.Pos = Pos;

	// Print New Shape
	PrintFullShape(TetrisShape_, em_PrintShape::em_PrintShape_Square);
	return em_Result::em_Result_TRUE;
}

em_Result ChangeShapeDirection(_In_ _Out_ TetrisShape& TetrisShape_)
{
	struct ShapeDir
	{
		int nIndex;
		em_Dir emShapeDir;
	};

	CONST static std::vector<ShapeDir> VecShapeDir = 
	{
		{ 0, em_Dir_Top },	{ 1, em_Dir_Right },
		{ 2, em_Dir_Bottom }, { 3, em_Dir_Left },
	};

	auto pCurrentShapeDir = CLPublic::Vec_find_if_Const(VecShapeDir, [TetrisShape_](_In_ CONST ShapeDir& ShapeDir_){ return ShapeDir_.emShapeDir == TetrisShape_.emShapeDir; });
	if (pCurrentShapeDir == nullptr)
	{
		ShowErr(L"UnExist emShapeDir=%X in VecShapeDir", TetrisShape_.emShapeDir);
		return em_Result::em_Result_Exception;
	}

	int nNextIndex = pCurrentShapeDir->nIndex == 3 ? 0 : pCurrentShapeDir->nIndex + 1;
	auto pNextShapeDir = CLPublic::Vec_find_if_Const(VecShapeDir, [nNextIndex](_In_ CONST ShapeDir& ShapeDir_){ return nNextIndex == ShapeDir_.nIndex; });
	if (pNextShapeDir == nullptr)
	{
		ShowErr(L"UnExist Index=%d in VecShapeDir", nNextIndex);
		return em_Result::em_Result_Exception;
	}

	auto pVecPoint = GetShapeVec(TetrisShape_.emShape, pNextShapeDir->emShapeDir);
	if (pVecPoint == nullptr)
		return em_Result::em_Result_Exception;
	
	// Is Could Change Shape Dir
	if (!IsInWall(*pVecPoint,TetrisShape_.Pos))
		return em_Result::em_Result_FALSE;

	TetrisShape_.emShapeDir = pNextShapeDir->emShapeDir;
	TetrisShape_.VecShapePoint = *pVecPoint;
	return em_Result::em_Result_TRUE;
}

em_Result IsUnderFloor(_In_ CONST TetrisShape& TetrisShape_)
{
	Point Pos = TetrisShape_.Pos;
	Pos.Y += 1;
	return !IsInWall(TetrisShape_.VecShapePoint, Pos) ? em_Result::em_Result_TRUE : IsCollision(TetrisShape_, em_Dir::em_Dir_Bottom);
}

VOID RemoveFullofLine()
{
	BOOL bRefresh = FALSE;
	for (int i = 0; i < g_nMaxY; )
	{
		BOOL bExist = TRUE;
		// Is Exist a Line
		for (int j = 0; j < g_nMaxX && bExist; ++j)
		{
			if (TetrisShapeArray[i][j] == NULL)
				bExist = FALSE;
		}

		if (!bExist)
		{
			// Check Next Layer
			i += 1;
			continue;
		}

		bRefresh = TRUE;
		// Move the Top layer to the Next Layer
		for (int k = i; k - 1 > 0; --k)
		{
			memcpy(TetrisShapeArray[k], TetrisShapeArray[k - 1], sizeof(TetrisShapeArray[k]));
		}

		// Set Topest Layer to NULL
		ZeroMemory(TetrisShapeArray[0], sizeof(TetrisShapeArray[0]));
	}

	if (!bRefresh)
		return;

	for (int i = 0; i < g_nMaxY; ++i)
	{
		for (int j = 0; j < g_nMaxX; ++j)
		{
			PrintSingleShape(Point(j + 1, i + 1), TetrisShapeArray[i][j], em_PrintShape::em_PrintShape_Square);
		}
	}
}

VOID AddToTetrisShapeArray(_In_ CONST TetrisShape& TetrisShape_)
{
	Point Pos;
	for (auto nIndex : TetrisShape_.VecShapePoint)
	{
		Pos = TetrisShape_.Pos;
		Pos.SetPointByIndex(nIndex);
		TetrisShapeArray[Pos.Y - 1][Pos.X - 1] = TetrisShape_.wColor;
	}
}

VOID ResponeKeyboard(_In_ _Out_ TetrisShape& CurrentTetrisShape)
{
	if (_kbhit())
	{
		int nValue1 = _getch();
		if (nValue1 == 0xE0)
		{
			// Dir
			int nValue2 = _getch();
			switch (nValue2)
			{
			case 0x50:
				ResetShapeDir(CurrentTetrisShape, em_Dir::em_Dir_Bottom);
				break;
			case 0x4B:
				ResetShapeDir(CurrentTetrisShape, em_Dir::em_Dir_Left);
				break;
			case 0x4D:
				ResetShapeDir(CurrentTetrisShape, em_Dir::em_Dir_Right);
				break;
			default:
				break;
			}
		}
		else if (nValue1 == VK_SPACE)
		{
			// Clear Old Shape before Overwrite VecShapePoint
			PrintFullShape(CurrentTetrisShape, em_PrintShape::em_PrintShape_Empty);
			// Overwrite VecShapePoint
			ChangeShapeDirection(CurrentTetrisShape);
			// Write New Shape
			PrintFullShape(CurrentTetrisShape, em_PrintShape::em_PrintShape_Square);
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	SetConsole_Language_CHINA;

	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	::SetConsoleCursorInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
	PrintWall();

	TetrisShape CurrentTetrisShape;
	CreateTetrisShape(CurrentTetrisShape);
	PrintFullShape(CurrentTetrisShape, em_PrintShape::em_PrintShape_Square);

	ULONGLONG ulTick = ::GetTickCount64();
	while (true)
	{
		ResponeKeyboard(CurrentTetrisShape);
		if (static_cast<DWORD>(::GetTickCount64() - ulTick) >= 300)
		{
			ResetShapeDir(CurrentTetrisShape, em_Dir::em_Dir_Bottom);
			if (IsUnderFloor(CurrentTetrisShape) == em_Result::em_Result_TRUE)
			{
				// check user input in 500ms
				for (int i = 0; i < 5; ++i)
				{
					ResponeKeyboard(CurrentTetrisShape);
					::Sleep(100);
				}
				AddToTetrisShapeArray(CurrentTetrisShape);
				RemoveFullofLine();

				CreateTetrisShape(CurrentTetrisShape);
				PrintFullShape(CurrentTetrisShape, em_PrintShape::em_PrintShape_Square);
			}

			ulTick = ::GetTickCount64();
		}
		::Sleep(100);
	}

	return 0;
}

