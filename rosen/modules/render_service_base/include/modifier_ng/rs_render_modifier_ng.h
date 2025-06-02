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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_RS_RENDER_MODIFIER_NG_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_RS_RENDER_MODIFIER_NG_H

#include "recording/draw_cmd_list.h"

#include "common/rs_macros.h"
#include "common/rs_vector4.h"
#include "modifier/rs_render_property.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_recording_canvas.h"
#include "property/rs_filter_cache_manager.h"
#include "property/rs_properties_def.h"
#include "render/rs_gradient_blur_para.h"

namespace OHOS::Rosen {
class RSProperties;
class RSRenderNode;
namespace ModifierNG {
struct RSClipBoundsParams {
    Vector4f borderRadius_; // cornerRadius
    std::shared_ptr<RSPath> clipToBounds_;
    std::optional<RRect> clipRRect_;
};
struct RSModifierContext {
    Drawing::Rect frame_;
    Drawing::Rect bounds_;
    Gravity frameGravity_;
    RSClipBoundsParams clipBounds_;
    RRect GetRRect() const
    {
        return RRect(GetBoundsRect(), clipBounds_.borderRadius_);
    }
    RectF GetBoundsRect() const
    {
        return (frame_.GetWidth() <= 0 && frame_.GetHeight() <= 0)
                   ? RectF { 0, 0, frame_.GetWidth(), frame_.GetHeight() }
                   : RectF { 0, 0, bounds_.GetWidth(), bounds_.GetHeight() };
    }
};

class RSB_EXPORT RSDrawable : public std::enable_shared_from_this<RSDrawable> {
public:
    // move staging value to render value
    void Sync();
    // use render value to draw on canvas
    virtual void Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect) {};

    // clear staging values, and clear render value via a sync
    virtual void Purge();

protected:
    RSDrawable() = default;
    virtual ~RSDrawable() = default;
    bool needSync_ = true; // need call sync

    virtual void OnSync() {};
};

// =============================================
// life cycle of RSRenderModifier
// 1. Create & animate
//   Create instance of RSRenderModifier and attach RSRenderProperty(s) to it, animate may change RSRenderProperty's
//   value
// 2. Apply
//   Apply RSRenderProperty(s) to staging value
// 3. GenerateDrawable
//   Generate drawable from staging value
// 4. Sync
//   Move staging value to render value
// 5. Draw
//   Use render value to draw on canvas
// 6. Purge
//   Clear staging values, and clear render value via a sync
class RSB_EXPORT RSRenderModifier : public RSDrawable {
public:
    RSRenderModifier() = default;
    ~RSRenderModifier() override = default;

    // RSRenderProperty(s) to staging value
    bool Apply(RSModifierContext& context);
    void Purge() override;
    void ApplyLegacyProperty(RSProperties& properties);

    void AttachProperty(RSPropertyType type, const std::shared_ptr<RSRenderPropertyBase>& property);
    void DetachProperty(RSPropertyType type);

    bool IsAttached() const;
    void OnAttachModifier(RSRenderNode& node);
    void OnDetachModifier();
    void SetDirty();

    // TODO: re-design this modifier->drawable procedure
    virtual std::shared_ptr<const RSDrawable> GenerateDrawable() const { return shared_from_this(); }

    virtual ModifierNG::RSModifierType GetType() const = 0;
    ModifierId GetId() const
    {
        return id_;
    }

    bool Marshalling(Parcel& parcel) const;
    [[nodiscard]] static RSRenderModifier* Unmarshalling(Parcel& parcel);

protected:
    virtual bool OnApply(RSModifierContext& context) = 0;

    // only accept properties on white list ?
    ModifierId id_;
    bool dirty_ = true;
    std::weak_ptr<RSRenderNode> target_;

    // sub-class should not directly access properties_, use GetPropertyValue instead
    std::map<RSPropertyType, std::shared_ptr<RSRenderPropertyBase>> properties_;

    inline bool HasProperty(RSPropertyType type) const
    {
        return properties_.count(type);
    };
    template<typename T>
    inline T Getter(RSPropertyType type, const T& defaultValue = {}) const
    {
        auto it = properties_.find(type);
        if (it == properties_.end()) {
            return defaultValue;
        }
        auto property = std::static_pointer_cast<RSRenderProperty<T>>(it->second);
        return property->Get();
    }
    template<template<typename> class PropertyType, typename T>
    inline void Setter(RSPropertyType type, const T& value)
    {
        auto it = properties_.find(type);
        if (it != properties_.end()) {
            auto property = std::static_pointer_cast<PropertyType<T>>(it->second);
            property->Set(value);
        } else {
            // should not happen
        }
    }

    using LegacyPropertyApplier = std::function<void(RSProperties& context, RSRenderPropertyBase&)>;
    using LegacyPropertyApplierMap = std::unordered_map<ModifierNG::RSPropertyType, LegacyPropertyApplier>;

    template<typename T, auto Setter>
    static void PropertyApplyHelper(RSProperties& properties, RSRenderPropertyBase& property)
    {
        const auto& value = static_cast<RSRenderProperty<T>&>(property).GetRef();
        (properties.*Setter)(value);
    }

    template<typename T, auto Setter, auto Getter>
    static void PropertyApplyHelperAdd(RSProperties& properties, RSRenderPropertyBase& property)
    {
        const auto& orig = (properties.*Getter)();
        const auto& value = static_cast<RSRenderProperty<T>&>(property).GetRef();
        const auto newValue = Add(orig, value);
        (properties.*Setter)(newValue);
    }

    template<typename T, auto Setter, auto Getter>
    static void PropertyApplyHelperMultiply(RSProperties& properties, RSRenderPropertyBase& property)
    {
        const auto& orig = (properties.*Getter)();
        const auto& value = static_cast<RSRenderProperty<T>&>(property).GetRef();
        const auto newValue = Multiply(orig, value);
        (properties.*Setter)(newValue);
    }

    static const LegacyPropertyApplierMap emptyLegacyPropertyApplierMap_;

    virtual const LegacyPropertyApplierMap& GetLegacyPropertyApplierMap() const
    {
        return emptyLegacyPropertyApplierMap_;
    };

    using Constructor = std::function<RSRenderModifier*()>;

private:
    static std::array<Constructor, ModifierNG::MODIFIER_TYPE_COUNT> ConstructorLUT_;

    template<typename T>
    static T Add(const T& a, const T& b)
    {
        return a + b;
    }

    template<typename T>
    static T Add(const std::optional<T>& a, const T& b)
    {
        return a.has_value() ? *a + b : b;
    }

    template<typename T>
    static T Add(const std::optional<T>& a, const std::optional<T>& b)
    {
        if (a.has_value() && b.has_value()) {
            return *a + *b;
        }
        if (!a.has_value() && b.has_value()) {
            return *b;
        }
        if (a.has_value() && !b.has_value()) {
            return *a;
        }
        return {};
    }

    template<typename T>
    static T Multiply(const T& a, const T& b)
    {
        return a * b;
    }

    template<typename T>
    static T Multiply(const std::optional<T>& a, const T& b)
    {
        return a.has_value() ? *a * b : b;
    }

    friend class RSModifier;
    friend class OHOS::Rosen::RSRenderNode;
};

// =============================================
// modifiers that can be recorded as display list
class RSB_EXPORT RSDisplayListRenderModifier : public RSRenderModifier {
public:
    void Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect) override;
    void Purge() override;

protected:
    RSDisplayListRenderModifier() = default;
    ~RSDisplayListRenderModifier() override = default;

    void OnSync() override;

    // Staging Value
    Drawing::DrawCmdListPtr stagingDrawCmd_;
    // Render Value
    Drawing::DrawCmdListPtr renderDrawCmd_;
    friend class RSDisplayListModifierUpdater;

    std::string renderPropertyDescription_;
    std::string stagingPropertyDescription_;
};

class StyleTypeConvertor {
public:
    static ModifierNG::RSPropertyType getPropertyType(ModifierNG::RSModifierType T)
    {
        auto it = map_.find(T);
        if (it == map_.end()) {
            return ModifierNG::RSPropertyType::INVALID;
        }
        return it->second;
    }

private:
    StyleTypeConvertor() = default;
    ~StyleTypeConvertor() = default;

    StyleTypeConvertor(const StyleTypeConvertor&) = delete;
    StyleTypeConvertor& operator=(const StyleTypeConvertor&) = delete;

    static inline std::unordered_map<ModifierNG::RSModifierType, ModifierNG::RSPropertyType> map_ = {
        { ModifierNG::RSModifierType::TRANSITION, ModifierNG::RSPropertyType::TRANSITION },
        { ModifierNG::RSModifierType::BACKGROUND_STYLE, ModifierNG::RSPropertyType::BACKGROUND_STYLE },
        { ModifierNG::RSModifierType::CONTENT_STYLE, ModifierNG::RSPropertyType::CONTENT_STYLE },
        { ModifierNG::RSModifierType::FOREGROUND_STYLE, ModifierNG::RSPropertyType::FOREGROUND_STYLE },
        { ModifierNG::RSModifierType::OVERLAY_STYLE, ModifierNG::RSPropertyType::OVERLAY_STYLE },
    };
};

// Holds DrawCmdList
template<RSModifierType type>
class RSB_EXPORT RSCustomRenderModifier : public RSDisplayListRenderModifier {
public:
    RSCustomRenderModifier() = default;
    ~RSCustomRenderModifier() override = default;

    static inline constexpr auto Type = type;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect) override;
    void Purge() override;

protected:
    bool OnApply(RSModifierContext& context) override;
    void OnSync() override;

    // bool needSync_ = true;
    bool needClearOp_ = false;

    bool isCanvasNode_ = false;
    bool stagingIsCanvasNode_ = false;

    // TBD
    Gravity gravity_ = Gravity::DEFAULT;
    Gravity stagingGravity_ = Gravity::DEFAULT;

    // friend class RSDisplayListModifierUpdater;

private:
    // bool OnApply(RSModifierContext& context) override;
};

class RSB_EXPORT RSFilterRenderModifier : public RSRenderModifier {
public:
    void Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect) override;
    void Purge() override;

protected:
    RSFilterRenderModifier() = default;
    ~RSFilterRenderModifier() override = default;

    virtual bool IsForeground() const
    {
        return false;
    }

    bool NeedBlurFuzed();
    void ClearFilterCache();
    std::shared_ptr<Drawing::ColorFilter> GetMaterialColorFilter(float sat, float brightness);
    bool needDrawBehindWindow_ = false;
    bool stagingNeedDrawBehindWindow_ = false;
    // flags for clearing filter cache
    // All stagingXXX variables should be read & written by render_service thread
    bool stagingForceUseCache_ = false;
    bool stagingForceClearCache_ = false;
    bool stagingFilterHashChanged_ = false;
    bool stagingFilterRegionChanged_ = false;
    bool stagingFilterInteractWithDirty_ = false;
    bool stagingRotationChanged_ = false;
    bool stagingForceClearCacheForLastFrame_ = false;
    bool stagingIsAIBarInteractWithHWC_ = false;
    bool stagingIsEffectNode_ = false;
    bool stagingIntersectWithDRM_ = false;
    bool stagingIsDarkColorMode_ = false;
    int stagingUpdateInterval_ = 0;
    FilterCacheType stagingLastCacheType_ = FilterCacheType::NONE;

    // clear one of snapshot cache and filtered cache after drawing
    // All renderXXX variables should be read & written by render_thread or OnSync() function
    bool renderClearFilteredCacheAfterDrawing_ = false;
    bool renderFilterHashChanged_ = false;
    bool renderForceClearCacheForLastFrame_ = false;
    bool renderIsEffectNode_ = false;
    bool renderIsSkipFrame_ = false;
    bool renderIntersectWithDRM_ = false;
    bool renderIsDarkColorMode_ = false;
    // the type cache needed clear before drawing
    FilterCacheType stagingClearType_ = FilterCacheType::NONE;
    FilterCacheType renderClearType_ = FilterCacheType::NONE;
    FilterCacheType lastCacheType_ = FilterCacheType::NONE;
    bool stagingIsOccluded_ = false;

    // force cache with cacheUpdateInterval_
    bool stagingIsLargeArea_ = false;
    bool canSkipFrame_ = false;
    bool stagingIsSkipFrame_ = false;
    bool isFilterCacheValid_ = false; // catch status in current frame
    bool pendingPurge_ = false;
    uint32_t stagingCachedFilterHash_ = 0;
    RSFilter::FilterType filterType_ = RSFilter::NONE;
    int cacheUpdateInterval_ = 0;
    NodeId stagingNodeId_ = INVALID_NODEID;
    NodeId renderNodeId_ = INVALID_NODEID;
    std::string stagingNodeName_ = "invalid0";
    std::string renderNodeName_ = "invalid0";

    std::shared_ptr<RSFilter> filter_;
    std::shared_ptr<RSFilter> stagingFilter_;
    std::unique_ptr<RSFilterCacheManager> cacheManager_;
    RectI drawBehindWindowRegion_;
    RectI stagingDrawBehindWindowRegion_;
    void OnSync() override;
};
} // namespace ModifierNG
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_RS_RENDER_MODIFIER_NG_H
