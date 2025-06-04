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

#include "rotateoffscreen_param.h"

#include "platform/common/rs_log.h"

namespace OHOS::Rosen {

bool RotateOffScreenParam::GetRotateOffScreenDisplayNodeEnable()
{
    return isRotateOffScreenDisplayNodeEnable_;
}

bool RotateOffScreenParam::GetRotateOffScreenSurfaceNodeEnable()
{
    return isRotateOffScreenSurfaceNodeEnable_;
}

void RotateOffScreenParam::SetRotateOffScreenDisplayNodeEnable(bool enable)
{
    isRotateOffScreenDisplayNodeEnable_ = enable;
}

void RotateOffScreenParam::SetRotateOffScreenSurfaceNodeEnable(bool enable)
{
    isRotateOffScreenSurfaceNodeEnable_ = enable;
}

bool RotateOffScreenParam::GetRotateOffScreenDowngradeEnable()
{
    return isRotateOffScreenDowngradeEnable_;
}

void RotateOffScreenParam::SetRotateOffScreenDowngradeEnable(bool enable)
{
    isRotateOffScreenDowngradeEnable_ = enable;
}
} // namespace OHOS::Rosen