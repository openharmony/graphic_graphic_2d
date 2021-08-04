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

#include "window_option_impl.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMWindowOptionImpl"};
}

WMError WindowOptionImpl::SetWindowType(WindowType t)
{
    if (!(t >= 0 && t < WINDOW_TYPE_MAX)) {
        WMLOGFE("type is invalid, should in [0, %{public}d)", WINDOW_TYPE_MAX);
        return WM_ERROR_INVALID_PARAM;
    }

    type = t;
    return WM_OK;
}

WMError WindowOptionImpl::SetWindowMode(WindowMode m)
{
    if (!(m >= 0 && m < WINDOW_MODE_MAX)) {
        WMLOGFE("mode is invalid, should in [0, %{public}d)", WINDOW_MODE_MAX);
        return WM_ERROR_INVALID_PARAM;
    }

    mode = m;
    return WM_OK;
}

WMError WindowOptionImpl::SetDisplay(int32_t did)
{
    if (!(did >= 0)) {
        WMLOGFE("displayId is invalid, should >= 0");
        return WM_ERROR_INVALID_PARAM;
    }

    displayId = did;
    return WM_OK;
}

WMError WindowOptionImpl::SetConsumerSurface(const sptr<Surface> &surface)
{
    if (surface != nullptr && !(surface->IsConsumer() == true)) {
        WMLOGFE("surface is invalid, should be consumer");
        return WM_ERROR_INVALID_PARAM;
    }

    consumerSurface = surface;
    return WM_OK;
}

WMError WindowOptionImpl::SetX(int32_t x)
{
    this->x = x;
    this->settingX = true;
    return WM_OK;
}

WMError WindowOptionImpl::SetY(int32_t y)
{
    this->y = y;
    this->settingY = true;
    return WM_OK;
}

WMError WindowOptionImpl::SetWidth(uint32_t width)
{
    if (!(width > 0)) {
        WMLOGFE("width is invalid, should > 0");
        return WM_ERROR_INVALID_PARAM;
    }

    this->width = width;
    this->settingWidth = true;
    return WM_OK;
}

WMError WindowOptionImpl::SetHeight(uint32_t height)
{
    if (!(height > 0)) {
        WMLOGFE("height is invalid, should > 0");
        return WM_ERROR_INVALID_PARAM;
    }

    this->height = height;
    this->settingHeight = true;
    return WM_OK;
}

WindowType WindowOptionImpl::GetWindowType() const
{
    return type;
}

WindowMode WindowOptionImpl::GetWindowMode() const
{
    return mode;
}

int32_t WindowOptionImpl::GetDisplay() const
{
    return displayId;
}

sptr<Surface> WindowOptionImpl::GetConsumerSurface() const
{
    return consumerSurface;
}

int32_t WindowOptionImpl::GetX() const
{
    return x;
}

int32_t WindowOptionImpl::GetY() const
{
    return y;
}

uint32_t WindowOptionImpl::GetWidth() const
{
    return width;
}

uint32_t WindowOptionImpl::GetHeight() const
{
    return height;
}

bool WindowOptionImpl::IsSettingX() const
{
    return settingX;
}

bool WindowOptionImpl::IsSettingY() const
{
    return settingY;
}

bool WindowOptionImpl::IsSettingWidth() const
{
    return settingWidth;
}

bool WindowOptionImpl::IsSettingHeight() const
{
    return settingHeight;
}
} // namespace OHOS
