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

#include "recording/cmd_list_helper.h"
#include "utils/log.h"

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
    auto colorFilter = std::make_shared<RecordingColorFilter>();

    auto colorFilterHandle1 = CmdListHelper::AddRecordedToCmdList<RecordingColorFilter>(*colorFilter->GetCmdList(), f1);
    auto colorFilterHandle2 = CmdListHelper::AddRecordedToCmdList<RecordingColorFilter>(*colorFilter->GetCmdList(), f2);
    colorFilter->GetCmdList()->AddOp<CreateComposeOpItem>(colorFilterHandle1, colorFilterHandle2);
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
    auto colorFilterHandle = CmdListHelper::AddRecordedToCmdList<RecordingColorFilter>(*cmdList_, filter);
    cmdList_->AddOp<ColorFilterComposeOpItem>(colorFilterHandle);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
