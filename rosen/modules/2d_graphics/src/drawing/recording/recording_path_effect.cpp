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

#include "recording/recording_path_effect.h"

#include "recording/recording_path.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RecordingPathEffect::RecordingPathEffect() noexcept : cmdList_(std::make_shared<PathEffectCmdList>()) {}

std::shared_ptr<RecordingPathEffect> RecordingPathEffect::CreateDashPathEffect(
    const std::vector<scalar>& intervals, scalar phase)
{
    auto pathEffect = std::make_shared<RecordingPathEffect>();
    std::pair<int, size_t> intervalsData(0, 0);
    if (!intervals.empty()) {
        const void* data = static_cast<const void*>(intervals.data());
        size_t size = intervals.size() * sizeof(scalar);
        auto offset = pathEffect->GetCmdList()->AddCmdListData({ data, size });
        intervalsData.first = offset;
        intervalsData.second = size;
    }

    pathEffect->GetCmdList()->AddOp<CreateDashPathEffectOpItem>(intervalsData, phase);
    return pathEffect;
}

std::shared_ptr<RecordingPathEffect> RecordingPathEffect::CreatePathDashEffect(
    const Path& path, scalar advance, scalar phase, PathDashStyle style)
{
    auto pathCmdList = static_cast<const RecordingPath&>(path).GetCmdList();
    if (pathCmdList == nullptr) {
        LOGE("RecordingPathEffect::CreatePathDashEffect, path is valid!");
        return nullptr;
    }

    auto cmdData = pathCmdList->GetData();
    auto pathEffect = std::make_shared<RecordingPathEffect>();
    auto offset = pathEffect->GetCmdList()->AddCmdListData(cmdData);
    CmdListSiteInfo pathData(offset, cmdData.second);

    pathEffect->GetCmdList()->AddOp<CreatePathDashEffectOpItem>(pathData, advance, phase, style);
    return pathEffect;
}

std::shared_ptr<RecordingPathEffect> RecordingPathEffect::CreateCornerPathEffect(scalar radius)
{
    auto pathEffect = std::make_shared<RecordingPathEffect>();
    pathEffect->GetCmdList()->AddOp<CreateCornerPathEffectOpItem>(radius);
    return pathEffect;
}

std::shared_ptr<RecordingPathEffect> RecordingPathEffect::CreateSumPathEffect(
    const PathEffect& e1, const PathEffect& e2)
{
    auto e1CmdList = static_cast<const RecordingPathEffect&>(e1).GetCmdList();
    auto e2CmdList = static_cast<const RecordingPathEffect&>(e2).GetCmdList();
    if (e1CmdList == nullptr || e2CmdList == nullptr) {
        LOGE("RecordingPathEffect::CreateSumPathEffect, effect1 or effect2 is valid!");
        return nullptr;
    }

    auto pathEffect = std::make_shared<RecordingPathEffect>();
    auto cmdData = e1CmdList->GetData();
    auto offset = pathEffect->GetCmdList()->AddCmdListData(cmdData);
    CmdListSiteInfo e1Data(offset, cmdData.second);

    cmdData = e2CmdList->GetData();
    offset = pathEffect->GetCmdList()->AddCmdListData(cmdData);
    CmdListSiteInfo e2Data(offset, cmdData.second);

    pathEffect->GetCmdList()->AddOp<CreateSumPathEffectOpItem>(e1Data, e2Data);
    return pathEffect;
}

std::shared_ptr<RecordingPathEffect> RecordingPathEffect::CreateComposePathEffect(
    const PathEffect& e1, const PathEffect& e2)
{
    auto e1CmdList = static_cast<const RecordingPathEffect&>(e1).GetCmdList();
    auto e2CmdList = static_cast<const RecordingPathEffect&>(e2).GetCmdList();
    if (e1CmdList == nullptr || e2CmdList == nullptr) {
        LOGE("RecordingPathEffect::CreateSumPathEffect, effect1 or effect2 is valid!");
        return nullptr;
    }

    auto pathEffect = std::make_shared<RecordingPathEffect>();
    auto cmdData = e1CmdList->GetData();
    auto offset = pathEffect->GetCmdList()->AddCmdListData(cmdData);
    CmdListSiteInfo e1Data(offset, cmdData.second);

    cmdData = e2CmdList->GetData();
    offset = pathEffect->GetCmdList()->AddCmdListData(cmdData);
    CmdListSiteInfo e2Data(offset, cmdData.second);

    pathEffect->GetCmdList()->AddOp<CreateComposePathEffectOpItem>(e1Data, e2Data);
    return pathEffect;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
