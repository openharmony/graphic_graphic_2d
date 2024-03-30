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

#include "drawable/rs_property_drawable_utils.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
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

    auto shadowBatching = node.GetRenderProperties().GetUseShadowBatching();
    if (shadowBatching) {
        for (const auto& child : *children) {
            if (UNLIKELY(child->GetSharedTransitionParam()) && OnSharedTransition(child)) {
                continue;
            }
            // Generate shadow only drawable
            if (auto childDrawable = RSRenderNodeDrawableAdapter::OnGenerateShadowDrawable(child)) {
                stagingChildrenDrawableVec_.push_back(std::move(childDrawable));
            }
        }
    }
    for (const auto& child : *children) {
        if (UNLIKELY(child->GetSharedTransitionParam()) && OnSharedTransition(child)) {
            continue;
        }
        if (auto childDrawable = RSRenderNodeDrawableAdapter::OnGenerate(child)) {
            childDrawable->SetSkipShadow(!shadowBatching);
            stagingChildrenDrawableVec_.push_back(std::move(childDrawable));
        }
    }

    return !stagingChildrenDrawableVec_.empty();
}

bool RSChildrenDrawable::OnSharedTransition(const RSRenderNode::SharedPtr& node)
{
    auto nodeId = node->GetId();
    const auto& sharedTransitionParam = node->GetSharedTransitionParam();
    // Test if this node is lower in the hierarchy
    bool isLower = sharedTransitionParam->UpdateHierarchyAndReturnIsLower(nodeId);

    auto pairedNode = sharedTransitionParam->GetPairedNode(nodeId);
    if (!pairedNode) {
        // clear invalid shared transition param
        node->SetSharedTransitionParam(nullptr);
        return false;
    }
    if (!pairedNode->IsOnTheTree()) {
        // clear invalid shared transition param
        node->SetSharedTransitionParam(nullptr);
        pairedNode->SetSharedTransitionParam(nullptr);
        return false;
    }
    if (isLower) {
        // for lower hierarchy node, we skip it here, and add to unpaired share transitions
        SharedTransitionParam::unpairedShareTransitions_.emplace(sharedTransitionParam->inNodeId_, sharedTransitionParam);
    } else {
        // for higher hierarchy node, we add paired node (lower in hierarchy) first, then add it
        if (auto childDrawable = RSRenderNodeDrawableAdapter::OnGenerate(pairedNode)) {
            stagingChildrenDrawableVec_.push_back(std::move(childDrawable));
        }
        // remove successful paired node
        SharedTransitionParam::unpairedShareTransitions_.erase(sharedTransitionParam->inNodeId_);
    }
    childrenHasSharedTransition_ = true;
    return isLower;
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
        for (const auto& drawable : ptr->childrenDrawableVec_) {
            drawable->Draw(*canvas);
        }
    };
}

// ==================== RSCustomModifierDrawable ===================
RSDrawable::Ptr RSCustomModifierDrawable::OnGenerate(const RSRenderNode& node, RSModifierType type)
{
    if (auto ret = std::make_shared<RSCustomModifierDrawable>(type); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSCustomModifierDrawable::OnUpdate(const RSRenderNode& node)
{
    const auto& drawCmdModifiers = node.GetDrawCmdModifiers();
    auto itr = drawCmdModifiers.find(type_);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return false;
    }

    // regenerate stagingDrawCmdList_
    needSync_ = true;
    stagingDrawCmdListVec_.clear();
    for (const auto& modifier : itr->second) {
        auto property = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(modifier->GetProperty());
        if (const auto& drawCmdList = property->GetRef()) {
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
    std::swap(stagingDrawCmdListVec_, drawCmdListVec_);
    stagingDrawCmdListVec_.clear();
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSCustomModifierDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSCustomModifierDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        for (const auto& drawCmdList : ptr->drawCmdListVec_) {
            drawCmdList->Playback(*canvas, rect);
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

RSDrawable::Ptr RSBeginBlendModeDrawable::OnGenerate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto blendMode = properties.GetColorBlendMode();
    if (blendMode == static_cast<int>(RSColorBlendMode::NONE)) {
        // no blend
        return nullptr;
    }

    return std::make_shared<RSBeginBlendModeDrawable>(blendMode, properties.GetColorBlendApplyType());
}

bool RSBeginBlendModeDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto blendMode = properties.GetColorBlendMode();
    if (blendMode == static_cast<int>(RSColorBlendMode::NONE)) {
        // no blend
        return false;
    }

    stagingBlendMode_ = blendMode;
    stagingBlendApplyType_ = properties.GetColorBlendApplyType();
    needSync_ = true;

    return true;
}

void RSBeginBlendModeDrawable::OnSync()
{
    if (needSync_ == false) {
        return;
    }
    blendMode_ = stagingBlendMode_;
    blendApplyType_ = stagingBlendApplyType_;
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSBeginBlendModeDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSBeginBlendModeDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        RSPropertyDrawableUtils::BeginBlendMode(*paintFilterCanvas, ptr->blendMode_, ptr->blendApplyType_);
    };
}

RSDrawable::Ptr RSEndBlendModeDrawable::OnGenerate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (properties.GetColorBlendMode() == static_cast<int>(RSColorBlendMode::NONE) ||
        properties.GetColorBlendApplyType() == static_cast<int>(RSColorBlendApplyType::FAST)) {
        // no blend
        return nullptr;
    }

    return std::make_shared<RSEndBlendModeDrawable>();
};

bool RSEndBlendModeDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (properties.GetColorBlendMode() == static_cast<int>(RSColorBlendMode::NONE) ||
        properties.GetColorBlendApplyType() == static_cast<int>(RSColorBlendApplyType::FAST)) {
        // no blend
        return false;
    }

    return true;
}

Drawing::RecordingCanvas::DrawFunc RSEndBlendModeDrawable::CreateDrawFunc() const
{
    return [](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        RSPropertyDrawableUtils::EndBlendMode(*paintFilterCanvas);
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
    auto& drawCmdModifiers = const_cast<RSRenderContent::DrawCmdContainer&>(node.GetDrawCmdModifiers());
    auto itr = drawCmdModifiers.find(RSModifierType::ENV_FOREGROUND_COLOR);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return false;
    }
    const auto& modifier = itr->second.back();
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(modifier->GetProperty());
    stagingEnvFGColor_ = renderProperty->Get();
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
        // TODO: implement alpha offscreen
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
    auto& drawCmdModifiers = const_cast<RSRenderContent::DrawCmdContainer&>(node.GetDrawCmdModifiers());
    auto itr = drawCmdModifiers.find(RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return false;
    }
    const auto& modifier = itr->second.back();
    auto property = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType>>(modifier->GetProperty());
    stagingEnvFGColorStrategy_ = property->Get();
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
} // namespace DrawableV2
} // namespace OHOS::Rosen
