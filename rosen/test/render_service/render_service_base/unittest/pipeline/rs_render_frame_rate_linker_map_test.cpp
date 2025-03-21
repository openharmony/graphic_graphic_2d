/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include <iremote_stub.h>
#include "pipeline/rs_render_frame_rate_linker_map.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback.h"

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

class CustomFrameRateLinkerCallback : public IRemoteStub<RSIFrameRateLinkerExpectedFpsUpdateCallback> {
public:
    CustomFrameRateLinkerCallback() = default;
    ~CustomFrameRateLinkerCallback() = default;
    void OnFrameRateLinkerExpectedFpsUpdate(pid_t dstPid, int32_t expectedFps) override {};
};

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
    EXPECT_TRUE(frameRateLinkerMap.RegisterFrameRateLinker(frameRateLinker));
    EXPECT_FALSE(frameRateLinkerMap.RegisterFrameRateLinker(frameRateLinker));
    EXPECT_EQ(frameRateLinkerMap.GetFrameRateLinker(id), frameRateLinker);
    EXPECT_EQ(frameRateLinkerMap.Get().size(), 1);
}

/**
 * @tc.name: UnregisterFrameRateLinker
 * @tc.desc: Test UnregisterFrameRateLinker
 * @tc.type: FUNC
 * @tc.require: issueI9VAI2
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

/**
 * @tc.name: RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest
 * @tc.desc: Test RegisterFrameRateLinkerExpectedFpsUpdateCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerMapTest, RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest, TestSize.Level1)
{
    RSRenderFrameRateLinkerMap frameRateLinkerMap;
    FrameRateLinkerId linkerId = 0x100000000;
    auto frameRateLinker = std::make_shared<RSRenderFrameRateLinker>(linkerId);
    ASSERT_NE(frameRateLinker, nullptr);
    frameRateLinkerMap.RegisterFrameRateLinker(frameRateLinker);

    uint32_t listenerPid = 2;
    auto cb = new CustomFrameRateLinkerCallback();
    EXPECT_TRUE(
        frameRateLinkerMap.RegisterFrameRateLinkerExpectedFpsUpdateCallback(listenerPid, ExtractPid(linkerId), cb));
    frameRateLinkerMap.UnRegisterExpectedFpsUpdateCallbackByListener(listenerPid);
    frameRateLinkerMap.frameRateLinkerMap_.clear();
    EXPECT_FALSE(
        frameRateLinkerMap.RegisterFrameRateLinkerExpectedFpsUpdateCallback(listenerPid, ExtractPid(linkerId), cb));
}
} // namespace OHOS::Rosen