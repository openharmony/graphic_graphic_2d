/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "effect/rs_render_filter_base.h"

#include <unordered_map>

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "effect/rs_render_mask_base.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
using MaskCreator = std::function<std::shared_ptr<RSNGRenderMaskBase>()>;

static std::unordered_map<RSNGEffectType, MaskCreator> creatorLUT = {
    {RSNGEffectType::RIPPLE_MASK, [] {
            return std::make_shared<RSNGRenderRippleMask>();
        }
    },
    {RSNGEffectType::PIXEL_MAP_MASK, [] {
            return std::make_shared<RSNGRenderPixelMapMask>();
        }
    },
};

std::shared_ptr<RSNGRenderMaskBase> RSNGRenderMaskBase::Create(RSNGEffectType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}

[[nodiscard]] bool RSNGRenderMaskBase::Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderMaskBase>& val)
{
    std::shared_ptr<RSNGRenderMaskBase> head = nullptr;
    auto current = head;
    for (size_t effectCount = 0; effectCount <= EFFECT_COUNT_LIMIT; ++effectCount) {
        RSNGEffectTypeUnderlying type = 0;
        if (!RSMarshallingHelper::Unmarshalling(parcel, type)) {
            ROSEN_LOGE("RSNGRenderMaskBase: Unmarshalling type failed");
            return false;
        }

        if (type == END_OF_CHAIN) {
            val = head;
            return true;
        }

        auto mask = Create(static_cast<RSNGEffectType>(type));
        if (mask && !mask->OnUnmarshalling(parcel)) {
            ROSEN_LOGE("RSNGRenderMaskBase: Unmarshalling mask failed with type %{public}d", type);
            return false;
        }
        if (!current) {
            head = mask; // init head
        } else {
            current->nextEffect_ = mask;
        }
        current = mask;
    }

    ROSEN_LOGE("RSNGRenderMaskBase: UnMarshalling mask count arrive limit(%{public}zu)",
        EFFECT_COUNT_LIMIT);
    return false;
}
} // namespace Rosen
} // namespace OHOS
