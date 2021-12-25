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

#include "subwindow_option_impl.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMSubwindowOptionImpl"};
}

GSError SubwindowOptionImpl::SetWindowType(SubwindowType t)
{
    if (!(t >= 0 && t < SUBWINDOW_TYPE_MAX)) {
        WMLOGFE("type is invalid, should in [0, %{public}d)", SUBWINDOW_TYPE_MAX);
        return GSERROR_INVALID_ARGUMENTS;
    }

    type = t;
    return GSERROR_OK;
}

GSError SubwindowOptionImpl::SetX(int32_t x)
{
    this->x = x;
    return GSERROR_OK;
}

GSError SubwindowOptionImpl::SetY(int32_t y)
{
    this->y = y;
    return GSERROR_OK;
}

GSError SubwindowOptionImpl::SetWidth(uint32_t w)
{
    if (!(w > 0)) {
        WMLOGFE("width is invalid, should > 0");
        return GSERROR_INVALID_ARGUMENTS;
    }

    width = w;
    return GSERROR_OK;
}

GSError SubwindowOptionImpl::SetHeight(uint32_t h)
{
    if (!(h > 0)) {
        WMLOGFE("height is invalid, should > 0");
        return GSERROR_INVALID_ARGUMENTS;
    }

    height = h;
    return GSERROR_OK;
}

GSError SubwindowOptionImpl::SetConsumerSurface(const sptr<Surface> &surf)
{
    if (surf != nullptr && !(surf->IsConsumer() == true)) {
        WMLOGFE("surf is invalid, should be consumer");
        return GSERROR_INVALID_ARGUMENTS;
    }

    consumerSurface = surf;
    return GSERROR_OK;
}

SubwindowType SubwindowOptionImpl::GetWindowType() const
{
    return type;
}

int32_t SubwindowOptionImpl::GetX() const
{
    return x;
}

int32_t SubwindowOptionImpl::GetY() const
{
    return y;
}

uint32_t SubwindowOptionImpl::GetWidth() const
{
    return width;
}

uint32_t SubwindowOptionImpl::GetHeight() const
{
    return height;
}

sptr<Surface> SubwindowOptionImpl::GetConsumerSurface() const
{
    return consumerSurface;
}
} // namespace OHOS
