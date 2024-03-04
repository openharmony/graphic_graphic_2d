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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_UTILITIES_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_UTILITIES_DRAWABLE_H

#include <bitset>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_set>

#include "drawable/rs_drawable.h"
#include "modifier/rs_modifier_type.h"

namespace OHOS::Rosen {
// RSChildrenDrawable, for drawing children of RSRenderNode, updates on child add/remove
class RSRenderNodeDrawableAdapter;
class RSChildrenDrawableContent : public RSDrawable {
public:
    RSChildrenDrawableContent() = default;
    ~RSChildrenDrawableContent() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& content) override;
    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

private:
    bool needSync_ = false;
    std::vector<std::unique_ptr<RSRenderNodeDrawableAdapter>> childrenDrawables_;
    std::vector<std::unique_ptr<RSRenderNodeDrawableAdapter>> stagingChildrenDrawables_;
    friend class RSChildrenDrawable;
};

// RSChildrenDrawable, for drawing custom modifiers
enum class RSModifierType : int16_t;
namespace Drawing {
class DrawCmdList;
}
class RSCustomModifierDrawableContent : public RSDrawable {
public:
    RSCustomModifierDrawableContent(RSModifierType type) : type_(type) {}
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& content, RSModifierType type);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

private:
    RSModifierType type_;

    bool needSync_ = false;
    std::vector<const std::shared_ptr<Drawing::DrawCmdList>> drawCmdList_;
    std::vector<const std::shared_ptr<Drawing::DrawCmdList>> stagingDrawCmdList_;
    friend class RSCustomModifierDrawable;
};

// Save/Restore

// Clip

// Alpha & restore

} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_UTILITIES_DRAWABLE_H
