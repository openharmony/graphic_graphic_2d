/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FILTER_COMMON_H
#define FILTER_COMMON_H

#include "include/core/SkImageFilter.h"
#include "pixel_map.h"
#include "sk_image_chain.h"

namespace OHOS {
namespace Rosen {
class FilterCommon {
public:
    static std::shared_ptr<FilterCommon> CreateEffect(
        const std::shared_ptr<Media::PixelMap>& pixmap, uint32_t& errorCode);
    static bool Blur(float radius);
    static bool Invert();
    static bool Brightness(float bright);
    static bool Grayscale();
    static bool SetColorMatrix(std::vector<float> cjcolorMatrix, uint32_t& code);
    static std::shared_ptr<OHOS::Media::PixelMap> GetEffectPixelMap();
    static thread_local std::shared_ptr<FilterCommon> sConstructor_;
    void AddNextFilter(sk_sp<SkImageFilter> filter);
    std::shared_ptr<Media::PixelMap> GetDstPixelMap();
    Rosen::DrawError Render(bool forceCPU);
    std::vector<sk_sp<SkImageFilter>> skFilters_;
    std::shared_ptr<Media::PixelMap> srcPixelMap_ = nullptr;
    std::shared_ptr<Media::PixelMap> dstPixelMap_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // FILTER_COMMON_H