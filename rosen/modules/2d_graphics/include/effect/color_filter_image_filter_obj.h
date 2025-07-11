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

#ifndef COLOR_FILTER_IMAGE_FILTER_OBJ_H
#define COLOR_FILTER_IMAGE_FILTER_OBJ_H

#include "effect/color_filter.h"
#include "effect/image_filter.h"
#include "effect/image_filter_obj.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class ColorFilterImageFilterObj final : public ImageFilterObj {
public:
    static std::shared_ptr<ColorFilterImageFilterObj> CreateForUnmarshalling();
    static std::shared_ptr<ColorFilterImageFilterObj> Create(const std::shared_ptr<ColorFilter>& colorFilter,
        const std::shared_ptr<ImageFilter>& input, const Rect& cropRect = noCropRect);
    ~ColorFilterImageFilterObj() override = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) override;
    bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth = 0) override;
#endif
    std::shared_ptr<void> GenerateBaseObject() override;

private:
    ColorFilterImageFilterObj();
    ColorFilterImageFilterObj(const std::shared_ptr<ColorFilter>& colorFilter,
        const std::shared_ptr<ImageFilter>& input, const Rect& cropRect = noCropRect);
    std::shared_ptr<ColorFilter> colorFilter_ = nullptr;
    std::shared_ptr<ImageFilter> input_ = nullptr;
    Rect cropRect_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif  // COLOR_FILTER_IMAGE_FILTER_OBJ_H