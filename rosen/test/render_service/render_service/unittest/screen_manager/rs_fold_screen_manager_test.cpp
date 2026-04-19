/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "gmock/gmock.h"
#include <parameter.h>
#include <parameters.h>
#include "param/sys_param.h"
#include "screen_manager/product_feature/fold/rs_fold_screen_manager.h"
#include "screen_manager/rs_screen_preprocessor.h"
#include "screen_manager/rs_screen_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSFoldScreenManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    static inline RSFoldScreenManager* foldScreenManager_;
    static inline RSScreenPreprocessor* rsScreenPreprocessor_;
    static inline RSScreenManager* rsScreenManager_;
    static inline RSScreenCallbackManager* callbackMgr_;
};

void RSFoldScreenManagerTest::SetUpTestCase()
{
    rsScreenManager_ = new RSScreenManager();
    rsScreenPreprocessor_ = new RSScreenPreprocessor(*rsScreenManager_, *rsScreenManager_->callbackMgr_, nullptr,
                                                     rsScreenManager_->isFoldScreenFlag_);
    foldScreenManager_ = new RSFoldScreenManager(*rsScreenPreprocessor_);
}

void RSFoldScreenManagerTest::TearDownTestCase()
{
    delete foldScreenManager_;
    delete rsScreenPreprocessor_;
    delete rsScreenManager_;
}

void RSFoldScreenManagerTest::SetUp() {}
void RSFoldScreenManagerTest::TearDown() {}

/*
 * @tc.name: HandleSensorDataTest
 * @tc.desc: Test HandleSensorData
 * @tc.type: FUNC
 */
HWTEST_F(RSFoldScreenManagerTest, HandleSensorDataTest, TestSize.Level1)
{
    ASSERT_NE(nullptr, foldScreenManager_);
    foldScreenManager_->HandleSensorData(0.f);
    EXPECT_EQ(foldScreenManager_->activeScreenId_, foldScreenManager_->externalScreenId_);
    foldScreenManager_->HandleSensorData(0.f);
    EXPECT_EQ(foldScreenManager_->activeScreenId_, foldScreenManager_->externalScreenId_);
    foldScreenManager_->isPostureSensorDataHandled_ = false;
    foldScreenManager_->HandleSensorData(0.f);
    EXPECT_EQ(foldScreenManager_->activeScreenId_, foldScreenManager_->externalScreenId_);
    foldScreenManager_->HandleSensorData(180.f);
    EXPECT_EQ(foldScreenManager_->activeScreenId_, foldScreenManager_->innerScreenId_);
    foldScreenManager_->HandleSensorData(180.f);
    EXPECT_EQ(foldScreenManager_->activeScreenId_, foldScreenManager_->innerScreenId_);
    foldScreenManager_->isPostureSensorDataHandled_ = false;
    foldScreenManager_->HandleSensorData(180.f);
    EXPECT_EQ(foldScreenManager_->activeScreenId_, foldScreenManager_->innerScreenId_);
    foldScreenManager_->HandleSensorData(0.f);
    EXPECT_EQ(foldScreenManager_->activeScreenId_, foldScreenManager_->externalScreenId_);
    foldScreenManager_->activeScreenId_ = 0;
}

/*
 * @tc.name: GetActiveScreenId_001
 * @tc.desc: Test GetActiveScreenId
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RSFoldScreenManagerTest, GetActiveScreenId_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, foldScreenManager_);
    auto activeScreenId = foldScreenManager_->GetActiveScreenId();
    bool isFoldScreenFlag = false;
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    isFoldScreenFlag = system::GetParameter("const.window.foldscreen.type", "") != "";
#endif
    if (isFoldScreenFlag) {
        ASSERT_NE(INVALID_SCREEN_ID, activeScreenId);
    } else {
        ASSERT_EQ(INVALID_SCREEN_ID, activeScreenId);
    }
}

/*
 * @tc.name: RegisterSensorCallbackTest001
 * @tc.desc: Test RegisterSensorCallback when hasRegisterSensorCallback_ is true
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSFoldScreenManagerTest, RegisterSensorCallbackTest001, TestSize.Level1)
{
    foldScreenManager_->hasRegisterSensorCallback_ = true;
    foldScreenManager_->RegisterSensorCallback();
    ASSERT_NE(foldScreenManager_, nullptr);
}

/*
 * @tc.name: RegisterSensorCallbackTest002
 * @tc.desc: Test RegisterSensorCallback when hasRegisterSensorCallback_ is false
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSFoldScreenManagerTest, RegisterSensorCallbackTest002, TestSize.Level1)
{
    foldScreenManager_->hasRegisterSensorCallback_ = false;
    foldScreenManager_->RegisterSensorCallback();
    ASSERT_NE(foldScreenManager_, nullptr);
}

/*
 * @tc.name: UnRegisterSensorCallbackTest001
 * @tc.desc: Test UnRegisterSensorCallback when hasRegisterSensorCallback_ is false
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSFoldScreenManagerTest, UnRegisterSensorCallbackTest001, TestSize.Level1)
{
    foldScreenManager_->hasRegisterSensorCallback_ = false;
    foldScreenManager_->UnRegisterSensorCallback();
    ASSERT_NE(foldScreenManager_, nullptr);
}

/*
 * @tc.name: UnRegisterSensorCallbackTest002
 * @tc.desc: Test UnRegisterSensorCallback when hasRegisterSensorCallback_ is true
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSFoldScreenManagerTest, UnRegisterSensorCallbackTest002, TestSize.Level1)
{
    foldScreenManager_->hasRegisterSensorCallback_ = true;
    foldScreenManager_->UnRegisterSensorCallback();
    ASSERT_NE(foldScreenManager_, nullptr);
}

/*
 * @tc.name: OnBootCompleteTest001
 * @tc.desc: Test OnBootComplete when key is equal but value is not equal
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSFoldScreenManagerTest, OnBootCompleteTest001, TestSize.Level1)
{
    string key = "bootevent.boot.completed";
    string value = "false";
    foldScreenManager_->OnBootComplete(key.c_str(), value.c_str(), nullptr);
    ASSERT_NE(foldScreenManager_, nullptr);
}

/*
 * @tc.name: OnBootCompleteTest002
 * @tc.desc: Test OnBootComplete when key is not equal and value is not equal
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSFoldScreenManagerTest, OnBootCompleteTest002, TestSize.Level1)
{
    string key = "bootevent.boot.uncompleted";
    string value = "false";
    foldScreenManager_->OnBootComplete(key.c_str(), value.c_str(), nullptr);
    ASSERT_NE(foldScreenManager_, nullptr);
}

/*
 * @tc.name: OnBootCompleteTest003
 * @tc.desc: Test OnBootComplete when key is not equal but value is equal
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSFoldScreenManagerTest, OnBootCompleteTest003, TestSize.Level1)
{
    string key = "bootevent.boot.uncompleted";
    string value = "true";
    foldScreenManager_->OnBootComplete(key.c_str(), value.c_str(), nullptr);
    ASSERT_NE(foldScreenManager_, nullptr);
}

/*
 * @tc.name: OnBootCompleteTest004
 * @tc.desc: Test OnBootComplete when key is equal, value is equal and context is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSFoldScreenManagerTest, OnBootCompleteTest004, TestSize.Level1)
{
    string key = "bootevent.boot.completed";
    string value = "true";
    foldScreenManager_->OnBootComplete(key.c_str(), value.c_str(), nullptr);
    ASSERT_NE(foldScreenManager_, nullptr);
}

/*
 * @tc.name: OnBootCompleteTest005
 * @tc.desc: Test OnBootComplete when key is equal, value is equal and context isn't nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSFoldScreenManagerTest, OnBootCompleteTest005, TestSize.Level1)
{
    string key = "bootevent.boot.completed";
    string value = "true";
    foldScreenManager_->OnBootComplete(key.c_str(), value.c_str(), foldScreenManager_);
    ASSERT_NE(foldScreenManager_, nullptr);
}

/*
 * @tc.name: HandlePostureDataTest001
 * @tc.desc: Test HandlePostureData when event is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSFoldScreenManagerTest, HandlePostureDataTest001, TestSize.Level1)
{
    foldScreenManager_->HandlePostureData(nullptr);
    ASSERT_NE(foldScreenManager_, nullptr);
}

/*
 * @tc.name: HandlePostureDataTest002
 * @tc.desc: Test HandlePostureData when event isn't nullptr but data is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSFoldScreenManagerTest, HandlePostureDataTest002, TestSize.Level1)
{
    auto event = std::make_shared<SensorEvent>();
    foldScreenManager_->HandlePostureData(event.get());
    ASSERT_NE(foldScreenManager_, nullptr);
}

/*
 * @tc.name: HandlePostureDataTest003
 * @tc.desc: Test HandlePostureData when event dataLen < sizeof(PostureData)
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSFoldScreenManagerTest, HandlePostureDataTest003, TestSize.Level1)
{
    auto data = std::make_shared<uint8_t>(1000);
    auto event = std::make_shared<SensorEvent>();
    event->data = data.get();
    foldScreenManager_->HandlePostureData(event.get());
    ASSERT_NE(foldScreenManager_, nullptr);
}

/*
 * @tc.name: HandlePostureDataTest004
 * @tc.desc: Test HandlePostureData when event dataLen >= sizeof(PostureData)
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSFoldScreenManagerTest, HandlePostureDataTest004, TestSize.Level1)
{
    auto data = std::make_shared<uint8_t>(1000);
    auto event = std::make_shared<SensorEvent>();
    event->data = data.get();
    event->dataLen = sizeof(PostureData);
    foldScreenManager_->HandlePostureData(event.get());
    ASSERT_NE(foldScreenManager_, nullptr);
}
} // namespace OHOS::Rosen