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

#include "params/rs_display_render_params.h"

#include "platform/common/rs_log.h"
namespace OHOS::Rosen {
RSDisplayRenderParams::RSDisplayRenderParams() {}

std::vector<RSBaseRenderNode::SharedPtr>& RSDisplayRenderParams::GetAllMainAndLeashSurfaces()
{
    return allMainAndLeashSurfaces_;
}

void RSDisplayRenderParams::SetAllMainAndLeashSurfaces(
    std::vector<RSBaseRenderNode::SharedPtr>& allMainAndLeashSurfaces)
{
    std::swap(allMainAndLeashSurfaces_, allMainAndLeashSurfaces);
}

void RSDisplayRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    auto targetDisplayParams = static_cast<RSDisplayRenderParams*>(target.get());
    if (targetDisplayParams == nullptr) {
        RS_LOGE("RSDisplayRenderParams::OnSync targetDisplayParams is nullptr");
        return;
    }
    targetDisplayParams->SetAllMainAndLeashSurfaces(allMainAndLeashSurfaces_);
    targetDisplayParams->offsetX_ = offsetX_;
    targetDisplayParams->offsetY_ = offsetY_;
    targetDisplayParams->nodeRotation_ = nodeRotation_;
    targetDisplayParams->screenRotation_ = screenRotation_;
    targetDisplayParams->screenId_ = screenId_;
    targetDisplayParams->mirroredId_ = mirroredId_;
    targetDisplayParams->compositeType_ = compositeType_;
    targetDisplayParams->mirrorSource_ = mirrorSource_;
    targetDisplayParams->screenInfo_ = std::move(screenInfo_);
    RSRenderParams::OnSync(target);
}

std::string RSDisplayRenderParams::ToString() const
{
    std::string ret = RSRenderParams::ToString() + "\nRSDisplayRenderParams: {\n";
    ret += RENDER_BASIC_PARAM_TO_STRING(offsetX_);
    ret += RENDER_BASIC_PARAM_TO_STRING(offsetY_);
    ret += RENDER_BASIC_PARAM_TO_STRING(int(nodeRotation_));
    ret += RENDER_BASIC_PARAM_TO_STRING(int(screenRotation_));
    ret += RENDER_BASIC_PARAM_TO_STRING(screenId_);
    ret += RENDER_BASIC_PARAM_TO_STRING(mirroredId_);
    ret += RENDER_BASIC_PARAM_TO_STRING(compositeType_);
    ret += RENDER_BASIC_PARAM_TO_STRING(allMainAndLeashSurfaces_.size());
    // ret += RENDER_BASIC_PARAM_TO_STRING(mirrorSource_);
    ret += RENDER_PARAM_TO_STRING(screenInfo_);
    ret += "}";
    return ret;
}

} // namespace OHOS::Rosen
