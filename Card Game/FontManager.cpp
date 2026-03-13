#include "FontManager.h"

FontManager Font;

void FontManager::Init()
{
    Small = CreateFontToHandle("MS ÉSÉVÉbÉN", 16, 3);
    Normal = CreateFontToHandle("MS ÉSÉVÉbÉN", 32, 3);
    Big = CreateFontToHandle("ÇlÇr ñæí©", 64, 3);
}

void FontManager::End()
{
    DeleteFontToHandle(Small);
    DeleteFontToHandle(Normal);
    DeleteFontToHandle(Big);
}