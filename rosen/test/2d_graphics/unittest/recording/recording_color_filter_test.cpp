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

#include "recording/recording_color_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingColorFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RecordingColorFilterTest::SetUpTestCase() {}
void RecordingColorFilterTest::TearDownTestCase() {}
void RecordingColorFilterTest::SetUp() {}
void RecordingColorFilterTest::TearDown() {}

/**
 * @tc.name: CreateBlendModeColorFilter001
 * @tc.desc: Test the playback of the CreateBlendModeColorFilter function.
 * @tc.type: FUNC
 * @tc.require:I77JNG
 */
HWTEST_F(RecordingColorFilterTest, CreateBlendModeColorFilter001, TestSize.Level1)
{
    auto recordingColorFilter =
        RecordingColorFilter::CreateBlendModeColorFilter(11, OHOS::Rosen::Drawing::BlendMode::CLEAR);
    EXPECT_TRUE(recordingColorFilter != nullptr);
    auto colorFilterCmdList = recordingColorFilter->GetCmdList();
    EXPECT_TRUE(colorFilterCmdList != nullptr);
    auto colorFilter = colorFilterCmdList->Playback();
    EXPECT_TRUE(colorFilter != nullptr);

    auto newCmdList = ColorFilterCmdList::CreateFromData(colorFilterCmdList->GetData(), true);
    EXPECT_TRUE(newCmdList != nullptr);
    colorFilter = newCmdList->Playback();
    EXPECT_TRUE(colorFilter != nullptr);
}

/**
 * @tc.name: CreateBlendModeColorFilter002
 * @tc.desc: Test the playback of the CreateBlendModeColorFilter function.
 * @tc.type: FUNC
 * @tc.require:I77JNG
 */
HWTEST_F(RecordingColorFilterTest, CreateBlendModeColorFilter002, TestSize.Level1)
{
    auto recordingColorFilter =
        RecordingColorFilter::CreateBlendModeColorFilter(10, OHOS::Rosen::Drawing::BlendMode::SRC);
    EXPECT_TRUE(recordingColorFilter != nullptr);
    auto colorFilterCmdList = recordingColorFilter->GetCmdList();
    EXPECT_TRUE(colorFilterCmdList != nullptr);
    auto colorFilter = colorFilterCmdList->Playback();
    EXPECT_TRUE(colorFilter != nullptr);
}

/**
 * @tc.name: CreateComposeColorFilter001
 * @tc.desc: Test the playback of the CreateComposeColorFilter function.
 * @tc.type: FUNC
 * @tc.require:I77JNG
 */
HWTEST_F(RecordingColorFilterTest, CreateComposeColorFilter001, TestSize.Level1)
{
    auto colorFilter1 = RecordingColorFilter::CreateBlendModeColorFilter(11, OHOS::Rosen::Drawing::BlendMode::CLEAR);
    auto colorFilter2 = RecordingColorFilter::CreateBlendModeColorFilter(10, OHOS::Rosen::Drawing::BlendMode::SRC);
    auto recordingColorFilter =
        RecordingColorFilter::CreateComposeColorFilter(*colorFilter1.get(), *colorFilter2.get());
    EXPECT_TRUE(recordingColorFilter != nullptr);
    auto colorFilterCmdList = recordingColorFilter->GetCmdList();
    EXPECT_TRUE(colorFilterCmdList != nullptr);
    auto colorFilter = colorFilterCmdList->Playback();
    EXPECT_TRUE(colorFilter != nullptr);
}

/**
 * @tc.name: CreateComposeColorFilter002
 * @tc.desc: Test the playback of the CreateComposeColorFilter function.
 * @tc.type: FUNC
 * @tc.require:I77JNG
 */
HWTEST_F(RecordingColorFilterTest, CreateComposeColorFilter002, TestSize.Level1)
{
    auto colorFilter1 = RecordingColorFilter::CreateBlendModeColorFilter(11, OHOS::Rosen::Drawing::BlendMode::CLEAR);
    auto colorFilter2 = RecordingColorFilter::CreateBlendModeColorFilter(10, OHOS::Rosen::Drawing::BlendMode::SRC);
    auto recordingColorFilter =
        RecordingColorFilter::CreateComposeColorFilter(*colorFilter1.get(), *colorFilter2.get());
    EXPECT_TRUE(recordingColorFilter != nullptr);
    auto colorFilterCmdList = recordingColorFilter->GetCmdList();
    EXPECT_TRUE(colorFilterCmdList != nullptr);
    auto colorFilter = colorFilterCmdList->Playback();
    EXPECT_TRUE(colorFilter != nullptr);
}

/**
 * @tc.name: CreateMatrixColorFilter001
 * @tc.desc: Test the playback of the CreateMatrixColorFilter function.
 * @tc.type: FUNC
 * @tc.require:I77JNG
 */
HWTEST_F(RecordingColorFilterTest, CreateMatrixColorFilter001, TestSize.Level1)
{
    ColorMatrix colorMatrix;
    auto recordingColorFilter = RecordingColorFilter::CreateMatrixColorFilter(colorMatrix);
    EXPECT_TRUE(recordingColorFilter != nullptr);
    auto colorFilterCmdList = recordingColorFilter->GetCmdList();
    EXPECT_TRUE(colorFilterCmdList != nullptr);
    auto colorFilter = colorFilterCmdList->Playback();
    EXPECT_TRUE(colorFilter != nullptr);
}

/**
 * @tc.name: CreateLinearToSrgbGamma001
 * @tc.desc: Test the playback of the CreateLinearToSrgbGamma function.
 * @tc.type: FUNC
 * @tc.require:I77JNG
 */
HWTEST_F(RecordingColorFilterTest, CreateLinearToSrgbGamma001, TestSize.Level1)
{
    auto recordingColorFilter = RecordingColorFilter::CreateLinearToSrgbGamma();
    EXPECT_TRUE(recordingColorFilter != nullptr);
    auto colorFilterCmdList = recordingColorFilter->GetCmdList();
    EXPECT_TRUE(colorFilterCmdList != nullptr);
    auto colorFilter = colorFilterCmdList->Playback();
    EXPECT_TRUE(colorFilter != nullptr);
}

/**
 * @tc.name: CreateSrgbGammaToLinear001
 * @tc.desc: Test the playback of the CreateSrgbGammaToLinear function.
 * @tc.type: FUNC
 * @tc.require:I77JNG
 */
HWTEST_F(RecordingColorFilterTest, CreateSrgbGammaToLinear001, TestSize.Level1)
{
    auto recordingColorFilter = RecordingColorFilter::CreateSrgbGammaToLinear();
    EXPECT_TRUE(recordingColorFilter != nullptr);
    auto colorFilterCmdList = recordingColorFilter->GetCmdList();
    EXPECT_TRUE(colorFilterCmdList != nullptr);
    auto colorFilter = colorFilterCmdList->Playback();
    EXPECT_TRUE(colorFilter != nullptr);
}

/**
 * @tc.name: CreateLumaColorFilter001
 * @tc.desc: Test the playback of the CreateLumaColorFilter function.
 * @tc.type: FUNC
 * @tc.require:I77JNG
 */
HWTEST_F(RecordingColorFilterTest, CreateLumaColorFilter001, TestSize.Level1)
{
    auto recordingColorFilter = RecordingColorFilter::CreateLumaColorFilter();
    EXPECT_TRUE(recordingColorFilter != nullptr);
    auto colorFilterCmdList = recordingColorFilter->GetCmdList();
    EXPECT_TRUE(colorFilterCmdList != nullptr);
    auto colorFilter = colorFilterCmdList->Playback();
    EXPECT_TRUE(colorFilter != nullptr);
}

/**
 * @tc.name: Compose001
 * @tc.desc: Test the playback of the Compose function.
 * @tc.type: FUNC
 * @tc.require:I77JNG
 */
HWTEST_F(RecordingColorFilterTest, Compose001, TestSize.Level1)
{
    auto blend = RecordingColorFilter::CreateBlendModeColorFilter(11, OHOS::Rosen::Drawing::BlendMode::CLEAR);
    EXPECT_TRUE(blend != nullptr);
    ColorMatrix colorMatrix;
    auto matrix = RecordingColorFilter::CreateMatrixColorFilter(colorMatrix);
    EXPECT_TRUE(matrix != nullptr);
    blend->Compose(*matrix.get());
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS