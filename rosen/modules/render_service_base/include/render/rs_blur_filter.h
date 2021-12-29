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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BLUR_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BLUR_FILTER_H

#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
class RSBlurFilter : public RSSkiaFilter {
public:
    RSBlurFilter(float blurRadiusX, float blurRadiusY);
    ~RSBlurFilter() override;
    float GetBlurRadiusX();
    float GetBlurRadiusY();

    std::shared_ptr<RSFilter> Add(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Sub(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Multiply(float rhs) override;
    std::shared_ptr<RSFilter> Negate() override;
private:
    float blurRadiusX_;
    float blurRadiusY_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BLUR_FILTER_H