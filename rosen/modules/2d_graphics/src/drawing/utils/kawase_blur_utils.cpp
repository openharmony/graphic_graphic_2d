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

#include "utils/kawase_blur_utils.h"
#include "engine_adapter/impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
KawaseBlurUtils::KawaseBlurUtils() noexcept : impl_(ImplFactory::CreateKawaseBlurImpl()) {}

KawaseBlurUtils::~KawaseBlurUtils() {}

bool KawaseBlurUtils::ApplyKawaseBlur(Drawing::Canvas& canvas, Drawing::Brush& brush,
    const std::shared_ptr<Drawing::Image>& image, const KawaseParameters& blurParams,
    const KawaseProperties& properties)
{
    if (impl_) {
        return impl_->DrawWithKawaseBlur(canvas, brush, image, blurParams, properties);
    }
    return false;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS