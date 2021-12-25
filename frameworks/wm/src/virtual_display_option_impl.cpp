/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "virtual_display_option_impl.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMVirtualDisplayOptionImpl"};
}

GSError VirtualDisplayOptionImpl::SetX(int32_t x)
{
    this->x = x;
    this->settingX = true;
    return GSERROR_OK;
}

GSError VirtualDisplayOptionImpl::SetY(int32_t y)
{
    this->y = y;
    this->settingY = true;
    return GSERROR_OK;
}

GSError VirtualDisplayOptionImpl::SetWidth(uint32_t width)
{
    if (!(width > 0)) {
        WMLOGFE("width is invalid, should > 0");
        return GSERROR_INVALID_ARGUMENTS;
    }

    this->width = width;
    this->settingWidth = true;
    return GSERROR_OK;
}

GSError VirtualDisplayOptionImpl::SetHeight(uint32_t height)
{
    if (!(height > 0)) {
        WMLOGFE("height is invalid, should > 0");
        return GSERROR_INVALID_ARGUMENTS;
    }

    this->height = height;
    this->settingHeight = true;
    return GSERROR_OK;
}

int32_t VirtualDisplayOptionImpl::GetX() const
{
    return x;
}

int32_t VirtualDisplayOptionImpl::GetY() const
{
    return y;
}

uint32_t VirtualDisplayOptionImpl::GetWidth() const
{
    return width;
}

uint32_t VirtualDisplayOptionImpl::GetHeight() const
{
    return height;
}

bool VirtualDisplayOptionImpl::IsSettingX() const
{
    return settingX;
}

bool VirtualDisplayOptionImpl::IsSettingY() const
{
    return settingY;
}

bool VirtualDisplayOptionImpl::IsSettingWidth() const
{
    return settingWidth;
}

bool VirtualDisplayOptionImpl::IsSettingHeight() const
{
    return settingHeight;
}
} // namespace OHOS
