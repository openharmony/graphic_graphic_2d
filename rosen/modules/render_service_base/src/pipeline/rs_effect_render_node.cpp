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

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "memory/rs_memory_track.h"
#include "params/rs_effect_render_params.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {

RSEffectRenderNode::RSEffectRenderNode(NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : RSRenderNode(id, context, isTextureExportNode)
{
#ifndef ROSEN_ARKUI_X
    MemoryInfo info = { sizeof(*this), ExtractPid(id), id, 0, MEMORY_TYPE::MEM_RENDER_NODE, ExtractPid(id) };
    MemoryTrack::Instance().AddNodeRecord(id, info);
#endif
    MemorySnapshot::Instance().AddCpuMemory(ExtractPid(id), sizeof(*this));
}

RSEffectRenderNode::~RSEffectRenderNode()
{
#ifndef ROSEN_ARKUI_X
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
#endif
    MemorySnapshot::Instance().RemoveCpuMemory(ExtractPid(GetId()), sizeof(*this));
}

void RSEffectRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->PrepareEffectRenderNode(*this);
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
        !(RSSystemProperties::GetEffectMergeEnabled() && RSFilterCacheManager::isCCMEffectMergeEnable_) ||
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

void RSEffectRenderNode::CheckBlurFilterCacheNeedForceClearOrSave(bool rotationChanged, bool rotationStatusChanged)
{
#ifdef RS_ENABLE_GPU
    if (GetRenderProperties().GetBackgroundFilter() == nullptr) {
        return;
    }
    auto filterDrawable = GetFilterDrawable(false);
    if (filterDrawable == nullptr) {
        return;
    }
    filterDrawable->MarkEffectNode();
    RSRenderNode::CheckBlurFilterCacheNeedForceClearOrSave(rotationChanged, rotationStatusChanged);
    if (IsForceClearOrUseFilterCache(filterDrawable)) {
        return;
    }
    if (CheckFilterCacheNeedForceClear()) {
        filterDrawable->MarkFilterForceClearCache();
    } else if (CheckFilterCacheNeedForceSave()) {
        filterDrawable->MarkFilterForceUseCache();
    }
#endif
}

void RSEffectRenderNode::UpdateFilterCacheWithSelfDirty()
{
#ifdef RS_ENABLE_GPU
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (!RSProperties::filterCacheEnabled_) {
        ROSEN_LOGE("RSEffectRenderNode::UpdateFilterCacheManagerWithCacheRegion filter cache is disabled.");
        return;
    }
#endif
    auto filterDrawable = GetFilterDrawable(false);
    if (filterDrawable == nullptr || IsForceClearOrUseFilterCache(filterDrawable)) {
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSEffectRenderNode[%llu]::UpdateFilterCacheWithSelfDirty lastRect:%s, currRegion:%s",
        GetId(), GetFilterCachedRegion().ToString().c_str(), filterRegion_.ToString().c_str());
    if (filterRegion_ == GetFilterCachedRegion()) {
        return;
    }
    // effect render node  only support background filter
    MarkFilterStatusChanged(false, true);
#endif
}

#ifdef RS_ENABLE_GPU
void RSEffectRenderNode::MarkFilterCacheFlags(std::shared_ptr<DrawableV2::RSFilterDrawable>& filterDrawable,
    RSDirtyRegionManager& dirtyManager, bool needRequestNextVsync)
{
    lastFrameHasVisibleEffect_ = ChildHasVisibleEffect();
    if (IsForceClearOrUseFilterCache(filterDrawable)) {
        return;
    }
    // use for skip-frame when screen rotation
    if (isRotationChanged_) {
        filterDrawable->MarkRotationChanged();
    }
    RSRenderNode::MarkFilterCacheFlags(filterDrawable, dirtyManager, needRequestNextVsync);
}
#endif

bool RSEffectRenderNode::CheckFilterCacheNeedForceSave()
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSEffectRenderNode[%llu]::CheckFilterCacheNeedForceSave"
        " isBackgroundImage:%d, isRotationChanged_:%d, IsStaticCached():%d",
        GetId(), GetRenderProperties().GetBgImage() != nullptr, isRotationChanged_, IsStaticCached());
    return GetRenderProperties().GetBgImage() != nullptr || (IsStaticCached() && !isRotationChanged_);
}

bool RSEffectRenderNode::CheckFilterCacheNeedForceClear()
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSEffectRenderNode[%llu]::CheckFilterCacheNeedForceClear foldStatusChanged_:%d,"
        " preRotationStatus_:%d, isRotationChanged_:%d, preStaticStatus_:%d, isStaticCached:%d",
        GetId(), foldStatusChanged_, preRotationStatus_, isRotationChanged_, preStaticStatus_, IsStaticCached());
    if (preStaticStatus_ != isStaticCached_) {
        RS_TRACE_NAME_FMT("RSEffectRenderNode[%llu]::CheckFilterCacheNeedForceClear"
            " preStaticStatus_:%d, isStaticCached:%d", GetId(), preStaticStatus_, IsStaticCached());
        RS_LOGI("RSEffectRenderNode[%{public}lld]::CheckFilterCacheNeedForceClear foldStatusChanged_:%{public}d,"
            " preRotationStatus_:%{public}d, isRotationChanged_:%{public}d,"
            " preStaticStatus_:%{public}d, isStaticCached:%{public}d", static_cast<long long>(GetId()),
            foldStatusChanged_, preRotationStatus_, isRotationChanged_, preStaticStatus_, IsStaticCached());
    }
    // case 3: the state of freeze changed to false, and the last cache maybe wrong.
    bool res = foldStatusChanged_ || (preRotationStatus_ != isRotationChanged_) ||
        (preStaticStatus_ != isStaticCached_ && isStaticCached_ == false);
    preStaticStatus_ = IsStaticCached();
    return res;
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

void RSEffectRenderNode::InitRenderParams()
{
#ifdef RS_ENABLE_GPU
    stagingRenderParams_ = std::make_unique<RSEffectRenderParams>(GetId());
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(shared_from_this());
    if (renderDrawable_ == nullptr) {
        RS_LOGE("RSEffectRenderNode::InitRenderParams failed");
        return;
    }
#endif
}

void RSEffectRenderNode::MarkFilterHasEffectChildren()
{
#ifdef RS_ENABLE_GPU
    if (GetRenderProperties().GetBackgroundFilter() == nullptr) {
        return;
    }
    auto effectParams = static_cast<RSEffectRenderParams*>(stagingRenderParams_.get());
    if (effectParams == nullptr) {
        return;
    }
    effectParams->SetHasEffectChildren(ChildHasVisibleEffect());
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (!RSProperties::filterCacheEnabled_) {
        UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::BACKGROUND_FILTER);
    }
#endif
#endif
}

void RSEffectRenderNode::OnFilterCacheStateChanged()
{
#ifdef RS_ENABLE_GPU
    auto filterDrawable = GetFilterDrawable(false);
    auto effectParams = static_cast<RSEffectRenderParams*>(stagingRenderParams_.get());
    if (filterDrawable == nullptr || effectParams == nullptr) {
        return;
    }
    effectParams->SetCacheValid(filterDrawable->IsFilterCacheValid());
#endif
}

bool RSEffectRenderNode::EffectNodeShouldPaint() const
{
    return ChildHasVisibleEffect();
}

bool RSEffectRenderNode::FirstFrameHasEffectChildren() const
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSEffectRenderNode[%llu]::FirstFrameHasEffectChildren lastHasVisibleEffect:%d,"
        "hasVisibleEffect:%d", GetId(), lastFrameHasVisibleEffect_, ChildHasVisibleEffect());
    return GetRenderProperties().GetBackgroundFilter() != nullptr &&
        !lastFrameHasVisibleEffect_ && ChildHasVisibleEffect();
}

bool RSEffectRenderNode::FirstFrameHasNoEffectChildren() const
{
    return GetRenderProperties().GetBackgroundFilter() != nullptr &&
        !ChildHasVisibleEffect() && lastFrameHasVisibleEffect_;
}

void RSEffectRenderNode::MarkClearFilterCacheIfEffectChildrenChanged()
{
#ifdef RS_ENABLE_GPU
    // the first frame node has no effect child, it should not be painted and filter cache need to be cleared.
    auto filterDrawable = GetFilterDrawable(false);
    if (filterDrawable == nullptr || filterDrawable->IsForceClearFilterCache()) {
        return;
    }
    if (!FirstFrameHasEffectChildren() && !FirstFrameHasNoEffectChildren()) {
        return;
    }
    filterDrawable->MarkFilterForceClearCache();
    // the first frame node has no effect child, force use cache should be cancled.
    if (filterDrawable->IsForceUseFilterCache()) {
        filterDrawable->MarkFilterForceUseCache(false);
    }
#endif
}

void RSEffectRenderNode::SetEffectIntersectWithDRM(bool intersect)
{
    auto effectParams = static_cast<RSEffectRenderParams*>(stagingRenderParams_.get());
    if (effectParams == nullptr) {
        return;
    }
    effectParams->SetEffectIntersectWithDRM(intersect);
}

void RSEffectRenderNode::SetDarkColorMode(bool isDark)
{
    auto effectParams = static_cast<RSEffectRenderParams*>(stagingRenderParams_.get());
    if (effectParams == nullptr) {
        return;
    }
    effectParams->SetDarkColorMode(isDark);
}
} // namespace Rosen
} // namespace OHOS
