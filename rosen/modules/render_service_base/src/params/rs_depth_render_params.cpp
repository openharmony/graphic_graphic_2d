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

#include "params/rs_depth_render_params.h"

#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSDepthRenderParams::RSDepthRenderParams(NodeId id) : RSRenderParams(id) {}

void RSDepthRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    OnSyncDepthSpaceParams(target);
    RSRenderParams::OnSync(target);
}

void RSDepthRenderParams::OnSyncDepthSpaceParams(const std::unique_ptr<RSRenderParams>& target)
{
    auto targetDepthParams = static_cast<RSDepthRenderParams*>(target.get());
    if (targetDepthParams == nullptr) {
        RS_LOGE("RSDepthRenderParams::OnSyncDepthSpaceParams targetDepthParams is null");
        return;
    }
    targetDepthParams->depthImage_ = depthImage_;
    targetDepthParams->depthSrcSurfaceDrawable_ = depthSrcSurfaceDrawable_;
    targetDepthParams->depthCameraPara_ = depthCameraPara_;
    targetDepthParams->depthLightPara_ = depthLightPara_;
    targetDepthParams->imageMatrix_ = imageMatrix_;
    targetDepthParams->backgroundMatrix_ = backgroundMatrix_;
}

void RSDepthRenderParams::SetDepthImage(const std::shared_ptr<Drawing::Image>& depthImage)
{
    depthImage_ = depthImage;
    needSync_ = true;
}

std::shared_ptr<Drawing::Image> RSDepthRenderParams::GetDepthImage() const
{
    return depthImage_;
}

void RSDepthRenderParams::ResetDepthSrcSurfaceDrawable()
{
    depthSrcSurfaceDrawable_.reset();
    needSync_ = true;
}

void RSDepthRenderParams::SetDepthSrcSurfaceDrawable(
    const std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter> depthSrcSurfaceDrawable)
{
    depthSrcSurfaceDrawable_ = depthSrcSurfaceDrawable;
    needSync_ = true;
}

std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter> RSDepthRenderParams::GetDepthSrcSurfaceDrawable() const
{
    return depthSrcSurfaceDrawable_;
}

void RSDepthRenderParams::SetDepthCameraPara(const std::optional<DepthCameraPara>& depthCameraPara)
{
    depthCameraPara_ = depthCameraPara;
    needSync_ = true;
}

std::optional<DepthCameraPara> RSDepthRenderParams::GetDepthCameraPara() const
{
    return depthCameraPara_;
}

void RSDepthRenderParams::SetDepthLightPara(const std::optional<DepthLightPara>& depthLightPara)
{
    depthLightPara_ = depthLightPara;
    needSync_ = true;
}

std::optional<DepthLightPara> RSDepthRenderParams::GetDepthLightPara() const
{
    return depthLightPara_;
}

void RSDepthRenderParams::SetImageMatrix(const Matrix3f& imageMatrix)
{
    Drawing::Matrix::Buffer buffer;
    const auto* data = imageMatrix.GetConstData();
    std::copy(data, data + Drawing::Matrix::MATRIX_SIZE, buffer.begin());
    imageMatrix_.SetAll(buffer);

    needSync_ = true;
}

Drawing::Matrix RSDepthRenderParams::GetImageMatrix() const
{
    return imageMatrix_;
}

void RSDepthRenderParams::SetBackgroundMatrix(const Drawing::Matrix& mat)
{
    backgroundMatrix_ = mat;
    needSync_ = true;
}

const Drawing::Matrix& RSDepthRenderParams::GetBackgroundMatrix() const
{
    return backgroundMatrix_;
}
} // namespace OHOS::Rosen