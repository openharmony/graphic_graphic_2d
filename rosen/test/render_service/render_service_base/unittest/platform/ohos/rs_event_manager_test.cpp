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
};

void RSEventMgrTest::SetUpTestCase() {}
void RSEventMgrTest::TearDownTestCase() {}
void RSEventMgrTest::SetUp() {}
void RSEventMgrTest::TearDown() {}

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
} // namespace Rosen
} // namespace OHOS