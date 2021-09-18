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


	CeguiBgfxRenderer::CeguiBgfxRenderer()
	{
		//d_renderBuffers.push_back(new CeguiBgfxRenderTarget(*this));
		const bgfx::Stats* temp = bgfx::getStats();

		bgfx::ShaderHandle vsh = loadShader(vs_textured_bin, "CEGUI VS Textured");
		bgfx::ShaderHandle fsh = loadShader(fs_textured_bin, "CEGUI FS Textured");
		d_textureUniform = bgfx::createUniform("s_texture0", bgfx::UniformType::Sampler);
		updateScreenSize(bgfx::getStats()->width, bgfx::getStats()->height);

		//BX_DELETE(d_allocator, s_fileReader);

		d_viewId = 1;
		

		//screenArea ;
		d_defaultTarget = new CeguiBgfxViewportTarget(*this);
	}

	CeguiBgfxRenderer & CeguiBgfxRenderer::bootstrapSystem()
	{
		CeguiBgfxRenderer& renderer(create());

		DefaultResourceProvider* rp = new DefaultResourceProvider();
		System::create(renderer, rp);
		return renderer;
	}

	CeguiBgfxRenderer& CeguiBgfxRenderer::create() {
		return *new CeguiBgfxRenderer();
	}

	void CeguiBgfxRenderer::destroy()
	{
		destroyAllGeometryBuffers();
		destroyAllTextures();
		destroyAllTextureTargets();
		bgfx::destroy(d_program);
		bgfx::destroy(d_textureUniform);
		delete this;
	}

	void CeguiBgfxRenderer::updateScreenSize(int width, int height)
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

	CeguiBgfxRenderer::~CeguiBgfxRenderer()
	{
	}

	RenderTarget & CeguiBgfxRenderer::getDefaultRenderTarget()
	{
		return *d_defaultTarget;
	}

	GeometryBuffer & CeguiBgfxRenderer::createGeometryBuffer()
	{
		CeguiBgfxGeometry* ret = new CeguiBgfxGeometry(*this);
		d_geometryBuffers.push_back(ret);
		ret->setProgramHandle(d_program, d_textureUniform);
		return *ret;
	}

	void CeguiBgfxRenderer::destroyGeometryBuffer(const GeometryBuffer & buffer)
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

	void CeguiBgfxRenderer::destroyAllGeometryBuffers()
	{
		while (!d_geometryBuffers.empty())
			destroyGeometryBuffer(**d_geometryBuffers.begin());
	}

	TextureTarget * CeguiBgfxRenderer::createTextureTarget()
	{
		TextureTarget* t = new CeguiBgfxTextureTarget(*this);
	}

	void CeguiBgfxRenderer::destroyTextureTarget(TextureTarget * target)
	{
	}

	void CeguiBgfxRenderer::destroyAllTextureTargets()
	{
	}

	Texture & CeguiBgfxRenderer::createTexture(const String & name)
	{
		CeguiBgfxTexture* ret = new CeguiBgfxTexture(name);
		d_textures[name.c_str()] = ret;
		return *ret;
	}

	Texture & CeguiBgfxRenderer::createTexture(const String & name, const String & filename, const String & resourceGroup)
	{
		CeguiBgfxTexture *ret = (CeguiBgfxTexture*)&createTexture(name);
		ret->loadFromFile(filename, resourceGroup);
		return *ret;
	}

	Texture & CeguiBgfxRenderer::createTexture(const String & name, const Sizef & size)
	{
		return createTexture(name);
	}

	void CeguiBgfxRenderer::destroyTexture(Texture & texture)
	{
		destroyTexture(texture.getName());
	}

	void CeguiBgfxRenderer::destroyTexture(const String & ceguiName)
	{
		std::string name = ceguiName.c_str();
		if (d_textures.count(name) != 0) {
			d_textures[name]->destroy();
			delete d_textures[name];
			d_textures.erase(d_textures.find(name));
		}
	}

	void CeguiBgfxRenderer::destroyAllTextures()
	{
		for (auto pair : d_textures) {
			pair.second->destroy();
			delete pair.second;
		}
		d_textures.clear();
	}

	Texture & CeguiBgfxRenderer::getTexture(const String & name) const
	{
		return *d_textures.at(name.c_str());
	}

	bool CeguiBgfxRenderer::isTextureDefined(const String & name) const
	{
		return d_textures.count(name.c_str()) > 0;
	}

	void CeguiBgfxRenderer::beginRendering()
	{
		
		//unsigned char pass = 0;
		//d_default
		//for (auto& target : d_renderBuffers) {
		//	if (typeid(*target) == typeid(CeguiBgfxTextureTarget)) {
		//		target->setPassId(256 - d_renderBuffers.size() + pass++);
		//	}
		//	else {
		//		target->setPassId(1);
		//	}
		//}
	}

	void CeguiBgfxRenderer::endRendering()
	{
	}

	void CeguiBgfxRenderer::setDisplaySize(const Sizef & size)
	{
		updateScreenSize(size.d_width, size.d_height);
	}

	const Vector2f & CeguiBgfxRenderer::getDisplayDPI() const
	{
		static Vector2f tmp = Vector2f(72, 72);
		return tmp;
	}

	uint CeguiBgfxRenderer::getMaxTextureSize() const
	{
		auto caps = bgfx::getCaps();
		return caps->limits.maxTextureSize;
	}

	const String & CeguiBgfxRenderer::getIdentifierString() const
	{
		static String tmp = "Bgfx Renderer";
		return tmp;
	}

	//void CeguiBgfxRenderer::activateTarget(CeguiBgfxRenderTarget * target)
	//{
	//	currentPass = target->getPassId();
	//}

	//void CeguiBgfxRenderer::activateRenderTarget()
	//{
	//	activateTarget(*d_renderBuffers.begin());
	//}

	bgfx::ViewId CeguiBgfxRenderer::getViewID() const
	{
		 return d_viewId; 
	}

	void CeguiBgfxRenderer::throwIfNameExists(const String & name) const
	{
		throw "not yet implemented";
	}
}
//template<> CeguiBgfxRenderer* Singleton<CeguiBgfxRenderer>::ms_Singleton = 0;

