#include "CeguiBgfxRenderer/CeguiBgfxViewportTarget.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/Logger.h"

namespace CEGUI 
{
	CeguiBgfxViewportTarget::CeguiBgfxViewportTarget(CeguiBgfxRenderer & owner) :
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

	void CeguiBgfxViewportTarget::activate()
	{
		logInfo("Activating viewport target");
		bgfx::setViewFrameBuffer(getViewId(), BGFX_INVALID_HANDLE);
		CeguiBgfxRenderTarget::activate();
	}
}

#include "CeguiBgfxRenderTarget.inl"