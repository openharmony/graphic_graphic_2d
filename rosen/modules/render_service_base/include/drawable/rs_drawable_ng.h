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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_DRAWABLE_NG_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_DRAWABLE_NG_H

#include <bitset>
#include <memory>
#include <set>
#include <vector>

#include "recording/draw_cmd_list.h"

// #include "modifier/rs_render_modifier.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS::Rosen {
class RSProperties;
class RSRenderContent;
class RSRenderNode;

// Pure virtual base class
class RSDrawableNG {
public:
    RSDrawableNG() = default;
    virtual ~RSDrawableNG() = default;

    // not copyable and moveable
    RSDrawableNG(const RSDrawableNG&) = delete;
    RSDrawableNG(const RSDrawableNG&&) = delete;
    RSDrawableNG& operator=(const RSDrawableNG&) = delete;
    RSDrawableNG& operator=(const RSDrawableNG&&) = delete;

    // 
    virtual void Draw(RSPaintFilterCanvas& canvas) const = 0;
};



// class RSCustomModifierDrawableNG : RSDrawableNG {
// public:
//     RSCustomModifierDrawableNG(const std::shared_ptr<RSRenderModifier>& modifier) 
//     {
//         // auto property = modifier->GetProperty();
//         // if (auto drawCmdProperty = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(property)) {
//         //     drawCmdList_ = drawCmdProperty->GetRef();
//         // }
//     }
//     ~RSCustomModifierDrawableNG() override = default;

//     //Generate
//     //Update

//     void OnSync()
//     {
//         if (stagingDrawCmdLists_.empty()) {
//             return;
//         }
//         std::swap(drawCmdLists_, stagingDrawCmdLists_);
//         stagingDrawCmdLists_.clear();
//     }

//     void Draw(RSPaintFilterCanvas& canvas) const override {}

// protected:
//     std::vector<std::shared_ptr<Drawing::DrawCmdList>> drawCmdLists_;
//     std::vector<std::shared_ptr<Drawing::DrawCmdList>> stagingDrawCmdLists_;
// };

class RSRenderNodeDrawable : RSDrawableNG {
public:
    explicit RSRenderNodeDrawable(std::shared_ptr<RSRenderNode> renderNode) : renderNode_(std::move(renderNode)) {}
    ~RSRenderNodeDrawable() override = default;

    static std::shared_ptr<RSRenderNodeDrawable> Generate(std::shared_ptr<RSRenderNode> node)
    {
        return std::make_shared<RSRenderNodeDrawable>(std::move(node));
    }

    void Draw(RSPaintFilterCanvas& canvas) const override
    {
        // TODO
    }

protected:
    std::shared_ptr<RSRenderNode> renderNode_;
};

} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PROPERTY_RS_DRAWABLE_NG_H
