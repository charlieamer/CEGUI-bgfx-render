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
		texture = NULL;
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
	}

	void CeguiBgfxTextureTarget::deactivate()
	{
	}

	void CeguiBgfxTextureTarget::clear()
	{
		memset(textureMemory->data, 0, textureMemory->size);
		bgfx::updateTexture2D(texture->getHandle(), 0, 0, 0, 0, texture->getSize().d_width, texture->getSize().d_height, textureMemory);
	}
	Texture & CeguiBgfxTextureTarget::getTexture() const
	{
		return *texture;
	}

	void CeguiBgfxTextureTarget::declareRenderSize(const Sizef & sz)
	{
		/*
		if (texture) {
			CEGUI_THROW(new RendererException("Texture for render target alredy initialized"));
		}
		texture = new CeguiBgfxTexture("Texture render target");
		uint8_t* data = new uint8_t[sz.d_width * sz.d_height * 4];
		textureMemory = bgfx::makeRef(data, sz.d_height * sz.d_width * 4);
		texture->loadFromMemory(textureMemory, sz, Texture::PixelFormat::PF_RGBA);
		*/
		handle = bgfx::createFrameBuffer(sz.d_width, sz.d_height, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT_WRITE_ONLY);
		d_area = Rectf(0, 0, sz.d_width, sz.d_height);
	}

	bool CeguiBgfxTextureTarget::isRenderingInverted() const
	{
		return true;
	}

	void CeguiBgfxTextureTarget::destroy()
	{
		if (texture) {
			texture->destroy();
			delete textureMemory->data;
		}
		bgfx::destroy(handle);
	}
}

#include "CeguiBgfxRenderTarget.inl"