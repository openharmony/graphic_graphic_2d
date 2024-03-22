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

#include "params/rs_surface_render_params.h"

namespace OHOS::Rosen {
RSSurfaceRenderParams::RSSurfaceRenderParams() {}

void RSSurfaceRenderParams::SetOcclusionVisible(bool visible)
{
    if (occlusionVisible_ == visible) {
        return;
    }
    occlusionVisible_ = visible;
    needSync_ = true;
}

bool RSSurfaceRenderParams::GetOcclusionVisible() const
{
    return occlusionVisible_;
}

Occlusion::Region RSSurfaceRenderParams::GetVisibleRegion() const
{
    return visibleRegion_;
}

void RSSurfaceRenderParams::SetVisibleRegion(const Occlusion::Region& visibleRegion)
{
    visibleRegion_ = visibleRegion;
}

void RSSurfaceRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    auto targetSurfaceParams = static_cast<RSSurfaceRenderParams*>(target.get());
    if (targetSurfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderParams::OnSync targetSurfaceParams is nullptr");
        return;
    }

    targetSurfaceParams->isMainWindowType_ = isMainWindowType_;
    targetSurfaceParams->rsSurfaceNodeType_ = rsSurfaceNodeType_;
    targetSurfaceParams->selfDrawingType_ = selfDrawingType_;
    targetSurfaceParams->ancestorDisplayNode_ = ancestorDisplayNode_;
    targetSurfaceParams->alpha_ = alpha_;
    targetSurfaceParams->isSpherizeValid_ = isSpherizeValid_;
    targetSurfaceParams->needBilinearInterpolation_ = needBilinearInterpolation_;
    targetSurfaceParams->frameGravity_ = frameGravity_;
    targetSurfaceParams->backgroundColor_ = backgroundColor_;
    targetSurfaceParams->dstRect_ = dstRect_;
    targetSurfaceParams->rrect_ = rrect_;
    targetSurfaceParams->occlusionVisible_ = occlusionVisible_;
    targetSurfaceParams->visibleRegion_ = visibleRegion_;
    RSRenderParams::OnSync(target);
}

std::string RSSurfaceRenderParams::ToString() const
{
    std::string ret = RSRenderParams::ToString() + "\nRSSurfaceRenderParams: {\n";
    ret += RENDER_BASIC_PARAM_TO_STRING(int(rsSurfaceNodeType_));
    ret += RENDER_BASIC_PARAM_TO_STRING(int(selfDrawingType_));
    ret += RENDER_BASIC_PARAM_TO_STRING(alpha_);
    ret += RENDER_BASIC_PARAM_TO_STRING(isSpherizeValid_);
    ret += RENDER_BASIC_PARAM_TO_STRING(needBilinearInterpolation_);
    ret += RENDER_BASIC_PARAM_TO_STRING(int(frameGravity_));
    ret += RENDER_BASIC_PARAM_TO_STRING(backgroundColor_.GetAlpha());
    ret += RENDER_RECT_PARAM_TO_STRING(dstRect_);
    ret += RENDER_BASIC_PARAM_TO_STRING(occlusionVisible_);
    ret += "}";
    return ret;
}

} // namespace OHOS::Rosen
