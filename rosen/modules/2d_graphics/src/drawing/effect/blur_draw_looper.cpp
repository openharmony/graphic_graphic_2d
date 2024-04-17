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

#include <securec.h>
#include "effect/blur_draw_looper.h"

#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
BlurDrawLooper::BlurDrawLooper(float blurRadius, scalar dx, scalar dy, const Color& color) noexcept
{
    blurDrawLooperPriv_.blurRadius = blurRadius;
    blurDrawLooperPriv_.dx = dx;
    blurDrawLooperPriv_.dy = dy;
    blurDrawLooperPriv_.color = color;
    blurMaskFilter_ = MaskFilter::CreateBlurMaskFilter(Drawing::BlurType::NORMAL,
        ConvertRadiusToSigma(blurRadius), true);
}

std::shared_ptr<BlurDrawLooper> BlurDrawLooper::CreateBlurDrawLooper(float blurRadius,
    scalar dx, scalar dy, const Color& color)
{
    if (blurRadius <= 0.f) {
        return nullptr;
    }
    return std::make_shared<BlurDrawLooper>(blurRadius, dx, dy, color);
}

bool operator==(const BlurDrawLooper& lhs, const BlurDrawLooper& rhs)
{
    return lhs.blurDrawLooperPriv_.color == rhs.blurDrawLooperPriv_.color &&
        IsScalarAlmostEqual(lhs.blurDrawLooperPriv_.dx, rhs.blurDrawLooperPriv_.dx) &&
        IsScalarAlmostEqual(lhs.blurDrawLooperPriv_.dy, rhs.blurDrawLooperPriv_.dy) &&
        IsScalarAlmostEqual(lhs.blurDrawLooperPriv_.blurRadius, rhs.blurDrawLooperPriv_.blurRadius);
}

bool operator!=(const BlurDrawLooper& lhs, const BlurDrawLooper& rhs)
{
    return !(lhs == rhs);
}

std::shared_ptr<Data> BlurDrawLooper::Serialize() const
{
    constexpr size_t len = sizeof(BlurDrawLooperPriv);
    auto blurData = std::make_shared<Data>();
    if (!blurData->BuildUninitialized(len)) {
        return nullptr;
    }
    uint8_t* memory = static_cast<uint8_t*>(blurData->WritableData());
    if (memcpy_s(memory, len, &blurDrawLooperPriv_, sizeof(BlurDrawLooperPriv)) != EOK) {
        return nullptr;
    }

    return blurData;
}

std::shared_ptr<BlurDrawLooper> BlurDrawLooper::Deserialize(std::shared_ptr<Data> data)
{
    constexpr size_t len = sizeof(BlurDrawLooperPriv);
    if (data == nullptr || data->GetSize() != len) {
        LOGD("BlurDrawLooper:blur data is valid!");
        return nullptr;
    }
    uint8_t* memory = reinterpret_cast<uint8_t*>(data->WritableData());
    if (memory == nullptr) {
        return nullptr;
    }
    BlurDrawLooperPriv blurDrawLooperPriv;
    if (memcpy_s(&blurDrawLooperPriv, sizeof(BlurDrawLooperPriv), memory, len) != EOK) {
        return nullptr;
    }

    return CreateBlurDrawLooper(blurDrawLooperPriv.blurRadius,
        blurDrawLooperPriv.dx, blurDrawLooperPriv.dy, blurDrawLooperPriv.color);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS