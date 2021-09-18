#include "CeguiBgfxRenderer/CeguiBgfxTexture.h"
#include <bgfx/bgfx.h>

namespace CEGUI
{
  CeguiBgfxTexture::CeguiBgfxTexture(String name)
  {
    this->name = name;
    handle = BGFX_INVALID_HANDLE;
    data = nullptr;
  }

  CeguiBgfxTexture::~CeguiBgfxTexture()
  {
    destroy();
  }

  const String &CeguiBgfxTexture::getName() const
  {
    return name;
  }

  const Sizef &CeguiBgfxTexture::getSize() const
  {
    return size;
  }

  const Sizef &CeguiBgfxTexture::getOriginalDataSize() const
  {
    return size;
  }

  const Vector2f &CeguiBgfxTexture::getTexelScaling() const
  {
    return texel;
  }

  void CeguiBgfxTexture::loadFromFile(const String &filename, const String &resourceGroup)
  {
    // Note from PDT:
    // There is somewhat tight coupling here between OpenGLTexture and the
    // ImageCodec classes - we have intimate knowledge of how they are
    // implemented and that knowledge is relied upon in an unhealthy way; this
    // should be addressed at some stage.

    // load file to memory via resource provider
    RawDataContainer texFile;
    System::getSingleton().getResourceProvider()->loadRawDataContainer(filename, texFile, resourceGroup);

    // get and check existence of CEGUI::System (needed to access ImageCodec)
    System *sys = System::getSingletonPtr();
    if (!sys)
      CEGUI_THROW(RendererException(
          "CEGUI::System object has not been created: "
          "unable to access ImageCodec."));

    Texture *res = sys->getImageCodec().load(texFile, this);

    // unload file data buffer
    System::getSingleton().getResourceProvider()->unloadRawDataContainer(texFile);

    if (!res)
      // It's an error
      CEGUI_THROW(RendererException(
          sys->getImageCodec().getIdentifierString() +
          " failed to load image '" + filename + "'."));
  }

  void CeguiBgfxTexture::loadFromMemory(const void *buffer, const Sizef &buffer_size, PixelFormat pixel_format)
  {
    setSize(buffer_size);

    char tmp[200];
    sprintf(tmp, "[CeguiBgfxTexture::loadFromMemory] Loading from memory: %dx%d (name: %s)", (int)buffer_size.d_width, (int)buffer_size.d_height, name.c_str());
    Logger::getSingleton().logEvent(tmp);

    long bytes;
    bgfx::TextureFormat::Enum format;
    switch (pixel_format)
    {
    case PixelFormat::PF_RGB:
      format = bgfx::TextureFormat::RGB8;
      bytes = buffer_size.d_width * buffer_size.d_height * 3;
      break;
    case PixelFormat::PF_RGBA:
      format = bgfx::TextureFormat::RGBA8;
      bytes = buffer_size.d_width * buffer_size.d_height * 4;
      break;
    default:
      CEGUI_THROW(RendererException("Unsupported pixel format"));
    }
    destroy();
    data = new unsigned char[bytes];
    memcpy(data, buffer, bytes);
    auto mem = bgfx::makeRef(data, bytes);
    loadFromMemory(mem, buffer_size, format);
  }

  void CeguiBgfxTexture::loadFromMemory(const bgfx::Memory *mem, const Sizef &buffer_size, bgfx::TextureFormat::Enum format)
  {
    handle = bgfx::createTexture2D(
        uint16_t(buffer_size.d_width), uint16_t(buffer_size.d_height), false, 1, format, 0, mem);
  }

  void CeguiBgfxTexture::blitFromMemory(const void *sourceData, const Rectf &area)
  {
    bgfx::updateTexture2D(handle, 0, 0,
                          area.d_min.d_x, area.d_min.d_y,
                          area.d_max.d_x - area.d_min.d_x, area.d_max.d_y - area.d_min.d_y,
                          bgfx::makeRef(sourceData, size.d_width * size.d_height));
  }

  void CeguiBgfxTexture::blitToMemory(void *targetData)
  {
    bgfx::readTexture(handle, targetData);
  }

  bool CeguiBgfxTexture::isPixelFormatSupported(const PixelFormat fmt) const
  {
    return true;
  }

  void CeguiBgfxTexture::destroy()
  {
    if (handle.idx != bgfx::kInvalidHandle)
    {
      char tmp[200];
      sprintf(tmp, "[CeguiBgfxTexture::destroy] Destroying texture %s", name.c_str());
      Logger::getSingleton().logEvent(tmp);

      bgfx::destroy(handle);
      // TODO: handle this properly. Right now it crashes in samples
      // if (data) delete[] data;
    }
  }
  bgfx::TextureHandle CeguiBgfxTexture::getHandle() const
  {
    return handle;
  }
  void CeguiBgfxTexture::setHandle(bgfx::TextureHandle newHandle)
  {
    handle = newHandle;
  }

  void CeguiBgfxTexture::setSize(const Sizef &value)
  {
    size = value;
    texel = Vector2f(1.0f / size.d_width, 1.0f / size.d_height);
  }

}