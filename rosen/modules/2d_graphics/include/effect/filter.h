/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FILTER_H
#define FILTER_H

#include "common/rs_macros.h"
#include "effect/image_filter.h"
#include "effect/mask_filter.h"
#include "effect/path_effect.h"
#include "utils/drawing_macros.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API Filter {
public:
    enum class FilterQuality {
        NONE,
        LOW,
        MEDIUM,
        HIGH,
    };

    Filter() noexcept;
    ~Filter() {};

    void SetColorFilter(std::shared_ptr<ColorFilter> colorFilter);
    std::shared_ptr<ColorFilter> GetColorFilter() const;

    void SetImageFilter(std::shared_ptr<ImageFilter> imageFilter);
    std::shared_ptr<ImageFilter> GetImageFilter() const;

    void SetMaskFilter(std::shared_ptr<MaskFilter> maskFilter);
    std::shared_ptr<MaskFilter> GetMaskFilter() const;

    FilterQuality GetFilterQuality() const;
    void SetFilterQuality(FilterQuality fq);

    void Reset();

    friend bool operator==(const Filter& f1, const Filter& f2);
    friend bool operator!=(const Filter& f1, const Filter& f2);

private:
    std::shared_ptr<ColorFilter> colorFilter_;
    std::shared_ptr<ImageFilter> imageFilter_;
    std::shared_ptr<MaskFilter> maskFilter_;
    FilterQuality filterQuality_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif