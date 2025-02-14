/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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