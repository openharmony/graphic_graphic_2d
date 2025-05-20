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
#ifndef RENDER_SERVICE_BASE_RENDER_RS_BEZIER_WARP_FILTER_H
#define RENDER_SERVICE_BASE_RENDER_RS_BEZIER_WARP_FILTER_H

#include "render/rs_shader_filter.h"

namespace OHOS {
namespace Rosen {
constexpr size_t BEZIER_WARP_POINT_NUM = 12; // 12 anchor points of a patch

class RSB_EXPORT RSBezierWarpFilter : public RSShaderFilter {
public:
    RSBezierWarpFilter(const std::array<Drawing::Point, BEZIER_WARP_POINT_NUM>& destinationPatch);
    RSBezierWarpFilter(const RSBezierWarpFilter&) = delete;
    RSBezierWarpFilter operator=(const RSBezierWarpFilter&) = delete;
    ~RSBezierWarpFilter() override;

    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;
    const std::array<Drawing::Point, BEZIER_WARP_POINT_NUM>& GetBezierWarpPoints() const;

private:
    std::array<Drawing::Point, BEZIER_WARP_POINT_NUM> destinationPatch_;
};
} // Rosen
} // OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RS_BEZIER_WARP_FILTER_H
