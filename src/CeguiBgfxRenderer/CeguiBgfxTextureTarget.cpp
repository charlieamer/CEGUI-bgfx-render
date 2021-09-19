#include "CeguiBgfxRenderer/CeguiBgfxTextureTarget.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/RenderQueue.h"
#include "CEGUI/GeometryBuffer.h"
#include "CEGUI/PropertyHelper.h"
#include <bgfx/bgfx.h>

namespace CEGUI
{
	CeguiBgfxTextureTarget::CeguiBgfxTextureTarget(CeguiBgfxRenderer& owner) : CeguiBgfxRenderTarget(owner)
	{
		char tmp[30];
		sprintf(tmp, "Texture render target - %u", getViewId());
		texture = static_cast<CEGUI::CeguiBgfxTexture*>(&owner.createTexture(tmp));
	}

	CeguiBgfxTextureTarget::~CeguiBgfxTextureTarget()
	{
	}

	bool CeguiBgfxTextureTarget::isImageryCache() const
	{
		return true;
	}

	void CeguiBgfxTextureTarget::activate()
	{
		bgfx::setViewFrameBuffer(getViewId(), handle);
		CeguiBgfxRenderTarget::activate();
	}

	void CeguiBgfxTextureTarget::deactivate()
	{
		CeguiBgfxRenderTarget::deactivate();
	}

	void CeguiBgfxTextureTarget::clear()
	{
		// TODO: Implement this
	}
	Texture & CeguiBgfxTextureTarget::getTexture() const
	{
		return *texture;
	}

	void CeguiBgfxTextureTarget::declareRenderSize(const Sizef & sz)
	{
		if (sz.d_width == 0 || sz.d_height == 0) {
			CEGUI::Logger::getSingleton().logEvent("Tried to create texture render target with size 0");
		} else {
			handle = bgfx::createFrameBuffer(sz.d_width, sz.d_height, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT_WRITE_ONLY);
			d_area = Rectf({0, 0}, sz);
		}
	}

	bool CeguiBgfxTextureTarget::isRenderingInverted() const
	{
		return true;
	}

	void CeguiBgfxTextureTarget::destroy()
	{
		if (texture) {
			texture->destroy();
		}
		bgfx::destroy(handle);
	}
}

#include "CeguiBgfxRenderTarget.inl"