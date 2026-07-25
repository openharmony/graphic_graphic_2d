/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "drawable/rs_depth_render_node_drawable.h"

#include "drawable/rs_surface_render_node_drawable.h"
#include "params/rs_depth_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
RSDepthRenderNodeDrawable::Registrar RSDepthRenderNodeDrawable::instance_;

RSDepthRenderNodeDrawable::RSDepthRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node)) {}

RSRenderNodeDrawable::Ptr RSDepthRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSDepthRenderNodeDrawable(std::move(node));
}

void RSDepthRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    const auto& params = static_cast<RSDepthRenderParams*>(GetRenderParams().get());
    if (params == nullptr) {
        RS_LOGE("RSDepthRenderNodeDrawable::OnDraw params is nullptr");
        return;
    }

    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);
    // The background matrix and DC's matrix are consistent
    params->SetBackgroundMatrix(paintFilterCanvas->GetTotalMatrix());

    auto drawableAdaptor = params->GetDepthSrcSurfaceDrawable().lock();
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(drawableAdaptor);
    if (surfaceDrawable) {
        uint32_t threadId = paintFilterCanvas->GetParallelThreadId();
        auto bufferDrawParams = RSUniRenderUtil::CreateBufferDrawParam(*surfaceDrawable, false, threadId);

        auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
        if (!renderEngine) {
            RS_LOGE("RSDepthRenderNodeDrawable::OnDraw renderEngine is nullptr");
            return;
        }
        VideoInfo videoInfo;
        auto surfaceNodeImage = renderEngine->CreateImageFromBuffer(*paintFilterCanvas, bufferDrawParams, videoInfo);
        if (surfaceNodeImage) {
            params->SetDepthImage(surfaceNodeImage);
        } else {
            RS_LOGE("RSDepthRenderNodeDrawable::OnDraw depth surface image is nullptr");
        }
    }

    RSRenderNodeDrawable::OnDraw(canvas);
}

void RSDepthRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    OnDraw(canvas);
}
} // namespace DrawableV2
} // namespace OHOS::Rosen