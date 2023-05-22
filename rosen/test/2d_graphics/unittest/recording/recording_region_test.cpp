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

#include "recording/recording_path.h"
#include "recording/recording_region.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingRegionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RecordingRegionTest::SetUpTestCase() {}
void RecordingRegionTest::TearDownTestCase() {}
void RecordingRegionTest::SetUp() {}
void RecordingRegionTest::TearDown() {}

/**
 * @tc.name: SetRectTest001
 * @tc.desc: Test the playback of the SetRect function.
 * @tc.type: FUNC
 * @tc.require: I76WW5
 */
HWTEST_F(RecordingRegionTest, SetRectTest001, TestSize.Level1)
{
    std::unique_ptr<RecordingRegion> recordingRegion = std::make_unique<RecordingRegion>();
    ASSERT_TRUE(recordingRegion != nullptr);
    RectI rectI;
    EXPECT_TRUE(recordingRegion->SetRect(rectI));
    auto regionCmdList = recordingRegion->GetCmdList();
    EXPECT_TRUE(regionCmdList != nullptr);
    Region region;
    regionCmdList->Playback(region);
}

/**
 * @tc.name: SetRectTest002
 * @tc.desc: Test the playback of the SetRect function.
 * @tc.type: FUNC
 * @tc.require: I76WW5
 */
HWTEST_F(RecordingRegionTest, SetRectTest002, TestSize.Level1)
{
    std::unique_ptr<RecordingRegion> recordingRegion = std::make_unique<RecordingRegion>();
    ASSERT_TRUE(recordingRegion != nullptr);
    RectI rectI(0, 0, 256, 256);
    EXPECT_TRUE(recordingRegion->SetRect(rectI));
    auto regionCmdList = recordingRegion->GetCmdList();
    EXPECT_TRUE(regionCmdList != nullptr);
    Region region;
    regionCmdList->Playback(region);
}

/**
 * @tc.name: SetPathTest001
 * @tc.desc: Test the playback of the SetPath function.
 * @tc.type: FUNC
 * @tc.require: I76WW5
 */
HWTEST_F(RecordingRegionTest, SetPathTest001, TestSize.Level1)
{
    std::unique_ptr<RecordingRegion> recordingRegion = std::make_unique<RecordingRegion>();
    ASSERT_TRUE(recordingRegion != nullptr);
    RecordingPath path;
    RecordingRegion clip;
    EXPECT_TRUE(recordingRegion->SetPath(path, clip));
    auto regionCmdList = recordingRegion->GetCmdList();
    EXPECT_TRUE(regionCmdList != nullptr);
    Region region;
    regionCmdList->Playback(region);
}

/**
 * @tc.name: SetPathTest002
 * @tc.desc: Test the playback of the SetPath function.
 * @tc.type: FUNC
 * @tc.require: I76WW5
 */
HWTEST_F(RecordingRegionTest, SetPathTest002, TestSize.Level1)
{
    std::unique_ptr<RecordingRegion> recordingRegion = std::make_unique<RecordingRegion>();
    ASSERT_TRUE(recordingRegion != nullptr);
    RecordingPath path;
    path.AddRect(1.0f, 4.0f, 3.0f, 2.0f, PathDirection::CCW_DIRECTION);
    RecordingRegion clip;
    EXPECT_TRUE(recordingRegion->SetPath(path, clip));
    auto regionCmdList = recordingRegion->GetCmdList();
    EXPECT_TRUE(regionCmdList != nullptr);
    Region region;
    regionCmdList->Playback(region);
}

/**
 * @tc.name: OpTest001
 * @tc.desc: Test the playback of the Op function.
 * @tc.type: FUNC
 * @tc.require: I76WW5
 */
HWTEST_F(RecordingRegionTest, OpTest001, TestSize.Level1)
{
    std::unique_ptr<RecordingRegion> recordingRegion = std::make_unique<RecordingRegion>();
    ASSERT_TRUE(recordingRegion != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 400, 500, 600);
    recordingRegion->SetRect(rectI);
    RecordingRegion other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(recordingRegion->Op(other, RegionOp::DIFFERENCE));
    auto regionCmdList = recordingRegion->GetCmdList();
    EXPECT_TRUE(regionCmdList != nullptr);
    Region region;
    regionCmdList->Playback(region);
}

/**
 * @tc.name: OpTest002
 * @tc.desc: Test the playback of the Op function.
 * @tc.type: FUNC
 * @tc.require: I76WW5
 */
HWTEST_F(RecordingRegionTest, OpTest002, TestSize.Level1)
{
    std::unique_ptr<RecordingRegion> recordingRegion = std::make_unique<RecordingRegion>();
    ASSERT_TRUE(recordingRegion != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 0, 256, 256);
    recordingRegion->SetRect(rectI);
    RecordingRegion other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(recordingRegion->Op(other, RegionOp::INTERSECT));
    auto regionCmdList = recordingRegion->GetCmdList();
    EXPECT_TRUE(regionCmdList != nullptr);
    Region region;
    regionCmdList->Playback(region);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
