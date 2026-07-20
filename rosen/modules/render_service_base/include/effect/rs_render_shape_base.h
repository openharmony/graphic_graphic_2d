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

#ifndef RENDER_SERVICE_BASE_EFFECT_RS_RENDER_SHAPE_BASE_H
#define RENDER_SERVICE_BASE_EFFECT_RS_RENDER_SHAPE_BASE_H

#include <cstdint>

#include "effect/rs_render_effect_template.h"
#include "effect/rs_render_property_tag.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {

namespace Drawing {
    class GEShaderShape;
} // namespace Drawing

// Depth guard for shape-tree recursion (Unmarshalling / GenerateGEVisualEffect / CalculateHash).
// Caps nested shape depth to prevent stack-overflow DoS from attacker-crafted parcels or
// server-built deep trees (e.g. nested RSUnionRenderNode subtrees, which bypass the deserialization
// guard). One thread_local counter is shared by all three paths: the GenerateGEVisualEffect<->
// CalculateHash cross-call in UpdateVisualEffectParamImpl does not double-count, because hash
// recursion at generate-depth D only spans the subtree below D (depth M-D), so the peak stays at
// the tree depth M. These paths are synchronous CPU work: do NOT add FFRT/coroutine suspension
// inside them, or thread_local would stop tracking logical depth after a cross-thread resume.
class RSB_EXPORT RSShapeRecursionGuard {
public:
    static constexpr int32_t MAX_DEPTH = 128;
    RSShapeRecursionGuard()
    {
        ++Depth();
    }
    ~RSShapeRecursionGuard()
    {
        --Depth();
    }
    bool ExceedsLimit() const;
    RSShapeRecursionGuard(const RSShapeRecursionGuard&) = delete;
    RSShapeRecursionGuard& operator=(const RSShapeRecursionGuard&) = delete;

private:
    static int32_t& Depth();
};

class RSB_EXPORT RSNGRenderShapeBase : public RSNGRenderEffectBase<RSNGRenderShapeBase> {
public:
    virtual ~RSNGRenderShapeBase() = default;

    static std::shared_ptr<RSNGRenderShapeBase> Create(RSNGEffectType type);

    [[nodiscard]] static bool Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderShapeBase>& val);

    virtual std::shared_ptr<Drawing::GEVisualEffect> GenerateGEVisualEffect() { return nullptr; }

    const RectF& GetTransformDrawRect() const { return transformDrawRect_; }

    // Owner identity: the render property instance that Attached this shape (its `this` pointer
    // as uintptr_t). Only that property cascades Detach (see RSRenderProperty<shape>::OnDetach).
    // Borrowers (ONLY_VALUE, no Attach) skip, so a shared SDFShape borrowed by e.g.
    // FrostedGlass.Shape won't be Detach'd by the borrower and won't unregister inner
    // sub-properties still used by the real owner. Uses the instance pointer (not GetId()) so
    // default-constructed sub-properties (id 0) don't collide.
    uintptr_t GetOwnerId() const { return ownerId_; }
    void SetOwnerId(uintptr_t id) { ownerId_ = id; }

protected:
    RectF transformDrawRect_;
    uintptr_t ownerId_ = 0;

private:
    friend class RSNGRenderShapeHelper;
};

// Opt-in: RSNGRenderShapeBase allows self-type-as-property (SHAPE_PTR) because all recursion
// paths are guarded by RSShapeRecursionGuard (MAX_DEPTH=128). The static_assert in
// RSNGRenderEffectTemplate bans this pattern for all other effect types (filter/mask/shader).
template <>
struct allow_self_type_property<RSNGRenderShapeBase> : std::true_type {};

template<RSNGEffectType Type, typename... PropertyTags>
class RSNGRenderShapeTemplate : public RSNGRenderEffectTemplate<RSNGRenderShapeBase, Type, PropertyTags...> {
public:
    using EffectTemplateBase = RSNGRenderEffectTemplate<RSNGRenderShapeBase, Type, PropertyTags...>;

    RSNGRenderShapeTemplate() : EffectTemplateBase() {}
    ~RSNGRenderShapeTemplate() override = default;
    RSNGRenderShapeTemplate(const std::tuple<PropertyTags...>& properties) noexcept
        : EffectTemplateBase(properties) {}

    std::shared_ptr<Drawing::GEVisualEffect> GenerateGEVisualEffect() override
    {
        RSShapeRecursionGuard guard;
        if (guard.ExceedsLimit()) {
            return nullptr;
        }
        RS_OPTIONAL_TRACE_FMT("RSRenderShape, Type: %s",
            RSNGRenderEffectHelper::GetEffectTypeString(Type).c_str());
        auto geShape = RSNGRenderEffectHelper::CreateGEVisualEffect(Type);
        OnGenerateGEVisualEffect(geShape);
        std::apply([&geShape](const auto&... propTag) {
                (RSNGRenderEffectHelper::UpdateVisualEffectParam<std::decay_t<decltype(propTag)>>(
                    geShape, propTag), ...);
            }, EffectTemplateBase::properties_);
        // Currently only a single Shape is used, so we do not handle the nextEffect_ here.
        return geShape;
    }

protected:
    virtual void OnGenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffect>) {}
};

class RSNGRenderShapeHelper {
public:
    static RectF CalcRect(
        const std::shared_ptr<RSNGRenderShapeBase>& shape, const RectF& bound, bool needUpdate = true);
    static void FillEmptyDistortOpShape(std::shared_ptr<RSNGRenderShapeBase>& sdfShape, const RRect& sdfRRect,
        NodeId nodeId);
};

#define ADD_PROPERTY_TAG(Effect, Prop) Effect##Prop##RenderTag
#define DECLARE_SHAPE(ShapeName, ShapeType, ...) \
    using RSNGRender##ShapeName = RSNGRenderShapeTemplate<RSNGEffectType::ShapeType, ##__VA_ARGS__>

#include "effect/rs_render_shape_def.in"

#undef ADD_PROPERTY_TAG
#undef DECLARE_SHAPE

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_EFFECT_RS_RENDER_SHAPE_BASE_H
