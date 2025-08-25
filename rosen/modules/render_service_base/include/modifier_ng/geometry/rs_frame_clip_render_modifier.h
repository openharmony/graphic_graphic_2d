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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_GEOMETRY_RS_FRAME_CLIP_RENDER_MODIFIER_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_GEOMETRY_RS_FRAME_CLIP_RENDER_MODIFIER_H

#include "modifier_ng/rs_render_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSB_EXPORT RSFrameClipRenderModifier : public RSRenderModifier {
public:
    RSFrameClipRenderModifier() = default;
    ~RSFrameClipRenderModifier() override = default;

    static inline constexpr auto Type = RSModifierType::CLIP_TO_FRAME;
    RSModifierType GetType() const override
    {
        return Type;
    };

    static void ResetProperties(RSProperties& properties);

    void Apply(RSPaintFilterCanvas* canvas, RSProperties& properties) override
    {
        if (HasProperty(RSPropertyType::CUSTOM_CLIP_TO_FRAME) && canvas) {
            auto customClipToFrame = Getter<Vector4f>(RSPropertyType::CUSTOM_CLIP_TO_FRAME);
            auto customClipRect =
                Drawing::Rect(customClipToFrame.x_, customClipToFrame.y_, customClipToFrame.z_, customClipToFrame.w_);
            canvas->ClipRect(customClipRect);
        }
    }

private:
    static const LegacyPropertyApplierMap LegacyPropertyApplierMap_;
    const LegacyPropertyApplierMap& GetLegacyPropertyApplierMap() const override
    {
        return LegacyPropertyApplierMap_;
    }

    void OnSetDirty() override;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_GEOMETRY_RS_FRAME_CLIP_RENDER_MODIFIER_H
