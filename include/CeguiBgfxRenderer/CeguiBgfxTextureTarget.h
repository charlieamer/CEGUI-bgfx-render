#pragma once
#include "CeguiBgfxRenderer/CeguiBgfxRenderer.h"
#include "CEGUI/TextureTarget.h"
#include "CeguiBgfxRenderer/CeguiBgfxRenderTarget.h"
#include "CEGUI/Rect.h"
#include <bgfx/bgfx.h>

#include "CeguiBgfxRenderer/CeguiBgfxTexture.h"
namespace CEGUI
{
	class CeguiBgfxTextureTarget : /*public TextureTarget,*/ public CeguiBgfxRenderTarget<TextureTarget>
	{
		bgfx::FrameBufferHandle handle;
		const bgfx::Memory* textureMemory;
	public:
		CeguiBgfxTextureTarget(CeguiBgfxRenderer& owner);
		~CeguiBgfxTextureTarget();

		// Inherited via TextureTarget
		virtual bool isImageryCache() const override;
		virtual void activate() override;
		virtual void deactivate() override;
		virtual void clear() override;
		virtual Texture & getTexture() const override;
		virtual void declareRenderSize(const Sizef & sz) override;
		virtual bool isRenderingInverted() const override;

		CeguiBgfxTexture* texture;
		virtual void destroy() override;
	};

}