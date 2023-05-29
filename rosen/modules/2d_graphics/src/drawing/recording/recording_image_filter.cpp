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

#include "recording/cmd_list_helper.h"
#include "recording/recording_color_filter.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RecordingImageFilter::RecordingImageFilter() noexcept : cmdList_(std::make_shared<ImageFilterCmdList>()) {}

std::shared_ptr<RecordingImageFilter> RecordingImageFilter::CreateBlurImageFilter(
    scalar sigmaX, scalar sigmaY, TileMode mode, const std::shared_ptr<ImageFilter>& input)
{
    auto imageFilter = std::make_shared<RecordingImageFilter>();
    auto inputHandle = CmdListHelper::AddRecordedToCmdList(
        *imageFilter->GetCmdList(), std::static_pointer_cast<RecordingImageFilter>(input));
    imageFilter->GetCmdList()->AddOp<CreateBlurImageFilterOpItem>(sigmaX, sigmaY, mode, inputHandle);
    return imageFilter;
}

std::shared_ptr<RecordingImageFilter> RecordingImageFilter::CreateColorFilterImageFilter(
    const ColorFilter& cf, const std::shared_ptr<ImageFilter>& input)
{
    auto imageFilter = std::make_shared<RecordingImageFilter>();

    auto colorFilterHandle = CmdListHelper::AddRecordedToCmdList(
        *imageFilter->GetCmdList(), static_cast<const RecordingColorFilter&>(cf));

    auto inputHandle = CmdListHelper::AddRecordedToCmdList(
        *imageFilter->GetCmdList(), std::static_pointer_cast<RecordingImageFilter>(input));
    imageFilter->GetCmdList()->AddOp<CreateColorFilterImageFilterOpItem>(colorFilterHandle, inputHandle);
    return imageFilter;
}

std::shared_ptr<RecordingImageFilter> RecordingImageFilter::CreateOffsetImageFilter(
    scalar dx, scalar dy, const std::shared_ptr<ImageFilter>& input)
{
    auto imageFilter = std::make_shared<RecordingImageFilter>();
    auto inputHandle = CmdListHelper::AddRecordedToCmdList(
        *imageFilter->GetCmdList(), std::static_pointer_cast<RecordingImageFilter>(input));
    imageFilter->GetCmdList()->AddOp<CreateOffsetImageFilterOpItem>(dx, dy, inputHandle);
    return imageFilter;
}

std::shared_ptr<RecordingImageFilter> RecordingImageFilter::CreateArithmeticImageFilter(
    const std::vector<scalar>& coefficients, bool enforcePMColor,
    const std::shared_ptr<ImageFilter>& background, const std::shared_ptr<ImageFilter>& foreground)
{
    auto imageFilter = std::make_shared<RecordingImageFilter>();

    auto coefficientsData = CmdListHelper::AddVectorToCmdList<scalar>(*imageFilter->GetCmdList(), coefficients);
    auto backgroundHandle = CmdListHelper::AddRecordedToCmdList(
        *imageFilter->GetCmdList(), std::static_pointer_cast<RecordingImageFilter>(background));
    auto foregroundHandle = CmdListHelper::AddRecordedToCmdList(
        *imageFilter->GetCmdList(), std::static_pointer_cast<RecordingImageFilter>(foreground));

    imageFilter->GetCmdList()->AddOp<CreateArithmeticImageFilterOpItem>(
        coefficientsData, enforcePMColor, backgroundHandle, foregroundHandle);
    return imageFilter;
}

std::shared_ptr<RecordingImageFilter> RecordingImageFilter::CreateComposeImageFilter(
    const std::shared_ptr<ImageFilter>& f1, const std::shared_ptr<ImageFilter>& f2)
{
    auto imageFilter = std::make_shared<RecordingImageFilter>();

    auto imageFilterHandle1 = CmdListHelper::AddRecordedToCmdList(
        *imageFilter->GetCmdList(), std::static_pointer_cast<RecordingImageFilter>(f1));

    auto imageFilterHandle2 = CmdListHelper::AddRecordedToCmdList(
        *imageFilter->GetCmdList(), std::static_pointer_cast<RecordingImageFilter>(f2));

    imageFilter->GetCmdList()->AddOp<CreateComposeImageFilterOpItem>(imageFilterHandle1, imageFilterHandle2);
    return imageFilter;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
