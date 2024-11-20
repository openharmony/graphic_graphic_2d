/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef EFFECT_NATIVE_FILTER_H
#define EFFECT_NATIVE_FILTER_H

#include <memory>
#include <mutex>
#include <vector>

#include "include/core/SkImageFilter.h"
#include "pixelmap_native_impl.h"
#include "sk_image_filter_factory.h"

namespace OHOS {
namespace Rosen {
class Filter {
public:
    Filter(){};
    explicit Filter(std::shared_ptr<OHOS::Media::PixelMap> pixelMap);

    std::shared_ptr<OHOS::Media::PixelMap> GetPixelMap();
    bool Blur(float radius);
    bool Brightness(float brightness);
    bool Grayscale();
    bool Invert();
    bool SetColorMatrix(const PixelColorMatrix& matrix);
    private:
    void AddNextFilter(sk_sp<SkImageFilter> filter);
    void Render(bool forceCPU);
    std::vector<sk_sp<SkImageFilter> > skFilters_;
    std::shared_ptr<OHOS::Media::PixelMap> srcPixelMap_ = nullptr;
    std::shared_ptr<OHOS::Media::PixelMap> dstPixelMap_ = nullptr;
};
}
}
#endif // EFFECT_NATIVE_FILTER_H