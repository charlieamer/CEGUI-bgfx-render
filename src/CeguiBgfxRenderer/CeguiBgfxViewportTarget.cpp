#include "CeguiBgfxRenderer/CeguiBgfxViewportTarget.h"
#include "CEGUI/Exceptions.h"

namespace CEGUI 
{
	CEGUI::CeguiBgfxViewportTarget::CeguiBgfxViewportTarget(CeguiBgfxRenderer & owner) :
		CeguiBgfxRenderTarget<>(owner)
	{
		const bgfx::Stats* stats = bgfx::getStats();
		Rectf init_area(Vector2f(0,0),
						Sizef(stats->width, stats->height));
		setArea(init_area);
	}
	CeguiBgfxViewportTarget::CeguiBgfxViewportTarget(CeguiBgfxRenderer & owner, const Rectf & area) :
		CeguiBgfxRenderTarget<>(owner)
	{
		setArea(area);
	}
	bool CeguiBgfxViewportTarget::isImageryCache() const
	{
		return false;
	}
}

#include "CeguiBgfxRenderTarget.inl"