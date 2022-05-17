/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "screen_manager/rs_virtual_screen_resolution.h"

namespace OHOS {
namespace Rosen {
RSVirtualScreenResolution::RSVirtualScreenResolution(uint32_t width, uint32_t height)
    : width_(width), height_(height)
{
}

RSVirtualScreenResolution::RSVirtualScreenResolution(const RSVirtualScreenResolution& other)
    : width_(other.width_), height_(other.height_)
{
}

RSVirtualScreenResolution& RSVirtualScreenResolution::operator=(const RSVirtualScreenResolution& other)
{
    width_ = other.width_;
    height_ = other.height_;
    return *this;
}

bool RSVirtualScreenResolution::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint32(width_) && parcel.WriteUint32(height_);
}

RSVirtualScreenResolution* RSVirtualScreenResolution::Unmarshalling(Parcel& parcel)
{
    uint32_t width;
    uint32_t height;
    if (!(parcel.ReadUint32(width) && parcel.ReadUint32(height))) {
        return nullptr;
    }

    RSVirtualScreenResolution* virtualScreenResolution = new RSVirtualScreenResolution(width, height);
    return virtualScreenResolution;
}

uint32_t RSVirtualScreenResolution::GetVirtualScreenWidth() const
{
    return width_;
}

uint32_t RSVirtualScreenResolution::GetVirtualScreenHeight() const
{
    return height_;
}

void RSVirtualScreenResolution::SetVirtualScreenWidth(uint32_t width)
{
    width_ = width;
}

void RSVirtualScreenResolution::SetVirtualScreenHeight(uint32_t height)
{
    height_ = height;
}
} // namespace Rosen
} // namespace OHOS