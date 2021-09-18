#include "CeguiBimgImageCodec/CeguiBimgImageCodecModule.h"
#include "bx/allocator.h"

bx::AllocatorI* bxAllocator = nullptr;

CEGUI::ImageCodec* createImageCodec(void)
{
  return CEGUI_NEW_AO BimgImageCodec();
}

void destroyImageCodec(CEGUI::ImageCodec* imageCodec)
{
  CEGUI_DELETE_AO imageCodec;
  if (bxAllocator) CEGUI_DELETE_AO bxAllocator;
}

bx::AllocatorI* getBxAllocator() {
  if (!bxAllocator) {
    setBxAllocator(CEGUI_NEW_AO bx::DefaultAllocator());
  }
  return bxAllocator;
}

void setBxAllocator(bx::AllocatorI* newAllocator) {
  if (bxAllocator) CEGUI_DELETE_AO bxAllocator;
  bxAllocator = newAllocator;
}