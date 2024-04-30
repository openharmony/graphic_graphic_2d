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

#ifndef SKIA_BLENDER_H
#define SKIA_BLENDER_H

#include "effect/blender.h"
#include "include/core/SkBlender.h"

#include "impl_interface/blender_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaBlender : public BlenderImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaBlender() noexcept;
    ~SkiaBlender() override {};

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    sk_sp<SkBlender> GetBlender() const;
    void SetSkBlender(const sk_sp<SkBlender>& skBlender);

    static std::shared_ptr<Blender> CreateWithBlendMode(BlendMode mode);

private:
    sk_sp<SkBlender> blender_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif