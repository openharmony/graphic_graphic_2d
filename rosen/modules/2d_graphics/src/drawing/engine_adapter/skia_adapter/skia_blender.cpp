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

#include "skia_blender.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaBlender::SkiaBlender() noexcept : blender_(nullptr) {}

sk_sp<SkBlender> SkiaBlender::GetBlender() const
{
    return blender_;
}

void SkiaBlender::SetSkBlender(const sk_sp<SkBlender>& skBlender)
{
    blender_ = skBlender;
}

std::shared_ptr<Blender> SkiaBlender::CreateWithBlendMode(BlendMode mode)
{
    sk_sp<SkBlender> skBlender = SkBlender::Mode(static_cast<SkBlendMode>(mode));
    if (!skBlender) {
        return nullptr;
    }
    auto blender = std::make_shared<Blender>();
    blender->GetImpl<SkiaBlender>()->SetSkBlender(skBlender);
    return blender;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS