#pragma once

#include "CEGuiGLFWSharedBase.h"

class CEGuiBgfxBaseApplication : public CEGuiGLFWSharedBase
{
public:
    CEGuiBgfxBaseApplication();

protected:
    static void setGLFWWindowCreationHints();
};