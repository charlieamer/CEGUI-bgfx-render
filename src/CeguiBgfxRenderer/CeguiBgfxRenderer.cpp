#include "CeguiBgfxRenderer/CeguiBgfxRenderer.h"
#include "CeguiBgfxRenderer/CeguiBgfxGeometry.h"
#include "CeguiBgfxRenderer/CeguiBgfxTextureTarget.h"
#include "CeguiBgfxRenderer/CeguiBgfxViewportTarget.h"
#include "fs_textured.bin.h"
#include "vs_textured.bin.h"
#include <bx/file.h>
#include <bgfx/bgfx.h>
class FileReader : public bx::FileReader
{
	typedef bx::FileReader super;

public:
	virtual bool open(const bx::FilePath& _filePath, bx::Error* _err) override
	{
		CEGUI::String filePath(_filePath.getCPtr());
		//filePath.append(_filePath.get());
		return super::open(filePath.c_str(), _err);
	}
};

namespace CEGUI
{
	static bx::DefaultAllocator defaultAlloc;

	static const bgfx::Memory* loadMem(const unsigned char* data)
	{
		return bgfx::makeRef(data, strlen((const char*)data) + 1);
	}

	static bgfx::ShaderHandle loadShader(const unsigned char* data, const char* name)
	{
		bgfx::ShaderHandle handle = bgfx::createShader(loadMem(data));;

		bgfx::setName(handle, name);

		return handle;
	}


	GuiBgfxRenderer::GuiBgfxRenderer()
	{
		//d_renderBuffers.push_back(new GuiBgfxRenderTarget(*this));
		const bgfx::Stats* temp = bgfx::getStats();

		bgfx::ShaderHandle vsh = loadShader(vs_textured_bin, "CEGUI VS Textured");
		bgfx::ShaderHandle fsh = loadShader(fs_textured_bin, "CEGUI FS Textured");
		d_textureUniform = bgfx::createUniform("s_texture0", bgfx::UniformType::Sampler);
		updateScreenSize(bgfx::getStats()->width, bgfx::getStats()->height);

		//BX_DELETE(d_allocator, s_fileReader);

		d_viewId = 1;
		

		//screenArea ;
		d_defaultTarget = new GuiBgfxViewportTarget(*this);
	}

	GuiBgfxRenderer & GuiBgfxRenderer::bootstrapSystem()
	{
		GuiBgfxRenderer& renderer(create());

		DefaultResourceProvider* rp = new DefaultResourceProvider();
		System::create(renderer, rp);
		return renderer;
	}

	GuiBgfxRenderer& GuiBgfxRenderer::create() {
		return *new GuiBgfxRenderer();
	}

	void GuiBgfxRenderer::destroy()
	{
		destroyAllGeometryBuffers();
		destroyAllTextures();
		destroyAllTextureTargets();
		bgfx::destroy(d_program);
		bgfx::destroy(d_textureUniform);
		delete this;
	}

	void GuiBgfxRenderer::updateScreenSize(int width, int height)
	{
		d_displaySize = Sizef(width, height);
		if(d_defaultTarget != nullptr){
					Rectf screenArea = d_defaultTarget->getArea();
		screenArea.d_max =(Vector2<float>(width, height));

		d_defaultTarget->setArea(screenArea);
		}
		if (System::getSingletonPtr()) {
			RenderTargetEventArgs args(&getDefaultRenderTarget());
			getDefaultRenderTarget().fireEvent(RenderTarget::EventAreaChanged, args);

		}
	}

	GuiBgfxRenderer::~GuiBgfxRenderer()
	{
	}

	RenderTarget & GuiBgfxRenderer::getDefaultRenderTarget()
	{
		return *d_defaultTarget;
	}

	GeometryBuffer & GuiBgfxRenderer::createGeometryBuffer()
	{
		GuiBgfxGeometry* ret = new GuiBgfxGeometry(*this);
		d_geometryBuffers.push_back(ret);
		ret->setProgramHandle(d_program, d_textureUniform);
		return *ret;
	}

	void GuiBgfxRenderer::destroyGeometryBuffer(const GeometryBuffer & buffer)
	{

		GeometryBufferList::iterator i = std::find(d_geometryBuffers.begin(),
			d_geometryBuffers.end(),
			&buffer);

		if (d_geometryBuffers.end() != i) {
			d_geometryBuffers.erase(i);
			delete &buffer;
		}
		else {
			CEGUI_THROW(NullObjectException("Geometry to destroy was not found"));
		}
	}

	void GuiBgfxRenderer::destroyAllGeometryBuffers()
	{
		while (!d_geometryBuffers.empty())
			destroyGeometryBuffer(**d_geometryBuffers.begin());
	}

	TextureTarget * GuiBgfxRenderer::createTextureTarget()
	{
		//TextureTarget* t = new GuiBgfxTextureTarget();

		return nullptr;
	}

	void GuiBgfxRenderer::destroyTextureTarget(TextureTarget * target)
	{
	}

	void GuiBgfxRenderer::destroyAllTextureTargets()
	{
	}

	Texture & GuiBgfxRenderer::createTexture(const String & name)
	{
		GuiBgfxTexture* ret = new GuiBgfxTexture(name);
		d_textures[name.c_str()] = ret;
		return *ret;
	}

	Texture & GuiBgfxRenderer::createTexture(const String & name, const String & filename, const String & resourceGroup)
	{
		GuiBgfxTexture *ret = (GuiBgfxTexture*)&createTexture(name);
		ret->loadFromFile(filename, resourceGroup);
		return *ret;
	}

	Texture & GuiBgfxRenderer::createTexture(const String & name, const Sizef & size)
	{
		return createTexture(name);
	}

	void GuiBgfxRenderer::destroyTexture(Texture & texture)
	{
		destroyTexture(texture.getName());
	}

	void GuiBgfxRenderer::destroyTexture(const String & ceguiName)
	{
		std::string name = ceguiName.c_str();
		if (d_textures.count(name) != 0) {
			d_textures[name]->destroy();
			delete d_textures[name];
			d_textures.erase(d_textures.find(name));
		}
	}

	void GuiBgfxRenderer::destroyAllTextures()
	{
		for (auto pair : d_textures) {
			pair.second->destroy();
			delete pair.second;
		}
		d_textures.clear();
	}

	Texture & GuiBgfxRenderer::getTexture(const String & name) const
	{
		return *d_textures.at(name.c_str());
	}

	bool GuiBgfxRenderer::isTextureDefined(const String & name) const
	{
		return d_textures.count(name.c_str()) > 0;
	}

	void GuiBgfxRenderer::beginRendering()
	{
		
		//unsigned char pass = 0;
		//d_default
		//for (auto& target : d_renderBuffers) {
		//	if (typeid(*target) == typeid(GuiBgfxTextureTarget)) {
		//		target->setPassId(256 - d_renderBuffers.size() + pass++);
		//	}
		//	else {
		//		target->setPassId(1);
		//	}
		//}
	}

	void GuiBgfxRenderer::endRendering()
	{
	}

	void GuiBgfxRenderer::setDisplaySize(const Sizef & size)
	{
		updateScreenSize(size.d_width, size.d_height);
	}

	const Vector2f & GuiBgfxRenderer::getDisplayDPI() const
	{
		static Vector2f tmp = Vector2f(72, 72);
		return tmp;
	}

	uint GuiBgfxRenderer::getMaxTextureSize() const
	{
		auto caps = bgfx::getCaps();
		return caps->limits.maxTextureSize;
	}

	const String & GuiBgfxRenderer::getIdentifierString() const
	{
		static String tmp = "Bgfx Renderer";
		return tmp;
	}

	//void GuiBgfxRenderer::activateTarget(GuiBgfxRenderTarget * target)
	//{
	//	currentPass = target->getPassId();
	//}

	//void GuiBgfxRenderer::activateRenderTarget()
	//{
	//	activateTarget(*d_renderBuffers.begin());
	//}

	bgfx::ViewId GuiBgfxRenderer::getViewID() const
	{
		 return d_viewId; 
	}

	void GuiBgfxRenderer::throwIfNameExists(const String & name) const
	{
		throw "not yet implemented";
	}
}
//template<> GuiBgfxRenderer* Singleton<GuiBgfxRenderer>::ms_Singleton = 0;

