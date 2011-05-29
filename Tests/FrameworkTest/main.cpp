// FrameworkTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <d3d9.h>
#include <d3dx9.h>

#include "Math/Math.h"
#include "Utility/StringUtils.h"
#include "Math/RandomUtils.h"

using namespace std;
using namespace Apoc3D;
using namespace Apoc3D::Math;
using namespace Apoc3D::Utility;

void PrintString(const wchar_t* name)
{
	wprintf(name);  wprintf(L"\n");
}
void PrintValue(const wchar_t* name, uint64 v)
{
	wprintf(name); wprintf(StringUtils::ToString(v).c_str()); wprintf(L"\n");
}
void PrintValue(const wchar_t* name, float v)
{
	wprintf(name); wprintf(StringUtils::ToString(v, 10).c_str()); wprintf(L"\n");
}

void PrintVector(const wchar_t* name, const Vector2& v)
{
	wprintf(name); wprintf(Vector2Utils::ToTextString(v).c_str()); wprintf(L"\n");
}

void PrintVector(const wchar_t* name, const D3DXVECTOR2& v)
{
	wprintf(name);
	wprintf(StringUtils::ToString(v.x,10, 0,' ', 0).c_str()); wprintf(L", ");
	wprintf(StringUtils::ToString(v.y,10, 0,' ', 0).c_str()); wprintf(L"\n");
}

void StringUtilsTest()
{
	const bool a0 = true;

	const float a1 = 1.234567890123f;
	const wchar_t* a2 = L"Test 1234567中";

	const int32 a3 = -12;

	const int64 a4 = -1 * ((int64)1 << 32);
	const uint64 a5 = (uint64)1<<35;
	
	// StringUtils::ToString
	PrintString(L"StringUtils::ToString");
	PrintString(StringUtils::ToString(a0).c_str());
	PrintString(StringUtils::ToString(a1).c_str());
	PrintString(StringUtils::ToString(a2).c_str());
	PrintString(StringUtils::ToString(a3).c_str());
	PrintString(StringUtils::ToString(a4).c_str());
	PrintString(StringUtils::ToString(a5).c_str());

	PrintString(L"");
	

	// StringUtils::StartsWidth
	PrintString(L"StringUtils::StartsWidth");
	PrintString(StringUtils::ToString(
		StringUtils::StartsWidth(L"Abcdefg", L"Abc", false)
		).c_str());
	PrintString(StringUtils::ToString(
		StringUtils::StartsWidth(L"Abcdefg", L"abc", true)
		).c_str());
	PrintString(StringUtils::ToString(
		StringUtils::StartsWidth(L"Abcdefg", L"xbc", false)
		).c_str());

	// StringUtils::EndsWidth
	PrintString(L"StringUtils::EndsWidth");
	PrintString(StringUtils::ToString(
		StringUtils::EndsWidth(L"Abcdefg", L"EFG", false)
		).c_str());
	PrintString(StringUtils::ToString(
		StringUtils::EndsWidth(L"Abcdefg", L"EFG", true)
		).c_str());
	PrintString(StringUtils::ToString(
		StringUtils::EndsWidth(L"Abcdefg", L"dfg", false)
		).c_str());


	// StringUtils::GetHashCode
	PrintString(L"StringUtils::GetHashCode");
	PrintValue(L"hash('APOC3D')=", StringUtils::GetHashCode(L"APOC3D"));
	PrintValue(L"hash('APoC3D')=", StringUtils::GetHashCode(L"APoC3D"));
	PrintValue(L"hash('APOC3D ')=", StringUtils::GetHashCode(L"APOC3D "));
	PrintValue(L"hash('APOC3D Engine')=", StringUtils::GetHashCode(L"APOC3D Engine"));


	// StringUtils::Trim*
	PrintString(L"StringUtils::Trim*");
	
	String temp = L"		content ,		";
	PrintString(temp.c_str());
	PrintString(L"StringUtils::Trim:");
	StringUtils::Trim(temp, L"		");
	temp.append(L"[]");
	PrintString(temp.c_str());

	temp = L"		content ,		";
	PrintString(L"StringUtils::TrimLeft:");
	StringUtils::TrimLeft(temp, L"		");
	temp.append(L"[]");
	PrintString(temp.c_str());


	temp = L"		content ,		";
	PrintString(L"StringUtils::TrimRight");
	StringUtils::TrimRight(temp, L"		");
	temp.append(L"[]");
	PrintString(temp.c_str());


	// StringUtils::ToLowerCase
	PrintString(L"StringUtils::ToLowerCase");
	temp = L"CONTENT2";
	PrintString(temp.c_str());
	StringUtils::ToLowerCase(temp);
	PrintString(temp.c_str());

	PrintString(L"StringUtils::ToUpperCase");
	temp = L"content12";
	PrintString(temp.c_str());
	StringUtils::ToUpperCase(temp);
	PrintString(temp.c_str());


	// StringUtils::Split
	PrintString(L"StringUtils::Split");
	temp = L"2, 3, ,4,,, 5";
	
	vector<String> args = StringUtils::Split(temp, L" ,");
	for (size_t i=0;i<args.size();i++)
	{
		PrintString(args[i].c_str());
	}
	PrintString(L"");



	getchar();
}

void MathTest()
{
	// vector2
	Vector2 a = Vector2Utils::LDVector(3, 4);
	Vector2 b = Vector2Utils::LDVector(2, 5);
	PrintVector(L"a       = ", a);
	PrintVector(L"b       = ", b);

	PrintValue (L"len(a)  = ", Vector2Utils::Length(a));
	PrintValue (L"len(b)  = ", Vector2Utils::Length(b));

	PrintValue (L"lensq(a)= ", Vector2Utils::LengthSquared(a));
	PrintValue (L"lensq(b)= ", Vector2Utils::LengthSquared(b));

	PrintVector(L"max(a,b)= ", Vector2Utils::Maximize(a, b));
	PrintVector(L"min(a,b)= ", Vector2Utils::Minimize(a, b));
	PrintValue (L"crs(a,b)= ", Vector2Utils::Cross(a, b));
	PrintVector(L"a + b   = ", Vector2Utils::Add(a, b));
	PrintVector(L"a - b   = ", Vector2Utils::Subtract(a, b));
	PrintValue (L"dst(a,b)= ", Vector2Utils::Distance(a, b));
	PrintValue (L"dot(a,b)= ", Vector2Utils::Dot(a, b));
	PrintVector(L"n(a)    = ", Vector2Utils::Normalize(a));
	PrintVector(L"n(b)    = ", Vector2Utils::Normalize(b));
	PrintVector(L"-(a)    = ", Vector2Utils::Negate(a));
	PrintVector(L"-(b)    = ", Vector2Utils::Negate(b));
	PrintVector(L"3(a)    = ", Vector2Utils::Multiply(a, 3));
	PrintVector(L"2(b)    = ", Vector2Utils::Multiply(b, 2));
	PrintVector(L"mod(a,b)= ", Vector2Utils::Modulate(a, b));
	//PrintVector(L"div(a,b)= ", Vector2Utils::Divide(a, b));
	PrintVector(L"do2(a,b)= ", Vector2Utils::Dot2(a, b));

	float buffer[4];
	Vector2Utils::Store(a, buffer);
	PrintValue(L"st(a)x   =", buffer[0]);
	PrintValue(L"st(a)y   =", buffer[1]);


	wprintf(L"\n");wprintf(L"\n");





	D3DXVECTOR2 da = D3DXVECTOR2(3,4);
	D3DXVECTOR2 db = D3DXVECTOR2(2,5);
	PrintVector(L"a       = ", da); 
	PrintVector(L"b       = ", db);


	PrintValue (L"len(a)  = ", D3DXVec2Length(&da));
	PrintValue (L"len(b)  = ", D3DXVec2Length(&db));


	PrintValue (L"lensq(a)= ", D3DXVec2LengthSq(&da));
	PrintValue (L"lensq(b)= ", D3DXVec2LengthSq(&db));


	D3DXVECTOR2 dc;
	D3DXVec2Maximize(&dc, &da, &db);
	PrintVector(L"max(a,b)= ", dc);
	D3DXVec2Minimize(&dc, &da, &db);
	PrintVector(L"min(a,b)= ", dc);
	PrintValue (L"crs(a,b)= ", D3DXVec2CCW(&da, &db));


	D3DXVec2Add(&dc, &da, &db);
	PrintVector(L"a + b   = ", dc);

	D3DXVec2Subtract(&dc, &da, &db);
	PrintVector(L"a - b   = ", dc);
	PrintValue (L"dst(a,b)= ", D3DXVec2Length(&dc));
	float dot = D3DXVec2Dot(&da, &db);
	PrintValue (L"dot(a,b)= ", dot);

	D3DXVec2Normalize(&dc, &da);
	PrintVector(L"n(a)    = ", dc);
	D3DXVec2Normalize(&dc, &db);
	PrintVector(L"n(b)    = ", dc);

	dc = -da;
	PrintVector(L"-(a)    = ", dc);
	dc = -db;
	PrintVector(L"-(b)    = ", dc);

	dc = da * 3;
	PrintVector(L"3(a)    = ", dc);

	dc = db * 2;
	PrintVector(L"2(b)    = ", dc);

	dc = da;
	dc.x *= db.x; dc.y *= db.y;
	PrintVector(L"mod(a,b)= ", dc);
	//dc = da;
	//dc.x /= db.x; dc.y /= db.y;
	//PrintVector(L"div(a,b)= ", dc);
	PrintVector(L"do2(a,b)= ", D3DXVECTOR2(dot, dot));

	getchar();
}

void RandomTest()
{

}

int _tmain(int argc, _TCHAR* argv[])
{
	//MathTest();
	

	
	return 0;
}

