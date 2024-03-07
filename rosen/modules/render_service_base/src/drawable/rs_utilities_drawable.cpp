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

#include "drawable/rs_utilities_drawable.h"

#include "drawable/rs_render_node_drawable_adapter.h"
#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen {

// ==================== RSChildrenDrawableContent =====================
RSDrawable::Ptr RSChildrenDrawableContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSChildrenDrawableContent>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSChildrenDrawableContent::OnUpdate(const RSRenderNode& node)
{
    auto children = node.GetSortedChildren();
    if (children == nullptr || children->empty()) {
        return false;
    }

    // Regenerate children drawables
    needSync_ = true;
    stagingChildrenDrawables_.clear();
    for (const auto& child : *children) {
        stagingChildrenDrawables_.push_back(RSRenderNodeDrawableAdapter::OnGenerate(child));
    }
    return true;
}

void RSChildrenDrawableContent::OnSync()
{
    if (!needSync_) {
        return;
    }
    childrenDrawables_ = std::move(stagingChildrenDrawables_);
    needSync_ = false;
}

// RSDrawable::Ptr RSChildrenDrawableContent::CreateDrawable() const
Drawing::RecordingCanvas::DrawFunc RSChildrenDrawableContent::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSChildrenDrawableContent>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        for (const auto& drawable : ptr->childrenDrawables_) {
            drawable->OnDraw(canvas);
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
    stagingDrawCmdList_.clear();
    for (const auto& modifier : itr->second) {
        auto property = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(modifier->GetProperty());
        if (property == nullptr) {
            continue;
        }
        if (const auto& drawCmdList = property->GetRef()) {
            stagingDrawCmdList_.push_back(drawCmdList);
        }
    }
    return true;
}

void RSCustomModifierDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    drawCmdList_ = std::move(stagingDrawCmdList_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSCustomModifierDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSCustomModifierDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        for (const auto& drawCmdList : ptr->drawCmdList_) {
            drawCmdList->Playback(*canvas);
        }
    };
}

// ============================================================================
// Alpha
RSDrawable::Ptr RSAlphaDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSAlphaDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}
bool RSAlphaDrawable::OnUpdate(const RSRenderNode& node)
{
    auto alpha = node.GetRenderProperties().GetAlpha();
    if (alpha == 1) {
        return false;
    }
    stagingAlpha_ = alpha;
    stagingOffscreen_ = node.GetRenderProperties().GetAlphaOffscreen();
    needSync_ = true;
    return true;
}
void RSAlphaDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    alpha_ = stagingAlpha_;
    offscreen_ = stagingOffscreen_;
    needSync_ = false;
}
Drawing::RecordingCanvas::DrawFunc RSAlphaDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSAlphaDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        // TODO: implement alpha offscreen
        paintFilterCanvas->MultiplyAlpha(ptr->alpha_);
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
} // namespace OHOS::Rosen
