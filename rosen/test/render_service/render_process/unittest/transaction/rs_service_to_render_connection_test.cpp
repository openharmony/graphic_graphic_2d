/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include <iremote_broker.h>

#include "irs_render_to_composer_connection.h"
#include "parameters.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "render_process/rs_render_process_agent.h"
#include "rs_render_process_manager.h"
#include "rs_render_service.h"
#include "rs_render_to_composer_connection_proxy.h"
#include "rs_render_single_process_manager.h"
#include "render/rs_typeface_cache.h"
#include "screen_manager/rs_screen_property.h"
#include "transaction/rs_service_to_render_connection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr const int WAIT_HANDLER_TIME = 1;
constexpr const int WAIT_HANDLER_TIME_COUNT = 5;
constexpr const int SLEEP_TIME = 110;
constexpr ScreenId TEST_SCREEN_ID = 0;
constexpr ScreenId TEST_SCREEN_ID_2 = 1;

std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
sptr<RSServiceToRenderConnection> g_rsConn = nullptr;
sptr<RSRenderProcess> g_renderProcess = nullptr;
sptr<RSRenderProcessAgent> g_renderProcessAgent = nullptr;
sptr<RSServiceToRenderConnection> g_rsConnWithProcessAgent = nullptr;
}

class RSServiceToRenderConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void WaitHandlerTask();
};

void RSServiceToRenderConnectionTest::SetUpTestCase()
{
    renderPipeline = std::make_shared<RSRenderPipeline>();
    renderPipeline->imageEnhanceManager_ = std::make_shared<ImageEnhanceManager>();

    auto runner1 = AppExecFwk::EventRunner::Create(true);
    auto handler1 = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner1);
    auto mainThread = RSMainThread::Instance();
    mainThread->hwcContext_ =
        std::make_shared<RSHwcContext>(HWCParam::GetSourceTuningForAppMap(), HWCParam::GetSolidColorLayerMap());
    renderPipeline->mainThread_ = mainThread;
    renderPipeline->mainThread_->handler_ = handler1;
    runner1->Run();

    renderPipeline->uniRenderThread_ = &(RSUniRenderThread::Instance());
    auto runner2 = AppExecFwk::EventRunner::Create(true);
    renderPipeline->uniRenderThread_->runner_ = runner2;
    renderPipeline->uniRenderThread_->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner2);
    runner2->Run();

    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    renderPipeline->uniRenderThread_->uniRenderEngine_ = std::make_shared<OHOS::Rosen::RSRenderEngine>();
    renderPipeline->uniRenderThread_->uniRenderEngine_->renderContext_ = OHOS::Rosen::RenderContext::Create();
    g_rsConn = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
}

void RSServiceToRenderConnectionTest::TearDownTestCase()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));

    WaitHandlerTask();
    renderPipeline->mainThread_->handler_->eventRunner_->Stop();
    renderPipeline->uniRenderThread_->runner_->Stop();

    renderPipeline->mainThread_->handler_ = nullptr;
    renderPipeline->mainThread_->receiver_ = nullptr;
    renderPipeline->mainThread_->renderEngine_ = nullptr;

    renderPipeline->uniRenderThread_->handler_ = nullptr;
    renderPipeline->uniRenderThread_->runner_ = nullptr;
    renderPipeline->uniRenderThread_->uniRenderEngine_->renderContext_ = nullptr;
    renderPipeline->uniRenderThread_->uniRenderEngine_ = nullptr;
    renderPipeline->uniRenderThread_ = nullptr;
    renderPipeline = nullptr;

    g_rsConn = nullptr;
}

void RSServiceToRenderConnectionTest::SetUp() {}
void RSServiceToRenderConnectionTest::TearDown() {}

void RSServiceToRenderConnectionTest::WaitHandlerTask()
{
    auto count = 0;
    auto isMainThreadRunning = !renderPipeline->mainThread_->handler_->IsIdle();
    auto isUniRenderThreadRunning = !renderPipeline->uniRenderThread_->handler_->IsIdle();
    while (count < WAIT_HANDLER_TIME_COUNT && (isMainThreadRunning || isUniRenderThreadRunning)) {
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_HANDLER_TIME));
    }
    if (count >= WAIT_HANDLER_TIME_COUNT) {
        renderPipeline->mainThread_->handler_->RemoveAllEvents();
        renderPipeline->uniRenderThread_->handler_->RemoveAllEvents();
    }
}

/**
 * @tc.name: HgmForceUpdateTaskTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionTest, HgmForceUpdateTaskTest, TestSize.Level1)
{
    g_rsConn->HgmForceUpdateTask(false, "");
    g_rsConn->HgmForceUpdateTask(true, "");
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: GetRealtimeRefreshRateTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionTest, GetRealtimeRefreshRateTest, TestSize.Level1)
{
    ASSERT_TRUE(g_rsConn);
    auto result = g_rsConn->GetRealtimeRefreshRate(INVALID_SCREEN_ID);
    EXPECT_GE(result, 0);
}


/**
 * @tc.name: SetBehindWindowFilterEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionTest, SetBehindWindowFilterEnabledTest, TestSize.Level1)
{
    bool enabled = true;
    bool enabled1 = false;
    g_rsConn->SetBehindWindowFilterEnabled(enabled);
    g_rsConn->SetBehindWindowFilterEnabled(enabled1);
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: GetBehindWindowFilterEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionTest, GetBehindWindowFilterEnabledTest, TestSize.Level1)
{
    bool enabled = true;
    bool enabled1 = false;
    g_rsConn->GetBehindWindowFilterEnabled(enabled);
    g_rsConn->GetBehindWindowFilterEnabled(enabled1);
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: SetShowRefreshRateEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionTest, SetShowRefreshRateEnabledTest, TestSize.Level1)
{
    bool enabled = true;
    bool enabled1 = false;
    int32_t type = 1;
    g_rsConn->SetShowRefreshRateEnabled(enabled, type);
    g_rsConn->SetShowRefreshRateEnabled(enabled1, type);
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: GetShowRefreshRateEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionTest, GetShowRefreshRateEnabledTest, TestSize.Level1)
{
    bool enabled = true;
    bool enabled1 = false;
    g_rsConn->GetShowRefreshRateEnabled(enabled);
    g_rsConn->GetShowRefreshRateEnabled(enabled1);
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: NotifyPackageEventTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionTest, NotifyPackageEventTest, TestSize.Level1)
{
    uint32_t listSize1 = 0;
    std::vector<std::string> package1;
    uint32_t listSize2 = 2;
    std::vector<std::string> package2 = {"package1", "package2"};
    g_rsConn->NotifyPackageEvent(listSize1, package1);
    g_rsConn->NotifyPackageEvent(listSize2, package2);
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: RegisterSharedTypefaceTest001
 * @tc.desc: Test RegisterTypeface(SharedTypeface&, bool) with isLocal=true
 * @tc.type: FUNC
 */
HWTEST_F(RSServiceToRenderConnectionTest, RegisterSharedTypefaceTest001, TestSize.Level1)
{
    Drawing::SharedTypeface sharedTypeface;
    sharedTypeface.id_ = 42;
    sharedTypeface.originId_ = 0;
    // isLocal=true should return true without doing anything
    bool ret = g_rsConn->RegisterTypeface(sharedTypeface, true);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: RegisterSharedTypefaceTest002
 * @tc.desc: Test RegisterTypeface with isLocal=false, originId>0 (variation typeface path)
 * @tc.type: FUNC
 */
HWTEST_F(RSServiceToRenderConnectionTest, RegisterSharedTypefaceTest002, TestSize.Level1)
{
    Drawing::SharedTypeface sharedTypeface;
    sharedTypeface.id_ = 42;
    sharedTypeface.originId_ = 100;
    sharedTypeface.fd_ = -1;
    bool ret = g_rsConn->RegisterTypeface(sharedTypeface, false);
    // InsertVariationTypeface returns -1 when base typeface not cached
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: RegisterSharedTypefaceTest003
 * @tc.desc: Test RegisterTypeface with isLocal=false, cached typeface found via UpdateDrawingTypefaceRef
 * @tc.type: FUNC
 */
HWTEST_F(RSServiceToRenderConnectionTest, RegisterSharedTypefaceTest003, TestSize.Level1)
{
    auto typeface = Drawing::Typeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    uint64_t uniqueId = (static_cast<uint64_t>(getpid()) << 32) | typeface->GetUniqueID();
    // Pre-cache the typeface so UpdateDrawingTypefaceRef finds it
    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueId, typeface);

    Drawing::SharedTypeface sharedTypeface(uniqueId, typeface);
    sharedTypeface.fd_ = -1;
    bool ret = g_rsConn->RegisterTypeface(sharedTypeface, false);
    EXPECT_TRUE(ret);

    RSTypefaceCache::Instance().RemoveDrawingTypefaceByGlobalUniqueId(uniqueId);
}

/**
 * @tc.name: RegisterSharedTypefaceTest004
 * @tc.desc: Test RegisterTypeface with isLocal=false, not cached, MakeFromAshmem fails
 * @tc.type: FUNC
 */
HWTEST_F(RSServiceToRenderConnectionTest, RegisterSharedTypefaceTest004, TestSize.Level1)
{
    Drawing::SharedTypeface sharedTypeface;
    sharedTypeface.id_ = 9999;
    sharedTypeface.originId_ = 0;
    sharedTypeface.fd_ = -1;
    sharedTypeface.size_ = 0;
    // Not cached, invalid ashmem -> should return false
    bool ret = g_rsConn->RegisterTypeface(sharedTypeface, false);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: UnRegisterSharedTypefaceTest001
 * @tc.desc: Test UnRegisterTypeface with cached typeface
 * @tc.type: FUNC
 */
HWTEST_F(RSServiceToRenderConnectionTest, UnRegisterSharedTypefaceTest001, TestSize.Level1)
{
    bool ret = g_rsConn->UnRegisterTypeface(0);
    EXPECT_TRUE(ret);
    auto typeface = Drawing::Typeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    uint64_t uniqueId = (static_cast<uint64_t>(getpid()) << 32) | typeface->GetUniqueID();
    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueId, typeface);

    ret = g_rsConn->UnRegisterTypeface(uniqueId);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: NotifyScreenConnectInfoToRenderTest001
 * @tc.desc: Test NotifyScreenConnectInfoToRender with valid screen property and null connections
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionTest, NotifyScreenConnectInfoToRenderTest001, TestSize.Level1)
{
    g_renderProcess = sptr<RSRenderProcess>::MakeSptr();
    g_renderProcessAgent = sptr<RSRenderProcessAgent>::MakeSptr(*g_renderProcess);
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    g_rsConnWithProcessAgent =
        sptr<RSServiceToRenderConnection>::MakeSptr(g_renderProcessAgent, renderPipelineAgent);
    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    auto screenProperty = sptr<RSScreenProperty>::MakeSptr();
    sptr<IRSRenderToComposerConnection> renderToComposerConn = nullptr;
    sptr<IRSComposerToRenderConnection> composerToRenderConn = nullptr;

    g_rsConnWithProcessAgent->NotifyScreenConnectInfoToRender(
        screenProperty, renderToComposerConn, composerToRenderConn);

    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    g_rsConnWithProcessAgent = nullptr;
    g_renderProcessAgent = nullptr;
    g_renderProcess = nullptr;
}

/**
 * @tc.name: NotifyScreenConnectInfoToRenderTest002
 * @tc.desc: Test NotifyScreenConnectInfoToRender with null screen property
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionTest, NotifyScreenConnectInfoToRenderTest002, TestSize.Level1)
{
    g_renderProcess = sptr<RSRenderProcess>::MakeSptr();
    g_renderProcessAgent = sptr<RSRenderProcessAgent>::MakeSptr(*g_renderProcess);
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    g_rsConnWithProcessAgent =
        sptr<RSServiceToRenderConnection>::MakeSptr(g_renderProcessAgent, renderPipelineAgent);
    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    sptr<RSScreenProperty> screenProperty = nullptr;
    sptr<IRSRenderToComposerConnection> renderToComposerConn = nullptr;
    sptr<IRSComposerToRenderConnection> composerToRenderConn = nullptr;

    g_rsConnWithProcessAgent->NotifyScreenConnectInfoToRender(
        screenProperty, renderToComposerConn, composerToRenderConn);

    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    g_rsConnWithProcessAgent = nullptr;
    g_renderProcessAgent = nullptr;
    g_renderProcess = nullptr;
}

/**
 * @tc.name: NotifyScreenDisconnectInfoToRenderTest001
 * @tc.desc: Test NotifyScreenDisconnectInfoToRender with valid screen ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionTest, NotifyScreenDisconnectInfoToRenderTest001, TestSize.Level1)
{
    g_renderProcess = sptr<RSRenderProcess>::MakeSptr();
    g_renderProcessAgent = sptr<RSRenderProcessAgent>::MakeSptr(*g_renderProcess);
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    g_rsConnWithProcessAgent =
        sptr<RSServiceToRenderConnection>::MakeSptr(g_renderProcessAgent, renderPipelineAgent);
    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    g_rsConnWithProcessAgent->NotifyScreenDisconnectInfoToRender(TEST_SCREEN_ID);

    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    g_rsConnWithProcessAgent = nullptr;
    g_renderProcessAgent = nullptr;
    g_renderProcess = nullptr;
}

/**
 * @tc.name: NotifyScreenDisconnectInfoToRenderTest002
 * @tc.desc: Test NotifyScreenDisconnectInfoToRender with different screen ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionTest, NotifyScreenDisconnectInfoToRenderTest002, TestSize.Level1)
{
    g_renderProcess = sptr<RSRenderProcess>::MakeSptr();
    g_renderProcessAgent = sptr<RSRenderProcessAgent>::MakeSptr(*g_renderProcess);
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    g_rsConnWithProcessAgent =
        sptr<RSServiceToRenderConnection>::MakeSptr(g_renderProcessAgent, renderPipelineAgent);
    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    g_rsConnWithProcessAgent->NotifyScreenDisconnectInfoToRender(TEST_SCREEN_ID_2);

    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    g_rsConnWithProcessAgent = nullptr;
    g_renderProcessAgent = nullptr;
    g_renderProcess = nullptr;
}

/**
 * @tc.name: NotifyScreenPropertyChangedInfoToRenderTest001
 * @tc.desc: Test NotifyScreenPropertyChangedInfoToRender with RENDER_RESOLUTION type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionTest, NotifyScreenPropertyChangedInfoToRenderTest001, TestSize.Level1)
{
    g_renderProcess = sptr<RSRenderProcess>::MakeSptr();
    g_renderProcessAgent = sptr<RSRenderProcessAgent>::MakeSptr(*g_renderProcess);
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    g_rsConnWithProcessAgent =
        sptr<RSServiceToRenderConnection>::MakeSptr(g_renderProcessAgent, renderPipelineAgent);
    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    ScreenPropertyType type = ScreenPropertyType::RENDER_RESOLUTION;
    sptr<ScreenPropertyBase> screenProperty = nullptr;

    g_rsConnWithProcessAgent->NotifyScreenPropertyChangedInfoToRender(
        TEST_SCREEN_ID, type, screenProperty);

    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    g_rsConnWithProcessAgent = nullptr;
    g_renderProcessAgent = nullptr;
    g_renderProcess = nullptr;
}

/**
 * @tc.name: NotifyScreenPropertyChangedInfoToRenderTest002
 * @tc.desc: Test NotifyScreenPropertyChangedInfoToRender with PHYSICAL_RESOLUTION_REFRESHRATE type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionTest, NotifyScreenPropertyChangedInfoToRenderTest002, TestSize.Level1)
{
    g_renderProcess = sptr<RSRenderProcess>::MakeSptr();
    g_renderProcessAgent = sptr<RSRenderProcessAgent>::MakeSptr(*g_renderProcess);
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    g_rsConnWithProcessAgent =
        sptr<RSServiceToRenderConnection>::MakeSptr(g_renderProcessAgent, renderPipelineAgent);
    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    ScreenPropertyType type = ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE;
    sptr<ScreenPropertyBase> screenProperty = nullptr;

    g_rsConnWithProcessAgent->NotifyScreenPropertyChangedInfoToRender(
        TEST_SCREEN_ID_2, type, screenProperty);

    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    g_rsConnWithProcessAgent = nullptr;
    g_renderProcessAgent = nullptr;
    g_renderProcess = nullptr;
}

/**
 * @tc.name: NotifyScreenPropertyChangedInfoToRenderTest003
 * @tc.desc: Test NotifyScreenPropertyChangedInfoToRender with CORRECTION type and different screen ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionTest, NotifyScreenPropertyChangedInfoToRenderTest003, TestSize.Level1)
{
    g_renderProcess = sptr<RSRenderProcess>::MakeSptr();
    g_renderProcessAgent = sptr<RSRenderProcessAgent>::MakeSptr(*g_renderProcess);
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    g_rsConnWithProcessAgent =
        sptr<RSServiceToRenderConnection>::MakeSptr(g_renderProcessAgent, renderPipelineAgent);
    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    ScreenPropertyType type = ScreenPropertyType::CORRECTION;
    sptr<ScreenPropertyBase> screenProperty = nullptr;

    g_rsConnWithProcessAgent->NotifyScreenPropertyChangedInfoToRender(
        INVALID_SCREEN_ID, type, screenProperty);

    ASSERT_TRUE(g_rsConnWithProcessAgent != nullptr);

    g_rsConnWithProcessAgent = nullptr;
    g_renderProcessAgent = nullptr;
    g_renderProcess = nullptr;
}
} // namespace OHOS::Rosen
