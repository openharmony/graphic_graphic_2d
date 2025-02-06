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

#include "render/rs_flow_light_sweep_shader.h"
#include "platform/common/rs_log.h"
#include "ge_visual_effect_impl.h"

namespace OHOS {
namespace Rosen {

RSFlowLightSweepShader::RSFlowLightSweepShader(const std::vector<std::pair<Drawing::Color, float>>& effectColors)
{
    params_->effectColors_ = effectColors;
    type_ = ShaderType::FLOW_LIGHT_SWEEP;
}

void RSFlowLightSweepShader::MakeDrawingShader(const RectF& rect, float progress)
{
    Drawing::Rect drawingRect = Drawing::Rect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    if (geShader_) {
        geShader_->MakeDrawingShader(drawingRect, progress);
    }
    type_ = ShaderType::FLOW_LIGHT_SWEEP;
}

const std::shared_ptr<Drawing::ShaderEffect>& RSFlowLightSweepShader::GetDrawingShader() const
{
    if (geShader_) {
        return geShader_->GetDrawingShader();
    }
    return RSShader::GetDrawingShader();
}

bool RSFlowLightSweepShader::Marshalling(Parcel& parcel)
{
    if (!params_) {
        return false;
    }

    return params_->Marshalling(parcel);
}

bool RSFlowLightSweepShader::Unmarshalling(Parcel& parcel, bool& needReset)
{
    needReset = false;
    params_ = std::make_shared<GEXFlowLightSweepParams>();
    if (!params_) {
        return false;
    }
    if (!params_->Unmarshalling(parcel)) {
        ROSEN_LOGD("RSFlowLightSweepShader::Unmarshalling failed");
        return false;
    }

    geShader_ = GEXFlowLightSweepShader::CreateDynamicImpl(params_->effectColors_);
    if (!geShader_) {
        ROSEN_LOGD("RSFlowLightSweepShader::Unmarshalling CreateDynamicImpl failed");
        return false;
    }

    return true;
}

} // namespace Rosen
} // namespace OHOS
