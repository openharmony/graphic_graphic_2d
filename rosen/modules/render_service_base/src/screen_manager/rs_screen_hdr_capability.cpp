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

#include "screen_manager/rs_screen_hdr_capability.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSScreenHDRCapability::RSScreenHDRCapability(float maxLum, float minLum, float maxAverageLum,
    const std::vector<ScreenHDRFormat>& formats) : maxLum_(maxLum), minLum_(minLum),
    maxAverageLum_(maxAverageLum), hdrFormats_(formats)
{
}

float RSScreenHDRCapability::GetMaxLum() const
{
    return maxLum_;
}

float RSScreenHDRCapability::GetMinLum() const
{
    return minLum_;
}

float RSScreenHDRCapability::GetMaxAverageLum() const
{
    return maxAverageLum_;
}

const std::vector<ScreenHDRFormat>& RSScreenHDRCapability::GetHdrFormats() const
{
    return hdrFormats_;
}

void RSScreenHDRCapability::SetMaxLum(float maxLum)
{
    maxLum_ = maxLum;
}

void RSScreenHDRCapability::SetMinLum(float minLum)
{
    minLum_ = minLum;
}

void RSScreenHDRCapability::SetMaxAverageLum(float maxAverageLum)
{
    maxAverageLum_ = maxAverageLum;
}

void RSScreenHDRCapability::SetHdrFormats(std::vector<ScreenHDRFormat> formats)
{
    hdrFormats_ = std::move(formats);
}

bool RSScreenHDRCapability::WriteVector(const std::vector<ScreenHDRFormat>& formats, Parcel &parcel) const
{
    if (!parcel.WriteUint32(static_cast<uint32_t>(formats.size()))) {
        ROSEN_LOGE("RSScreenHDRCapability::WriteVector WriteUint32 1 failed");
        return false;
    }
    for (ScreenHDRFormat format : formats) {
        if (!parcel.WriteUint32(static_cast<uint32_t>(format))) {
            ROSEN_LOGE("RSScreenHDRCapability::WriteVector WriteUint32 2 failed");
            return false;
        }
    }
    return true;
}

bool RSScreenHDRCapability::ReadVector(std::vector<ScreenHDRFormat>& unmarFormats, Parcel &parcel)
{
    uint32_t size;
    if (!parcel.ReadUint32(size)) {
        ROSEN_LOGE("RSScreenHDRCapability::ReadVector ReadUint32 1 failed");
        return false;
    }
    size_t readableSize = parcel.GetReadableBytes() / sizeof(ScreenHDRFormat);
    size_t len = static_cast<size_t>(size);
    if (len > readableSize || len > unmarFormats.max_size()) {
        RS_LOGE("RSScreenHDRCapability ReadVector Failed to read vector, size:%{public}zu,"
            " readableSize:%{public}zu", len, readableSize);
        return false;
    }
    for (uint32_t index = 0; index < size; index++) {
        uint32_t format;
        if (!parcel.ReadUint32(format)) {
            ROSEN_LOGE("RSScreenHDRCapability::ReadVector ReadUint32 2 failed");
            return false;
        }
        unmarFormats.push_back(static_cast<ScreenHDRFormat>(format));
    }
    return true;
}

bool RSScreenHDRCapability::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteFloat(maxLum_)) {
        ROSEN_LOGE("RSScreenHDRCapability::Marshalling WriteFloat 1 failed");
        return false;
    }
    if (!parcel.WriteFloat(minLum_)) {
        ROSEN_LOGE("RSScreenHDRCapability::Marshalling WriteFloat 2 failed");
        return false;
    }
    if (!parcel.WriteFloat(maxAverageLum_)) {
        ROSEN_LOGE("RSScreenHDRCapability::Marshalling WriteFloat 3 failed");
        return false;
    }
    if (!WriteVector(hdrFormats_, parcel)) {
        ROSEN_LOGE("RSScreenHDRCapability::Marshalling WriteVector failed");
        return false;
    }
    return true;
}

RSScreenHDRCapability* RSScreenHDRCapability::Unmarshalling(Parcel &parcel)
{
    float maxLum;
    float minLum;
    float maxAverageLum;
    std::vector<ScreenHDRFormat> formats;
    if (!parcel.ReadFloat(maxLum)) {
        ROSEN_LOGE("RSScreenHDRCapability::Unmarshalling ReadFloat 1 failed");
        return nullptr;
    }
    if (!parcel.ReadFloat(minLum)) {
        ROSEN_LOGE("RSScreenHDRCapability::Unmarshalling ReadFloat 2 failed");
        return nullptr;
    }
    if (!parcel.ReadFloat(maxAverageLum)) {
        ROSEN_LOGE("RSScreenHDRCapability::Unmarshalling ReadFloat 3 failed");
        return nullptr;
    }
    if (!ReadVector(formats, parcel)) {
        ROSEN_LOGE("RSScreenHDRCapability::Unmarshalling ReadVector failed");
        return nullptr;
    }
    RSScreenHDRCapability* screenHdrCapability = new RSScreenHDRCapability(maxLum, minLum, maxAverageLum, formats);
    return screenHdrCapability;
}
} // namespace Rosen
} // namespace OHOS