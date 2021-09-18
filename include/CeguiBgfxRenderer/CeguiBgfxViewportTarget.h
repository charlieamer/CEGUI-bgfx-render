#pragma once
#include "CeguiBgfxRenderer/CeguiBgfxRenderTarget.h"
#include "CEGUI/Rect.h"
namespace CEGUI
{
	class GuiBgfxViewportTarget : public GuiBgfxRenderTarget<>
	{
	public:
		GuiBgfxViewportTarget(GuiBgfxRenderer& owner);
		GuiBgfxViewportTarget(GuiBgfxRenderer& owner, const Rectf& area);

		bool isImageryCache() const;
	};
}
