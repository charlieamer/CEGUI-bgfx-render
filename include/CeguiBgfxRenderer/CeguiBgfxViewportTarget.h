#pragma once
#include "CeguiBgfxRenderer/CeguiBgfxRenderTarget.h"
#include "CEGUI/Rect.h"
namespace CEGUI
{
	class CeguiBgfxViewportTarget : public CeguiBgfxRenderTarget<>
	{
	public:
		CeguiBgfxViewportTarget(CeguiBgfxRenderer& owner);
		CeguiBgfxViewportTarget(CeguiBgfxRenderer& owner, const Rectf& area);

		bool isImageryCache() const;
	};
}
