#include "FontManager.h"

FontManager Font;

void FontManager::Init()
{
    Small = CreateFontToHandle(_T("MS ゴシック"), 16, 3);
    Normal = CreateFontToHandle(_T("MS ゴシック"), 32, 3);
    Big = CreateFontToHandle(_T("MS ゴシック"), 64, 5);
}

void FontManager::End()
{
    DeleteFontToHandle(Small);
    DeleteFontToHandle(Normal);
    DeleteFontToHandle(Big);
}