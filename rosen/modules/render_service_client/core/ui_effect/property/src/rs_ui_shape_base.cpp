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

#include "ui_effect/property/include/rs_ui_shape_base.h"

#include "platform/common/rs_log.h"
#include "pixel_map.h"

#undef LOG_TAG
#define LOG_TAG "RSNGShapeBase"

namespace OHOS {
namespace Rosen {
using ShapeCreator = std::function<std::shared_ptr<RSNGShapeBase>()>;

static thread_local std::unordered_map<RSNGEffectType, ShapeCreator> creatorLUT = {
    {RSNGEffectType::SDF_UNION_OP_SHAPE, [] {
            return std::make_shared<RSNGSDFUnionOpShape>();
        }
    },
    {RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE, [] {
            return std::make_shared<RSNGSDFSmoothUnionOpShape>();
        }
    },
    {RSNGEffectType::SDF_RRECT_SHAPE, [] {
            return std::make_shared<RSNGSDFRRectShape>();
        }
    },
};

std::shared_ptr<RSNGShapeBase> RSNGShapeBase::Create(RSNGEffectType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}
} // namespace Rosen
} // namespace OHOS
