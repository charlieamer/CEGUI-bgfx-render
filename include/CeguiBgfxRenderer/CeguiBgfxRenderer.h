#pragma once
//#define CEGUIBASE_EXPORTS
#include "CEGUI/Base.h"
#include "CEGUI/Renderer.h"
#include "CEGUI/Size.h"
#include "CEGUI/Vector.h"
#include <map>
#include <set>
#include <vector>
#include <string>
#include <bgfx/bgfx.h>

namespace CEGUI 
{
	class CeguiBgfxTexture;
	class CeguiBgfxTextureTarget;
	class CeguiBgfxGeometry;
	class CeguiBgfxViewportTarget;

	class CeguiBgfxRenderer : public Renderer//, public Singleton<CeguiBgfxRenderer>
	{
	public:
		static CeguiBgfxRenderer& bootstrapSystem();
		static CeguiBgfxRenderer& create();

		void destroy();
		void updateScreenSize(int width, int height);
		~CeguiBgfxRenderer();

		// Inherited via Renderer
		virtual RenderTarget & getDefaultRenderTarget() override;
		virtual GeometryBuffer & createGeometryBuffer() override;
		virtual void destroyGeometryBuffer(const GeometryBuffer & buffer) override;
		virtual void destroyAllGeometryBuffers() override;
		virtual TextureTarget * createTextureTarget() override;
		virtual void destroyTextureTarget(TextureTarget * target) override;
		virtual void destroyAllTextureTargets() override;
		virtual Texture & createTexture(const String & name) override;
		virtual Texture & createTexture(const String & name, const String & filename, const String & resourceGroup) override;
		virtual Texture & createTexture(const String & name, const Sizef & size) override;
		virtual void destroyTexture(Texture & texture) override;
		virtual void destroyTexture(const String & name) override;
		virtual void destroyAllTextures() override;
		virtual Texture & getTexture(const String & name) const override;
		virtual bool isTextureDefined(const String & name) const override;
		virtual void beginRendering() override;
		virtual void endRendering() override;
		virtual void setDisplaySize(const Sizef & size) override;
		virtual const Sizef & getDisplaySize() const override {
			return d_displaySize;
		};
		virtual const Vector2f & getDisplayDPI() const override;
		virtual uint getMaxTextureSize() const override;
		virtual const String & getIdentifierString() const override;

		void activateTarget(RenderTarget* target);

		unsigned char createNewViewID();
		unsigned char getActiveViewID();

	private:
		//! helper to throw exception if name is already used.
		void throwIfNameExists(const String& name) const;
		//! helper to safely log the creation of a named texture
		static void logTextureCreation(const String& name);
		//! helper to safely log the destruction of a named texture
		static void logTextureDestruction(const String& name);

		//Rectf screenArea;
		//Sizef screenSize;
		//! What the renderer considers to be the current display size.
		Sizef d_displaySize;
		//! Container used to track Texture Targets
		std::vector<CeguiBgfxTextureTarget*> d_textureTargets;
		//! Container used to track geomitryBuffers
		std::vector<CeguiBgfxGeometry*> d_geometryBuffers;
		//! The Default RenderTareget
		CeguiBgfxViewportTarget* d_defaultTarget;
		typedef std::map<String, CeguiBgfxTexture*, StringFastLessCompare
					CEGUI_MAP_ALLOC(String, CeguiBgfxTexture*)> TextureMap;
		
		TextureMap d_textures;

		bgfx::ProgramHandle d_program;
		bgfx::UniformHandle d_textureUniform;

		RenderTarget* d_activeRenderTarget;

		CeguiBgfxRenderer();
	};
}
#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

