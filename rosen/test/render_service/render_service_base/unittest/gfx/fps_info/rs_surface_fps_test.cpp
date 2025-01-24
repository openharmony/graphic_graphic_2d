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

#include "gtest/gtest.h"
#include "gfx/fps_info/rs_surface_fps.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceFpsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceFpsTest::SetUpTestCase() {}
void RSSurfaceFpsTest::TearDownTestCase() {}
void RSSurfaceFpsTest::SetUp() {}
void RSSurfaceFpsTest::TearDown() {}

/**
 * @tc.name: RecordPresentTime
 * @tc.desc: test results of RecordPresentTime
 * @tc.type:FUNC
 * @tc.require: IBE7GI
 */
HWTEST_F(RSSurfaceFpsTest, RecordPresentTime, TestSize.Level1)
{
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    uint32_t seqNum = 0;
    RSSurfaceFps surfaceFps("surfacefps");
    EXPECT_FALSE(surfaceFps.RecordPresentTime(timestamp, seqNum));
    seqNum = 1;
    EXPECT_TRUE(surfaceFps.RecordPresentTime(timestamp, seqNum));
}

/**
 * @tc.name: DumpAndClearDump
 * @tc.desc: test results of DumpAndClearDump
 * @tc.type:FUNC
 * @tc.require: IBE7GI
 */
HWTEST_F(RSSurfaceFpsTest, DumpAndClearDump, TestSize.Level1)
{
    RSSurfaceFps surfaceFps("surfacefps");
    std::string result("");
    surfaceFps.Dump(result);
    EXPECT_TRUE(result.length() != 0);
    surfaceFps.ClearDump();
}
}