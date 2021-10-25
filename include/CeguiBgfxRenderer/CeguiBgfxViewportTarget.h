#pragma once
#include "CeguiBgfxRenderer/CeguiBgfxRenderTarget.h"
#include "CEGUI/Rect.h"
namespace CEGUI
{
	class CeguiBgfxViewportTarget : public CeguiBgfxRenderTarget<RenderTarget>
	{
	public:
		CeguiBgfxViewportTarget(CeguiBgfxRenderer& owner);
		CeguiBgfxViewportTarget(CeguiBgfxRenderer& owner, const Rectf& area);

		virtual bool isImageryCache() const override;
		virtual void activate() override;
	};
}
