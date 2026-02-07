/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_misc_drawable.h"

#include <chrono>

#include "feature/color_picker/color_pick_alt_manager.h"
#include "feature/color_picker/rs_color_picker_manager.h"
#include "feature/color_picker/rs_color_picker_thread.h"
#include "rs_profiler.h"

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "drawable/rs_property_drawable_utils.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "memory/rs_tag_tracker.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
constexpr int TRACE_LEVEL_TWO = 2;
// ==================== RSChildrenDrawable =====================
RSDrawable::Ptr RSChildrenDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSChildrenDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSChildrenDrawable::OnUpdate(const RSRenderNode& node)
{
    childrenHasSharedTransition_ = false;
    auto children = node.GetSortedChildren();
    // Regenerate children drawables
    needSync_ = true;
    stagingChildrenDrawableVec_.clear();

    if (LIKELY(!node.GetRenderProperties().GetUseShadowBatching())) {
        // Non-ShadowBatching mode (default), draw all children in order
        for (const auto& child : *children) {
            if (UNLIKELY(child->GetSharedTransitionParam()) && OnSharedTransition(child)) {
                continue;
            }
            auto childDrawable = RSRenderNodeDrawableAdapter::OnGenerate(child);
            if (!childDrawable) {
                continue;
            }
            if (childDrawable->GetSkipType() == SkipType::SKIP_SHADOW) {
                childDrawable->SetSkip(SkipType::NONE);
            }
            stagingChildrenDrawableVec_.push_back(std::move(childDrawable));
        }
    } else {
        // ShadowBatching mode, draw all shadows, then draw all children
        decltype(stagingChildrenDrawableVec_) pendingChildren;
        for (const auto& child : *children) {
            if (UNLIKELY(child->GetSharedTransitionParam()) && OnSharedTransition(child)) {
                continue;
            }
            auto childDrawable = RSRenderNodeDrawableAdapter::OnGenerate(child);
            if (!childDrawable) {
                continue;
            }
            auto shadowDrawable = RSRenderNodeDrawableAdapter::OnGenerateShadowDrawable(child, childDrawable);
            if (!shadowDrawable) {
                continue;
            }
            stagingChildrenDrawableVec_.push_back(std::move(shadowDrawable));
            pendingChildren.push_back(std::move(childDrawable));
        }
        // merge two vectors, shadow drawables first, render node drawables second
        stagingChildrenDrawableVec_.insert(
            stagingChildrenDrawableVec_.end(), pendingChildren.begin(), pendingChildren.end());
    }
    const_cast<RSRenderNode&>(node).SetChildrenHasSharedTransition(childrenHasSharedTransition_);
    return !stagingChildrenDrawableVec_.empty();
}

bool RSChildrenDrawable::OnSharedTransition(const RSRenderNode::SharedPtr& node)
{
    auto nodeId = node->GetId();
    const auto& sharedTransitionParam = node->GetSharedTransitionParam();

    auto pairedNode = sharedTransitionParam->GetPairedNode(nodeId);
    if (!pairedNode || !pairedNode->IsOnTheTree()) {
        sharedTransitionParam->paired_ = false;
        return false;
    }

    childrenHasSharedTransition_ = true;
    // Skip if the shared transition is not paired (Note: this may cause the lower node to be drawn twice)
    if (!sharedTransitionParam->paired_) {
        return false;
    }

    // Relation will be set in QuickPrepare
    if (!sharedTransitionParam->HasRelation()) {
        sharedTransitionParam->SetNeedGenerateDrawable(true);
        return true;
    }

    // Test if this node is lower in the hierarchy
    bool isLower = sharedTransitionParam->IsLower(nodeId);
    if (isLower) {
        // for lower hierarchy node, we skip it here
        return true;
    } else {
        // for higher hierarchy node, we add paired node (lower in hierarchy) first, then add it
        if (auto childDrawable = RSRenderNodeDrawableAdapter::OnGenerate(pairedNode)) {
            stagingChildrenDrawableVec_.push_back(std::move(childDrawable));
        }
        sharedTransitionParam->SetNeedGenerateDrawable(false);
        return false;
    }
}

void RSChildrenDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    std::swap(stagingChildrenDrawableVec_, childrenDrawableVec_);
    RSRenderNodeDrawableAdapter::AddToClearDrawables(stagingChildrenDrawableVec_);
    needSync_ = false;
}

void RSChildrenDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    for (size_t i = 0; i < childrenDrawableVec_.size(); i++) {
#ifdef RS_ENABLE_PREFETCH
        size_t prefetchIndex = i + 2;
        if (prefetchIndex < childrenDrawableVec_.size()) {
            __builtin_prefetch(&(childrenDrawableVec_[prefetchIndex]), 0, 1);
        }
#endif
        const auto& drawable = childrenDrawableVec_[i];
        drawable->Draw(*canvas);
    }
}

// ==================== RSColorPickerDrawable =====================
RSColorPickerDrawable::RSColorPickerDrawable(bool useAlt, NodeId nodeId)
{
    if (useAlt) {
        colorPickerManager_ = std::make_shared<ColorPickAltManager>(nodeId);
    } else {
        colorPickerManager_ = std::make_shared<RSColorPickerManager>(nodeId);
    }
}
RSDrawable::Ptr RSColorPickerDrawable::OnGenerate(const RSRenderNode& node)
{
    auto colorPicker = node.GetRenderProperties().GetColorPicker();
    const bool useAlt = colorPicker ? colorPicker->strategy == ColorPickStrategyType::CLIENT_CALLBACK : false;
    if (auto ret = std::make_shared<RSColorPickerDrawable>(useAlt, node.GetId()); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

std::pair<bool, bool> RSColorPickerDrawable::PrepareForExecution(uint64_t vsyncTime, bool darkMode)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSColorPickerDrawable::Preparing node %" PRIu64 " darkMode=%d", nodeId_, darkMode);
    const bool darkModeChanged = std::exchange(stagingIsSystemDarkColorMode_, darkMode) != darkMode;
    const bool prev = std::exchange(stagingNeedColorPick_, false);
    if (!stagingColorPicker_ || stagingColorPicker_->strategy == ColorPickStrategyType::NONE) {
        return { false, prev != stagingNeedColorPick_ || darkModeChanged };
    }

    constexpr uint64_t NS_TO_MS = 1000000; // Convert nanoseconds to milliseconds
    uint64_t interval = stagingColorPicker_->interval;
    uint64_t vsyncTimeMs = vsyncTime / NS_TO_MS;
    if (vsyncTimeMs >= interval + lastUpdateTime_) {
        stagingNeedColorPick_ = true;
        lastUpdateTime_ = vsyncTimeMs;
        isTaskScheduled_ = false; // Reset flag when processing a new frame
    } else if (!isTaskScheduled_) {
        // Schedule a postponed task to catch frames rendered during cooldown
        uint64_t remainingDelay = (interval + lastUpdateTime_) - vsyncTimeMs;
        isTaskScheduled_ = true;
        RSColorPickerThread::Instance().PostTask(
            [nodeId = stagingNodeId_]() { RSColorPickerThread::Instance().NotifyNodeDirty(nodeId); }, false,
            static_cast<int64_t>(remainingDelay));
    }
    return { stagingNeedColorPick_, prev != stagingNeedColorPick_ || darkModeChanged };
}

bool RSColorPickerDrawable::OnUpdate(const RSRenderNode& node)
{
    stagingNodeId_ = node.GetId();
    stagingColorPicker_ = node.GetRenderProperties().GetColorPicker();
    needSync_ = true;
    return stagingColorPicker_ && stagingColorPicker_->strategy != ColorPickStrategyType::NONE;
}

void RSColorPickerDrawable::OnSync()
{
    needColorPick_ = stagingNeedColorPick_;
    if (colorPickerManager_) {
        colorPickerManager_->SetSystemDarkColorMode(stagingIsSystemDarkColorMode_);
    }
    if (!needSync_) {
        return;
    }
    nodeId_ = stagingNodeId_;
    params_ = stagingColorPicker_ ? *stagingColorPicker_ : ColorPickerParam();

    needSync_ = false;
}

void RSColorPickerDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (!colorPickerManager_) {
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    if (!paintFilterCanvas) {
        return;
    }

    auto maybeColor = colorPickerManager_->GetColorPick();
    if (maybeColor.has_value()) {
        paintFilterCanvas->SetColorPicked(maybeColor.value());
    }
    RS_OPTIONAL_TRACE_NAME_FMT("ColorPicker: onDraw nodeId=%" PRIu64 " rect=[%s], need execute = %d", nodeId_,
        rect ? rect->ToString().c_str() : "null", needColorPick_);

    if (needColorPick_) {
        colorPickerManager_->ScheduleColorPick(*paintFilterCanvas, rect, params_);
    }
}

void RSColorPickerDrawable::SetIsSystemDarkColorMode(bool isSystemDarkColorMode)
{
    if (stagingIsSystemDarkColorMode_ != isSystemDarkColorMode) {
        stagingIsSystemDarkColorMode_ = isSystemDarkColorMode;
        needSync_ = true;
    }
}

// ==================== RSCustomModifierDrawable ===================
RSDrawable::Ptr RSCustomModifierDrawable::OnGenerate(const RSRenderNode& node, ModifierNG::RSModifierType type)
{
    if (auto ret = std::make_shared<RSCustomModifierDrawable>(type); ret->OnUpdate(node)) {
        if (node.GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
            ret->needClearOp_ = true;
        }
        return std::move(ret);
    }
    return nullptr;
}

bool RSCustomModifierDrawable::OnUpdate(const RSRenderNode& node)
{
    auto customModifiers = node.GetModifiersNG(modifierTypeNG_);
    if (customModifiers.empty()) {
        return false;
    }
    std::stable_sort(
        customModifiers.begin(), customModifiers.end(), [](const auto& modifierA, const auto& modifierB) -> bool {
            return modifierA->template Getter<int16_t>(ModifierNG::RSPropertyType::CUSTOM_INDEX, 0) <
                   modifierB->template Getter<int16_t>(ModifierNG::RSPropertyType::CUSTOM_INDEX, 0);
        });

    stagingGravity_ = node.GetRenderProperties().GetFrameGravity();
    stagingIsCanvasNode_ = node.IsInstanceOf<RSCanvasRenderNode>() && !node.IsInstanceOf<RSCanvasDrawingRenderNode>();
    // regenerate stagingDrawCmdList_
    needSync_ = true;
    stagingDrawCmdListVec_.clear();
    if (node.GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE &&
        modifierTypeNG_ == ModifierNG::RSModifierType::CONTENT_STYLE) {
        auto& drawingNode = static_cast<const RSCanvasDrawingRenderNode&>(node);
        auto& cmdLists = drawingNode.GetDrawCmdListsNG();
        auto itr = cmdLists.find(modifierTypeNG_);
        if (itr == cmdLists.end() || itr->second.empty()) {
            return false;
        }
        for (auto& cmd : itr->second) {
            stagingDrawCmdListVec_.emplace_back(cmd);
        }
    } else {
        for (const auto& modifier : customModifiers) {
            auto propertyType = ModifierNG::ModifierTypeConvertor::GetPropertyType(modifierTypeNG_);
            auto drawCmdList = modifier->Getter<Drawing::DrawCmdListPtr>(propertyType, nullptr);
            if (drawCmdList == nullptr || drawCmdList->IsEmpty()) {
                continue;
            }
            if (drawCmdList->GetWidth() > 0 && drawCmdList->GetHeight() > 0) {
                stagingDrawCmdListVec_.push_back(drawCmdList);
            }
        }
    }
    return !stagingDrawCmdListVec_.empty();
}

void RSCustomModifierDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    gravity_ = stagingGravity_;
    isCanvasNode_ = stagingIsCanvasNode_;
    std::swap(stagingDrawCmdListVec_, drawCmdListVec_);
    RSRenderNodeDrawableAdapter::AddToClearCmdList(stagingDrawCmdListVec_);
    needSync_ = false;
}

void RSCustomModifierDrawable::OnPurge()
{
    for (auto &drawCmdList : drawCmdListVec_) {
        if (drawCmdList) {
            drawCmdList->Purge();
        }
    }
}

void RSCustomModifierDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(
        canvas ? canvas->GetGPUContext() : nullptr, RSTagTracker::SOURCETYPE::SOURCE_RSCUSTOMMODIFIERDRAWABLE);
#endif
    for (size_t i = 0; i < drawCmdListVec_.size(); i++) {
#ifdef RS_ENABLE_PREFETCH
        size_t prefetchIndex = i + 2;
        if (prefetchIndex < drawCmdListVec_.size()) {
            __builtin_prefetch(&(drawCmdListVec_[prefetchIndex]), 0, 1);
        }
#endif
        const auto& drawCmdList = drawCmdListVec_[i];
        Drawing::Matrix mat;
        if (isCanvasNode_ && RSPropertyDrawableUtils::GetGravityMatrix(
            gravity_, *rect, drawCmdList->GetWidth(), drawCmdList->GetHeight(), mat)) {
            canvas->ConcatMatrix(mat);
        }
        drawCmdList->Playback(*canvas, rect);
        if (needClearOp_ && modifierTypeNG_ == ModifierNG::RSModifierType::CONTENT_STYLE) {
            RS_PROFILER_DRAWING_NODE_ADD_CLEAROP(drawCmdList);
        }
    }
}

// ============================================================================
// Save and Restore
RSSaveDrawable::RSSaveDrawable(std::shared_ptr<uint32_t> content) : content_(std::move(content)) {}
void RSSaveDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    // Save and return save count
    *content_ = paintFilterCanvas->Save();
}

RSRestoreDrawable::RSRestoreDrawable(std::shared_ptr<uint32_t> content) : content_(std::move(content)) {}
void RSRestoreDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    // return to previous save count
    paintFilterCanvas->RestoreToCount(*content_);
}

RSCustomSaveDrawable::RSCustomSaveDrawable(
    std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content, RSPaintFilterCanvas::SaveType type)
    : content_(std::move(content)), type_(type)
{}
void RSCustomSaveDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    // Save and return save count
    *content_ = paintFilterCanvas->SaveAllStatus(type_);
}

RSCustomRestoreDrawable::RSCustomRestoreDrawable(std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content)
    : content_(std::move(content))
{}
void RSCustomRestoreDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    // return to previous save count
    paintFilterCanvas->RestoreStatus(*content_);
}

RSDrawable::Ptr RSBeginBlenderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBeginBlenderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

void RSBeginBlenderDrawable::PostUpdate(const RSRenderNode& node)
{
    enableEDREffect_ = node.GetRenderProperties().GetFgBrightnessEnableEDR();
    if (enableEDREffect_) {
        screenNodeId_ = node.GetScreenNodeId();
    }
}

bool RSBeginBlenderDrawable::OnUpdate(const RSRenderNode& node)
{
    if (!RSSystemProperties::GetFgBlenderEnabled()) {
        ROSEN_LOGD("RSBeginBlenderDrawable::OnUpdate: blender is Disabled!");
        return false;
    }
    // the order of blender and blendMode cannot be considered currently
    const RSProperties& properties = node.GetRenderProperties();
    auto blendMode = properties.GetColorBlendMode();
    stagingBlendApplyType_ = properties.GetColorBlendApplyType();
    // NOTE: stagingIsDangerous_ should be set true when adding a blender that may generate transparent pixels
    if (properties.IsFgBrightnessValid()) {
        if (Rosen::RSSystemProperties::GetDebugTraceLevel() >= TRACE_LEVEL_TWO) {
            stagingPropertyDescription_ = properties.GetFgBrightnessDescription();
        }
        stagingBlender_ = RSPropertyDrawableUtils::MakeDynamicBrightnessBlender(
            properties.GetFgBrightnessParams().value());
        stagingIsDangerous_ = false;
    } else if (blendMode && blendMode != static_cast<int>(RSColorBlendMode::NONE)) {
        if (Rosen::RSSystemProperties::GetDebugTraceLevel() >= TRACE_LEVEL_TWO) {
            stagingPropertyDescription_ = "BlendMode, blendMode: " + std::to_string(blendMode) +
                                          " blendApplyType: " + std::to_string(stagingBlendApplyType_);
        }
        // map blendMode to Drawing::BlendMode and convert to Blender
        stagingBlender_ = Drawing::Blender::CreateWithBlendMode(static_cast<Drawing::BlendMode>(blendMode - 1));
        stagingIsDangerous_ = RSPropertyDrawableUtils::IsDangerousBlendMode(blendMode - 1, stagingBlendApplyType_);
    } else if (properties.IsShadowBlenderValid()) {
        if (Rosen::RSSystemProperties::GetDebugTraceLevel() >= TRACE_LEVEL_TWO) {
            stagingPropertyDescription_ = properties.GetShadowBlenderDescription();
        }
        stagingBlender_ = RSPropertyDrawableUtils::MakeShadowBlender(properties.GetShadowBlenderParams().value());
        stagingIsDangerous_ = false;
    } else {
        return false;
    }

    needSync_ = true;
    PostUpdate(node);

    return true;
}

void RSBeginBlenderDrawable::OnSync()
{
    if (needSync_ == false) {
        return;
    }
    blender_ = stagingBlender_;
    blendApplyType_ = stagingBlendApplyType_;
    propertyDescription_ = stagingPropertyDescription_;
    stagingPropertyDescription_.clear();
    needSync_ = false;
}

void RSBeginBlenderDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (canvas->GetDrawingType() != Drawing::DrawingType::PAINT_FILTER) {
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(paintFilterCanvas ? paintFilterCanvas->GetGPUContext() : nullptr,
        RSTagTracker::SOURCETYPE::SOURCE_RSBEGINBLENDERDRAWABLE);
#endif
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO, "RSBeginBlenderDrawable:: %s, bounds: %s",
        propertyDescription_.c_str(), rect->ToString().c_str());
    RSPropertyDrawableUtils::BeginBlender(*paintFilterCanvas, blender_, blendApplyType_, isDangerous_);
}

RSDrawable::Ptr RSEndBlenderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSEndBlenderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSEndBlenderDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (properties.GetColorBlendMode() == static_cast<int>(RSColorBlendMode::NONE) ||
        properties.GetColorBlendApplyType() == static_cast<int>(RSColorBlendApplyType::FAST)) {
        // no blend
        return false;
    }

    stagingBlendApplyType_ = properties.GetColorBlendApplyType();
    needSync_ = true;

    return true;
}

void RSEndBlenderDrawable::OnSync()
{
    if (needSync_ == false) {
        return;
    }
    blendApplyType_ = stagingBlendApplyType_;
    needSync_ = false;
}

void RSEndBlenderDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    RSPropertyDrawableUtils::EndBlender(*paintFilterCanvas, blendApplyType_);
}

// ============================================================================
// EnvFGColor
RSDrawable::Ptr RSEnvFGColorDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSEnvFGColorDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSEnvFGColorDrawable::OnUpdate(const RSRenderNode& node)
{
    auto modifier = node.GetModifierNG(ModifierNG::RSModifierType::ENV_FOREGROUND_COLOR);
    if (modifier == nullptr) {
        return false;
    }
    if (!modifier->HasProperty(ModifierNG::RSPropertyType::ENV_FOREGROUND_COLOR)) {
        return false;
    }
    stagingEnvFGColor_ = modifier->Getter<Color>(ModifierNG::RSPropertyType::ENV_FOREGROUND_COLOR, Color());
    needSync_ = true;
    return true;
}

void RSEnvFGColorDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    envFGColor_ = stagingEnvFGColor_;
    needSync_ = false;
}

void RSEnvFGColorDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    // planning: implement alpha offscreen
    paintFilterCanvas->SetEnvForegroundColor(envFGColor_);
}

// ============================================================================
// EnvFGColorStrategy
RSDrawable::Ptr RSEnvFGColorStrategyDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSEnvFGColorStrategyDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSEnvFGColorStrategyDrawable::OnUpdate(const RSRenderNode& node)
{
    auto modifier = node.GetModifierNG(ModifierNG::RSModifierType::ENV_FOREGROUND_COLOR);
    if (modifier == nullptr) {
        return false;
    }
    stagingEnvFGColorStrategy_ = static_cast<ForegroundColorStrategyType>(
        modifier->Getter<int>(ModifierNG::RSPropertyType::ENV_FOREGROUND_COLOR_STRATEGY, 0));
    const auto& renderProperties = node.GetRenderProperties();
    stagingBackgroundColor_ = renderProperties.GetBackgroundColor();
    stagingNeedClipToBounds_ = renderProperties.GetClipToBounds();
    stagingBoundsRect_ = renderProperties.GetBounds();
    needSync_ = true;
    return true;
}

void RSEnvFGColorStrategyDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    envFGColorStrategy_ = stagingEnvFGColorStrategy_;
    backgroundColor_ = stagingBackgroundColor_;
    needClipToBounds_ = stagingNeedClipToBounds_;
    boundsRect_ = stagingBoundsRect_;
    needSync_ = false;
}

void RSEnvFGColorStrategyDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    switch (envFGColorStrategy_) {
        case ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR: {
            // calculate the color by screebshot
            Color color = RSPropertyDrawableUtils::GetInvertBackgroundColor(
                *paintFilterCanvas, needClipToBounds_, boundsRect_, backgroundColor_);
            paintFilterCanvas->SetEnvForegroundColor(color);
            break;
        }
        default: {
            break;
        }
    }
}

RSDrawable::Ptr RSCustomClipToFrameDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSCustomClipToFrameDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSCustomClipToFrameDrawable::OnUpdate(const RSRenderNode& node)
{
    const auto modifier = node.GetModifierNG(ModifierNG::RSModifierType::CLIP_TO_FRAME);
    if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::CUSTOM_CLIP_TO_FRAME)) {
        return false;
    }
    const auto& clipRectV4f = modifier->Getter<Vector4f>(ModifierNG::RSPropertyType::CUSTOM_CLIP_TO_FRAME, Vector4f());
    stagingCustomClipRect_ = Drawing::Rect(clipRectV4f.x_, clipRectV4f.y_, clipRectV4f.z_, clipRectV4f.w_);
    needSync_ = true;
    return true;
}

void RSCustomClipToFrameDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    customClipRect_ = stagingCustomClipRect_;
    needSync_ = false;
}

void RSCustomClipToFrameDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    canvas->ClipRect(customClipRect_);
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
