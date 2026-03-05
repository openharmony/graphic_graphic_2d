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
#include "feature/hyper_graphic_manager/rs_render_frame_rate_linker_map.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint32_t MAX_FRAME_RATE_LINKER_SIZE = 2048;

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
    void OnFrameRateLinkerExpectedFpsUpdate(
        pid_t dstPid, const std::string& xcomponentId, int32_t expectedFps) override {};
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

    for (FrameRateLinkerId linkId = 2; linkId <= MAX_FRAME_RATE_LINKER_SIZE; linkId++) {
        auto frameRateLinker2 = std::make_shared<RSRenderFrameRateLinker>(linkId);
        EXPECT_TRUE(frameRateLinkerMap.RegisterFrameRateLinker(frameRateLinker2));
    }
    EXPECT_EQ(frameRateLinkerMap.Get().size(), MAX_FRAME_RATE_LINKER_SIZE);
    auto frameRateLinker3 = std::make_shared<RSRenderFrameRateLinker>(MAX_FRAME_RATE_LINKER_SIZE + 1);
    EXPECT_FALSE(frameRateLinkerMap.RegisterFrameRateLinker(frameRateLinker3));
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
 * @tc.name: RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest001
 * @tc.desc: test RegisterFrameRateLinkerExpectedFpsUpdateCallback when ExtractPid(id) != dstPid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerMapTest, RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest001, TestSize.Level1)
{
    RSRenderFrameRateLinkerMap frameRateLinkerMap;
    FrameRateLinkerId id1 = 1;
    auto frameRateLinker = std::make_shared<RSRenderFrameRateLinker>(id1);
    ASSERT_NE(frameRateLinker, nullptr);

    frameRateLinkerMap.RegisterFrameRateLinker(frameRateLinker);
    EXPECT_EQ(frameRateLinkerMap.GetFrameRateLinker(id1), frameRateLinker);

    EXPECT_FALSE(frameRateLinkerMap.RegisterFrameRateLinkerExpectedFpsUpdateCallback(1, 1, nullptr));
}

/**
 * @tc.name: RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest002
 * @tc.desc: test RegisterFrameRateLinkerExpectedFpsUpdateCallback when linker == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerMapTest, RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest002, TestSize.Level1)
{
    RSRenderFrameRateLinkerMap frameRateLinkerMap;
    FrameRateLinkerId id1 = 1;
    auto [iter, _] = frameRateLinkerMap.frameRateLinkerMap_.emplace(id1, nullptr);
    ASSERT_EQ(iter->second, nullptr);

    EXPECT_FALSE(
        frameRateLinkerMap.RegisterFrameRateLinkerExpectedFpsUpdateCallback(1, static_cast<int32_t>(id1), nullptr));
}

/**
 * @tc.name: RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest003
 * @tc.desc: test RegisterFrameRateLinkerExpectedFpsUpdateCallback when success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerMapTest, RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest003, TestSize.Level1)
{
    RSRenderFrameRateLinkerMap frameRateLinkerMap;
    FrameRateLinkerId id1 = 0;
    auto frameRateLinker = std::make_shared<RSRenderFrameRateLinker>(id1);
    ASSERT_NE(frameRateLinker, nullptr);

    frameRateLinkerMap.RegisterFrameRateLinker(frameRateLinker);
    EXPECT_EQ(frameRateLinkerMap.GetFrameRateLinker(id1), frameRateLinker);

    EXPECT_TRUE(
        frameRateLinkerMap.RegisterFrameRateLinkerExpectedFpsUpdateCallback(1, static_cast<int32_t>(id1), nullptr));
}

/**
 * @tc.name: UnRegisterExpectedFpsUpdateCallbackByListenerTest001
 * @tc.desc: test UnRegisterExpectedFpsUpdateCallbackByListener when linker is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerMapTest, UnRegisterExpectedFpsUpdateCallbackByListenerTest001, TestSize.Level1)
{
    RSRenderFrameRateLinkerMap frameRateLinkerMap;
    FrameRateLinkerId id1 = 1;
    auto [iter, _] = frameRateLinkerMap.frameRateLinkerMap_.emplace(id1, nullptr);
    ASSERT_EQ(iter->second, nullptr);

    frameRateLinkerMap.UnRegisterExpectedFpsUpdateCallbackByListener(1);
}

/**
 * @tc.name: UnRegisterExpectedFpsUpdateCallbackByListenerTest002
 * @tc.desc: test UnRegisterExpectedFpsUpdateCallbackByListener when linker is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerMapTest, UnRegisterExpectedFpsUpdateCallbackByListenerTest002, TestSize.Level1)
{
    RSRenderFrameRateLinkerMap frameRateLinkerMap;
    FrameRateLinkerId id1 = 1;
    auto frameRateLinker = std::make_shared<RSRenderFrameRateLinker>(id1);
    ASSERT_NE(frameRateLinker, nullptr);

    frameRateLinkerMap.RegisterFrameRateLinker(frameRateLinker);
    EXPECT_EQ(frameRateLinkerMap.GetFrameRateLinker(id1), frameRateLinker);

    pid_t listenerPid = 1;
    auto cb = sptr<CustomFrameRateLinkerCallback>::MakeSptr();
    EXPECT_TRUE(frameRateLinkerMap.RegisterFrameRateLinkerExpectedFpsUpdateCallback(
        listenerPid, static_cast<int32_t>(id1), cb));
    EXPECT_NE(frameRateLinker->expectedFpsChangeCallbacks_.find(listenerPid),
        frameRateLinker->expectedFpsChangeCallbacks_.end());

    frameRateLinkerMap.UnRegisterExpectedFpsUpdateCallbackByListener(listenerPid);
    EXPECT_EQ(frameRateLinker->expectedFpsChangeCallbacks_.find(listenerPid),
        frameRateLinker->expectedFpsChangeCallbacks_.end());
}

/**
 * @tc.name: UpdateFrameRateLinkersTest001
 * @tc.desc: test UpdateFrameRateLinkers when linker is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerMapTest, UpdateFrameRateLinkersTest001, TestSize.Level1)
{
    RSRenderFrameRateLinkerMap frameRateLinkerMap;
    FrameRateLinkerId id1 = 1;
    FrameRateLinkerUpdateInfo info;
    FrameRateLinkerUpdateInfoMap map {
        { id1, info },
    };
    ASSERT_EQ(frameRateLinkerMap.GetFrameRateLinker(id1), nullptr);

    frameRateLinkerMap.UpdateFrameRateLinkers(map);
}

/**
 * @tc.name: UpdateFrameRateLinkersTest002
 * @tc.desc: test UpdateFrameRateLinkers when linker is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderFrameRateLinkerMapTest, UpdateFrameRateLinkersTest002, TestSize.Level1)
{
    RSRenderFrameRateLinkerMap frameRateLinkerMap;
    FrameRateLinkerId id1 = 1;
    auto frameRateLinker = std::make_shared<RSRenderFrameRateLinker>(id1);
    ASSERT_NE(frameRateLinker, nullptr);

    frameRateLinkerMap.RegisterFrameRateLinker(frameRateLinker);
    EXPECT_EQ(frameRateLinkerMap.GetFrameRateLinker(id1), frameRateLinker);

    FrameRateRange range;
    int32_t animatorExpectedFrameRate = 120;
    FrameRateLinkerUpdateInfo info { range, animatorExpectedFrameRate };
    FrameRateLinkerUpdateInfoMap map {
        { id1, info },
    };
    frameRateLinkerMap.UpdateFrameRateLinkers(map);
    EXPECT_EQ(frameRateLinker->animatorExpectedFrameRate_, animatorExpectedFrameRate);
}
} // namespace OHOS::Rosen