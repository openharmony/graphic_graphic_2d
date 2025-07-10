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
#include "render/rs_bezier_warp_filter.h"

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {
RSBezierWarpFilter::RSBezierWarpFilter(const std::array<Drawing::Point, BEZIER_WARP_POINT_NUM>& destinationPatch)
    :destinationPatch_(destinationPatch)
{
    type_ = ShaderFilterType::BEZIER_WARP;
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    hash_ = hashFunc(&destinationPatch_, sizeof(destinationPatch_), hash_);
}

RSBezierWarpFilter::~RSBezierWarpFilter() = default;

const std::array<Drawing::Point, BEZIER_WARP_POINT_NUM>& RSBezierWarpFilter::GetBezierWarpPoints() const
{
    return destinationPatch_;
}

void RSBezierWarpFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto bezierWarpFilter = std::make_shared<Drawing::GEVisualEffect>("BEZIER_WARP",
        Drawing::DrawingPaintType::BRUSH);

    bezierWarpFilter->SetParam("BEZIER_WARP_DESTINATION_PATCH", destinationPatch_);
    visualEffectContainer->AddToChainedFilter(bezierWarpFilter);
}
} // namespace Rosen
} // namespace OHOS
