/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "monitor/self_drawing_node_monitor.h"
#include "transaction/rs_render_service_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class SelfDrawingNodeMonitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static SelfDrawingNodeMonitor* monitor_;
};

void SelfDrawingNodeMonitorTest::SetUpTestCase()
{
    monitor_ = &SelfDrawingNodeMonitor::GetInstance();
}
void SelfDrawingNodeMonitorTest::TearDownTestCase() {}
void SelfDrawingNodeMonitorTest::SetUp() {}
void SelfDrawingNodeMonitorTest::TearDown() {}
SelfDrawingNodeMonitor* SelfDrawingNodeMonitorTest::monitor_ = nullptr;

/*
 * @tc.name: IsListeningEnabledTest
 * @tc.desc: Test IsListeningEnabled
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, IsListeningEnabledTest, TestSize.Level1)
{
    sptr<RSISelfDrawingNodeRectChangeCallback> callback;
    monitor_->RegisterRectChangeCallback(0, callback);
    ASSERT_TRUE(monitor_-> IsListeningEnabled());
    monitor_->UnRegisterRectChangeCallback(0);
    ASSERT_FALSE(monitor_-> IsListeningEnabled());
}

/*
 * @tc.name: InsertCurRectMapTest
 * @tc.desc: Test InsertCurRectMap
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, InsertCurRectMapTest, TestSize.Level1)
{
    NodeId id = 0;
    std::string nodeName = "testNode";
    RectI rect = RectI(0, 0, 1, 1);
    monitor_->InsertCurRectMap(id, nodeName, rect);
    ASSERT_EQ(monitor_->curRect_.size(), 1);
    monitor_->EraseCurRectMap(id);
    ASSERT_EQ(monitor_->curRect_.size(), 0);
    monitor_->InsertCurRectMap(id, nodeName, rect);
    ASSERT_EQ(monitor_->curRect_.size(), 1);
    monitor_->ClearRectMap();
    ASSERT_EQ(monitor_->curRect_.size(), 0);
}

/*
 * @tc.name: SelfDrawingNodeRectCallbackTest
 * @tc.desc: Test TriggerRectChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, SelfDrawingNodeRectCallbackTest, TestSize.Level1)
{
    sptr<RSISelfDrawingNodeRectChangeCallback> callback;
    monitor_->RegisterRectChangeCallback(0, callback);
    ASSERT_TRUE(monitor_-> IsListeningEnabled());
    NodeId id = 0;
    std::string nodeName = "testNode";
    RectI rect = RectI(0, 0, 1, 1);
    monitor_->InsertCurRectMap(id, nodeName, rect);
    ASSERT_NE(monitor_->curRect_, monitor_->lastRect_);
    monitor_->TriggerRectChangeCallback();
}
} // namespace OHOS::Rosen