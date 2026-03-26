/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <parameter.h>
#include <parameters.h>

#include "gtest/gtest.h"
#include "param/sys_param.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/rs_screen.h"
#include "screen_manager/rs_screen_preprocessor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSScreenPreprocessorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSScreenCallbackManager> callbackMgr_ = std::make_shared<RSScreenCallbackManager>();
    sptr<RSScreenManager> screenManager_ = sptr<RSScreenManager>::MakeSptr();
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = AppExecFwk::EventRunner::Create(false);
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    std::unique_ptr<RSScreenPreprocessor> preprocessor_ =
        std::make_unique<RSScreenPreprocessor>(*screenManager_, *callbackMgr_, handler_, false);
};

void RSScreenPreprocessorTest::SetUpTestCase() {}
void RSScreenPreprocessorTest::TearDownTestCase() {}
void RSScreenPreprocessorTest::SetUp() {
    preprocessor_->Init();
}
void RSScreenPreprocessorTest::TearDown() {}

/*
 * @tc.name: OnHotPlugTest001
 * @tc.desc: Test OnHotPlug
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHotPlugTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t screenId = 1000;
    // output is nullptr
    std::shared_ptr<HdiOutput> output = nullptr;
    RSScreenPreprocessor::OnHotPlug(output, false, nullptr);
    // output not nullptr, processor is nullptr
    output = std::make_shared<HdiOutput>(screenId);
    RSScreenPreprocessor::OnHotPlug(output, false, nullptr);
    // output not nullptr, processor not nullptr
    RSScreenPreprocessor::OnHotPlug(output, false, preprocessor_.get());
}

/*
 * @tc.name: OnRefreshTest001
 * @tc.desc: Test OnRefresh when data is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnRefreshTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t screenId = 1000;
    void* data = nullptr;
    RSScreenPreprocessor::OnRefresh(screenId, data);
    data = static_cast<void*>(preprocessor_.get());
    RSScreenPreprocessor::OnRefresh(screenId, data);
}

/*
 * @tc.name: OnHwcDeadTest001
 * @tc.desc: Test OnHwcDead
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcDeadTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    // Test OnHwcDead when data is nullptr
    void* data = nullptr;
    RSScreenPreprocessor::OnHwcDead(nullptr);
    // Test OnHwcDead when data is not nullptr
    data = static_cast<void*>(preprocessor_.get());
    RSScreenPreprocessor::OnHwcDead(data);
}

/*
 * @tc.name: OnHwcEventTest001
 * @tc.desc: Test OnHwcEvent when data is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcEventTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t deviceId = 1000;
    uint32_t eventId = 1000;
    std::vector<int32_t> eventData;
    RSScreenPreprocessor::OnHwcEvent(deviceId, eventId, eventData, nullptr);
}

/*
 * @tc.name: OnHwcEventTest002
 * @tc.desc: Test OnHwcEvent when data is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcEventTest002, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t deviceId = 1000;
    uint32_t eventId = 1000;
    std::vector<int32_t> eventData;
    RSScreenPreprocessor::OnHwcEvent(deviceId, eventId, eventData, preprocessor_.get());
}

/*
 * @tc.name: OnScreenVBlankIdleTest001
 * @tc.desc: Test OnScreenVBlankIdle when data is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnScreenVBlankIdleTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t devId = 1000;
    uint64_t ns = 1000;
    void* data = nullptr;
    RSScreenPreprocessor::OnScreenVBlankIdle(devId, ns, nullptr);
    data = static_cast<void*>(preprocessor_.get());
    RSScreenPreprocessor::OnScreenVBlankIdle(devId, ns, data);
}

/*
 * @tc.name: OnHotPlugEventTest001
 * @tc.desc: Test OnHotPlugEvent when pendingHotPlugEvents_ can't find screenId, and isHwcDead_ is true
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHotPlugEventTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    preprocessor_->isHwcDead_ = true;
    preprocessor_->OnHotPlugEvent(output, false);
}

/*
 * @tc.name: OnHotPlugEventTest002
 * @tc.desc: Test OnHotPlugEvent when pendingHotPlugEvents_ can find screenId, and isHwcDead_ is false
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHotPlugEventTest002, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    ScreenHotPlugEvent screenHotPlugEvent;
    preprocessor_->pendingHotPlugEvents_.emplace(screenId, screenHotPlugEvent);
    preprocessor_->isHwcDead_ = false;
    preprocessor_->OnHotPlugEvent(output, false);
}

/*
 * @tc.name: ProcessScreenHotPlugEventsTest001
 * @tc.desc: Test ProcessScreenHotPlugEvents when event in pendingHotPlugEvents_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ProcessScreenHotPlugEventsTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    output.reset();
    ScreenHotPlugEvent screenHotPlugEvent;
    screenHotPlugEvent.output = output;
    preprocessor_->pendingHotPlugEvents_.emplace(screenId, screenHotPlugEvent);
    preprocessor_->ProcessScreenHotPlugEvents();
}

/*
 * @tc.name: ProcessScreenHotPlugEventsTest002
 * @tc.desc: Test ProcessScreenHotPlugEvents when event's connected in pendingHotPlugEvents_ is false
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ProcessScreenHotPlugEventsTest002, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    ScreenHotPlugEvent screenHotPlugEvent;
    screenHotPlugEvent.output = output;
    screenHotPlugEvent.connected = false;
    preprocessor_->pendingHotPlugEvents_.emplace(screenId, screenHotPlugEvent);
    preprocessor_->ProcessScreenHotPlugEvents();
}

/*
 * @tc.name: ProcessScreenHotPlugEventsTest003
 * @tc.desc: Test ProcessScreenHotPlugEvents when event's connected in pendingHotPlugEvents_ is true
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ProcessScreenHotPlugEventsTest003, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    ScreenHotPlugEvent screenHotPlugEvent;
    screenHotPlugEvent.output = output;
    screenHotPlugEvent.connected = true;
    preprocessor_->pendingHotPlugEvents_.emplace(screenId, screenHotPlugEvent);
    preprocessor_->ProcessScreenHotPlugEvents();
}

/*
 * @tc.name: ConfigureScreenConnectedTest001
 * @tc.desc: Test ConfigureScreenConnected when screenManager_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ConfigureScreenConnectedTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t screenId = 1000;
    preprocessor_->screenManager_.screens_.clear();
    preprocessor_->screenManager_.screens_[screenId] = std::make_shared<RSScreen>(screenId);
    auto output = std::make_shared<HdiOutput>(screenId);
    preprocessor_->isHwcDead_ = false;
    preprocessor_->ConfigureScreenConnected(output);
    preprocessor_->isHwcDead_ = true;
    output = std::make_shared<HdiOutput>(screenId + 1);
    preprocessor_->ConfigureScreenConnected(output);
}

/*
 * @tc.name: ConfigureScreenDisconnectedTest001
 * @tc.desc: Test ConfigureScreenDisconnected when screenManager_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ConfigureScreenDisconnectedTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t screenId = 1000;
    preprocessor_->screenManager_.screens_.clear();
    preprocessor_->screenManager_.screens_[screenId] = std::make_shared<RSScreen>(screenId);
    auto output = std::make_shared<HdiOutput>(screenId);
    preprocessor_->ConfigureScreenDisconnected(output);
    output = std::make_shared<HdiOutput>(screenId + 1);
    preprocessor_->ConfigureScreenDisconnected(output);
}

/*
 * @tc.name: OnRefreshEventTest001
 * @tc.desc: Test OnRefreshEvent
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnRefreshEventTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t screenId = 1000;
    preprocessor_->OnRefreshEvent(screenId);
    screenId = 1001;
    preprocessor_->OnRefreshEvent(screenId);
}

/*
 * @tc.name: OnHwcDeadEventTest001
 * @tc.desc: Test OnHwcDeadEvent
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcDeadEventTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    preprocessor_->screenManager_.screens_[1] = std::make_shared<RSScreen>(1);
    preprocessor_->composer_ = HdiBackend::GetInstance();
    preprocessor_->OnHwcDeadEvent();
    usleep(50000);
    preprocessor_->composer_ = nullptr;
    preprocessor_->OnHwcDeadEvent();
    usleep(50000);
    preprocessor_->composer_ = HdiBackend::GetInstance();
}

/*
 * @tc.name: OnHwcEventCallbackTest001
 * @tc.desc: Test OnHwcEventCallback when screenManager_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcEventCallbackTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t deviceId = 1000;
    uint32_t eventId = 1000;
    std::vector<int32_t> eventData;
    preprocessor_->OnHwcEventCallback(deviceId, eventId, eventData);
}

/*
 * @tc.name: OnHwcEventCallbackTest002
 * @tc.desc: Test OnHwcEventCallback when screenManager_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcEventCallbackTest002, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t deviceId = 1000;
    uint32_t eventId = 1000;
    std::vector<int32_t> eventData;
    preprocessor_->OnHwcEventCallback(deviceId, eventId, eventData);
}

/*
 * @tc.name: OnScreenVBlankIdleEventTest001
 * @tc.desc: Test OnScreenVBlankIdleEvent when screenManager_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnScreenVBlankIdleEventTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t devId = 1000;
    uint64_t ns = 1000;
    preprocessor_->OnScreenVBlankIdleEvent(devId, ns);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnScreenVBlankIdleEventTest002
 * @tc.desc: Test OnScreenVBlankIdleEvent
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnScreenVBlankIdleEventTest002, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    uint32_t devId = 1000;
    uint64_t ns = 1000;
    preprocessor_->OnScreenVBlankIdleEvent(devId, ns);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: ScheduleTaskTest001
 * @tc.desc: Test ScheduleTask
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ScheduleTaskTest001, TestSize.Level1)
{
    ASSERT_NE(preprocessor_, nullptr);
    std::function<void()> task;
    auto handler = preprocessor_->mainHandler_;
    preprocessor_->mainHandler_ = nullptr;
    preprocessor_->ScheduleTask(task);
    preprocessor_->mainHandler_ = handler;
    preprocessor_->ScheduleTask(task);
}
} // namespace OHOS::Rosen