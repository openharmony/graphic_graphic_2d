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

#include "rs_profiler.h"

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "drawable/rs_property_drawable_utils.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "memory/rs_tag_tracker.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_render_node.h"

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
    stagingChildrenDrawableVec_.clear();
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSChildrenDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSChildrenDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        for (size_t i = 0; i < ptr->childrenDrawableVec_.size(); i++) {
#ifdef RS_ENABLE_PREFETCH
            size_t prefetchIndex = i + 2;
            if (prefetchIndex < ptr->childrenDrawableVec_.size()) {
                __builtin_prefetch(&(ptr->childrenDrawableVec_[prefetchIndex]), 0, 1);
            }
#endif
            const auto& drawable = ptr->childrenDrawableVec_[i];
            drawable->Draw(*canvas);
        }
    };
}

// ==================== RSCustomModifierDrawable ===================
RSDrawable::Ptr RSCustomModifierDrawable::OnGenerate(const RSRenderNode& node, RSModifierType type)
{
    if (auto ret = std::make_shared<RSCustomModifierDrawable>(type); ret->OnUpdate(node)) {
        if (node.GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
            ret->needClearOp_ = true;
        }
        return std::move(ret);
    }
    return nullptr;
}

#if defined(MODIFIER_NG)
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
#else
bool RSCustomModifierDrawable::OnUpdate(const RSRenderNode& node)
{
    const auto& drawCmdModifiers = node.GetDrawCmdModifiers();
    auto itr = drawCmdModifiers.find(modifierType_);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return false;
    }
    std::vector<std::shared_ptr<RSRenderModifier>> modifiersVec(itr->second.begin(), itr->second.end());
    std::stable_sort(
        modifiersVec.begin(), modifiersVec.end(), [](const auto& modifierA, const auto& modifierB) -> bool {
            return std::static_pointer_cast<RSDrawCmdListRenderModifier>(modifierA)->GetIndex() <
                   std::static_pointer_cast<RSDrawCmdListRenderModifier>(modifierB)->GetIndex();
        });

    stagingGravity_ = node.GetRenderProperties().GetFrameGravity();
    stagingIsCanvasNode_ = node.IsInstanceOf<RSCanvasRenderNode>() && !node.IsInstanceOf<RSCanvasDrawingRenderNode>();
    // regenerate stagingDrawCmdList_
    needSync_ = true;
    stagingDrawCmdListVec_.clear();
    if (node.GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE && modifierType_ == RSModifierType::CONTENT_STYLE) {
        auto& drawingNode = static_cast<const RSCanvasDrawingRenderNode&>(node);
        auto& cmdLists = drawingNode.GetDrawCmdLists();
        auto itr = cmdLists.find(modifierType_);
        if (itr == cmdLists.end() || itr->second.empty()) {
            return false;
        }
        for (auto& cmd : itr->second) {
            stagingDrawCmdListVec_.emplace_back(cmd);
        }
    } else {
        for (const auto& modifier : modifiersVec) {
            auto property =
                std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(modifier->GetProperty());
            if (const auto& drawCmdList = property->GetRef()) {
                if (drawCmdList->GetWidth() > 0 && drawCmdList->GetHeight() > 0) {
                    stagingDrawCmdListVec_.push_back(drawCmdList);
                }
            }
        }
    }
    return !stagingDrawCmdListVec_.empty();
}
#endif

void RSCustomModifierDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    gravity_ = stagingGravity_;
    isCanvasNode_ = stagingIsCanvasNode_;
    std::swap(stagingDrawCmdListVec_, drawCmdListVec_);
    stagingDrawCmdListVec_.clear();
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

Drawing::RecordingCanvas::DrawFunc RSCustomModifierDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSCustomModifierDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
        RSTagTracker::SOURCETYPE::SOURCE_RSCUSTOMMODIFIERDRAWABLE);
#endif
        for (size_t i = 0; i < ptr->drawCmdListVec_.size(); i++) {
#ifdef RS_ENABLE_PREFETCH
            size_t prefetchIndex = i + 2;
            if (prefetchIndex < ptr->drawCmdListVec_.size()) {
                __builtin_prefetch(&(ptr->drawCmdListVec_[prefetchIndex]), 0, 1);
            }
#endif
            const auto& drawCmdList = ptr->drawCmdListVec_[i];
            Drawing::Matrix mat;
            if (ptr->isCanvasNode_ &&
                RSPropertyDrawableUtils::GetGravityMatrix(ptr->gravity_, *rect, drawCmdList->GetWidth(),
                    drawCmdList->GetHeight(), mat)) {
                canvas->ConcatMatrix(mat);
            }
            drawCmdList->Playback(*canvas, rect);
            if (ptr->needClearOp_ && ptr->modifierType_ == RSModifierType::CONTENT_STYLE) {
                RS_PROFILER_DRAWING_NODE_ADD_CLEAROP(drawCmdList);
            }
        }
    };
}

// ============================================================================
// Save and Restore
RSSaveDrawable::RSSaveDrawable(std::shared_ptr<uint32_t> content) : content_(std::move(content)) {}
Drawing::RecordingCanvas::DrawFunc RSSaveDrawable::CreateDrawFunc() const
{
    return [content = content_](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        // Save and return save count
        *content = canvas->Save();
    };
}

RSRestoreDrawable::RSRestoreDrawable(std::shared_ptr<uint32_t> content) : content_(std::move(content)) {}
Drawing::RecordingCanvas::DrawFunc RSRestoreDrawable::CreateDrawFunc() const
{
    return [content = content_](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        // return to previous save count
        canvas->RestoreToCount(*content);
    };
}

RSCustomSaveDrawable::RSCustomSaveDrawable(
    std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content, RSPaintFilterCanvas::SaveType type)
    : content_(std::move(content)), type_(type)
{}
Drawing::RecordingCanvas::DrawFunc RSCustomSaveDrawable::CreateDrawFunc() const
{
    return [content = content_, type = type_](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        // Save and return save count
        *content = paintFilterCanvas->SaveAllStatus(type);
    };
}

RSCustomRestoreDrawable::RSCustomRestoreDrawable(std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content)
    : content_(std::move(content))
{}
Drawing::RecordingCanvas::DrawFunc RSCustomRestoreDrawable::CreateDrawFunc() const
{
    return [content = content_](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        // return to previous save count
        paintFilterCanvas->RestoreStatus(*content);
    };
}

RSDrawable::Ptr RSBeginBlenderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBeginBlenderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSBeginBlenderDrawable::OnUpdate(const RSRenderNode& node)
{
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

Drawing::RecordingCanvas::DrawFunc RSBeginBlenderDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSBeginBlenderDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        if (paintFilterCanvas == nullptr) {
            return;
        }
#ifdef RS_ENABLE_GPU
        RSTagTracker tagTracker(paintFilterCanvas->GetGPUContext(),
            RSTagTracker::SOURCETYPE::SOURCE_RSBEGINBLENDERDRAWABLE);
#endif
        RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO, "RSBeginBlenderDrawable:: %s, bounds: %s",
            ptr->propertyDescription_.c_str(), rect->ToString().c_str());
        RSPropertyDrawableUtils::BeginBlender(*paintFilterCanvas, ptr->blender_, ptr->blendApplyType_,
            ptr->isDangerous_);
    };
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

Drawing::RecordingCanvas::DrawFunc RSEndBlenderDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSEndBlenderDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        RSPropertyDrawableUtils::EndBlender(*paintFilterCanvas, ptr->blendApplyType_);
    };
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
#if defined(MODIFIER_NG)
    auto modifier = node.GetModifierNG(ModifierNG::RSModifierType::ENV_FOREGROUND_COLOR);
    if (modifier == nullptr) {
        return false;
    }
    if (!modifier->HasProperty(ModifierNG::RSPropertyType::ENV_FOREGROUND_COLOR)) {
        return false;
    }
    stagingEnvFGColor_ = modifier->Getter<Color>(ModifierNG::RSPropertyType::ENV_FOREGROUND_COLOR, Color());
#else
    auto& drawCmdModifiers = const_cast<RSRenderNode::DrawCmdContainer&>(node.GetDrawCmdModifiers());
    auto itr = drawCmdModifiers.find(RSModifierType::ENV_FOREGROUND_COLOR);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return false;
    }
    const auto& modifier = itr->second.back();
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(modifier->GetProperty());
    stagingEnvFGColor_ = renderProperty->Get();
#endif
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

Drawing::RecordingCanvas::DrawFunc RSEnvFGColorDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSEnvFGColorDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        // planning: implement alpha offscreen
        paintFilterCanvas->SetEnvForegroundColor(ptr->envFGColor_);
    };
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
#if defined(MODIFIER_NG)
    auto modifier = node.GetModifierNG(ModifierNG::RSModifierType::ENV_FOREGROUND_COLOR);
    if (modifier == nullptr) {
        return false;
    }
    stagingEnvFGColorStrategy_ = static_cast<ForegroundColorStrategyType>(
        modifier->Getter<int>(ModifierNG::RSPropertyType::ENV_FOREGROUND_COLOR_STRATEGY, 0));
#else
    auto& drawCmdModifiers = const_cast<RSRenderNode::DrawCmdContainer&>(node.GetDrawCmdModifiers());
    auto itr = drawCmdModifiers.find(RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return false;
    }
    const auto& modifier = itr->second.back();
    auto property = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType>>(modifier->GetProperty());
    stagingEnvFGColorStrategy_ = property->Get();
#endif
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

Drawing::RecordingCanvas::DrawFunc RSEnvFGColorStrategyDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSEnvFGColorStrategyDrawable>(shared_from_this());
    return [this, ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        switch (envFGColorStrategy_) {
            case ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR: {
                // calculate the color by screebshot
                Color color = RSPropertyDrawableUtils::GetInvertBackgroundColor(*paintFilterCanvas, needClipToBounds_,
                    boundsRect_, backgroundColor_);
                paintFilterCanvas->SetEnvForegroundColor(color);
                break;
            }
            default: {
                break;
            }
        }
    };
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
#if defined(MODIFIER_NG)
    const auto modifier = node.GetModifierNG(ModifierNG::RSModifierType::CLIP_TO_FRAME);
    if (modifier == nullptr || !modifier->HasProperty(ModifierNG::RSPropertyType::CUSTOM_CLIP_TO_FRAME)) {
        return false;
    }
    const auto& clipRectV4f = modifier->Getter<Vector4f>(ModifierNG::RSPropertyType::CUSTOM_CLIP_TO_FRAME, Vector4f());
#else
    auto& drawCmdModifiers = const_cast<RSRenderNode::DrawCmdContainer&>(node.GetDrawCmdModifiers());
    auto itr = drawCmdModifiers.find(RSModifierType::CUSTOM_CLIP_TO_FRAME);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return false;
    }
    const auto& modifier = itr->second.back();
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(modifier->GetProperty());
    const auto& clipRectV4f = renderProperty->Get();
#endif
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

Drawing::RecordingCanvas::DrawFunc RSCustomClipToFrameDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSCustomClipToFrameDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        canvas->ClipRect(ptr->customClipRect_);
    };
}

} // namespace DrawableV2
} // namespace OHOS::Rosen
