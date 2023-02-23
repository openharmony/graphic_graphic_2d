/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_ENGINE_CORE_RENDER_RS_FILTER_H
#define ROSEN_ENGINE_CORE_RENDER_RS_FILTER_H

#include <memory>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
enum BLUR_COLOR_MODE : int {
    PRE_DEFINED = 0,           // use the pre-defined mask color
    AVERAGE     = 1,           // use the average color of the blurred area as mask color
    DEFAULT     = PRE_DEFINED
};

class RSB_EXPORT RSFilter : public std::enable_shared_from_this<RSFilter> {
public:
    virtual ~RSFilter();
    static std::shared_ptr<RSFilter> CreateBlurFilter(float blurRadiusX, float blurRadiusY);
    static std::shared_ptr<RSFilter> CreateMaterialFilter(int style, float dipScale, BLUR_COLOR_MODE mode = DEFAULT);

    enum FilterType {
        NONE = 0,
        BLUR,
        MATERIAL,
    };
    FilterType GetFilterType() const
    {
        return type_;
    }
    bool IsValid() const
    {
        return type_ != FilterType::NONE;
    }

protected:
    FilterType type_;
    RSFilter();
    virtual std::shared_ptr<RSFilter> Add(const std::shared_ptr<RSFilter>& rhs) { return nullptr; }
    virtual std::shared_ptr<RSFilter> Sub(const std::shared_ptr<RSFilter>& rhs) { return nullptr; }
    virtual std::shared_ptr<RSFilter> Multiply(float rhs) { return nullptr; }
    virtual std::shared_ptr<RSFilter> Negate() { return nullptr; }
    friend RSB_EXPORT std::shared_ptr<RSFilter> operator+(const std::shared_ptr<RSFilter>& lhs,
                                                         const std::shared_ptr<RSFilter>& rhs);
    friend RSB_EXPORT std::shared_ptr<RSFilter> operator-(const std::shared_ptr<RSFilter>& lhs,
                                                         const std::shared_ptr<RSFilter>& rhs);
    friend RSB_EXPORT std::shared_ptr<RSFilter> operator*(const std::shared_ptr<RSFilter>& lhs, float rhs);
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_RENDER_RS_FILTER_H
