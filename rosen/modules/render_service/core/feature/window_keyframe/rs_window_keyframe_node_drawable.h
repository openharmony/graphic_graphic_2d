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

#ifndef RENDER_SERVICE_DRAWABLE_RS_WINDOW_KEYFRAME_NODE_DRAWABLE_H
#define RENDER_SERVICE_DRAWABLE_RS_WINDOW_KEYFRAME_NODE_DRAWABLE_H

#include "drawable/rs_canvas_render_node_drawable.h"

namespace OHOS::Rosen::DrawableV2 {

class RSWindowKeyFrameNodeDrawable : public RSRenderNodeDrawable {
public:
    ~RSWindowKeyFrameNodeDrawable() override = default;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    static bool CheckAndDrawToOffscreen(Drawing::Canvas& canvas, RSCanvasRenderNodeDrawable& linkedNodeDrawable);

    RSRenderNodeDrawableType GetDrawableType() const override
    {
        return RSRenderNodeDrawableType::WINDOW_KEYFRAME_NODE_DRAWABLE;
    }

    void OnDraw(Drawing::Canvas& canvas) override;
    
private:
    explicit RSWindowKeyFrameNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);

    bool OnLinkedNodeDraw(Drawing::Canvas& canvas, RSCanvasRenderNodeDrawable& linkedNodeDrawable,
        const RSRenderParams& linkedNodeParams);
    bool DrawOffscreenBuffer(Drawing::Canvas& canvas, Drawing::Surface& surface, const RSRenderParams& params);
    void DrawLinkedNodeOffscreenBuffer(RSPaintFilterCanvas& canvas, const Drawing::Rect& bounds);

    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::WINDOW_KEYFRAME_NODE, OnGenerate>;
    static Registrar instance_;

    std::shared_ptr<Drawing::Image> cachedOffscreenImg_ = nullptr;
    std::shared_ptr<Drawing::Image> preCachedOffscreenImg_ = nullptr;
    Drawing::Rect linkedNodeBounds_;
};

} // namespace OHOS::Rosen::DrawableV2
#endif // RENDER_SERVICE_DRAWABLE_RS_WINDOW_KEYFRAME_NODE_DRAWABLE_H
