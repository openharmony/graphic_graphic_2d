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

#include "pipeline/rs_effect_render_node.h"

#include "memory/rs_memory_track.h"

#include "common/rs_obj_abs_geometry.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "visitor/rs_node_visitor.h"
#include "platform/common/rs_system_properties.h"
#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {

RSEffectRenderNode::RSEffectRenderNode(NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : RSRenderNode(id, context, isTextureExportNode)
{
#ifndef ROSEN_ARKUI_X
    MemoryInfo info = { sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE };
    MemoryTrack::Instance().AddNodeRecord(id, info);
#endif
}

RSEffectRenderNode::~RSEffectRenderNode()
{
#ifndef ROSEN_ARKUI_X
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
#endif
}

void RSEffectRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->PrepareEffectRenderNode(*this);
    preStaticStatus_ = IsStaticCached();
}

void RSEffectRenderNode::QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->QuickPrepareEffectRenderNode(*this);
}

void RSEffectRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessEffectRenderNode(*this);
}

void RSEffectRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    RSRenderNode::ProcessTransitionBeforeChildren(canvas);
    auto& properties = GetRenderProperties();
    // Disable effect region if either of the following conditions is met:
    // 1. No child with useEffect(true) (needFilter_ == false)
    // 2. Background filter is null
    // 3. Canvas is offscreen
    if (!properties.GetHaveEffectRegion() || properties.GetBackgroundFilter() == nullptr ||
        !RSSystemProperties::GetEffectMergeEnabled() ||
        canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        canvas.SetEffectData(nullptr);
        return;
    }

    if (properties.GetBgImage() == nullptr) {
        // EffectRenderNode w/o background image, use snapshot as underlay image
        RSPropertiesPainter::DrawBackgroundEffect(properties, canvas);
    } else {
        // EffectRenderNode w/ background image, use bg image as underlay image
        RSPropertiesPainter::DrawBackgroundImageAsEffect(properties, canvas);
    }
}

RectI RSEffectRenderNode::GetFilterRect() const
{
    if (!ChildHasVisibleEffect()) {
        return {};
    }
    return RSRenderNode::GetFilterRect();
}

void RSEffectRenderNode::SetEffectRegion(const std::optional<Drawing::RectI>& effectRegion)
{
    if (!effectRegion.has_value() || !effectRegion->IsValid()) {
        ROSEN_LOGD("RSEffectRenderNode::SetEffectRegion: no effect region.");
        GetMutableRenderProperties().SetHaveEffectRegion(false);
        return;
    }

    const auto& properties = GetRenderProperties();
    const auto& absRect = properties.GetBoundsGeometry()->GetAbsRect();
    Drawing::RectI effectRect = effectRegion.value();
    if (!effectRect.Intersect(
        Drawing::RectI(absRect.GetLeft(), absRect.GetTop(), absRect.GetRight(), absRect.GetBottom()))) {
        ROSEN_LOGD("RSEffectRenderNode::SetEffectRegion: no valid effect region.");
        GetMutableRenderProperties().SetHaveEffectRegion(false);
        return;
    }
    GetMutableRenderProperties().SetHaveEffectRegion(true);
}

void RSEffectRenderNode::UpdateFilterCacheWithSelfDirty()
{
    if (!RSProperties::FilterCacheEnabled) {
        ROSEN_LOGE("RSEffectRenderNode::UpdateFilterCacheManagerWithCacheRegion filter cache is disabled.");
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("effectNode[%llu] UpdateFilterCacheWithSelfDirty lastRect:%s, currRegion:%s",
        GetId(), GetFilterCachedRegion().ToString().c_str(), filterRegion_.ToString().c_str());
    if (filterRegion_ == GetFilterCachedRegion()) {
        return;
    }
    // effect render node  only support background filter
    MarkFilterStatusChanged(false, true);
}

void RSEffectRenderNode::MarkFilterCacheFlagsAfterPrepare(
    std::shared_ptr<DrawableV2::RSFilterDrawable>& filterDrawable, bool isForeground)
{
    if (filterDrawable == nullptr) {
        return;
    }
    if (!(filterDrawable->GetFilterForceClearCache()) && CheckFilterCacheNeedForceClear()) {
        filterDrawable->MarkFilterForceClearCache();
    } else if (CheckFilterCacheNeedForceSave()) {
        filterDrawable->MarkFilterForceUseCache();
    }
    if (isRotationChanged_) {
        filterDrawable->MarkRotationChanged();
    }

    RSRenderNode::MarkFilterCacheFlagsAfterPrepare(filterDrawable, isForeground);
    preStaticStatus_ = IsStaticCached();
    lastFrameHasVisibleEffect_ = ChildHasVisibleEffect();
}

bool RSEffectRenderNode::CheckFilterCacheNeedForceSave()
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSEffectRenderNode[%llu]::CheckFilterCacheNeedForceSave"
        " isBackgroundImage:%d, isRotationChanged_:%d, IsStaticCached():%d,",
        GetId(), GetRenderProperties().GetBgImage() != nullptr, isRotationChanged_, IsStaticCached());
    return GetRenderProperties().GetBgImage() != nullptr || (IsStaticCached() && !isRotationChanged_);
}

bool RSEffectRenderNode::CheckFilterCacheNeedForceClear()
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSEffectRenderNode[%llu]::CheckFilterCacheNeedForceClear foldStatusChanged_:%d,"
        " preRotationStatus_:%d, isRotationChanged_:%d, preStaticStatus_:%d, isStaticCached:%d,"
        " hasVisibleEffect:%d", GetId(), foldStatusChanged_, preRotationStatus_, isRotationChanged_,
        preStaticStatus_, IsStaticCached(), ChildHasVisibleEffect());
    return foldStatusChanged_ || (preRotationStatus_ != isRotationChanged_) ||
        (preStaticStatus_ != IsStaticCached()) || (!ChildHasVisibleEffect() && lastFrameHasVisibleEffect_);
}

void RSEffectRenderNode::SetRotationChanged(bool isRotationChanged)
{
    preRotationStatus_ = isRotationChanged_;
    isRotationChanged_ = isRotationChanged;
}

bool RSEffectRenderNode::GetRotationChanged() const
{
    return isRotationChanged_;
}

void RSEffectRenderNode::SetCurrentAttachedScreenId(uint64_t screenId)
{
    currentAttachedScreenId_ = screenId;
}

uint64_t RSEffectRenderNode::GetCurrentAttachedScreenId() const
{
    return currentAttachedScreenId_;
}

void RSEffectRenderNode::SetFoldStatusChanged(bool foldStatusChanged)
{
    foldStatusChanged_ = foldStatusChanged;
}
} // namespace Rosen
} // namespace OHOS
