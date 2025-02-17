/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
 * @tc.name: AddEvent eventIntervalMs less than zero
 * @tc.desc: AddEvent eventIntervalMs less than zero
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventMgrTest, AddEventLT0, TestSize.Level1)
{
    RSEventManager* rsEventMgr1 = new RSEventManager();
    ASSERT_NE(rsEventMgr1, nullptr);
    std::shared_ptr<RSBaseEventDetector> rsEvntDtr1 = RSBaseEventDetector::CreateRSTimeOutDetector(0, "AddEvent_LT0");
    ASSERT_NE(rsEvntDtr1, nullptr);
    rsEventMgr1->AddEvent(rsEvntDtr1, -1); // eventIntervalMs -1
    delete rsEventMgr1;
}

/**
 * @tc.name: AddEvent nullptr
 * @tc.desc: AddEvent nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventMgrTest, AddEventNullptr, TestSize.Level1)
{
    RSEventManager* rsEventMgr1 = new RSEventManager();
    ASSERT_NE(rsEventMgr1, nullptr);
    rsEventMgr1->AddEvent(nullptr, 0);
    delete rsEventMgr1;
}

/**
 * @tc.name: AddEvent eventIntervalMs greater than zero
 * @tc.desc: AddEvent eventIntervalMs greater than zero
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventMgrTest, AddEventGT0, TestSize.Level1)
{
    RSEventManager* rsEventMgr1 = new RSEventManager();
    ASSERT_NE(rsEventMgr1, nullptr);
    std::shared_ptr<RSBaseEventDetector> rsEvntDtr1 = RSBaseEventDetector::CreateRSTimeOutDetector(0, "AddEvent_GT0");
    ASSERT_NE(rsEvntDtr1, nullptr);
    rsEventMgr1->AddEvent(rsEvntDtr, 1); // eventIntervalMs 1
    delete rsEventMgr1;
}

/**
 * @tc.name: ClearTest001
 * @tc.desc: Verify function Clear
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, ClearTest001, TestSize.Level1)
{
    RSEventManager* rsEventManager1 = new RSEventManager();
    delete rsEventManager1;
    rsEventManager1 = nullptr;
    EXPECT_FALSE(rsEventManager1);
}

/**
 * @tc.name: SetParam Test
 * @tc.desc: SetParam Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventMgrTest, SetParamTest, TestSize.Level1)
{
    std::shared_ptr<RSBaseEventDetector> rsEvntDtr1 = RSBaseEventDetector::CreateRSTimeOutDetector(0, "SetParam_Test");
    ASSERT_NE(rsEvntDtr1, nullptr);
    rsEvntDtr1->SetParam("A_Key", "SetParam_Test");
}

/**
 * @tc.name: DumpEventIntervalMsTest001
 * @tc.desc: Verify function DumpEventIntervalMs
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, DumpEventIntervalMsTest001, TestSize.Level1)
{
    std::string dumpString = "0";
    RSEventState rsEventState;
    std::string key0 = "0";
    auto detectorPtr1 = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    detectorPtr1->stringId_ = "0";
    eventManager->DumpEventIntervalMs(nullptr, dumpString);
    eventManager->DumpEventIntervalMs(detectorPtr1, dumpString);
    eventManager->eventStateList_[key0] = rsEventState;
    eventManager->DumpEventIntervalMs(detectorPtr1, dumpString);
    EXPECT_FALSE(eventManager->eventStateList_.empty());
}

/**
 * @tc.name: DumpDetectorParamTest001
 * @tc.desc: Verify function DumpDetectorParam
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, DumpDetectorParamTest001, TestSize.Level1)
{
    std::string value0 = "0";
    std::string dumpString = "0";
    std::string key0 = "0";
    auto detectorPtr1 = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    eventManager->DumpDetectorParam(nullptr, dumpString);
    eventManager->DumpDetectorParam(detectorPtr1, dumpString);
    detectorPtr1->paramList_[key0] = value0;
    eventManager->DumpDetectorParam(detectorPtr1, dumpString);
    EXPECT_FALSE(detectorPtr1->paramList_.empty());
}

/**
 * @tc.name: DumpAllEventParamTest001
 * @tc.desc: Verify function DumpAllEventParam
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, DumpAllEventParamTest001, TestSize.Level1)
{
    std::string key0 = "0";
    std::string key1 = "1";
    std::string dumpString = "0";
    std::shared_ptr<RSBaseEventDetector> sharedPtrNULL = nullptr;
    std::weak_ptr<RSBaseEventDetector> detectorPtrNULL = sharedPtrNULL;
    auto detectorPtr = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    eventManager->eventDetectorList_[key0] = detectorPtrNULL;
    eventManager->eventDetectorList_[key1] = detectorPtr;
    eventManager->DumpAllEventParam(dumpString);
    EXPECT_FALSE(eventManager->eventDetectorList_.empty());
}

/**
 * @tc.name: UpdateEventIntervalMsTest001
 * @tc.desc: Verify function UpdateEventIntervalMs
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, UpdateEventIntervalMsTest001, TestSize.Level1)
{
    std::string key0 = "0";
    RSEventState rsEventState;
    auto detectorPtr = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    detectorPtr->stringId_ = "0";
    eventManager->UpdateEventIntervalMs(nullptr);
    eventManager->UpdateEventIntervalMs(detectorPtr);
    eventManager->eventStateList_[key0] = rsEventState;
    eventManager->UpdateEventIntervalMs(detectorPtr);
    eventManager->eventStateList_.clear();
    detectorPtr->stringId_ = "-1";

    std::string key1 = "-1";
    eventManager->eventStateList_[key1] = rsEventState;
    eventManager->UpdateEventIntervalMs(detectorPtr);
    EXPECT_FALSE(eventManager->eventStateList_.empty());
}

/**
 * @tc.name: UpdateDetectorParamTest001
 * @tc.desc: Verify function UpdateDetectorParam
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, UpdateDetectorParamTest001, TestSize.Level1)
{
    std::string key0 = "0";
    std::string value0 = "0";
    auto detectorPtr = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    detectorPtr->paramList_[key0] = value0;
    eventManager->UpdateDetectorParam(nullptr);
    eventManager->UpdateDetectorParam(detectorPtr);
    EXPECT_FALSE(detectorPtr->paramList_.empty());
}

/**
 * @tc.name: AddEventTest001
 * @tc.desc: Verify function AddEvent
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, AddEventTest001, TestSize.Level1)
{
    std::string key0 = "0";
    auto detectorPtr1 = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    detectorPtr1->stringId_ = "1";
    eventManager->eventDetectorList_[key0] = detectorPtr1;
    eventManager->AddEvent(detectorPtr1, 0);
    eventManager->AddEvent(detectorPtr1, 1);
    detectorPtr1->stringId_ = "0";
    eventManager->AddEvent(detectorPtr1, 1);
    EXPECT_FALSE(eventManager->eventDetectorList_.empty());
}

/**
 * @tc.name: EventReportTest001
 * @tc.desc: Verify function EventReport
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, EventReportTest001, TestSize.Level1)
{
    std::string key0 = "0";
    RSEventState rsEventState;
    eventManager->eventStateList_[key0] = rsEventState;
    RSSysEventMsg eventMsg10;
    eventMsg10.stringId = "1";
    eventManager->EventReport(eventMsg10);
    eventMsg10.stringId = "0";
    eventManager->EventReport(eventMsg10);
    EXPECT_FALSE(eventManager->eventStateList_.empty());
}

/**
 * @tc.name: RemoveEventTest001
 * @tc.desc: Verify function RemoveEvent
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventMgrTest, RemoveEventTest001, TestSize.Level1)
{
    auto detectorPtr = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
    std::string key0 = "0";
    eventManager->eventDetectorList_[key0] = detectorPtr;
    std::string stringId01 = "0";
    eventManager->RemoveEvent(stringId01);
    stringId01 = "1";
    eventManager->RemoveEvent(stringId01);
    EXPECT_FALSE(eventManager->eventDetectorList_.empty());
}
} // namespace Rosen
} // namespace OHOS