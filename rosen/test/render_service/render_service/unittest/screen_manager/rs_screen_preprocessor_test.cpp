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
        std::make_unique<RSScreenPreprocessor>(wptr<RSScreenManager>(screenManager_), callbackMgr_, handler_, false);
};

void RSScreenPreprocessorTest::SetUpTestCase() {}
void RSScreenPreprocessorTest::TearDownTestCase() {}
void RSScreenPreprocessorTest::SetUp() {}
void RSScreenPreprocessorTest::TearDown() {}

/*
 * @tc.name: OnHotPlugTest001
 * @tc.desc: Test OnHotPlug when output is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHotPlugTest001, TestSize.Level1)
{
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    output.reset();
    RSScreenPreprocessor::OnHotPlug(output, false, nullptr);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnHotPlugTest002
 * @tc.desc: Test OnHotPlug when data is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHotPlugTest002, TestSize.Level1)
{
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    RSScreenPreprocessor::OnHotPlug(output, false, nullptr);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnHotPlugTest003
 * @tc.desc: Test OnHotPlug when no nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHotPlugTest003, TestSize.Level1)
{
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    RSScreenPreprocessor::OnHotPlug(output, false, preprocessor_.get());
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnRefreshTest001
 * @tc.desc: Test OnRefresh when data is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnRefreshTest001, TestSize.Level1)
{
    uint32_t screenId = 1000;
    RSScreenPreprocessor::OnRefresh(screenId, nullptr);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnRefreshTest002
 * @tc.desc: Test OnRefresh when data is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnRefreshTest002, TestSize.Level1)
{
    uint32_t screenId = 1000;
    RSScreenPreprocessor::OnRefresh(screenId, preprocessor_.get());
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnHwcDeadTest001
 * @tc.desc: Test OnHwcDead when data is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcDeadTest001, TestSize.Level1)
{
    RSScreenPreprocessor::OnHwcDead(nullptr);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnHwcDeadTest002
 * @tc.desc: Test OnHwcDead when data is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcDeadTest002, TestSize.Level1)
{
    RSScreenPreprocessor::OnHwcDead(preprocessor_.get());
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnHwcEventTest001
 * @tc.desc: Test OnHwcEvent when data is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcEventTest001, TestSize.Level1)
{
    uint32_t deviceId = 1000;
    uint32_t eventId = 1000;
    std::vector<int32_t> eventData;
    RSScreenPreprocessor::OnHwcEvent(deviceId, eventId, eventData, nullptr);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnHwcEventTest002
 * @tc.desc: Test OnHwcEvent when data is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcEventTest002, TestSize.Level1)
{
    uint32_t deviceId = 1000;
    uint32_t eventId = 1000;
    std::vector<int32_t> eventData;
    RSScreenPreprocessor::OnHwcEvent(deviceId, eventId, eventData, preprocessor_.get());
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnScreenVBlankIdleTest001
 * @tc.desc: Test OnScreenVBlankIdle when data is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnScreenVBlankIdleTest001, TestSize.Level1)
{
    uint32_t devId = 1000;
    uint64_t ns = 1000;
    RSScreenPreprocessor::OnScreenVBlankIdle(devId, ns, nullptr);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnScreenVBlankIdleTest002
 * @tc.desc: Test OnScreenVBlankIdle when data is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnScreenVBlankIdleTest002, TestSize.Level1)
{
    uint32_t devId = 1000;
    uint64_t ns = 1000;
    RSScreenPreprocessor::OnScreenVBlankIdle(devId, ns, preprocessor_.get());
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: InitTest001
 * @tc.desc: Test Init
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, InitTest001, TestSize.Level1)
{
    preprocessor_->Init();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: InitFoldSensorTest001
 * @tc.desc: Test InitFoldSensor when isFoldScreenFlag_ is false
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, InitFoldSensorTest001, TestSize.Level1)
{
    preprocessor_->isFoldScreenFlag_ = false;
    preprocessor_->InitFoldSensor();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: InitFoldSensorTest002
 * @tc.desc: Test InitFoldSensor when isFoldScreenFlag_ is true
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, InitFoldSensorTest002, TestSize.Level1)
{
    preprocessor_->isFoldScreenFlag_ = true;
    preprocessor_->InitFoldSensor();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: RegisterSensorCallbackTest001
 * @tc.desc: Test RegisterSensorCallback when hasRegisterSensorCallback_ is true
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, RegisterSensorCallbackTest001, TestSize.Level1)
{
    preprocessor_->hasRegisterSensorCallback_ = true;
    preprocessor_->RegisterSensorCallback();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: RegisterSensorCallbackTest002
 * @tc.desc: Test RegisterSensorCallback when hasRegisterSensorCallback_ is false
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, RegisterSensorCallbackTest002, TestSize.Level1)
{
    preprocessor_->hasRegisterSensorCallback_ = false;
    preprocessor_->RegisterSensorCallback();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: UnRegisterSensorCallbackTest001
 * @tc.desc: Test UnRegisterSensorCallback when hasRegisterSensorCallback_ is false
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, UnRegisterSensorCallbackTest001, TestSize.Level1)
{
    preprocessor_->hasRegisterSensorCallback_ = false;
    preprocessor_->UnRegisterSensorCallback();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: UnRegisterSensorCallbackTest002
 * @tc.desc: Test UnRegisterSensorCallback when hasRegisterSensorCallback_ is true
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, UnRegisterSensorCallbackTest002, TestSize.Level1)
{
    preprocessor_->hasRegisterSensorCallback_ = true;
    preprocessor_->UnRegisterSensorCallback();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnBootCompleteTest001
 * @tc.desc: Test OnBootComplete when key is equal but value is not equal
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnBootCompleteTest001, TestSize.Level1)
{
    string key = "bootevent.boot.completed";
    string value = "false";
    preprocessor_->OnBootComplete(key.c_str(), value.c_str(), nullptr);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnBootCompleteTest002
 * @tc.desc: Test OnBootComplete when key is not equal and value is not equal
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnBootCompleteTest002, TestSize.Level1)
{
    string key = "bootevent.boot.uncompleted";
    string value = "false";
    preprocessor_->OnBootComplete(key.c_str(), value.c_str(), nullptr);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnBootCompleteTest003
 * @tc.desc: Test OnBootComplete when key is not equal but value is equal
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnBootCompleteTest003, TestSize.Level1)
{
    string key = "bootevent.boot.uncompleted";
    string value = "true";
    preprocessor_->OnBootComplete(key.c_str(), value.c_str(), nullptr);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnBootCompleteTest004
 * @tc.desc: Test OnBootComplete when key is equal, value is equal and context is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnBootCompleteTest004, TestSize.Level1)
{
    string key = "bootevent.boot.completed";
    string value = "true";
    preprocessor_->OnBootComplete(key.c_str(), value.c_str(), nullptr);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnBootCompleteTest005
 * @tc.desc: Test OnBootComplete when key is equal, value is equal and context isn't nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnBootCompleteTest005, TestSize.Level1)
{
    string key = "bootevent.boot.completed";
    string value = "true";
    preprocessor_->OnBootComplete(key.c_str(), value.c_str(), preprocessor_.get());
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnBootCompleteEventTest001
 * @tc.desc: Test OnBootCompleteEvent when isFoldScreenFlag_ is false
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnBootCompleteEventTest001, TestSize.Level1)
{
    preprocessor_->isFoldScreenFlag_ = false;
    preprocessor_->OnBootCompleteEvent();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnBootCompleteEventTest002
 * @tc.desc: Test OnBootCompleteEvent when isFoldScreenFlag_ is true
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnBootCompleteEventTest002, TestSize.Level1)
{
    preprocessor_->isFoldScreenFlag_ = true;
    preprocessor_->OnBootCompleteEvent();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: HandlePostureDataTest001
 * @tc.desc: Test HandlePostureData when event is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, HandlePostureDataTest001, TestSize.Level1)
{
    preprocessor_->HandlePostureData(nullptr);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: HandlePostureDataTest002
 * @tc.desc: Test HandlePostureData when event isn't nullptr but data is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, HandlePostureDataTest002, TestSize.Level1)
{
    auto event = std::make_shared<SensorEvent>();
    preprocessor_->HandlePostureData(event.get());
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: HandlePostureDataTest003
 * @tc.desc: Test HandlePostureData when event dataLen < sizeof(PostureData)
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, HandlePostureDataTest003, TestSize.Level1)
{
    auto data = std::make_shared<uint8_t>(1000);
    auto event = std::make_shared<SensorEvent>();
    event->data = data.get();
    preprocessor_->HandlePostureData(event.get());
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: HandlePostureDataTest004
 * @tc.desc: Test HandlePostureData when event dataLen >= sizeof(PostureData)
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, HandlePostureDataTest004, TestSize.Level1)
{
    auto data = std::make_shared<uint8_t>(1000);
    auto event = std::make_shared<SensorEvent>();
    event->data = data.get();
    event->dataLen = sizeof(PostureData);
    preprocessor_->HandlePostureData(event.get());
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnHotPlugEventTest001
 * @tc.desc: Test OnHotPlugEvent when pendingHotPlugEvents_ can't find screenId, and isHwcDead_ is true
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHotPlugEventTest001, TestSize.Level1)
{
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    preprocessor_->isHwcDead_ = true;
    preprocessor_->OnHotPlugEvent(output, false);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnHotPlugEventTest002
 * @tc.desc: Test OnHotPlugEvent when pendingHotPlugEvents_ can find screenId, and isHwcDead_ is false
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHotPlugEventTest002, TestSize.Level1)
{
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    ScreenHotPlugEvent screenHotPlugEvent;
    preprocessor_->pendingHotPlugEvents_.emplace(screenId, screenHotPlugEvent);
    preprocessor_->isHwcDead_ = false;
    preprocessor_->OnHotPlugEvent(output, false);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: ProcessScreenHotPlugEventsTest001
 * @tc.desc: Test ProcessScreenHotPlugEvents when event in pendingHotPlugEvents_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ProcessScreenHotPlugEventsTest001, TestSize.Level1)
{
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    output.reset();
    ScreenHotPlugEvent screenHotPlugEvent;
    screenHotPlugEvent.output = output;
    preprocessor_->pendingHotPlugEvents_.emplace(screenId, screenHotPlugEvent);
    preprocessor_->ProcessScreenHotPlugEvents();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: ProcessScreenHotPlugEventsTest002
 * @tc.desc: Test ProcessScreenHotPlugEvents when event's connected in pendingHotPlugEvents_ is false
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ProcessScreenHotPlugEventsTest002, TestSize.Level1)
{
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    ScreenHotPlugEvent screenHotPlugEvent;
    screenHotPlugEvent.output = output;
    screenHotPlugEvent.connected = false;
    preprocessor_->pendingHotPlugEvents_.emplace(screenId, screenHotPlugEvent);
    preprocessor_->ProcessScreenHotPlugEvents();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: ProcessScreenHotPlugEventsTest003
 * @tc.desc: Test ProcessScreenHotPlugEvents when event's connected in pendingHotPlugEvents_ is true
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ProcessScreenHotPlugEventsTest003, TestSize.Level1)
{
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    ScreenHotPlugEvent screenHotPlugEvent;
    screenHotPlugEvent.output = output;
    screenHotPlugEvent.connected = true;
    preprocessor_->pendingHotPlugEvents_.emplace(screenId, screenHotPlugEvent);
    preprocessor_->ProcessScreenHotPlugEvents();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: ProcessScreenHotPlugEventsTest004
 * @tc.desc: Test ProcessScreenHotPlugEvents when screenManager_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ProcessScreenHotPlugEventsTest004, TestSize.Level1)
{
    screenManager_ = nullptr;
    preprocessor_->ProcessScreenHotPlugEvents();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: ConfigureScreenConnectedTest001
 * @tc.desc: Test ConfigureScreenConnected when screenManager_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ConfigureScreenConnectedTest001, TestSize.Level1)
{
    screenManager_ = nullptr;
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    preprocessor_->ConfigureScreenConnected(output);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: ConfigureScreenConnectedTest002
 * @tc.desc: Test ConfigureScreenConnected when callbackMgrWeak_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ConfigureScreenConnectedTest002, TestSize.Level1)
{
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    callbackMgr_.reset();
    preprocessor_->isHwcDead_ = false;
    preprocessor_->ConfigureScreenConnected(output);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: ConfigureScreenConnectedTest003
 * @tc.desc: Test ConfigureScreenConnected when callbackMgrWeak_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ConfigureScreenConnectedTest003, TestSize.Level1)
{
    uint32_t screenId = 0;
    auto output = std::make_shared<HdiOutput>(screenId);
    callbackMgr_.reset();
    preprocessor_->isHwcDead_ = true;
    preprocessor_->ConfigureScreenConnected(output);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: ConfigureScreenDisconnectedTest001
 * @tc.desc: Test ConfigureScreenDisconnected when screenManager_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ConfigureScreenDisconnectedTest001, TestSize.Level1)
{
    screenManager_ = nullptr;
    uint32_t screenId = 1000;
    auto output = std::make_shared<HdiOutput>(screenId);
    preprocessor_->ConfigureScreenDisconnected(output);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: ConfigureScreenDisconnectedTest002
 * @tc.desc: Test ConfigureScreenDisconnected when screenManager_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ConfigureScreenDisconnectedTest002, TestSize.Level1)
{
    uint32_t screenId = 0;
    auto output = std::make_shared<HdiOutput>(screenId);
    preprocessor_->ConfigureScreenDisconnected(output);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: ConfigureScreenDisconnectedTest003
 * @tc.desc: Test ConfigureScreenDisconnected when screenManager_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ConfigureScreenDisconnectedTest003, TestSize.Level1)
{
    uint32_t screenId = 0;
    auto output = std::make_shared<HdiOutput>(screenId);
    callbackMgr_.reset();
    preprocessor_->ConfigureScreenDisconnected(output);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnRefreshEventTest001
 * @tc.desc: Test OnRefreshEvent when screenManager_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnRefreshEventTest001, TestSize.Level1)
{
    uint32_t screenId = 1000;
    preprocessor_->OnRefreshEvent(screenId);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnRefreshEventTest002
 * @tc.desc: Test OnRefreshEvent when screenManager_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnRefreshEventTest002, TestSize.Level1)
{
    screenManager_ = nullptr;
    uint32_t screenId = 1000;
    preprocessor_->OnRefreshEvent(screenId);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnHwcDeadEventTest001
 * @tc.desc: Test OnHwcDeadEvent when screenManager_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcDeadEventTest001, TestSize.Level1)
{
    screenManager_ = nullptr;
    preprocessor_->OnHwcDeadEvent();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnHwcDeadEventTest002
 * @tc.desc: Test OnHwcDeadEvent when composer_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcDeadEventTest002, TestSize.Level1)
{
    preprocessor_->composer_ = nullptr;
    preprocessor_->OnHwcDeadEvent();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnHwcDeadEventTest003
 * @tc.desc: Test OnHwcDeadEvent when composer_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcDeadEventTest003, TestSize.Level1)
{
    preprocessor_->composer_ = HdiBackend::GetInstance();
    preprocessor_->OnHwcDeadEvent();
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnHwcEventCallbackTest001
 * @tc.desc: Test OnHwcEventCallback when screenManager_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcEventCallbackTest001, TestSize.Level1)
{
    uint32_t deviceId = 1000;
    uint32_t eventId = 1000;
    std::vector<int32_t> eventData;
    screenManager_ = nullptr;
    preprocessor_->OnHwcEventCallback(deviceId, eventId, eventData);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnHwcEventCallbackTest002
 * @tc.desc: Test OnHwcEventCallback when screenManager_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnHwcEventCallbackTest002, TestSize.Level1)
{
    uint32_t deviceId = 1000;
    uint32_t eventId = 1000;
    std::vector<int32_t> eventData;
    preprocessor_->OnHwcEventCallback(deviceId, eventId, eventData);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnScreenVBlankIdleEventTest001
 * @tc.desc: Test OnScreenVBlankIdleEvent when screenManager_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnScreenVBlankIdleEventTest001, TestSize.Level1)
{
    uint32_t devId = 1000;
    uint64_t ns = 1000;
    preprocessor_->OnScreenVBlankIdleEvent(devId, ns);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: OnScreenVBlankIdleEventTest002
 * @tc.desc: Test OnScreenVBlankIdleEvent when screenManager_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, OnScreenVBlankIdleEventTest002, TestSize.Level1)
{
    uint32_t devId = 1000;
    uint64_t ns = 1000;
    screenManager_ = nullptr;
    preprocessor_->OnScreenVBlankIdleEvent(devId, ns);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: ScheduleTaskTest001
 * @tc.desc: Test ScheduleTask when mainHandler_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ScheduleTaskTest001, TestSize.Level1)
{
    std::function<void()> task;
    preprocessor_->mainHandler_ = nullptr;
    preprocessor_->ScheduleTask(task);
    ASSERT_NE(preprocessor_, nullptr);
}

/*
 * @tc.name: ScheduleTaskTest002
 * @tc.desc: Test ScheduleTask when mainHandler_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenPreprocessorTest, ScheduleTaskTest002, TestSize.Level1)
{
    std::function<void()> task;
    preprocessor_->ScheduleTask(task);
    ASSERT_NE(preprocessor_, nullptr);
}
} // namespace OHOS::Rosen