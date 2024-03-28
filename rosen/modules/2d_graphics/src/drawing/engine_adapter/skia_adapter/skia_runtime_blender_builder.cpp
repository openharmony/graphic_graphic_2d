/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "skia_runtime_blender_builder.h"

#include "skia_blender.h"
#include "skia_runtime_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaRuntimeBlenderBuilder::SkiaRuntimeBlenderBuilder(
    std::shared_ptr<RuntimeEffect> effect) noexcept : skRuntimeBlendBuilder_(std::make_shared<SkRuntimeBlendBuilder>(
    effect->GetImpl<SkiaRuntimeEffect>()->GetRuntimeEffect())) {}

std::shared_ptr<Blender> SkiaRuntimeBlenderBuilder::MakeBlender()
{
    if (!skRuntimeBlendBuilder_) {
        return nullptr;
    }
    sk_sp<SkBlender> skBlender = skRuntimeBlendBuilder_->makeBlender();
    auto blender = std::make_shared<Blender>();
    blender->GetImpl<SkiaBlender>()->SetSkBlender(skBlender);
    return blender;
}

void SkiaRuntimeBlenderBuilder::SetUniform(const std::string& name, float val)
{
    if (!skRuntimeBlendBuilder_) {
        return;
    }
    skRuntimeBlendBuilder_->uniform(name.c_str()) = val;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS