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

#ifndef RECORDING_REGION_H
#define RECORDING_REGION_H

#include "utils/region.h"
#include "recording/region_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingRegion : public Region {
public:
    RecordingRegion();
    ~RecordingRegion();

    std::shared_ptr<RegionCmdList> GetCmdList() const;

    bool SetRect(const RectI& rectI) override;
    bool SetPath(const Path& path, const Region& clip) override;
    bool Op(const Region& region, RegionOp op) override;

private:
    std::shared_ptr<RegionCmdList> cmdList_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // RECORDING_REGION_H
