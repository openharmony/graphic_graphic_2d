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

#include "recording/recording_color_filter.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

RecordingColorFilter::RecordingColorFilter() noexcept : cmdList_(std::make_shared<ColorFilterCmdList>())
{}

std::shared_ptr<RecordingColorFilter> RecordingColorFilter::CreateBlendModeColorFilter(ColorQuad c, BlendMode mode)
{
    auto colorFilter = std::make_shared<RecordingColorFilter>();
    colorFilter->GetCmdList()->AddOp<CreateBlendModeOpItem>(c, mode);
    return colorFilter;
}

std::shared_ptr<RecordingColorFilter> RecordingColorFilter::CreateComposeColorFilter(
    const ColorFilter& f1, const ColorFilter& f2)
{
    auto f1CmdList = static_cast<const RecordingColorFilter&>(f1).GetCmdList();
    auto f2CmdList = static_cast<const RecordingColorFilter&>(f2).GetCmdList();
    if (f1CmdList == nullptr || f2CmdList == nullptr) {
        LOGE("RecordingColorFilter::CreateComposeColorFilter, color filter is valid!");
        return nullptr;
    }

    auto colorFilter = std::make_shared<RecordingColorFilter>();
    auto cmdData = f1CmdList->GetData();
    auto offset = colorFilter->GetCmdList()->AddCmdListData(cmdData);
    CmdListSiteInfo f1Data(offset, cmdData.second);

    cmdData = f2CmdList->GetData();
    offset = colorFilter->GetCmdList()->AddCmdListData(cmdData);
    CmdListSiteInfo f2Data(offset, cmdData.second);

    colorFilter->GetCmdList()->AddOp<CreateComposeOpItem>(f1Data, f2Data);
    return colorFilter;
}

std::shared_ptr<RecordingColorFilter> RecordingColorFilter::CreateMatrixColorFilter(const ColorMatrix& m)
{
    auto colorFilter = std::make_shared<RecordingColorFilter>();
    colorFilter->GetCmdList()->AddOp<CreateMatrixOpItem>(m);
    return colorFilter;
}

std::shared_ptr<RecordingColorFilter> RecordingColorFilter::CreateLinearToSrgbGamma()
{
    auto colorFilter = std::make_shared<RecordingColorFilter>();
    colorFilter->GetCmdList()->AddOp<CreateLinearToSrgbGammaOpItem>();
    return colorFilter;
}

std::shared_ptr<RecordingColorFilter> RecordingColorFilter::CreateSrgbGammaToLinear()
{
    auto colorFilter = std::make_shared<RecordingColorFilter>();
    colorFilter->GetCmdList()->AddOp<CreateSrgbGammaToLinearOpItem>();
    return colorFilter;
}

std::shared_ptr<RecordingColorFilter> RecordingColorFilter::CreateLumaColorFilter()
{
    auto colorFilter = std::make_shared<RecordingColorFilter>();
    colorFilter->GetCmdList()->AddOp<CreateLumaOpItem>();
    return colorFilter;
}

void RecordingColorFilter::Compose(const ColorFilter& filter)
{
    auto filterCmdList = static_cast<const RecordingColorFilter&>(filter).GetCmdList();
    if (filterCmdList == nullptr) {
        LOGE("RecordingColorFilter::Compose failed, color filter is valid!");
        return;
    }
    auto cmdData = filterCmdList->GetData();
    auto offset = cmdList_->AddCmdListData(cmdData);
    CmdListSiteInfo filterData(offset, cmdData.second);

    cmdList_->AddOp<ColorFilterComposeOpItem>(filterData);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
