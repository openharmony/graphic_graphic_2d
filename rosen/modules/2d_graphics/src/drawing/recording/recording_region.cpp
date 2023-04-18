/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "recording/recording_region.h"
#include "recording/recording_path.h"
#include "recording/mem_allocator.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RecordingRegion::RecordingRegion()
{
    cmdList_ = std::make_shared<RegionCmdList>();
}

RecordingRegion::~RecordingRegion() {}

std::shared_ptr<RegionCmdList> RecordingRegion::GetCmdList() const
{
    return cmdList_;
}

bool RecordingRegion::SetRect(const RectI& rectI)
{
    if (cmdList_ == nullptr) {
        return false;
    }
    cmdList_->AddOp<SetRectOpItem>(rectI);
    return true;
}

bool RecordingRegion::SetPath(const Path& path, const Region& clip)
{
    if (cmdList_ == nullptr) {
        return false;
    }
    auto recordingPath = static_cast<const RecordingPath&>(path);
    Offset_t pathOffset = cmdList_->AddCmdListData(recordingPath.GetCmdList()->GetData());
    auto recordingRegion = static_cast<const RecordingRegion&>(clip);
    Offset_t regionOffset = cmdList_->AddCmdListData(recordingRegion.GetCmdList()->GetData());
    cmdList_->AddOp<SetPathOpItem>(pathOffset, recordingPath.GetCmdList()->GetData().second,
                                    regionOffset, recordingRegion.GetCmdList()->GetData().second);
    return true;
}

bool RecordingRegion::Op(const Region& region, RegionOp op)
{
    if (cmdList_ == nullptr) {
        return false;
    }
    auto recordingRegion = static_cast<const RecordingRegion&>(region);
    Offset_t regionOffset = cmdList_->AddCmdListData(recordingRegion.GetCmdList()->GetData());
    cmdList_->AddOp<RegionOpWithOpItem>(op, regionOffset, recordingRegion.GetCmdList()->GetData().second);
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
