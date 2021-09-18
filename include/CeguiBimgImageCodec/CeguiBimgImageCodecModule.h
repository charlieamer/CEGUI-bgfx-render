#ifndef _CEGUIBimgImageCodecModule_h_
#define _CEGUIBimgImageCodecModule_h_

#include "CeguiBimgImageCodec.h"
#include "bx/allocator.h"

/*! 
  \brief 
  exported function that creates the ImageCodec based object and 
  returns a pointer to that object.
*/
extern "C" CEGUIBIMGIMAGECODEC_API CEGUI::ImageCodec* createImageCodec(void);

/*!
  \brief
  exported function that deletes an ImageCodec based object previously 
  created by this module.
*/
extern "C" CEGUIBIMGIMAGECODEC_API void destroyImageCodec(CEGUI::ImageCodec* imageCodec);

extern "C" CEGUIBIMGIMAGECODEC_API bx::AllocatorI* getBxAllocator();
extern "C" CEGUIBIMGIMAGECODEC_API void setBxAllocator(bx::AllocatorI* newAllocator);

#endif // end of guard _CEGUIBimgImageCodecModule_h_
