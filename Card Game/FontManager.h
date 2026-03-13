#pragma once
#include "DxLib.h"

struct FontManager
{
    int Small;
    int Normal;
    int Big;

    void Init();
    void End();
};

extern FontManager Font;