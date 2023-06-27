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

#include "recording/cmd_list_helper.h"
#include "recording/recording_path.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RecordingRegion::RecordingRegion() : cmdList_(std::make_shared<RegionCmdList>()) {}

std::shared_ptr<RegionCmdList> RecordingRegion::GetCmdList() const
{
    return cmdList_;
}

bool RecordingRegion::SetRect(const RectI& rectI)
{
    cmdList_->AddOp<SetRectOpItem>(rectI);
    return true;
}

bool RecordingRegion::SetPath(const Path& path, const Region& clip)
{
    auto pathHandle = CmdListHelper::AddRecordedToCmdList<RecordingPath>(*cmdList_, path);
    auto regionHandle = CmdListHelper::AddRecordedToCmdList<RecordingRegion>(*cmdList_, clip);
    cmdList_->AddOp<SetPathOpItem>(pathHandle, regionHandle);
    return true;
}

bool RecordingRegion::Op(const Region& region, RegionOp op)
{
    auto regionHandle = CmdListHelper::AddRecordedToCmdList<RecordingRegion>(*cmdList_, region);
    cmdList_->AddOp<RegionOpWithOpItem>(regionHandle, op);
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
