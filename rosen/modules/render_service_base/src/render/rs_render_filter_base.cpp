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

#include "ge_shader_filter_params.h"
#include "render/rs_render_filter_base.h"

namespace OHOS {
namespace Rosen {
RSUIFilterType RSRenderFilterParaBase::GetType() const
{
    return type_;
}

bool RSRenderFilterParaBase::IsValid() const
{
    return type_ != RSUIFilterType::NONE;
}

void RSRenderFilterParaBase::SetGeometry(Drawing::Canvas& canvas, float geoWidth, float geoHeight)
{
    auto dst = canvas.GetDeviceClipBounds();
    geoWidth_ = std::ceil(geoWidth);
    geoHeight_ = std::ceil(geoHeight);
    tranX_ = dst.GetLeft();
    tranY_ = dst.GetTop();
    mat_ = canvas.GetTotalMatrix();
}
} // namespace Rosen
} // namespace OHOS
