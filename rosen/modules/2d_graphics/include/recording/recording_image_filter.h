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

#ifndef RECORDING_IMAGE_FILTER_H
#define RECORDING_IMAGE_FILTER_H

#include "effect/image_filter.h"
#include "recording/image_filter_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API RecordingImageFilter : public ImageFilter {
public:
    /*
     * @brief         Creates a CreateBlurImageFilterOpItem to add to the ImageFilterCmdList.
     * @param sigmaX  The Gaussian sigma value of X axis.
     * @param sigmaY  The Gaussian sigma value of Y axis.
     * @param input   The input filter. If null, will use source bitmap.
     * @param mode    The mode of tiling applied at edges.
     */
    static std::shared_ptr<RecordingImageFilter> CreateBlurImageFilter(scalar sigmaX, scalar sigmaY,
        TileMode mode, const std::shared_ptr<ImageFilter>& input);

    /*
     * @brief        Creates a CreateColorFilterImageFilterOpItem to add to the ImageFilterCmdList.
     * @param cf     The color filter that transforms the input image.
     * @param input  The input filter. If null, will use source bitmap.
     */
    static std::shared_ptr<RecordingImageFilter> CreateColorFilterImageFilter(const ColorFilter& cf,
        const std::shared_ptr<ImageFilter>& input);

    /*
     * @brief        Creates a CreateOffsetImageFilterOpItem to add to the ImageFilterCmdList.
     * @param dx     The x offset of the image in local space.
     * @param dy     The y offset of the image in local space.
     * @param input  The input filter. If null, will use source bitmap.
     */
    static std::shared_ptr<RecordingImageFilter> CreateOffsetImageFilter(scalar dx, scalar dy,
        const std::shared_ptr<ImageFilter>& input);

    /*
     * @brief                 Creates a CreateArithmeticImageFilterOpItem to add to the ImageFilterCmdList.
     * @param coefficients    Get the four coefficients used to combine the foreground and background in the vector.
                              And The vector size must be four, otherwise the call fails.
     * @param enforcePMColor  If true, the RGB channels will be clamped to the Calculated alpha.
     * @param background      The Background content, using the source bitmap when this is null.
     * @param foreground      The foreground content, using the source bitmap when this is null.
     */
    static std::shared_ptr<RecordingImageFilter> CreateArithmeticImageFilter(
        const std::vector<scalar>& coefficients, bool enforcePMColor,
        const std::shared_ptr<ImageFilter>& background, const std::shared_ptr<ImageFilter>& foreground);

    /*
     * @brief     Creates a CreateComposeImageFilterOpItem to add to the ImageFilterCmdList.
     * @param f1  The outer filter that evaluates the results of inner.
     * @param f2  The inner filter that produces the input to outer.
     */
    static std::shared_ptr<RecordingImageFilter> CreateComposeImageFilter(const std::shared_ptr<ImageFilter>& f1,
        const std::shared_ptr<ImageFilter>& f2);

    RecordingImageFilter() noexcept;
    ~RecordingImageFilter() override = default;

    DrawingType GetDrawingType() const override
    {
        return DrawingType::RECORDING;
    }

    /*
     * @brief  Gets the pointer to the ImageFilterCmdList.
     */
    std::shared_ptr<ImageFilterCmdList> GetCmdList() const
    {
        return cmdList_;
    }

private:
    std::shared_ptr<ImageFilterCmdList> cmdList_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
