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
#include "render/rs_magnifier_shader_filter.h"

#include "platform/common/rs_log.h"
#include "src/core/SkOpts.h"

namespace OHOS {
namespace Rosen {

RSMagnifierShaderFilter::RSMagnifierShaderFilter(float offsetX, float offsetY)
    : offsetX_(offsetX), offsetY_(offsetY)
{
    type_ = ShaderFilterType::MAGNIFIER;
    hash_ = SkOpts::hash(&offsetX_, sizeof(offsetX_), hash_);
    hash_ = SkOpts::hash(&offsetY_, sizeof(offsetY_), hash_);
}

RSMagnifierShaderFilter::~RSMagnifierShaderFilter() = default;

void RSMagnifierShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto magnifierFilter = std::make_shared<Drawing::GEVisualEffect>("MAGNIFIER", Drawing::DrawingPaintType::BRUSH);
    if (magnifierFilter == nullptr) {
        ROSEN_LOGD("RSMagnifierShaderFilter::GenerateGEVisualEffect magnifierFilter is nullptr!");
        return;
    }

    visualEffectContainer->AddToChainedFilter(magnifierFilter);
}

} // namespace Rosen
} // namespace OHOS
