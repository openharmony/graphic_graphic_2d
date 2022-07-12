#ifndef __TEST_PICTURE_FILES_H__
#define __TEST_PICTURE_FILES_H__

#include <stddef.h>
#include <stdint.h>
namespace OHOS {
namespace Rosen {
uint8_t* getTestPngBuffer(size_t& bufSize);
uint8_t* getTestJpgBuffer(size_t& bufSize);
}
}
#endif