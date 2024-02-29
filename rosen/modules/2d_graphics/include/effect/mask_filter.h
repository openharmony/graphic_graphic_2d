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

#ifndef MASK_FILTER_H
#define MASK_FILTER_H

#include <memory>

#include "common/rs_macros.h"
#include "drawing/engine_adapter/impl_interface/mask_filter_impl.h"
#include "utils/drawing_macros.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class BlurType {
    NORMAL,
    SOLID,
    OUTER,
    INNER,
};

#ifndef USE_ROSEN_DRAWING
class RS_EXPORT MaskFilter {
#else
class DRAWING_API MaskFilter {
#endif
public:
    enum class FilterType {
        NO_TYPE,
        BLUR,
    };

    /**
     * @brief Create a blur Maskfilter.
     * @param style The Drawing::BlurStyle to use.
     * @param sigma Standard deviation of the Gaussian blur to apply. Must be > 0.
     * @param respectCTM if true the blur's sigma is modified by the CTM.
     * @return A shared pointer to MaskFilter
     */
    static std::shared_ptr<MaskFilter> CreateBlurMaskFilter(BlurType blurType, scalar sigma, bool respectCTM = true);

    virtual ~MaskFilter() = default;
    FilterType GetType() const;
    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

    MaskFilter(FilterType t, BlurType blurType, scalar sigma, bool respectCTM = true) noexcept;
    MaskFilter(FilterType t) noexcept;

    std::shared_ptr<Data> Serialize() const;
    bool Deserialize(std::shared_ptr<Data> data);
protected:
    MaskFilter() noexcept;

private:
    FilterType type_;
    std::shared_ptr<MaskFilterImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif