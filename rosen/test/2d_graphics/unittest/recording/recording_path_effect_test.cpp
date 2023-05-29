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

#include "recording/recording_path.h"
#include "recording/recording_path_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingPathEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RecordingPathEffectTest::SetUpTestCase() {}
void RecordingPathEffectTest::TearDownTestCase() {}
void RecordingPathEffectTest::SetUp() {}
void RecordingPathEffectTest::TearDown() {}

/**
 * @tc.name: CreateDashPathEffect001
 * @tc.desc: Test the playback of the CreateDashPathEffect function.
 * @tc.type: FUNC
 * @tc.require: I76Q4L
 */
HWTEST_F(RecordingPathEffectTest, CreateDashPathEffect001, TestSize.Level1)
{
    std::vector<scalar> intervals;
    intervals.push_back(1.0f);
    intervals.push_back(2.0f);
    intervals.push_back(1.5f);
    intervals.push_back(3.0f);
    auto recordingPathEffect = RecordingPathEffect::CreateDashPathEffect(intervals, 2.2f);
    EXPECT_TRUE(recordingPathEffect != nullptr);
    auto pathEffectCmdList = recordingPathEffect->GetCmdList();
    EXPECT_TRUE(pathEffectCmdList != nullptr);
    auto pathEffect = pathEffectCmdList->Playback();
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreatePathDashEffect001
 * @tc.desc: Test the playback of the CreatePathDashEffect function.
 * @tc.type: FUNC
 * @tc.require: I76Q4L
 */
HWTEST_F(RecordingPathEffectTest, CreatePathDashEffect001, TestSize.Level1)
{
    RecordingPath path;
    auto recordingPathEffect = RecordingPathEffect::CreatePathDashEffect(path, 1.1f, 1.5f, PathDashStyle::TRANSLATE);
    EXPECT_TRUE(recordingPathEffect != nullptr);
    auto pathEffectCmdList = recordingPathEffect->GetCmdList();
    EXPECT_TRUE(pathEffectCmdList != nullptr);
    auto pathEffect = pathEffectCmdList->Playback();
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreatePathDashEffect002
 * @tc.desc: Test the playback of the CreatePathDashEffect function.
 * @tc.type: FUNC
 * @tc.require: I76Q4L
 */
HWTEST_F(RecordingPathEffectTest, CreatePathDashEffect002, TestSize.Level1)
{
    RecordingPath path;
    path.AddRect(1.0f, 4.0f, 3.0f, 2.0f, PathDirection::CCW_DIRECTION);
    auto recordingPathEffect = RecordingPathEffect::CreatePathDashEffect(path, 1.1f, 1.5f, PathDashStyle::TRANSLATE);
    EXPECT_TRUE(recordingPathEffect != nullptr);
    auto pathEffectCmdList = recordingPathEffect->GetCmdList();
    EXPECT_TRUE(pathEffectCmdList != nullptr);
    auto pathEffect = pathEffectCmdList->Playback();
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreateCornerPathEffect001
 * @tc.desc: Test the playback of the CreateCornerPathEffect function.
 * @tc.type: FUNC
 * @tc.require: I76Q4L
 */
HWTEST_F(RecordingPathEffectTest, CreateCornerPathEffect001, TestSize.Level1)
{
    auto recordingPathEffect = RecordingPathEffect::CreateCornerPathEffect(0.5f);
    EXPECT_TRUE(recordingPathEffect != nullptr);
    auto pathEffectCmdList = recordingPathEffect->GetCmdList();
    EXPECT_TRUE(pathEffectCmdList != nullptr);
    auto pathEffect = pathEffectCmdList->Playback();
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreateSumPathEffect001
 * @tc.desc: Test the playback of the CreateSumPathEffect function.
 * @tc.type: FUNC
 * @tc.require: I76Q4L
 */
HWTEST_F(RecordingPathEffectTest, CreateSumPathEffect001, TestSize.Level1)
{
    auto pathEffect1 = RecordingPathEffect::CreateCornerPathEffect(0.5f);
    auto pathEffect2 = RecordingPathEffect::CreateCornerPathEffect(0.2f);
    auto recordingPathEffect = RecordingPathEffect::CreateSumPathEffect(*pathEffect1.get(), *pathEffect2.get());
    EXPECT_TRUE(recordingPathEffect != nullptr);
    auto pathEffectCmdList = recordingPathEffect->GetCmdList();
    EXPECT_TRUE(pathEffectCmdList != nullptr);
    auto pathEffect = pathEffectCmdList->Playback();
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreateComposePathEffect001
 * @tc.desc: Test the playback of the CreateComposePathEffect function.
 * @tc.type: FUNC
 * @tc.require: I76Q4L
 */
HWTEST_F(RecordingPathEffectTest, CreateComposePathEffect001, TestSize.Level1)
{
    auto pathEffect1 = RecordingPathEffect::CreateCornerPathEffect(0.5f);
    auto pathEffect2 = RecordingPathEffect::CreateCornerPathEffect(0.2f);
    auto recordingPathEffect = RecordingPathEffect::CreateComposePathEffect(*pathEffect1.get(), *pathEffect2.get());
    EXPECT_TRUE(recordingPathEffect != nullptr);
    auto pathEffectCmdList = recordingPathEffect->GetCmdList();
    EXPECT_TRUE(pathEffectCmdList != nullptr);
    auto pathEffect = pathEffectCmdList->Playback();
    EXPECT_TRUE(pathEffect != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
