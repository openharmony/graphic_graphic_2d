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

#include "transaction/rs_self_drawing_node_rect_data.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
bool RSSelfDrawingNodeRectData::Marshalling(Parcel& parcel) const
{
    bool marshallingSuccess = true;
    marshallingSuccess &= parcel.WriteUint32(rectData_.size());
    for (const auto& data : rectData_) {
        marshallingSuccess &= parcel.WriteUint64(data.first);
        marshallingSuccess &= parcel.WriteInt32(data.second.GetLeft());
        marshallingSuccess &= parcel.WriteInt32(data.second.GetTop());
        marshallingSuccess &= parcel.WriteInt32(data.second.GetWidth());
        marshallingSuccess &= parcel.WriteInt32(data.second.GetHeight());
    }
    if (!marshallingSuccess) {
        RS_LOGE("RSSelfDrawingNodeRectData::Marshalling failed");
    }
    return marshallingSuccess;
}

RSSelfDrawingNodeRectData* RSSelfDrawingNodeRectData::Unmarshalling(Parcel& parcel)
{
    uint32_t mapSize;
    if (!parcel.ReadUint32(mapSize)) {
        RS_LOGE("RSSelfDrawingNodeRectData::Unmarshalling read mapSize failed");
        return nullptr;
    }

    auto SelfDrawingNodeRectData = new RSSelfDrawingNodeRectData();
    if (!SelfDrawingNodeRectData) {
        return nullptr;
    }

    if (mapSize > SelfDrawingNodeRectData->rectData_.max_size()) {
        RS_LOGE("RSSelfDrawingNodeRectData Unmarshalling failed, map size overflow.");
        delete SelfDrawingNodeRectData;
        return nullptr;
    }

    for (uint32_t index = 0; index < mapSize; ++index) {
        uint64_t nodeId;
        std::string nodeName;
        RectI rect;
        if (!parcel.ReadUint64(nodeId) || !parcel.ReadInt32(rect.left_) || !parcel.ReadInt32(rect.top_) ||
            !parcel.ReadInt32(rect.width_) || !parcel.ReadInt32(rect.height_)) {
            RS_LOGE("RSSelfDrawingNodeRectData::Unmarshalling read rectData failed");
            delete SelfDrawingNodeRectData;
            return nullptr;
        }
        SelfDrawingNodeRectData->rectData_.insert(std::make_pair(nodeId, rect));
    }
    return SelfDrawingNodeRectData;
}
} // namespace Rosen
} // namespace OHOS
