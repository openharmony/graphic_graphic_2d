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
#include "feature/delegate_composite/rs_delegate_composite_params.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

RsDelegateCompositeParams::RsDelegateCompositeParams(std::string name, NodeId nodeId)
{
    name_ = name;
    nodeId_ = nodeId;
}

void RsDelegateCompositeParams::SetDelegateDstRect(float positionX, float positionY,
    float positionZ, float positionW)
{
    delegateDstRect_.x_ = positionX;
    delegateDstRect_.y_ = positionY;
    delegateDstRect_.z_ = positionZ;
    delegateDstRect_.w_ = positionW;
    RS_TRACE_NAME_FMT("RsDelegateCompositeParams::SetDelegateDstRect: rect=[%.2f, %.2f, %.2f, %.2f], name=%s",
        positionX, positionY, positionZ, positionW, name_.c_str());
}

Vector4f RsDelegateCompositeParams::GetDelegateDstRect()
{
    return delegateDstRect_;
}

void RsDelegateCompositeParams::SetDelegateSrcRect(float positionX, float positionY,
    float positionZ, float positionW)
{
    delegateSrcRect_.x_ = positionX;
    delegateSrcRect_.y_ = positionY;
    delegateSrcRect_.z_ = positionZ;
    delegateSrcRect_.w_ = positionW;
    RS_TRACE_NAME_FMT("RsDelegateCompositeParams::SetDelegateSrcRect: rect=[%.2f, %.2f, %.2f, %.2f], name=%s",
        positionX, positionY, positionZ, positionW, name_.c_str());
}

Vector4f RsDelegateCompositeParams::GetDelegateSrcRect()
{
    return delegateSrcRect_;
}

void RsDelegateCompositeParams::SetDelegateMode(bool isSetDelegateMode)
{
    isSetDelegateMode_.store(isSetDelegateMode);
}

bool RsDelegateCompositeParams::GetDelegateMode()
{
    return isSetDelegateMode_.load();
}
} // namespace Rosen
} // namespace OHOS
