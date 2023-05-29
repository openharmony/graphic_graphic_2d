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

#ifndef RECORDING_COLOR_SPACE_H
#define RECORDING_COLOR_SPACE_H

#include "effect/color_space.h"
#include "recording/color_space_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingColorSpace : public ColorSpace {
public:

    /*
     * @brief  Creates a CreateSRGBOpItem to add to the ColorSpaceCmdList.
     */
    static std::shared_ptr<RecordingColorSpace> CreateSRGB();

    /*
     * @brief  Creates a CreateSRGBLinearOpItem to add to the ColorSpaceCmdList.
     */
    static std::shared_ptr<RecordingColorSpace> CreateSRGBLinear();

    /*
     * @brief        Creates a CreateRefImageOpItem to add to the ColorSpaceCmdList.
     * @param image  The range of colors.
     */
    static std::shared_ptr<RecordingColorSpace> CreateRefImage(const Image& image);

    /*
     * @brief         Creates a CreateRGBOpItem to add to the ColorSpaceCmdList.
     * @param func    The type of transfer function.
     * @param matrix  The type of transfer matrix.
     */
    static std::shared_ptr<RecordingColorSpace> CreateRGB(const CMSTransferFuncType& func, const CMSMatrixType& matrix);

    RecordingColorSpace() noexcept;
    ~RecordingColorSpace() override = default;

    /*
     * @brief  Gets the pointer to the ColorSpaceCmdList.
     */
    std::shared_ptr<ColorSpaceCmdList> GetCmdList() const
    {
        return cmdList_;
    }

private:
    std::shared_ptr<ColorSpaceCmdList> cmdList_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
