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

#ifndef IMAGE_FILTER_LAZY_H
#define IMAGE_FILTER_LAZY_H

#include "effect/image_filter.h"
#include "effect/image_filter_obj.h"
#include "utils/drawing_macros.h"
#include "utils/object.h"
#include "utils/rect.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

// Only supports the Drawing NDK interface.
class DRAWING_API ImageFilterLazy : public ImageFilter {
public:
    static std::shared_ptr<ImageFilterLazy> CreateBlur(scalar sigmaX, scalar sigmaY,
        TileMode tileMode, const std::shared_ptr<ImageFilter>& input = nullptr,
        ImageBlurType blurType = ImageBlurType::GAUSS, const Rect& cropRect = noCropRect);
    static std::shared_ptr<ImageFilterLazy> CreateColorFilter(const std::shared_ptr<ColorFilter>& colorFilter,
        const std::shared_ptr<ImageFilter>& input = nullptr, const Rect& cropRect = noCropRect);
    static std::shared_ptr<ImageFilterLazy> CreateOffset(scalar dx, scalar dy,
        const std::shared_ptr<ImageFilter>& input = nullptr, const Rect& cropRect = noCropRect);
    static std::shared_ptr<ImageFilterLazy> CreateShader(const std::shared_ptr<ShaderEffect>& shader,
        const Rect& cropRect = noCropRect);
    static std::shared_ptr<ImageFilterLazy> CreateFromImageFilterObj(std::shared_ptr<ImageFilterObj> object);

    ~ImageFilterLazy() override = default;

    bool IsLazy() const override { return true; }

    std::shared_ptr<ImageFilterObj> GetImageFilterObj() const { return imageFilterObj_; }
    std::shared_ptr<ImageFilter> Materialize();

    // WARNING: Do not call Serialize()/Deserialize() on Lazy objects!
    // Use Object Marshalling()/Unmarshalling() instead.
    // These methods will return nullptr/false and log errors.
    std::shared_ptr<Data> Serialize() const override;
    bool Deserialize(std::shared_ptr<Data> data) override;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) override;
    static std::shared_ptr<ImageFilterLazy> Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth = 0);
#endif

private:
    ImageFilterLazy() noexcept : ImageFilter(FilterType::LAZY_IMAGE_FILTER) {}
    ImageFilterLazy(std::shared_ptr<ImageFilterObj> imageFilterObj) noexcept;

    std::shared_ptr<ImageFilterObj> imageFilterObj_ = nullptr;
    std::shared_ptr<ImageFilter> imageFilterCache_ = nullptr; // not lazy
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // IMAGE_FILTER_LAZY_H