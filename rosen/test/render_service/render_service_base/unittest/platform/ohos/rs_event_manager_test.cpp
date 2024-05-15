/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "platform/common/rs_event_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSEventMgrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<RSEventManager> eventManager;
};

void RSEventMgrTest::SetUpTestCase() {}
void RSEventMgrTest::TearDownTestCase() {}
void RSEventMgrTest::SetUp()
{
    eventManager = std::make_shared<RSEventManager>();
}
void RSEventMgrTest::TearDown()
{
    eventManager.reset();
}

/**
 * @tc.name: AddEvent nullptr
 * @tc.desc: AddEvent nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventMgrTest, AddEvent_Nullptr, TestSize.Level1)
{
    RSEventManager* rsEventMgr = new RSEventManager();
    ASSERT_NE(rsEventMgr, nullptr);
    rsEventMgr->AddEvent(nullptr, 0);
    delete rsEventMgr;
}

/**
 * @tc.name: AddEvent eventIntervalMs less than zero
 * @tc.desc: AddEvent eventIntervalMs less than zero
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventMgrTest, AddEvent_LT0, TestSize.Level1)
{
    RSEventManager* rsEventMgr = new RSEventManager();
    ASSERT_NE(rsEventMgr, nullptr);
    std::shared_ptr<RSBaseEventDetector> rsEvntDtr = RSBaseEventDetector::CreateRSTimeOutDetector(0, "AddEvent_LT0");
    ASSERT_NE(rsEvntDtr, nullptr);
    rsEventMgr->AddEvent(rsEvntDtr, -1); // eventIntervalMs -1
    delete rsEventMgr;
}

/**
 * @tc.name: AddEvent eventIntervalMs greater than zero
 * @tc.desc: AddEvent eventIntervalMs greater than zero
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventMgrTest, AddEvent_GT0, TestSize.Level1)
{
    RSEventManager* rsEventMgr = new RSEventManager();
    ASSERT_NE(rsEventMgr, nullptr);
    std::shared_ptr<RSBaseEventDetector> rsEvntDtr = RSBaseEventDetector::CreateRSTimeOutDetector(0, "AddEvent_GT0");
    ASSERT_NE(rsEvntDtr, nullptr);
    rsEventMgr->AddEvent(rsEvntDtr, 1); // eventIntervalMs 1
    delete rsEventMgr;
}

/**
 * @tc.name: SetParam Test
 * @tc.desc: SetParam Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventMgrTest, SetParam_Test, TestSize.Level1)
{
    std::shared_ptr<RSBaseEventDetector> rsEvntDtr = RSBaseEventDetector::CreateRSTimeOutDetector(0, "SetParam_Test");
    ASSERT_NE(rsEvntDtr, nullptr);
    rsEvntDtr->SetParam("A_Key", "SetParam_Test");
}

/**
 * @tc.name: ClearTest
 * @tc.desc: Verify function Clear
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, ClearTest, TestSize.Level1)
{
    RSEventManager* rsEventManager = new RSEventManager();
    delete rsEventManager;
    rsEventManager = nullptr;
    EXPECT_FALSE(rsEventManager);
}

/**
 * @tc.name: DumpDetectorParamTest
 * @tc.desc: Verify function DumpDetectorParam
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, DumpDetectorParamTest, TestSize.Level1)
{
    auto detectorPtr = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    std::string dumpString = "0";
    std::string value = "0";
    std::string key = "0";
    eventManager->DumpDetectorParam(nullptr, dumpString);
    eventManager->DumpDetectorParam(detectorPtr, dumpString);
    detectorPtr->paramList_[key] = value;
    eventManager->DumpDetectorParam(detectorPtr, dumpString);
    EXPECT_FALSE(detectorPtr->paramList_.empty());
}

/**
 * @tc.name: DumpEventIntervalMsTest
 * @tc.desc: Verify function DumpEventIntervalMs
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, DumpEventIntervalMsTest, TestSize.Level1)
{
    auto detectorPtr = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    detectorPtr->stringId_ = "0";
    std::string dumpString = "0";
    eventManager->DumpEventIntervalMs(nullptr, dumpString);
    eventManager->DumpEventIntervalMs(detectorPtr, dumpString);
    RSEventState rsEventState;
    std::string key = "0";
    eventManager->eventStateList_[key] = rsEventState;
    eventManager->DumpEventIntervalMs(detectorPtr, dumpString);
    EXPECT_FALSE(eventManager->eventStateList_.empty());
}

/**
 * @tc.name: DumpAllEventParamTest
 * @tc.desc: Verify function DumpAllEventParam
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, DumpAllEventParamTest, TestSize.Level1)
{
    std::string dumpString = "0";
    auto detectorPtr = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    std::string key1 = "0";
    std::string key2 = "1";
    eventManager->eventDetectorList_[key1];
    eventManager->eventDetectorList_[key2] = detectorPtr;
    eventManager->DumpAllEventParam(dumpString);
    EXPECT_FALSE(eventManager->eventDetectorList_.empty());
}

/**
 * @tc.name: UpdateDetectorParamTest
 * @tc.desc: Verify function UpdateDetectorParam
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, UpdateDetectorParamTest, TestSize.Level1)
{
    auto detectorPtr = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    std::string key = "0";
    std::string value = "0";
    detectorPtr->paramList_[key] = value;
    eventManager->UpdateDetectorParam(nullptr);
    eventManager->UpdateDetectorParam(detectorPtr);
    EXPECT_FALSE(detectorPtr->paramList_.empty());
}

/**
 * @tc.name: UpdateEventIntervalMsTest
 * @tc.desc: Verify function UpdateEventIntervalMs
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, UpdateEventIntervalMsTest, TestSize.Level1)
{
    auto detectorPtr = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    detectorPtr->stringId_ = "0";
    RSEventState rsEventState;
    std::string key = "0";
    eventManager->UpdateEventIntervalMs(nullptr);
    eventManager->UpdateEventIntervalMs(detectorPtr);
    eventManager->eventStateList_[key] = rsEventState;
    eventManager->UpdateEventIntervalMs(detectorPtr);
    eventManager->eventStateList_.clear();
    detectorPtr->stringId_ = "-1";
    key = "-1";
    eventManager->eventStateList_[key] = rsEventState;
    eventManager->UpdateEventIntervalMs(detectorPtr);
    EXPECT_FALSE(eventManager->eventStateList_.empty());
}

/**
 * @tc.name: UpdateParamTest
 * @tc.desc: Verify function UpdateParam
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, UpdateParamTest, TestSize.Level1)
{
    auto detectorPtr = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    std::string key1 = "0";
    std::string key2 = "1";
    eventManager->eventDetectorList_[key1];
    eventManager->eventDetectorList_[key2] = detectorPtr;
    eventManager->UpdateParam();
    eventManager->updateCount_ = 200;
    eventManager->UpdateParam();
    EXPECT_EQ(eventManager->updateCount_, 0);
}

/**
 * @tc.name: AddEventTest
 * @tc.desc: Verify function AddEvent
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, AddEventTest, TestSize.Level1)
{
    auto detectorPtr = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    detectorPtr->stringId_ = "1";
    std::string key = "0";
    eventManager->eventDetectorList_[key] = detectorPtr;
    eventManager->AddEvent(detectorPtr, 0);
    eventManager->AddEvent(detectorPtr, 1);
    detectorPtr->stringId_ = "0";
    eventManager->AddEvent(detectorPtr, 1);
    EXPECT_FALSE(eventManager->eventDetectorList_.empty());
}

/**
 * @tc.name: RemoveEventTest
 * @tc.desc: Verify function RemoveEvent
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, RemoveEventTest, TestSize.Level1)
{
    auto detectorPtr = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    std::string key = "0";
    eventManager->eventDetectorList_[key] = detectorPtr;
    std::string stringId = "0";
    eventManager->RemoveEvent(stringId);
    stringId = "1";
    eventManager->RemoveEvent(stringId);
    EXPECT_FALSE(eventManager->eventDetectorList_.empty());
}

/**
 * @tc.name: EventReportTest
 * @tc.desc: Verify function EventReport
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, EventReportTest, TestSize.Level1)
{
    RSEventState rsEventState;
    std::string key = "0";
    eventManager->eventStateList_[key] = rsEventState;
    RSSysEventMsg eventMsg;
    eventMsg.stringId = "1";
    eventManager->EventReport(eventMsg);
    eventMsg.stringId = "0";
    eventManager->EventReport(eventMsg);
    EXPECT_FALSE(eventManager->eventStateList_.empty());
}
} // namespace Rosen
} // namespace OHOS