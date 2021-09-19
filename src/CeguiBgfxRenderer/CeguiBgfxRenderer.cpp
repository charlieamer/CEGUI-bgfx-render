#include "CeguiBgfxRenderer/CeguiBgfxRenderer.h"
#include "CeguiBgfxRenderer/CeguiBgfxGeometry.h"
#include "CeguiBgfxRenderer/CeguiBgfxTextureTarget.h"
#include "CeguiBgfxRenderer/CeguiBgfxViewportTarget.h"
#include "fs_textured.bin.h"
#include "vs_textured.bin.h"
#include <bx/file.h>
#include <bgfx/bgfx.h>
#include <CEGUI/Exceptions.h>

namespace CEGUI
{
  static bx::DefaultAllocator defaultAlloc;

  static const bgfx::Memory *loadMem(const unsigned char *data)
  {
    return bgfx::makeRef(data, strlen((const char *)data) + 1);
  }

  static bgfx::ShaderHandle loadShader(const unsigned char *data, const char *name)
  {
    bgfx::ShaderHandle handle = bgfx::createShader(loadMem(data));
    ;

    bgfx::setName(handle, name);

    return handle;
  }

  CeguiBgfxRenderer::CeguiBgfxRenderer(bool callBgfxFrame) : d_callBgfxFrame(callBgfxFrame)
  {
    const bgfx::Stats *temp = bgfx::getStats();

    // bgfx::ShaderHandle vsh = loadShader(vs_textured_bin, "CEGUI VS Textured");
    // bgfx::ShaderHandle fsh = loadShader(fs_textured_bin, "CEGUI FS Textured");
    d_textureUniform = bgfx::createUniform("s_texture0", bgfx::UniformType::Sampler);
    // d_program = bgfx::createProgram(vsh, fsh, true);

    d_defaultTarget = new CeguiBgfxViewportTarget(*this);
    d_activeRenderTarget = d_defaultTarget;
    updateScreenSize(bgfx::getStats()->width, bgfx::getStats()->height);
  }

  CeguiBgfxRenderer &CeguiBgfxRenderer::bootstrapSystem(bool callBgfxFrame)
  {
    CeguiBgfxRenderer &renderer(create(callBgfxFrame));

    DefaultResourceProvider *rp = new DefaultResourceProvider();
    System::create(renderer, rp);
    
    return renderer;
  }

  CeguiBgfxRenderer &CeguiBgfxRenderer::create(bool callBgfxFrame)
  {
    return *new CeguiBgfxRenderer(callBgfxFrame);
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
    if (d_defaultTarget != nullptr)
    {
      Rectf screenArea = d_defaultTarget->getArea();
      screenArea.d_max = Vector2<float>(width, height);

      d_defaultTarget->setArea(screenArea);
    }
    if (System::getSingletonPtr())
    {
      RenderTargetEventArgs args(&getDefaultRenderTarget());
      getDefaultRenderTarget().fireEvent(RenderTarget::EventAreaChanged, args);
    }
  }

  CeguiBgfxRenderer::~CeguiBgfxRenderer()
  {
  }

  RenderTarget &CeguiBgfxRenderer::getDefaultRenderTarget()
  {
    return *d_defaultTarget;
  }

  GeometryBuffer &CeguiBgfxRenderer::createGeometryBuffer()
  {
    CeguiBgfxGeometry *ret = new CeguiBgfxGeometry(*this);
    d_geometryBuffers.push_back(ret);
    ret->setProgramHandle(d_program, d_textureUniform);
    return *ret;
  }

  void CeguiBgfxRenderer::destroyGeometryBuffer(const GeometryBuffer &buffer)
  {
    auto i = std::find(d_geometryBuffers.begin(), d_geometryBuffers.end(), &buffer);

    if (d_geometryBuffers.end() != i)
    {
      d_geometryBuffers.erase(i);
      delete &buffer;
    }
    else
    {
      CEGUI_THROW(NullObjectException("Geometry to destroy was not found"));
    }
  }

  void CeguiBgfxRenderer::destroyAllGeometryBuffers()
  {
    while (!d_geometryBuffers.empty())
      destroyGeometryBuffer(**d_geometryBuffers.begin());
  }

  TextureTarget *CeguiBgfxRenderer::createTextureTarget()
  {
    CeguiBgfxTextureTarget *t = new CeguiBgfxTextureTarget(*this);
    d_textureTargets.push_back(t);
    return t;
  }

  void CeguiBgfxRenderer::destroyTextureTarget(TextureTarget *target)
  {
    static_cast<CeguiBgfxTextureTarget*>(target)->destroy();
    auto it = std::find(d_textureTargets.begin(), d_textureTargets.end(), target);
    if (it == d_textureTargets.end()) {
      CEGUI_THROW(InvalidRequestException("Tried to destroy texture target, but it was not found in memory."));
    }
    d_textureTargets.erase(it);
    CEGUI_DELETE_AO target;
  }

  void CeguiBgfxRenderer::destroyAllTextureTargets()
  {
    while(d_textureTargets.size())
    {
      destroyTextureTarget(static_cast<TextureTarget*>(d_textureTargets[0]));
    }
  }

  Texture &CeguiBgfxRenderer::createTexture(const String &name)
  {
    CeguiBgfxTexture *ret = new CeguiBgfxTexture(name);
    d_textures[name.c_str()] = ret;
    return *ret;
  }

  Texture &CeguiBgfxRenderer::createTexture(const String &name, const String &filename, const String &resourceGroup)
  {
    CeguiBgfxTexture *ret = (CeguiBgfxTexture *)&createTexture(name);
    ret->loadFromFile(filename, resourceGroup);
    return *ret;
  }

  Texture &CeguiBgfxRenderer::createTexture(const String &name, const Sizef &size)
  {
    return createTexture(name);
  }

  void CeguiBgfxRenderer::destroyTexture(Texture &texture)
  {
    destroyTexture(texture.getName());
  }

  void CeguiBgfxRenderer::destroyTexture(const String &ceguiName)
  {
    std::string name = ceguiName.c_str();
    if (d_textures.count(name) != 0)
    {
      d_textures[name]->destroy();
      CEGUI_DELETE_AO d_textures[name];
      d_textures.erase(d_textures.find(name));
    }
  }

  void CeguiBgfxRenderer::destroyAllTextures()
  {
    for (auto pair : d_textures)
    {
      pair.second->destroy();
      CEGUI_DELETE_AO pair.second;
    }
    d_textures.clear();
  }

  Texture &CeguiBgfxRenderer::getTexture(const String &name) const
  {
    return *d_textures.at(name.c_str());
  }

  bool CeguiBgfxRenderer::isTextureDefined(const String &name) const
  {
    return d_textures.count(name.c_str()) > 0;
  }

  void CeguiBgfxRenderer::activateTarget(RenderTarget* target)
  {
    d_activeRenderTarget = target;
  }

  unsigned char CeguiBgfxRenderer::createNewViewID()
  {
    for (unsigned char test = 254; test > 0; test--)
    {
      bool found = std::count_if(
        d_textureTargets.begin(),
        d_textureTargets.end(),
        [test](const CeguiBgfxTextureTarget* target) { return target->getViewId() == test; }
      );
      if (!found) return test;
    }
    CEGUI_THROW(GenericException("No available view IDs"));
  }

  unsigned char CeguiBgfxRenderer::getActiveViewID()
  {
    if (!d_activeRenderTarget) {
      CEGUI_THROW(CEGUI::NullObjectException("No active render target"));
    }
    auto asViewportTarget = dynamic_cast<CeguiBgfxViewportTarget*>(d_activeRenderTarget);
    if (asViewportTarget) {
      return asViewportTarget->getViewId();
    }
    auto asTextureTarget = dynamic_cast<CeguiBgfxTextureTarget*>(d_activeRenderTarget);
    if (asTextureTarget) {
      return asTextureTarget->getViewId();
    }
    CEGUI_THROW(CEGUI::UnknownObjectException("Couldn't find appropriate class for active render target"));
  }

  void CeguiBgfxRenderer::beginRendering()
  {
#ifdef DEBUG
    bgfx::dbgTextPrintf(0, 1, 0x4f, "BGFX renderer works!!!");
#endif
  }

  void CeguiBgfxRenderer::endRendering()
  {
    if (d_callBgfxFrame) {
      bgfx::frame();
    }
  }

  void CeguiBgfxRenderer::setDisplaySize(const Sizef &size)
  {
    updateScreenSize(size.d_width, size.d_height);
  }

  const Vector2f &CeguiBgfxRenderer::getDisplayDPI() const
  {
    static Vector2f tmp = Vector2f(72, 72);
    return tmp;
  }

  uint CeguiBgfxRenderer::getMaxTextureSize() const
  {
    auto caps = bgfx::getCaps();
    return caps->limits.maxTextureSize;
  }

  const String &CeguiBgfxRenderer::getIdentifierString() const
  {
    static String tmp = "Bgfx Renderer";
    return tmp;
  }

  void CeguiBgfxRenderer::throwIfNameExists(const String &name) const
  {
    throw "not yet implemented";
  }
}
