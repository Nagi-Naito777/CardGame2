#include "FontManager.h"

FontManager Font;

void FontManager::Init()
{
    Small = CreateFontToHandle(_T("MS ÉSÉVÉbÉN"), 16, 3);
    Normal = CreateFontToHandle(_T("MS ÉSÉVÉbÉN"), 32, 3);
    Big = CreateFontToHandle(_T("ÇlÇr ñæí©"), 64, 3);
}

void FontManager::End()
{
    DeleteFontToHandle(Small);
    DeleteFontToHandle(Normal);
    DeleteFontToHandle(Big);
}