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

#include "pipeline/rs_render_frame_rate_linker_map.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderFrameRateLinkerMapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderFrameRateLinkerMapTest::SetUpTestCase() {}
void RSRenderFrameRateLinkerMapTest::TearDownTestCase() {}
void RSRenderFrameRateLinkerMapTest::SetUp() {}
void RSRenderFrameRateLinkerMapTest::TearDown() {}

/**
 * @tc.name: RegisterFrameRateLinker
 * @tc.desc: Test RegisterFrameRateLinker
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerMapTest, RegisterFrameRateLinker, TestSize.Level1)
{
    RSRenderFrameRateLinkerMap frameRateLinkerMap;
    FrameRateLinkerId id = 1;
    auto frameRateLinker = std::make_shared<RSRenderFrameRateLinker>(id);
    ASSERT_NE(frameRateLinker, nullptr);
    frameRateLinkerMap.RegisterFrameRateLinker(frameRateLinker);
    EXPECT_EQ(frameRateLinkerMap.GetFrameRateLinker(id), frameRateLinker);
    EXPECT_EQ(frameRateLinkerMap.Get().size(), 1);
}

/**
 * @tc.name: UnregisterFrameRateLinker
 * @tc.desc: Test UnregisterFrameRateLinker
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerMapTest, UnregisterFrameRateLinker, TestSize.Level1)
{
    RSRenderFrameRateLinkerMap frameRateLinkerMap;
    FrameRateLinkerId id = 1;
    auto frameRateLinker = std::make_shared<RSRenderFrameRateLinker>(id);
    ASSERT_NE(frameRateLinker, nullptr);
    frameRateLinkerMap.RegisterFrameRateLinker(frameRateLinker);
    EXPECT_EQ(frameRateLinkerMap.Get().size(), 1);
    frameRateLinkerMap.UnregisterFrameRateLinker(id);
    EXPECT_EQ(frameRateLinkerMap.GetFrameRateLinker(id), nullptr);
}

/**
 * @tc.name: FilterFrameRateLinkerByPid
 * @tc.desc: Test FilterFrameRateLinkerByPid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerMapTest, FilterFrameRateLinkerByPid, TestSize.Level1)
{
    RSRenderFrameRateLinkerMap frameRateLinkerMap;
    FrameRateLinkerId id = 1;
    auto frameRateLinker = std::make_shared<RSRenderFrameRateLinker>(id);
    ASSERT_NE(frameRateLinker, nullptr);
    frameRateLinkerMap.RegisterFrameRateLinker(frameRateLinker);
    EXPECT_EQ(frameRateLinkerMap.GetFrameRateLinker(id), frameRateLinker);
    frameRateLinkerMap.FilterFrameRateLinkerByPid(ExtractPid(id));
    EXPECT_EQ(frameRateLinkerMap.GetFrameRateLinker(id), nullptr);
}

/**
 * @tc.name: GetFrameRateLinker
 * @tc.desc: Test GetFrameRateLinker
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerMapTest, GetFrameRateLinker, TestSize.Level1)
{
    RSRenderFrameRateLinkerMap frameRateLinkerMap;
    FrameRateLinkerId id1 = 1;
    auto frameRateLinker = std::make_shared<RSRenderFrameRateLinker>(id1);
    ASSERT_NE(frameRateLinker, nullptr);
    frameRateLinkerMap.RegisterFrameRateLinker(frameRateLinker);
    EXPECT_EQ(frameRateLinkerMap.GetFrameRateLinker(id1), frameRateLinker);
    FrameRateLinkerId id2 = 2;
    EXPECT_EQ(frameRateLinkerMap.GetFrameRateLinker(id2), nullptr);
}
} // namespace OHOS::Rosen