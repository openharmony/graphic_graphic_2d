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

#include "effect/rs_render_shape_base.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
using ShapeCreator = std::function<std::shared_ptr<RSNGRenderShapeBase>()>;

static std::unordered_map<RSNGEffectType, ShapeCreator> creatorLUT = {
    {RSNGEffectType::SDF_UNION_OP_SHAPE, [] {
            return std::make_shared<RSNGRenderSDFUnionOpShape>();
        }
    },
    {RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE, [] {
            return std::make_shared<RSNGRenderSDFSmoothUnionOpShape>();
        }
    },
    {RSNGEffectType::SDF_RRECT_SHAPE, [] {
            return std::make_shared<RSNGRenderSDFRRectShape>();
        }
    },
};

std::shared_ptr<RSNGRenderShapeBase> RSNGRenderShapeBase::Create(RSNGEffectType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}

[[nodiscard]] bool RSNGRenderShapeBase::Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderShapeBase>& val)
{
    std::shared_ptr<RSNGRenderShapeBase> head = nullptr;
    auto current = head;
    for (size_t effectCount = 0; effectCount <= EFFECT_COUNT_LIMIT; ++effectCount) {
        RSNGEffectTypeUnderlying type = 0;
        if (!RSMarshallingHelper::Unmarshalling(parcel, type)) {
            ROSEN_LOGE("RSNGRenderShapeBase: Unmarshalling type failed");
            return false;
        }

        if (type == END_OF_CHAIN) {
            val = head;
            return true;
        }

        auto shape = Create(static_cast<RSNGEffectType>(type));
        if (shape && !shape->OnUnmarshalling(parcel)) {
            ROSEN_LOGE("RSNGRenderShapeBase: Unmarshalling shape failed with type %{public}d", type);
            return false;
        }
        if (!current) {
            head = shape; // init head
        } else {
            current->nextEffect_ = shape;
        }
        current = shape;
    }

    ROSEN_LOGE("RSNGRenderShapeBase: UnMarshalling shape count arrive limit(%{public}zu)",
        EFFECT_COUNT_LIMIT);
    return false;
}
} // namespace Rosen
} // namespace OHOS
