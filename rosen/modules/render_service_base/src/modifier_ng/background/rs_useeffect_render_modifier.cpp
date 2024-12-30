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

#include "modifier_ng/background/rs_useeffect_render_modifier.h"

#include "common/rs_optional_trace.h"
#include "drawable/rs_property_drawable_utils.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen::ModifierNG {
const RSUseEffectRenderModifier::LegacyPropertyApplierMap RSUseEffectRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::USE_EFFECT, RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetUseEffect> },
    { RSPropertyType::USE_EFFECT_TYPE, RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetUseEffectType> },
};

void RSUseEffectRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetUseEffect(false);
    properties.SetUseEffectType(0);
}

bool RSUseEffectRenderModifier::OnApply(RSModifierContext& context)
{
    if (!Getter<bool>(RSPropertyType::USE_EFFECT, false)) {
        return false;
    }

    stagingUseEffectType_ = static_cast<UseEffectType>(
        Getter<int>(RSPropertyType::USE_EFFECT_TYPE, static_cast<int>(UseEffectType::DEFAULT)));

    // Find effect render node
    auto node = target_.lock();
    if (!node) {
        return false;
    }
    auto parentNode = node->GetParent().lock();
    while (parentNode && !parentNode->IsInstanceOf<RSEffectRenderNode>()) {
        parentNode = parentNode->GetParent().lock();
    }
    if (parentNode) {
        effectRenderNodeDrawableWeakRef_ = parentNode->GetRenderDrawable();
    } else {
        ROSEN_LOGD("RSUseEffectDrawable::OnGenerate: find EffectRenderNode failed.");
    }
    return true;
}

void RSUseEffectRenderModifier::OnSync()
{
    renderUseEffectType_ = stagingUseEffectType_;
}

void RSUseEffectRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    if (!RSSystemProperties::GetEffectMergeEnabled()) {
        return;
    }

    if (renderUseEffectType_ == UseEffectType::BEHIND_WINDOW && canvas.GetIsWindowFreezeCapture()) {
        RS_OPTIONAL_TRACE_NAME_FMT("RSUseEffectDrawable::CreateDrawFunc drawBehindWindow in surface capturing");
        RS_LOGD("RSUseEffectDrawable::CreateDrawFunc drawBehindWindow in surface capturing");
        canvas.Clear(Drawing::Color::COLOR_TRANSPARENT);
        return;
    }
    const auto& effectData = canvas.GetEffectData();
    if (renderUseEffectType_ != UseEffectType::BEHIND_WINDOW &&
        (effectData == nullptr || effectData->cachedImage_ == nullptr)) {
        ROSEN_LOGD("RSPropertyDrawableUtils::DrawUseEffect effectData null, try to generate.");
        auto drawable = effectRenderNodeDrawableWeakRef_.lock();
        if (!drawable) {
            return;
        }
        RS_TRACE_NAME_FMT("RSPropertyDrawableUtils::DrawUseEffect Fallback");
        RSAutoCanvasRestore arc(&canvas, RSPaintFilterCanvas::SaveType::kEnv);
        bool disableFilterCache = canvas.GetDisableFilterCache();
        canvas.SetDisableFilterCache(true);
        int8_t index = drawable->drawCmdIndex_.backgroundFilterIndex_;
        drawable->DrawImpl(static_cast<Drawing::Canvas&>(canvas), rect, index);
        canvas.SetDisableFilterCache(disableFilterCache);
        RSPropertyDrawableUtils::DrawUseEffect(&canvas, renderUseEffectType_);
        return;
    }
    RSPropertyDrawableUtils::DrawUseEffect(&canvas, renderUseEffectType_);
}
} // namespace OHOS::Rosen::ModifierNG
