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

#include "recording/recording_mask_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingMaskFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RecordingMaskFilterTest::SetUpTestCase() {}
void RecordingMaskFilterTest::TearDownTestCase() {}
void RecordingMaskFilterTest::SetUp() {}
void RecordingMaskFilterTest::TearDown() {}

/**
 * @tc.name: CreateBlurMaskFilter001
 * @tc.desc: Test the playback of the CreateBlurMaskFilter function.
 * @tc.type: FUNC
 * @tc.require: I72DAE
 */
HWTEST_F(RecordingMaskFilterTest, CreateBlurMaskFilter001, TestSize.Level1)
{
    auto recordingMaskFilter = RecordingMaskFilter::CreateBlurMaskFilter(BlurType::NORMAL, 0.5f);
    EXPECT_TRUE(recordingMaskFilter != nullptr);
    auto maskFilterCmdList = recordingMaskFilter->GetCmdList();
    EXPECT_TRUE(maskFilterCmdList != nullptr);
    auto maskFilter = maskFilterCmdList->Playback();
    EXPECT_TRUE(maskFilter != nullptr);

    auto newCmdList = MaskFilterCmdList::CreateFromData(maskFilterCmdList->GetData(), true);
    EXPECT_TRUE(newCmdList != nullptr);
    maskFilter = newCmdList->Playback();
    EXPECT_TRUE(maskFilter != nullptr);

    newCmdList = MaskFilterCmdList::CreateFromData(maskFilterCmdList->GetData(), false);
    EXPECT_TRUE(newCmdList != nullptr);
    maskFilter = newCmdList->Playback();
    EXPECT_TRUE(maskFilter != nullptr);
}

/**
 * @tc.name: CreateBlurMaskFilter002
 * @tc.desc: Test the playback of the CreateBlurMaskFilter function.
 * @tc.type: FUNC
 * @tc.require: I72DAE
 */
HWTEST_F(RecordingMaskFilterTest, CreateBlurMaskFilter002, TestSize.Level1)
{
    auto recordingMaskFilter = RecordingMaskFilter::CreateBlurMaskFilter(BlurType::OUTER, 0.2f);
    EXPECT_TRUE(recordingMaskFilter != nullptr);
    auto maskFilterCmdList = recordingMaskFilter->GetCmdList();
    EXPECT_TRUE(maskFilterCmdList != nullptr);
    auto maskFilter = maskFilterCmdList->Playback();
    EXPECT_TRUE(maskFilter != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS