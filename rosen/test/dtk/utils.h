/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 */
#ifndef RS_UTILS_H
#define RS_UTILS_H
#include "dtk_test_ext.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

Bitmap BuildRandomBitmap();
std::shared_ptr<Image> MakeImageRaster();
std::shared_ptr<Image> MakeImageEncoded();
std::shared_ptr<Image> MakeImageGpu(std::shared_ptr<GPUContext> gpuContext);
std::shared_ptr<Image> MakeImageYUVA(std::shared_ptr<GPUContext> gpuContext);

}
}
}
#endif