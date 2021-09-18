#ifndef _CEGUIBimgImageCodec_h_
#define _CEGUIBimgImageCodec_h_
#include "CEGUI/ImageCodec.h"

#if (defined(__WIN32__) || defined(_WIN32)) && !defined(CEGUI_STATIC)
#ifdef CEGUIBIMGIMAGECODEC_EXPORTS
#define CEGUIBIMGIMAGECODEC_API __declspec(dllexport)
#else
#define CEGUIBIMGIMAGECODEC_API __declspec(dllimport)
#endif
#else
#define CEGUIBIMGIMAGECODEC_API
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#endif

/*!
  \brief 
  Image codec based on the Bimg library 
*/
class CEGUIBIMGIMAGECODEC_API BimgImageCodec : public CEGUI::ImageCodec
{
public:
  BimgImageCodec();
  ~BimgImageCodec();

  CEGUI::Texture *load(const CEGUI::RawDataContainer &data, CEGUI::Texture *result);

protected:
private:
};

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif // end of guard _CEGUIBimgImageCodec_h_
