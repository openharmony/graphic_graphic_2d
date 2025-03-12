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

#ifndef RENDER_SERVICE_WINDOW_KEYFRAME_BUFFER_H
#define RENDER_SERVICE_WINDOW_KEYFRAME_BUFFER_H

#include <memory>
#include "draw/canvas.h"
#include "drawable/rs_canvas_render_node_drawable.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS::Rosen {

class RSWindowKeyframeBuffer {
public:
    explicit RSWindowKeyframeBuffer(DrawableV2::RSCanvasRenderNodeDrawable& drawable);
    ~RSWindowKeyframeBuffer() = default;

    bool OnDraw(Drawing::Canvas& canvas);
    bool DrawOffscreenBuffer(RSPaintFilterCanvas& canvas, const Drawing::Rect& bounds, float alpha, bool isFreezed);

private:
    RSWindowKeyframeBuffer(const RSWindowKeyframeBuffer&) = delete;
    RSWindowKeyframeBuffer(const RSWindowKeyframeBuffer&&) = delete;
    RSWindowKeyframeBuffer& operator=(const RSWindowKeyframeBuffer&) = delete;
    RSWindowKeyframeBuffer& operator=(const RSWindowKeyframeBuffer&&) = delete;

    bool DrawOffscreenImgToBuffer(Drawing::Canvas& canvas, const RSRenderParams& params,
        const Drawing::Rect& bounds, std::shared_ptr<Drawing::Surface>& surface);

    DrawableV2::RSCanvasRenderNodeDrawable& canvasNodeDrawable_;
    std::shared_ptr<Drawing::Image> cachedOffscreenImg_ = nullptr;
    std::shared_ptr<Drawing::Image> preCachedOffscreenImg_ = nullptr;
};

} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_WINDOW_KEYFRAME_BUFFER_H
