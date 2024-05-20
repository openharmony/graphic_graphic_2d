/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef SKIMAGE_FILTER_FACTORY_H
#define SKIMAGE_FILTER_FACTORY_H

#include "include/core/SkImageFilter.h"

namespace OHOS {
namespace Rosen {
    
enum class TileMode : int32_t {
    CLAMP = 0,
    REPEAT,
    MIRROR,
    DECAL
};

struct PixelColorMatrix {
    static constexpr uint8_t MATRIX_SIZE = 20;
    static constexpr uint8_t ROW_SIZE = 4;
    static constexpr uint8_t COL_SIZE = 5;
    float val[MATRIX_SIZE] = {0};
};

class SKImageFilterFactory {
public:
    static sk_sp<SkImageFilter> Blur(float radius, TileMode tileMode = TileMode::DECAL);
    static sk_sp<SkImageFilter> Brightness(float degree);
    static sk_sp<SkImageFilter> Grayscale();
    static sk_sp<SkImageFilter> Invert();
    static sk_sp<SkImageFilter> ApplyColorMatrix(const PixelColorMatrix &matrix);
};
} // namespace Rosen
} // namespace OHOS
#endif // SKIMAGE_FILTER_FACTORY_H
