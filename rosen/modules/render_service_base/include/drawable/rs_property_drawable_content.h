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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_CONTENT_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_CONTENT_H

#include <memory>

#include "recording/draw_cmd_list.h"

#include "drawable/rs_drawable_content.h"
#include "drawable/rs_property_drawable_ng.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen {
class RSRenderNode;

class RSPropertyDrawableContent : public RSDrawableContent {
public:
    RSPropertyDrawableContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList) : drawCmdList_(std::move(drawCmdList)) {}
    RSPropertyDrawableContent() = default;
    ~RSPropertyDrawableContent() override = default;

    void OnSync() override;
    RSDrawable::Ptr CreateDrawable() const override;

protected:
    bool needSync_ = false;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList_;
    std::shared_ptr<Drawing::DrawCmdList> stagingDrawCmdList_;

    friend class RSPropertyDrawCmdListUpdater;
    friend class RSPropertyDrawableNG;
};

// DEMO
class RSBackgroundContent : public RSPropertyDrawableContent {
public:
    RSBackgroundContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSBackgroundContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
};

// DEMO
class RSBorderContent : public RSPropertyDrawableContent {
public:
    RSBorderContent(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawableContent(std::move(drawCmdList))
    {}
    RSBorderContent() : RSPropertyDrawableContent()
    {}
    static RSDrawableContent::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    static void DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas,
        const std::shared_ptr<RSBorder>& border, const bool& isOutline);
};

} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_CONTENT_H
