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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_DEPTH_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_DEPTH_RENDER_PARAMS_H

#include "drawable/rs_render_node_drawable_adapter.h"
#include "params/rs_render_params.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSDepthRenderParams : public RSRenderParams {
public:
    explicit RSDepthRenderParams(NodeId id);
    ~RSDepthRenderParams() override = default;
    void OnSync(const std::unique_ptr<RSRenderParams>& target) override;

    void SetDepthImage(const std::shared_ptr<Drawing::Image>& depthImage);
    std::shared_ptr<Drawing::Image> GetDepthImage() const;

    void ResetDepthSrcSurfaceDrawable();
    void SetDepthSrcSurfaceDrawable(
        const std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter> depthSrcSurfaceDrawable);
    std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter> GetDepthSrcSurfaceDrawable() const;

    void SetDepthCameraPara(const std::optional<DepthCameraPara>& depthCameraPara);
    std::optional<DepthCameraPara> GetDepthCameraPara() const;

    void SetDepthLightPara(const std::optional<DepthLightPara>& depthLightPara);
    std::optional<DepthLightPara> GetDepthLightPara() const;

    void SetImageMatrix(const Matrix3f& imageMatrix);
    Drawing::Matrix GetImageMatrix() const;

    void SetBackgroundMatrix(const Drawing::Matrix& mat);
    const Drawing::Matrix& GetBackgroundMatrix() const;

private:
    std::shared_ptr<Drawing::Image> depthImage_ = nullptr;
    std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter> depthSrcSurfaceDrawable_;
    std::optional<DepthCameraPara> depthCameraPara_;
    std::optional<DepthLightPara> depthLightPara_;
    Drawing::Matrix imageMatrix_;
    Drawing::Matrix backgroundMatrix_;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_BASE_PARAMS_RS_DEPTH_RENDER_PARAMS_H