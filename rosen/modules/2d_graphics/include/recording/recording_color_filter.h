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

#ifndef RECORDING_COLOR_FILTER_H
#define RECORDING_COLOR_FILTER_H

#include "effect/color_filter.h"
#include "recording/color_filter_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API RecordingColorFilter : public ColorFilter {
public:
    /*
     * @brief       Creates a CreateBlendModeColorFilterOpItem to add to the ColorFilterCmdList.
     * @param c     32-bits ARGB color value.
     * @param mode  The mode of blend.
     */
    static std::shared_ptr<RecordingColorFilter> CreateBlendModeColorFilter(ColorQuad c, BlendMode mode);

    /*
     * @brief     Creates a CreateComposeColorFilterOpItem to add to the ColorFilterCmdList.
     * @param f1  A ColorFilter to combine.
     * @param f2  Another ColorFilter to combine.
     */
    static std::shared_ptr<RecordingColorFilter> CreateComposeColorFilter(const ColorFilter& f1, const ColorFilter& f2);

    /*
     * @brief    Creates a CreateMatrixColorFilterOpItem to add to the ColorFilterCmdList.
     * @param m  Initialize from the ColorMatrix.
     */
    static std::shared_ptr<RecordingColorFilter> CreateMatrixColorFilter(const ColorMatrix& m);

    /*
     * @brief  Creates a CreateLinearToSrgbGamaOpItem to add to the ColorFilterCmdList.
     */
    static std::shared_ptr<RecordingColorFilter> CreateLinearToSrgbGamma();

    /*
     * @brief  Creates a CreateSrgbGammaToLinearOpItem to add to the ColorFilterCmdList.
     */
    static std::shared_ptr<RecordingColorFilter> CreateSrgbGammaToLinear();

    /*
     * @brief  Creates a CreateLumaColorFilterOpItem to add to the ColorFilterCmdList.
     */
    static std::shared_ptr<RecordingColorFilter> CreateLumaColorFilter();

    RecordingColorFilter() noexcept;
    ~RecordingColorFilter() override = default;

    DrawingType GetDrawingType() const override
    {
        return DrawingType::RECORDING;
    }

    void Compose(const ColorFilter& filter) override;

    /*
     * @brief  Gets the pointer to the ColorFilterCmdList.
     */
    std::shared_ptr<ColorFilterCmdList> GetCmdList() const
    {
        return cmdList_;
    }

private:
    std::shared_ptr<ColorFilterCmdList> cmdList_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
