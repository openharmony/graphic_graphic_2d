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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_SELF_DRAWING_NODE_RECT_DATA_H
#define ROSEN_RENDER_SERVICE_BASE_RS_SELF_DRAWING_NODE_RECT_DATA_H

#include <map>
#include <parcel.h>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_rect.h"

namespace OHOS {
namespace Rosen {

using SelfDrawingNodeRectCallbackData = std::map<NodeId, RectI>;
class RSB_EXPORT RSSelfDrawingNodeRectData : public Parcelable {
public:
    RSSelfDrawingNodeRectData() = default;
    RSSelfDrawingNodeRectData(SelfDrawingNodeRectCallbackData& data)
    {
        for (auto iter = data.begin(); iter != data.end(); ++iter) {
            rectData_.insert(std::make_pair(iter->first, iter->second));
        }
    }
    RSSelfDrawingNodeRectData(RSSelfDrawingNodeRectData&& other) : rectData_(std::move(other.rectData_)) {}
    virtual ~RSSelfDrawingNodeRectData() noexcept = default;

    const SelfDrawingNodeRectCallbackData& GetRectData() const
    {
        return rectData_;
    }
    [[nodiscard]] static RSSelfDrawingNodeRectData* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;

private:
    SelfDrawingNodeRectCallbackData rectData_;
};
} // namespace Rosen
} // namespace OHOS

#endif