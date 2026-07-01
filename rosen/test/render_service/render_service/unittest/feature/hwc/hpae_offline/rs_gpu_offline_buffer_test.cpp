/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <memory>
#include <chrono>

#include "consumer_surface.h"
#include "feature/hwc/hpae_offline/rs_offline_processor.h"
#include "feature/hwc/hpae_offline/rs_gpu_offline_device.h"
#include "feature/hwc/hpae_offline/rs_gpu_offline_buffer.h"
#include "feature/hwc/hpae_offline/rs_gpu_offline_thread.h"
#include "feature/hwc/hpae_offline/rs_offline_device.h"
#include "feature/hwc/hpae_offline/rs_offline_util.h"
#include "feature/hwc/hpae_offline/rs_offline_result.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/rs_surface_render.node.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Rosen;

namespace {
constexpr NodeId TEST_NODE_ID = 12345;
}

class RSGPUOfflineBufferPrivateTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSGPUOfflineBufferPrivateTest::SetUpTestCase() {}
void RSGPUOfflineBufferPrivateTest::TearDownTestCase() {}
void RSGPUOfflineBufferPrivateTest::SetUp() {}
void RSGPUOfflineBufferPrivateTest::TearDown() {}

/**
 * @tc.name: RequestFrameSwitchTypeNoneSameConfigTest001
 * @tc.desc: Verify function RequestFrame
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineBufferPrivateTest, RequestFrameSwitchTypeNoneSameConfigTest001, Level1)
{
    auto buffer = std::make_shared<RSGPUOfflineBuffer>("test", TEST_NODE_ID);
    BufferRequestConfig config = {};
    config.width = 1920;
    config.height = 1080;
    auto renderEngine = std::make_shared<RSUniRenderEngine>();

    auto frame1 = buffer->RequestFrame(renderEngine, config, false, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
    EXPECT_EQ(frame1, nullptr);

    auto frame2 = buffer->RequestFrame(renderEngine, config, false, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
    EXPECT_EQ(frame2, nullptr);
    if (frame2) {
        frame2.reset();
    }
}

/**
 * @tc.name: RequestFrameSwitchTypeToSingleTriggersCleanCacheTest001
 * @tc.desc: Verify function RequestFrame
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineBufferPrivateTest, RequestFrameSwitchTypeToSingleTriggersCleanCacheTest001, Level1)
{
    auto buffer = std::make_shared<RSGPUOfflineBuffer>("test", TEST_NODE_ID);
    BufferRequestConfig config = {};
    config.width = 1920;
    config.height = 1080;
    auto renderEngine = std::make_shared<RSUniRenderEngine>();

    auto frame1 = buffer->RequestFrame(renderEngine, config, false, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
    EXPECT_EQ(frame1, nullptr);

    auto frame2 = buffer->RequestFrame(renderEngine, config, false, SingleBufferMode::SINGLE_BUFFER_MODE_TO_SINGLE);
    EXPECT_EQ(frame2, nullptr);
    if (frame2) {
        frame2.reset();
    }
}

/**
 * @tc.name: RequestFrameSwitchTypeToMultiTriggersCleanCacheTest001
 * @tc.desc: Verify function RequestFrame
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineBufferPrivateTest, RequestFrameSwitchTypeToMultiTriggersCleanCacheTest001, Level1)
{
    auto buffer = std::make_shared<RSGPUOfflineBuffer>("test", TEST_NODE_ID);
    BufferRequestConfig config = {};
    config.width = 1920;
    config.height = 1080;
    auto renderEngine = std::make_shared<RSUniRenderEngine>();

    auto frame1 = buffer->RequestFrame(renderEngine, config, false, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
    EXPECT_EQ(frame1, nullptr);

    auto frame2 = buffer->RequestFrame(renderEngine, config, false, SingleBufferMode::SINGLE_BUFFER_MODE_TO_MULTI);
    EXPECT_EQ(frame2, nullptr);
    if (frame2) {
        frame2.reset();
    }
}

/**
 * @tc.name: RequestFrameSwitchTypeToSingleConfigChangeTest001
 * @tc.desc: Verify function RequestFrame
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineBufferPrivateTest, RequestFrameSwitchTypeToSingleConfigChangeTest001, Level1)
{
    auto buffer = std::make_shared<RSGPUOfflineBuffer>("test", TEST_NODE_ID);
    BufferRequestConfig config1 = {};
    config1.width = 1920;
    config1.height = 1080;
    BufferRequestConfig config2 = {};
    config2.width = 1280;
    config2.height = 720;
    auto renderEngine = std::make_shared<RSUniRenderEngine>();

    auto frame1 = buffer->RequestFrame(renderEngine, config1, false, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
    EXPECT_EQ(frame1, nullptr);

    auto frame2 = buffer->RequestFrame(renderEngine, config2, false, SingleBufferMode::SINGLE_BUFFER_MODE_TO_SINGLE);
    EXPECT_EQ(frame2, nullptr);
    if (frame2) {
        frame2.reset();
    }
}

/**
 * @tc.name: RequestFrameSwitchTypeNoneConfigChangeTest001
 * @tc.desc: Verify function RequestFrame
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineBufferPrivateTest, RequestFrameSwitchTypeNoneConfigChangeTest001, Level1)
{
    auto buffer = std::make_shared<RSGPUOfflineBuffer>("test", TEST_NODE_ID);
    BufferRequestConfig config1 = {};
    config1.width = 1920;
    config1.height = 1080;
    BufferRequestConfig config2 = {};
    config2.width = 1280;
    config2.height = 720;
    auto renderEngine = std::make_shared<RSUniRenderEngine>();

    auto frame1 = buffer->RequestFrame(renderEngine, config1, false, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
    EXPECT_EQ(frame1, nullptr);

    auto frame2 = buffer->RequestFrame(renderEngine, config2, false, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
    EXPECT_EQ(frame2, nullptr);
    if (frame2) {
        frame2.reset();
    }
}


/**
 * @tc.name: RequestFrameSwitchTypeToMultiNoAttachSingleBufferTest001
 * @tc.desc: Verify function RequestFrame
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineBufferPrivateTest, RequestFrameSwitchTypeToMultiNoAttachSingleBufferTest001, Level1)
{
    auto buffer = std::make_shared<RSGPUOfflineBuffer>("test", TEST_NODE_ID);
    BufferRequestConfig config = {};
    config.width = 1920;
    config.height = 1080;
    auto renderEngine = std::make_shared<RSUniRenderEngine>();

    auto frame1 = buffer->RequestFrame(renderEngine, config, false, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
    EXPECT_EQ(frame1, nullptr);

    auto frame2 = buffer->RequestFrame(renderEngine, config, false, SingleBufferMode::SINGLE_BUFFER_MODE_TO_MULTI);
    EXPECT_EQ(frame2, nullptr);
    frame2.reset();
}

class NativeWindowBufferInfoMarshallingTest : public testing::Test {
    public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeWindowBufferInfoMarshallingTest::SetUpTestCase() {}
void NativeWindowBufferInfoMarshallingTest::TearDownTestCase() {}
void NativeWindowBufferInfoMarshallingTest::SetUp() {}
void NativeWindowBufferInfoMarshallingTest::TearDown() {}

/**
 * @tc.name: UnmarshallingSptrTest001
 * @tc.desc: Verify function Unmarshalling
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(NativeWindowBufferInfoMarshallingTest, UnmarshallingSptrTest001, Level1)
{
    Parcel parcel;
    OHOS::sptr<NativeWindowBufferInfo> result = NativeWindowBufferInfo::Unmarshalling(parcel, false);
    EXPECT_EQ(result, nullptr);
}


/**
 * @tc.name: UnmarshallingSptrAutoSeqTest001
 * @tc.desc: Verify function Unmarshalling
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(NativeWindowBufferInfoMarshallingTest, UnmarshallingSptrAutoSeqTest001, Level1)
{
    Parcel parcel;
    OHOS::sptr<NativeWindowBufferInfo> result = NativeWindowBufferInfo::Unmarshalling(parcel, true);
    EXPECT_EQ(result, nullptr);
}