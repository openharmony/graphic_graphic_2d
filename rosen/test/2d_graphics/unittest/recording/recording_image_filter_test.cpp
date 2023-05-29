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

#include "gtest/gtest.h"

#include <vector>

#include "recording/recording_image_filter.h"
#include "recording/recording_color_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingImageFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RecordingImageFilterTest::SetUpTestCase() {}
void RecordingImageFilterTest::TearDownTestCase() {}
void RecordingImageFilterTest::SetUp() {}
void RecordingImageFilterTest::TearDown() {}

/*
 * @tc.name: CreateBlurImageFilterTest001
 * @tc.desc: Test the playback of the CreateBlurImageFilter function.
 * @tc.type: FUNC
 * @tc.require: I72DAE
 */
HWTEST_F(RecordingImageFilterTest, CreateBlurImageFilterTest001, TestSize.Level1)
{
    auto recordingImageFilter = RecordingImageFilter::CreateBlurImageFilter(10.0f, 10.0f, TileMode::CLAMP, nullptr);
    EXPECT_TRUE(recordingImageFilter != nullptr);
    auto imageFilterCmdList = recordingImageFilter->GetCmdList();
    EXPECT_TRUE(imageFilterCmdList != nullptr);
    auto imageFilter = imageFilterCmdList->Playback();
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateColorFilterImageFilterTest001
 * @tc.desc: Test the playback of the CreateColorFilterImageFilter function.
 * @tc.type: FUNC
 * @tc.require: I72DAE
 */
HWTEST_F(RecordingImageFilterTest, CreateColorFilterImageFilterTest001, TestSize.Level1)
{
    auto recordingColorFilter =
        RecordingColorFilter::CreateBlendModeColorFilter(11, OHOS::Rosen::Drawing::BlendMode::CLEAR);
    auto recordingImageFilter = RecordingImageFilter::CreateColorFilterImageFilter(*recordingColorFilter, nullptr);
    EXPECT_TRUE(recordingImageFilter != nullptr);
    auto imageFilterCmdList = recordingImageFilter->GetCmdList();
    EXPECT_TRUE(imageFilterCmdList != nullptr);
    auto imageFilter = imageFilterCmdList->Playback();
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateOffsetImageFilterTest001
 * @tc.desc: Test the playback of the CreateOffsetImageFilter function.
 * @tc.type: FUNC
 * @tc.require: I72DAE
 */
HWTEST_F(RecordingImageFilterTest, CreateOffsetImageFilterTest001, TestSize.Level1)
{
    auto recordingImageFilter = RecordingImageFilter::CreateOffsetImageFilter(10.0f, 10.0f, nullptr);
    EXPECT_TRUE(recordingImageFilter != nullptr);
    auto imageFilterCmdList = recordingImageFilter->GetCmdList();
    EXPECT_TRUE(imageFilterCmdList != nullptr);
    auto imageFilter = imageFilterCmdList->Playback();
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateArithmeticImageFilterTest001
 * @tc.desc: Test the playback of the CreateArithmeticImageFilter function.
 * @tc.type: FUNC
 * @tc.require: I72DAE
 */
HWTEST_F(RecordingImageFilterTest, CreateArithmeticImageFilterTest001, TestSize.Level1)
{
    std::vector<scalar> coefficients;
    coefficients.push_back(10.0f);
    coefficients.push_back(10.0f);
    coefficients.push_back(10.0f);
    coefficients.push_back(10.0f);
    auto recordingImageFilter = RecordingImageFilter::CreateArithmeticImageFilter(coefficients, true, nullptr, nullptr);
    EXPECT_TRUE(recordingImageFilter != nullptr);
    auto imageFilterCmdList = recordingImageFilter->GetCmdList();
    EXPECT_TRUE(imageFilterCmdList != nullptr);
    auto imageFilter = imageFilterCmdList->Playback();
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateComposeImageFilterTest001
 * @tc.desc: Test the playback of the CreateComposeImageFilter function.
 * @tc.type: FUNC
 * @tc.require: I72DAE
 */
HWTEST_F(RecordingImageFilterTest, CreateComposeImageFilterTest001, TestSize.Level1)
{
    auto recordingImageFilter1 = RecordingImageFilter::CreateBlurImageFilter(10.0f, 20.0f, TileMode::CLAMP, nullptr);
    auto recordingImageFilter2 = RecordingImageFilter::CreateOffsetImageFilter(30.0f, 40.0f, nullptr);
    auto recordingImageFilter =
        RecordingImageFilter::CreateComposeImageFilter(recordingImageFilter1, recordingImageFilter2);
    EXPECT_TRUE(recordingImageFilter != nullptr);
    auto imageFilterCmdList = recordingImageFilter->GetCmdList();
    EXPECT_TRUE(imageFilterCmdList != nullptr);
    auto imageFilter = imageFilterCmdList->Playback();
    EXPECT_TRUE(imageFilter != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS