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

#include "recording/recording_color_space.h"

#include "image/image.h"
#include "recording/cmd_list_helper.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RecordingColorSpace::RecordingColorSpace() noexcept : cmdList_(std::make_shared<ColorSpaceCmdList>()) {}

std::shared_ptr<RecordingColorSpace> RecordingColorSpace::CreateSRGB()
{
    auto colorSpace = std::make_shared<RecordingColorSpace>();
    colorSpace->GetCmdList()->AddOp<CreateSRGBOpItem>();
    return colorSpace;
}

std::shared_ptr<RecordingColorSpace> RecordingColorSpace::CreateSRGBLinear()
{
    auto colorSpace = std::make_shared<RecordingColorSpace>();
    colorSpace->GetCmdList()->AddOp<CreateSRGBLinearOpItem>();
    return colorSpace;
}

std::shared_ptr<RecordingColorSpace> RecordingColorSpace::CreateRefImage(const Image& image)
{
    auto colorSpace = std::make_shared<RecordingColorSpace>();
    auto imageHandle = CmdListHelper::AddImageToCmdList(*colorSpace->GetCmdList(), image);

    colorSpace->GetCmdList()->AddOp<CreateRefImageOpItem>(imageHandle);
    return colorSpace;
}

std::shared_ptr<RecordingColorSpace> RecordingColorSpace::CreateRGB(
    const CMSTransferFuncType& func, const CMSMatrixType& matrix)
{
    auto colorSpace = std::make_shared<RecordingColorSpace>();
    colorSpace->GetCmdList()->AddOp<CreateRGBOpItem>(func, matrix);
    return colorSpace;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
