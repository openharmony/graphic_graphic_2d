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

#include "common/rs_vector4.h"
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
    {RSNGEffectType::SDF_SUB_OP_SHAPE, [] {
            return std::make_shared<RSNGRenderSDFSubOpShape>();
        }
    },
    {RSNGEffectType::SDF_SMOOTH_SUB_OP_SHAPE, [] {
            return std::make_shared<RSNGRenderSDFSmoothSubOpShape>();
        }
    },
    {RSNGEffectType::SDF_RRECT_SHAPE, [] {
            return std::make_shared<RSNGRenderSDFRRectShape>();
        }
    },
    {RSNGEffectType::SDF_TRIANGLE_SHAPE, [] {
            return std::make_shared<RSNGRenderSDFTriangleShape>();
        }
    },
    {RSNGEffectType::SDF_ELLIPSE_SHAPE, [] {
            return std::make_shared<RSNGRenderSDFEllipseShape>();
        }
    },
    {RSNGEffectType::SDF_PATH_SHAPE, [] {
            return std::make_shared<RSNGRenderSDFPathShape>();
        }
    },
    {RSNGEffectType::SDF_PIXELMAP_SHAPE, [] {
            return std::make_shared<RSNGRenderSDFPixelmapShape>();
        }
    },
    {RSNGEffectType::SDF_TRANSFORM_SHAPE, [] {
            return std::make_shared<RSNGRenderSDFTransformShape>();
        }
    },
    {RSNGEffectType::SDF_EMPTY_SHAPE, [] {
            return std::make_shared<RSNGRenderSDFEmptyShape>();
        }
    },
    {RSNGEffectType::SDF_DISTORT_OP_SHAPE, [] {
            ROSEN_LOGE("RSNGRenderSDFDistortOpShape Created");
            return std::make_shared<RSNGRenderSDFDistortOpShape>();
        }
    },
};

using ShapeGetTransformRect = std::function<RectF(std::shared_ptr<RSNGRenderShapeBase>, RectF)>;
static std::unordered_map<RSNGEffectType, ShapeGetTransformRect> getTransformRectLUT = {
    {
        RSNGEffectType::SDF_DISTORT_OP_SHAPE, [](std::shared_ptr<RSNGRenderShapeBase> shape, RectF rect) {
            auto distortOp = std::static_pointer_cast<RSNGRenderSDFDistortOpShape>(shape);
            auto base = distortOp->Getter<OHOS::Rosen::SDFDistortOpShapeShapeRenderTag>()->Get();
            if (base == nullptr) {
                return rect;
            }
            auto transformRect = rect;
            transformRect = RSNGRenderShapeHelper::CalcRect(base, rect);
            auto luCorner = distortOp->Getter<OHOS::Rosen::SDFDistortOpShapeLUCornerRenderTag>()->Get();
            auto ruCorner = distortOp->Getter<OHOS::Rosen::SDFDistortOpShapeRUCornerRenderTag>()->Get();
            auto rbCorner = distortOp->Getter<OHOS::Rosen::SDFDistortOpShapeRBCornerRenderTag>()->Get();
            auto lbCorner = distortOp->Getter<OHOS::Rosen::SDFDistortOpShapeLBCornerRenderTag>()->Get();
            auto distortion = distortOp->Getter<OHOS::Rosen::SDFDistortOpShapeBarrelDistortionRenderTag>()->Get();
            float left = transformRect.GetLeft() + std::min(luCorner[0], lbCorner[0]) * transformRect.GetWidth();
            float top = transformRect.GetTop() + std::min(luCorner[1], ruCorner[1]) * transformRect.GetHeight();
            float right = transformRect.GetLeft() + std::max(ruCorner[0], rbCorner[0]) * transformRect.GetWidth();
            float bottom = transformRect.GetTop() + std::max(lbCorner[1], rbCorner[1]) * transformRect.GetHeight();
            float width = std::abs(right - left);
            float height = std::abs(bottom - top);
            constexpr float halfUV = 0.5f;
            constexpr float distortScale = 0.25f;
            constexpr float tuneNum = 4.0f;
            constexpr float tuneDenomBase = 2.0f;
            if (distortion[0] > 0) {
                left -= width * (halfUV - distortScale * (tuneNum + distortion[0]) / (tuneDenomBase + distortion[0]));
            }
            if (distortion[1] > 0) {
                right += width * (halfUV - distortScale * (tuneNum + distortion[1]) / (tuneDenomBase + distortion[1]));
            }
            if (distortion[2] > 0) {
                top -= height * (halfUV - distortScale * (tuneNum + distortion[2]) / (tuneDenomBase + distortion[2]));
            }
            if (distortion[3] > 0) {
                bottom += height *
                    (halfUV - distortScale * (tuneNum + distortion[3]) / (tuneDenomBase + distortion[3]));
            }
            return RectF(floor(left), floor(top), ceil(right) - floor(left), ceil(bottom) - floor(top));
        },
    },
};

std::shared_ptr<RSNGRenderShapeBase> RSNGRenderShapeBase::Create(RSNGEffectType type)
{
    auto it = creatorLUT.find(type);
    return it != creatorLUT.end() ? it->second() : nullptr;
}

bool RSShapeRecursionGuard::ExceedsLimit() const
{
    if (Depth() > MAX_DEPTH) {
        ROSEN_LOGE("RSShapeRecursionGuard: recursion depth exceeds limit(%{public}d)", MAX_DEPTH);
        return true;
    }
    return false;
}

int32_t& RSShapeRecursionGuard::Depth()
{
    static thread_local int32_t depth = 0;
    return depth;
}

[[nodiscard]] bool RSNGRenderShapeBase::Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderShapeBase>& val)
{
    RSShapeRecursionGuard guard;
    if (guard.ExceedsLimit()) {
        return false;
    }
    std::shared_ptr<RSNGRenderShapeBase> head = nullptr;
    auto current = head;
    for (size_t effectCount = 0; effectCount < EFFECT_COUNT_LIMIT; ++effectCount) {
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
        if (!shape || !shape->OnUnmarshalling(parcel)) {
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

RectF RSNGRenderShapeHelper::CalcRect(
    const std::shared_ptr<RSNGRenderShapeBase>& shape, const RectF& bound, bool needUpdate)
{
    if (shape == nullptr) {
        return bound;
    }

    auto iter = getTransformRectLUT.find(shape->GetType());
    auto result = iter == getTransformRectLUT.end() ? bound : iter->second(shape, bound);
    if (needUpdate) {
        shape->transformDrawRect_ = result;
    }
    return result;
}

void RSNGRenderShapeHelper::FillEmptyDistortOpShape(
    std::shared_ptr<RSNGRenderShapeBase>& sdfShape, const RRect& sdfRRect, NodeId nodeId)
{
    if (!sdfShape || sdfShape->GetType() != RSNGEffectType::SDF_DISTORT_OP_SHAPE) {
        return;
    }
    auto distortOpShape = std::static_pointer_cast<RSNGRenderSDFDistortOpShape>(sdfShape);
    auto innerShape = distortOpShape->Getter<SDFDistortOpShapeShapeRenderTag>()->Get();
    // ownerId==0: auto-filled (ONLY_VALUE skips OnAttach) -> sync; !=0: IPC inner (attached) -> keep.
    bool sync = innerShape ? (innerShape->GetOwnerId() == 0) : true;
    if (!innerShape) {
        innerShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
        // ONLY_VALUE keeps ownerId==0 so the auto-filled inner stays distinguishable from IPC inner.
        distortOpShape->Setter<SDFDistortOpShapeShapeRenderTag>(innerShape,
            PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
        ROSEN_LOGD("RSNGRenderShapeHelper::FillEmptyDistortOpShape, add default SDF_RRECT_SHAPE, node %{public}"
            PRIu64, nodeId);
    }
    if (sync && innerShape && innerShape->GetType() == RSNGEffectType::SDF_RRECT_SHAPE) {
        auto defaultShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(innerShape);
        defaultShape->Setter<SDFRRectShapeRRectRenderTag>(sdfRRect);
        ROSEN_LOGD("RSNGRenderShapeHelper::FillEmptyDistortOpShape, update SDF_RRECT_SHAPE, node %{public}"
            PRIu64, nodeId);
    }
}
} // namespace Rosen
} // namespace OHOS
