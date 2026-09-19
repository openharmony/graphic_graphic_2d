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

#include "ui_effect/mask/include/atlas_frame_mask_para.h"
#include "platform/common/rs_log.h"
#include "pixel_map.h"

#ifdef ROSEN_OHOS
#include "media_errors.h"
#endif

namespace OHOS {
namespace Rosen {

AtlasFrameMaskPara::AtlasFrameMaskPara(const AtlasFrameMaskPara& other)
    : MaskPara(other), atlasInfo_(other.atlasInfo_)
{
#ifdef ROSEN_OHOS
    if (other.atlasInfo_.pixelMap != nullptr) {
        int32_t errorCode = Media::ERR_MEDIA_INVALID_VALUE;
        auto pixelMap = other.atlasInfo_.pixelMap->Clone(errorCode);
        atlasInfo_.pixelMap = std::move(pixelMap);
        if (errorCode != Media::SUCCESS || atlasInfo_.pixelMap == nullptr) {
            RS_LOGE("[ui_effect] AtlasFrameMaskPara clone pixelMap failed");
        }
    }
#endif
}

bool AtlasFrameMaskPara::Marshalling(Parcel& parcel) const
{
    bool isSuccess = parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteInt32(atlasInfo_.mode) &&
        parcel.WriteInt32(atlasInfo_.rows) &&
        parcel.WriteInt32(atlasInfo_.cols) &&
        parcel.WriteFloat(atlasInfo_.frameWidth) &&
        parcel.WriteFloat(atlasInfo_.frameHeight) &&
        parcel.WriteFloat(atlasInfo_.padding) &&
        parcel.WriteInt32(atlasInfo_.totalFrame) &&
        parcel.WriteFloat(atlasInfo_.frameIndex);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] AtlasFrameMaskPara Marshalling write scalar fields failed");
        return false;
    }
    if (atlasInfo_.pixelMap != nullptr && !atlasInfo_.pixelMap->Marshalling(parcel)) {
        RS_LOGE("[ui_effect] AtlasFrameMaskPara Marshalling write pixelMap failed");
        return false;
    }
    return true;
}

void AtlasFrameMaskPara::RegisterUnmarshallingCallback()
{
    MaskPara::RegisterUnmarshallingCallback(MaskPara::Type::ATLAS_FRAME_MASK, OnUnmarshalling);
}

bool AtlasFrameMaskPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val)
{
    uint16_t type = MaskPara::Type::NONE;
    if (!parcel.ReadUint16(type) || type != MaskPara::Type::ATLAS_FRAME_MASK) {
        RS_LOGE("[ui_effect] AtlasFrameMaskPara OnUnmarshalling read type failed");
        return false;
    }

    auto para = std::make_shared<AtlasFrameMaskPara>();
    AtlasInfo atlasInfo;
    if (!parcel.ReadInt32(atlasInfo.mode) ||
        !parcel.ReadInt32(atlasInfo.rows) ||
        !parcel.ReadInt32(atlasInfo.cols) ||
        !parcel.ReadFloat(atlasInfo.frameWidth) ||
        !parcel.ReadFloat(atlasInfo.frameHeight) ||
        !parcel.ReadFloat(atlasInfo.padding) ||
        !parcel.ReadInt32(atlasInfo.totalFrame) ||
        !parcel.ReadFloat(atlasInfo.frameIndex)) {
        RS_LOGE("[ui_effect] AtlasFrameMaskPara OnUnmarshalling read scalar fields failed");
        return false;
    }
    auto pixelMap = Media::PixelMap::Unmarshalling(parcel);
    if (pixelMap == nullptr) {
        RS_LOGE("[ui_effect] AtlasFrameMaskPara OnUnmarshalling read pixelMap failed");
        return false;
    }
    atlasInfo.pixelMap = std::shared_ptr<Media::PixelMap>(pixelMap);
    para->SetAtlasInfo(atlasInfo);
    val = std::move(para);
    return true;
}

std::shared_ptr<MaskPara> AtlasFrameMaskPara::Clone() const
{
    return std::make_shared<AtlasFrameMaskPara>(*this);
}

} // namespace Rosen
} // namespace OHOS
