#include "CeguiBimgImageCodec/CeguiBimgImageCodec.h"
#include "CeguiBimgImageCodec/CeguiBimgImageCodecModule.h"
#include <CEGUI/Size.h>
#include <CEGUI/Exceptions.h>
#include <bimg/decode.h>
#include <string.h>

//----------------------------------------------------------------------------//
BimgImageCodec::BimgImageCodec()
    : ImageCodec("BimgImageCodec - BIMG based image codec")
{
}

//----------------------------------------------------------------------------//
BimgImageCodec::~BimgImageCodec()
{
}

//----------------------------------------------------------------------------//
CEGUI::Texture *BimgImageCodec::load(const CEGUI::RawDataContainer &fileContainer, CEGUI::Texture *result)
{
  const void *dataPtr = fileContainer.getDataPtr();
  int size = fileContainer.getSize();
  bimg::ImageContainer *imageContainer = nullptr;

  CEGUI_TRY {
    imageContainer = bimg::imageParse(getBxAllocator(), dataPtr, size);
    CEGUI::Texture::PixelFormat ceguiPf;
    bimg::TextureFormat::Enum bimgPf = imageContainer->m_format;
    switch(bimgPf) {
    case bimg::TextureFormat::RGB8:
      ceguiPf = CEGUI::Texture::PixelFormat::PF_RGB;
      break;
    case bimg::TextureFormat::RGBA8:
      ceguiPf = CEGUI::Texture::PixelFormat::PF_RGBA;
      break;
    case bimg::TextureFormat::RGBA4:
      ceguiPf = CEGUI::Texture::PixelFormat::PF_RGBA_4444;
      break;
    case bimg::TextureFormat::R5G6B5:
      ceguiPf = CEGUI::Texture::PixelFormat::PF_RGB_565;
      break;
    case bimg::TextureFormat::PTC12:
      ceguiPf = CEGUI::Texture::PixelFormat::PF_PVRTC2;
      break;
    case bimg::TextureFormat::PTC14:
      ceguiPf = CEGUI::Texture::PixelFormat::PF_PVRTC4;
      break;
    case bimg::TextureFormat::BC1:
      ceguiPf = CEGUI::Texture::PixelFormat::PF_RGBA_DXT1;
      break;
    case bimg::TextureFormat::BC2:
      ceguiPf = CEGUI::Texture::PixelFormat::PF_RGBA_DXT3;
      break;
    case bimg::TextureFormat::BC3:
      ceguiPf = CEGUI::Texture::PixelFormat::PF_RGBA_DXT5;
      break;
    default:
      CEGUI_THROW(CEGUI::InvalidRequestException("CEGUI Doesn't support this texture type from BIMG"));
    }
    result->loadFromMemory(imageContainer->m_data, CEGUI::Sizef(imageContainer->m_width, imageContainer->m_height), ceguiPf);
    return result;
  } CEGUI_CATCH(...) {
    if (imageContainer != nullptr) {
      // TODO: Clean allocated memory in image container
    } 
    CEGUI_RETHROW;
  }
}
