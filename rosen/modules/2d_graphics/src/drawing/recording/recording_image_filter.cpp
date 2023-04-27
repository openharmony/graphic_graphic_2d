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

#include "recording/recording_image_filter.h"

#include "recording/recording_color_filter.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RecordingImageFilter::RecordingImageFilter() noexcept : cmdList_(std::make_shared<ImageFilterCmdList>()) {}

std::shared_ptr<RecordingImageFilter> RecordingImageFilter::CreateBlurImageFilter(
    scalar sigmaX, scalar sigmaY, TileMode mode, const std::shared_ptr<ImageFilter>& input)
{
    auto imageFilter = std::make_shared<RecordingImageFilter>();
    CmdListSiteInfo inputData = imageFilter->AddCmdListData(input);
    imageFilter->GetCmdList()->AddOp<CreateBlurImageFilterOpItem>(sigmaX, sigmaY, mode, inputData);
    return imageFilter;
}

std::shared_ptr<RecordingImageFilter> RecordingImageFilter::CreateColorFilterImageFilter(
    const ColorFilter& cf, const std::shared_ptr<ImageFilter>& input)
{
    auto cfCmdList = static_cast<const RecordingColorFilter&>(cf).GetCmdList();
    if (cfCmdList == nullptr) {
        LOGE("RecordingImageFilter::CreateColorFilterImageFilter, color filter is valid!");
        return nullptr;
    }

    auto imageFilter = std::make_shared<RecordingImageFilter>();
    auto cfCmdData = cfCmdList->GetData();
    auto cfDataOffset = imageFilter->GetCmdList()->AddCmdListData(cfCmdData);
    CmdListSiteInfo cfData = { cfDataOffset, cfCmdData.second };

    CmdListSiteInfo inputData = imageFilter->AddCmdListData(input);
    imageFilter->GetCmdList()->AddOp<CreateColorFilterImageFilterOpItem>(cfData, inputData);
    return imageFilter;
}

std::shared_ptr<RecordingImageFilter> RecordingImageFilter::CreateOffsetImageFilter(
    scalar dx, scalar dy, const std::shared_ptr<ImageFilter>& input)
{
    auto imageFilter = std::make_shared<RecordingImageFilter>();
    CmdListSiteInfo inputData = imageFilter->AddCmdListData(input);
    imageFilter->GetCmdList()->AddOp<CreateOffsetImageFilterOpItem>(dx, dy, inputData);
    return imageFilter;
}

std::shared_ptr<RecordingImageFilter> RecordingImageFilter::CreateArithmeticImageFilter(
    const std::vector<scalar>& coefficients, bool enforcePMColor,
    const std::shared_ptr<ImageFilter>& background, const std::shared_ptr<ImageFilter>& foreground)
{
    auto imageFilter = std::make_shared<RecordingImageFilter>();

    std::pair<int, size_t> coefficientsData = { 0, 0 };
    if (!coefficients.empty()) {
        const void* data = static_cast<const void*>(coefficients.data());
        size_t coefficientsByteSize = coefficients.size() * sizeof(scalar);
        auto offset = imageFilter->GetCmdList()->AddCmdListData({ data, coefficientsByteSize });
        coefficientsData = { offset, coefficientsByteSize };
    }

    CmdListSiteInfo backgroundData = imageFilter->AddCmdListData(background);
    CmdListSiteInfo foregroundData = imageFilter->AddCmdListData(foreground);

    imageFilter->GetCmdList()->AddOp<CreateArithmeticImageFilterOpItem>(
        coefficientsData, enforcePMColor, backgroundData, foregroundData);
    return imageFilter;
}

std::shared_ptr<RecordingImageFilter> RecordingImageFilter::CreateComposeImageFilter(
    const std::shared_ptr<ImageFilter>& f1, const std::shared_ptr<ImageFilter>& f2)
{
    auto imageFilter = std::make_shared<RecordingImageFilter>();
    CmdListSiteInfo f1Data = imageFilter->AddCmdListData(f1);
    CmdListSiteInfo f2Data = imageFilter->AddCmdListData(f2);
    imageFilter->GetCmdList()->AddOp<CreateComposeImageFilterOpItem>(f1Data, f2Data);
    return imageFilter;
}

CmdListSiteInfo RecordingImageFilter::AddCmdListData(const std::shared_ptr<ImageFilter>& filter)
{
    CmdListSiteInfo filterData = { 0, 0 };
    if (filter) {
        auto filterCmdList = static_cast<RecordingImageFilter*>(filter.get())->GetCmdList();
        if (filterCmdList) {
            auto cmdData = filterCmdList->GetData();
            auto offset = cmdList_->AddCmdListData(cmdData);
            filterData = { offset, cmdData.second };
        }
    }
    return filterData;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
