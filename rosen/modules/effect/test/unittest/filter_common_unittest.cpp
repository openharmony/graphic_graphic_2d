/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "filter_common_unittest.h"

#include "effect_errors.h"
#include "filter_common.h"
#include "image_source.h"
#include "pixel_map.h"
#include "test_picture_files.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Rosen {

std::unique_ptr<PixelMap> CreatePixelMap()
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    if (buffer == nullptr) {
        return nullptr;
    }

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    if ((errorCode != SUCCESS) || (imageSource == nullptr)) {
        return nullptr;
    }

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    if ((errorCode != SUCCESS) || (pixmap == nullptr)) {
        return nullptr;
    }

    return pixmap;
}

/**
 * @tc.name: FilterCommon_CreateEffect
 * @tc.desc: Create a effect filter with pixelmap.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(FilterCommonUnittest, FilterCommon_CreateEffect001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterCommonUnittest FilterCommon_CreateEffect001 start";
    std::unique_ptr<PixelMap> pixMap = CreatePixelMap();
    ASSERT_TRUE(pixMap != nullptr);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<FilterCommon> filter = FilterCommon::CreateEffect(nullptr, errorCode);
    ASSERT_TRUE(errorCode != SUCCESS);
    ASSERT_TRUE(filter == nullptr);
    errorCode = SUCCESS;
    filter = FilterCommon::CreateEffect(std::move(pixMap), errorCode);
    ASSERT_TRUE(errorCode == SUCCESS);
    ASSERT_TRUE(filter != nullptr);
}

/**
 * @tc.name: FilterCommon_Invert
 * @tc.desc: Create a invert effect filter.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(FilterCommonUnittest, FilterCommon_Invert001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterCommonUnittest FilterCommon_Filter_Invert001 start";
    std::unique_ptr<PixelMap> pixMap = CreatePixelMap();
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<FilterCommon> filter = FilterCommon::CreateEffect(std::move(pixMap), errorCode);
    ASSERT_TRUE(errorCode == SUCCESS);
    ASSERT_TRUE(filter != nullptr);
    ASSERT_TRUE(filter->Invert() == true);
}

/**
 * @tc.name: FilterCommon_Blur
 * @tc.desc: Create a  blur effect filter with pixelmap.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(FilterCommonUnittest, FilterCommon_Blur001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterCommonUnittest FilterCommon_Filter_Blur001 start";
    std::unique_ptr<PixelMap> pixMap = CreatePixelMap();
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<FilterCommon> filter = FilterCommon::CreateEffect(std::move(pixMap), errorCode);
    ASSERT_TRUE(errorCode == SUCCESS);
    ASSERT_TRUE(filter != nullptr);
    ASSERT_TRUE(filter->Blur(5.0f) == true);
}

/**
 * @tc.name: FilterCommon_Grayscale
 * @tc.desc: Create a gray scale effect filter with pixelmap.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(FilterCommonUnittest, FilterCommon_Grayscale001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterCommonUnittest FilterCommon_Grayscale001 start";
    std::unique_ptr<PixelMap> pixMap = CreatePixelMap();
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<FilterCommon> filter = FilterCommon::CreateEffect(std::move(pixMap), errorCode);
    ASSERT_TRUE(errorCode == SUCCESS);
    ASSERT_TRUE(filter != nullptr);
    ASSERT_TRUE(filter->Grayscale() == true);
}

/**
 * @tc.name: FilterCommon_Brighten
 * @tc.desc: Create a brighten effect filter with pixelmap.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(FilterCommonUnittest, FilterCommon_Brighten001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterCommonUnittest FilterCommon_Brighten001 start";
    std::unique_ptr<PixelMap> pixMap = CreatePixelMap();
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<FilterCommon> filter = FilterCommon::CreateEffect(std::move(pixMap), errorCode);
    ASSERT_TRUE(errorCode == SUCCESS);
    ASSERT_TRUE(filter != nullptr);
    ASSERT_TRUE(filter->Brightness(0.5f) == true);
}

/**
 * @tc.name: FilterCommon_SetColorMatrix
 * @tc.desc: Create a effect filter with pixelmap.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(FilterCommonUnittest, OH_Filter_SetColorMatrix001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterCommonUnittest FilterCommon_SetColorMatrix001 start";
    std::unique_ptr<PixelMap> pixMap = CreatePixelMap();
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<FilterCommon> filter = FilterCommon::CreateEffect(std::move(pixMap), errorCode);
    ASSERT_TRUE(errorCode == SUCCESS);
    ASSERT_TRUE(filter != nullptr);
    std::vector<float> matrix { -1.0, 0, 0, 0, 1, 0, -1.0, 0, 0, 1, 0, 0, -1.0, 0, 1, 0, 0, 0, 1, 0 };
    ASSERT_TRUE(filter->SetColorMatrix(matrix, errorCode) == true);
    ASSERT_TRUE(errorCode == SUCCESS);
}

/**
 * @tc.name: FilterCommon_GetEffectPixelMap
 * @tc.desc: Get a pixelmap created by the filter.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(FilterCommonUnittest, FilterCommon_GetEffectPixelMap001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterCommonUnittest FilterCommon_Filter_Invert001 start";
    std::unique_ptr<PixelMap> pixMap = CreatePixelMap();
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<FilterCommon> filter = FilterCommon::CreateEffect(std::move(pixMap), errorCode);
    ASSERT_TRUE(errorCode == SUCCESS);
    ASSERT_TRUE(filter != nullptr);
    ASSERT_TRUE(filter->Invert() == true);
    ASSERT_TRUE(filter->GetEffectPixelMap() != nullptr);
}
} // namespace Rosen
} // namespace OHOS
