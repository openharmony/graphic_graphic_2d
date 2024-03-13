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

#ifndef KAWASEBLUR_IMPL_H
#define KAWASEBLUR_IMPL_H

#include <memory>

#include "base_impl.h"
#include "utils/kawase_blur_param.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class KawaseBlurImpl : public BaseImpl {
public:
    KawaseBlurImpl() noexcept {}
    ~KawaseBlurImpl() override {}

    virtual bool DrawWithKawaseBlur(Drawing::Canvas& canvas, Drawing::Brush& brush,
        const std::shared_ptr<Drawing::Image>& image, const KawaseParameters& blurParam,
        const KawaseProperties& properties) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif