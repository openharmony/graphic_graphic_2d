/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "screen_manager/rs_screen_mode_info.h"

namespace OHOS {
namespace Rosen {
RSScreenModeInfo::RSScreenModeInfo(int32_t width, int32_t height, uint32_t refreshRate, int32_t id)
    : width_(width), height_(height), refreshRate_(refreshRate), modeId_(id)
{
}

RSScreenModeInfo::RSScreenModeInfo(const RSScreenModeInfo& other) : width_(other.width_),
    height_(other.height_), refreshRate_(other.refreshRate_), modeId_(other.modeId_)
{
}

RSScreenModeInfo& RSScreenModeInfo::operator=(const RSScreenModeInfo& other)
{
    width_ = other.width_;
    height_ = other.height_;
    refreshRate_ = other.refreshRate_;
    modeId_ = other.modeId_;
    return *this;
}

bool RSScreenModeInfo::Marshalling(Parcel& parcel) const
{
    return parcel.WriteInt32(width_) && parcel.WriteInt32(height_) &&
        parcel.WriteUint32(refreshRate_) && parcel.WriteInt32(modeId_);
}

RSScreenModeInfo* RSScreenModeInfo::Unmarshalling(Parcel& parcel)
{
    int32_t width;
    int32_t height;
    uint32_t refreshRate;
    int32_t id;
    if (!(parcel.ReadInt32(width) && parcel.ReadInt32(height) && parcel.ReadUint32(refreshRate)
        && parcel.ReadInt32(id))) {
        return nullptr;
    }

    RSScreenModeInfo* screenModeInfo = new RSScreenModeInfo(width, height, refreshRate, id);
    return screenModeInfo;
}

int32_t RSScreenModeInfo::GetScreenWidth() const
{
    return width_;
}

int32_t RSScreenModeInfo::GetScreenHeight() const
{
    return height_;
}

uint32_t RSScreenModeInfo::GetScreenRefreshRate() const
{
    return refreshRate_;
}

int32_t RSScreenModeInfo::GetScreenModeId() const
{
    return modeId_;
}

void RSScreenModeInfo::SetScreenWidth(int32_t width)
{
    width_ = width;
}

void RSScreenModeInfo::SetScreenHeight(int32_t height)
{
    height_ = height;
}

void RSScreenModeInfo::SetScreenRefreshRate(uint32_t refreshRate)
{
    refreshRate_ = refreshRate;
}

void RSScreenModeInfo::SetScreenModeId(int32_t id)
{
    modeId_ = id;
}
} // namespace Rosen
} // namespace OHOS