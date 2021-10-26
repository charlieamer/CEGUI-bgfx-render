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
		handle = BGFX_INVALID_HANDLE;
		char tmp[30];
		sprintf(tmp, "Texture render target - %u", getViewId());
		texture = static_cast<CEGUI::CeguiBgfxTexture*>(&owner.createTexture(tmp));
		logInfo("Initialize render target");
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
		logInfo("Activating texture target");
		bgfx::setViewFrameBuffer(getViewId(), handle);
		CeguiBgfxRenderTarget::activate();
	}

	void CeguiBgfxTextureTarget::deactivate()
	{
		logInfo("Deactivating texture target");
		CeguiBgfxRenderTarget::deactivate();
	}

	void CeguiBgfxTextureTarget::clear()
	{
		logWarn("Texture target clear");
		// TODO: Implement this
	}
	Texture & CeguiBgfxTextureTarget::getTexture() const
	{
		return *texture;
	}

	void CeguiBgfxTextureTarget::declareRenderSize(const Sizef & sz)
	{
		if (sz.d_width == 0 || sz.d_height == 0) {
			logWarn("Tried to create texture render target with size 0");
		} else {
			if (bgfx::isValid(handle)) {
				destroy();
			}
			texture->loadFromMemory(nullptr, sz, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
			auto textureHandle = texture->getHandle();
			handle = bgfx::createFrameBuffer(1, &textureHandle);
			
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
		if (isValid(handle)) {
			bgfx::destroy(handle);
		}
		bgfx::setViewFrameBuffer(getViewId(), BGFX_INVALID_HANDLE);
		logInfo("Destroying texture target");
	}
}

#include "CeguiBgfxRenderTarget.inl"