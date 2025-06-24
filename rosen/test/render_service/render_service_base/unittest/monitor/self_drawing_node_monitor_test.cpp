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
#include "ipc_callbacks/rs_self_drawing_node_rect_change_callback_stub.h"
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
void SelfDrawingNodeMonitorTest::TearDownTestCase()
{
    monitor_->ClearRectMap();
    monitor_->rectChangeCallbackListenner_.clear();
    monitor_->rectChangeCallbackFilter_.clear();
}
void SelfDrawingNodeMonitorTest::SetUp() {}
void SelfDrawingNodeMonitorTest::TearDown()
{
    monitor_->ClearRectMap();
    monitor_->rectChangeCallbackListenner_.clear();
    monitor_->rectChangeCallbackFilter_.clear();
}
SelfDrawingNodeMonitor* SelfDrawingNodeMonitorTest::monitor_ = nullptr;

class RSSelfDrawingNodeRectChangeCallbackStubMock : public RSSelfDrawingNodeRectChangeCallbackStub {
public:
    RSSelfDrawingNodeRectChangeCallbackStubMock() = default;
    ~RSSelfDrawingNodeRectChangeCallbackStubMock() = default;
    void OnSelfDrawingNodeRectChange(std::shared_ptr<RSSelfDrawingNodeRectData> data) override {};
};

/*
 * @tc.name: InsertCurRectMapTest
 * @tc.desc: Test InsertCurRectMap
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, InsertCurRectMapTest, TestSize.Level1)
{
    NodeId id = 0;
    RectI rect = RectI(0, 0, 1, 1);
    monitor_->InsertCurRectMap(id, rect);
    ASSERT_EQ(monitor_->curRect_.size(), 1);
    monitor_->EraseCurRectMap(id);
    ASSERT_EQ(monitor_->curRect_.size(), 0);
    monitor_->InsertCurRectMap(id, rect);
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
    RectFilter filter;
    monitor_->RegisterRectChangeCallback(0, filter, callback);
    ASSERT_TRUE(monitor_-> IsListeningEnabled());
    NodeId id = 0;
    RectI rect = RectI(0, 0, 1, 1);
    monitor_->InsertCurRectMap(id, rect);
    ASSERT_NE(monitor_->curRect_, monitor_->lastRect_);
    monitor_->TriggerRectChangeCallback();
}

/*
 * @tc.name: UnRegisterRectChangeCallbackTest001
 * @tc.desc: Test UnRegisterRectChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, UnRegisterRectChangeCallbackTest001, TestSize.Level1)
{
    sptr<RSISelfDrawingNodeRectChangeCallback> callback;
    RectFilter filter;
    monitor_->RegisterRectChangeCallback(0, filter, callback);
    ASSERT_TRUE(monitor_-> IsListeningEnabled());
    monitor_->TriggerRectChangeCallback();
}

/*
 * @tc.name: UnRegisterRectChangeCallbackTest002
 * @tc.desc: Test UnRegisterRectChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, UnRegisterRectChangeCallbackTest002, TestSize.Level1)
{
    sptr<RSISelfDrawingNodeRectChangeCallback> callback;
    RectFilter filter;
    monitor_->RegisterRectChangeCallback(0, filter, callback);
    ASSERT_TRUE(monitor_-> IsListeningEnabled());
    auto pid = monitor_->rectChangeCallbackListenner_.begin()->first;
    monitor_->UnRegisterRectChangeCallback(pid);
    ASSERT_FALSE(monitor_->IsListeningEnabled());
    pid_t callingPid = 0;
    monitor_->rectChangeCallbackListenner_.insert(std::make_pair(callingPid, callback));
    callingPid = 1;
    monitor_->rectChangeCallbackListenner_.insert(std::make_pair(callingPid, callback));
    monitor_->UnRegisterRectChangeCallback(0);
    ASSERT_TRUE(monitor_->IsListeningEnabled());
    monitor_->UnRegisterRectChangeCallback(1);
    ASSERT_TRUE(monitor_->IsListeningEnabled());
    monitor_->TriggerRectChangeCallback();
}

/*
 * @tc.name: TriggerRectChangeCallbackTest001
 * @tc.desc: Test TriggerRectChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, TriggerRectChangeCallbackTest001, TestSize.Level1)
{
    sptr<RSISelfDrawingNodeRectChangeCallback> callback = new RSSelfDrawingNodeRectChangeCallbackStubMock();
    ASSERT_NE(callback, nullptr);
    pid_t callingPid = 0;
    RectFilter filter;
    filter.pids = { 0 };
    monitor_->rectChangeCallbackListenner_[callingPid] = callback;
    monitor_->rectChangeCallbackFilter_[callingPid] = filter;
    ASSERT_TRUE(monitor_->IsListeningEnabled());
    NodeId id = 0;
    RectI rect = RectI(0, 0, 0, 0);
    monitor_->InsertCurRectMap(id, rect);
    ASSERT_NE(monitor_->curRect_, monitor_->lastRect_);
    monitor_->TriggerRectChangeCallback();
}

/*
 * @tc.name: TriggerRectChangeCallbackTest002
 * @tc.desc: Test TriggerRectChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, TriggerRectChangeCallbackTest002, TestSize.Level1)
{
    sptr<RSISelfDrawingNodeRectChangeCallback> callback = nullptr;
    pid_t callingPid = 0;
    RectFilter filter;
    filter.pids = { 0 };
    monitor_->rectChangeCallbackListenner_[callingPid] = callback;
    monitor_->rectChangeCallbackFilter_[callingPid] = filter;
    ASSERT_TRUE(monitor_->IsListeningEnabled());
    NodeId id = 0;
    RectI rect = RectI(0, 0, 0, 0);
    monitor_->InsertCurRectMap(id, rect);
    ASSERT_NE(monitor_->curRect_, monitor_->lastRect_);
    monitor_->TriggerRectChangeCallback();
}

/*
 * @tc.name: TriggerRectChangeCallbackTest003
 * @tc.desc: Test TriggerRectChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, TriggerRectChangeCallbackTest003, TestSize.Level1)
{
    sptr<RSISelfDrawingNodeRectChangeCallback> callback = new RSSelfDrawingNodeRectChangeCallbackStubMock();
    ASSERT_NE(callback, nullptr);
    pid_t callingPid = 0;
    RectFilter filter;
    monitor_->rectChangeCallbackListenner_[callingPid] = callback;
    monitor_->rectChangeCallbackFilter_[callingPid] = filter;
    ASSERT_TRUE(monitor_->IsListeningEnabled());
    NodeId id = 0;
    RectI rect = RectI(0, 0, 1, 1);
    monitor_->InsertCurRectMap(id, rect);
    ASSERT_NE(monitor_->curRect_, monitor_->lastRect_);
    monitor_->TriggerRectChangeCallback();
}

/*
 * @tc.name: TriggerRectChangeCallbackTest004
 * @tc.desc: Test TriggerRectChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, TriggerRectChangeCallbackTest004, TestSize.Level1)
{
    sptr<RSISelfDrawingNodeRectChangeCallback> callback = nullptr;
    pid_t callingPid = 0;
    RectFilter filter;
    monitor_->rectChangeCallbackListenner_[callingPid] = callback;
    monitor_->rectChangeCallbackFilter_[callingPid] = filter;
    ASSERT_TRUE(monitor_->IsListeningEnabled());
    NodeId id = 0;
    RectI rect = RectI(0, 0, 1, 1);
    monitor_->InsertCurRectMap(id, rect);
    ASSERT_NE(monitor_->curRect_, monitor_->lastRect_);
    monitor_->TriggerRectChangeCallback();
}

/*
 * @tc.name: TriggerRectChangeCallbackTest005
 * @tc.desc: Test TriggerRectChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, TriggerRectChangeCallbackTest005, TestSize.Level1)
{
    sptr<RSISelfDrawingNodeRectChangeCallback> callback;
    pid_t callingPid = 0;
    RectFilter filter;
    monitor_->rectChangeCallbackListenner_[callingPid] = callback;
    ASSERT_TRUE(monitor_->IsListeningEnabled());
    NodeId id = 0;
    RectI rect = RectI(0, 0, 1, 1);
    monitor_->InsertCurRectMap(id, rect);
    ASSERT_NE(monitor_->curRect_, monitor_->lastRect_);
    monitor_->TriggerRectChangeCallback();
}

/*
 * @tc.name: ShouldTriggerTest001
 * @tc.desc: Test ShouldTrigger when add rect element
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, ShouldTriggerTest001, TestSize.Level1)
{
    pid_t callingPid = 0;
    RectFilter filter;
    filter.pids = { 0 };
    SelfDrawingNodeRectCallbackData callbackData;
    monitor_->rectChangeCallbackFilter_[callingPid] = filter;
    NodeId id = 0;
    RectI rect = RectI(0, 0, 1, 1);
    monitor_->InsertCurRectMap(id, rect);
    EXPECT_FALSE(monitor_->ShouldTrigger(filter, callbackData));

    filter.range.highLimit.width = 1;
    filter.range.highLimit.height = 1;
    monitor_->rectChangeCallbackFilter_[callingPid] = filter;
    EXPECT_TRUE(monitor_->ShouldTrigger(filter, callbackData));
}

/*
 * @tc.name: ShouldTriggerTest002
 * @tc.desc: Test ShouldTrigger when change rect element
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, ShouldTriggerTest002, TestSize.Level1)
{
    pid_t callingPid = 0;
    RectFilter filter;
    filter.pids = { 0 };
    SelfDrawingNodeRectCallbackData callbackData;
    filter.range.highLimit.width = 1;
    filter.range.highLimit.height = 1;
    monitor_->rectChangeCallbackFilter_[callingPid] = filter;
    NodeId id = 0;
    RectI rect = RectI(0, 0, 1, 1);
    monitor_->InsertCurRectMap(id, rect);
    rect = RectI(0, 0, 1, 1);
    monitor_->lastRect_[id] = rect;
    EXPECT_FALSE(monitor_->ShouldTrigger(filter, callbackData));

    rect = RectI(0, 0, 2, 2);
    monitor_->lastRect_[id] = rect;
    EXPECT_TRUE(monitor_->ShouldTrigger(filter, callbackData));
}

/*
 * @tc.name: ShouldTriggerTest003
 * @tc.desc: Test ShouldTrigger when remove rect element
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, ShouldTriggerTest003, TestSize.Level1)
{
    pid_t callingPid = 0;
    NodeId id = 0;
    RectFilter filter;
    filter.pids = {};
    monitor_->rectChangeCallbackFilter_[callingPid] = filter;
    SelfDrawingNodeRectCallbackData callbackData;
    RectI rect = RectI(0, 0, 0, 0);
    monitor_->lastRect_[id] = rect;
    EXPECT_FALSE(monitor_->ShouldTrigger(filter, callbackData));

    filter.pids = { 0 };
    monitor_->rectChangeCallbackFilter_[callingPid] = filter;
    EXPECT_TRUE(monitor_->ShouldTrigger(filter, callbackData));

    filter.range.highLimit.width = 1;
    filter.range.highLimit.height = 1;
    rect = RectI(0, 0, 2, 2);
    monitor_->lastRect_[id] = rect;
    EXPECT_FALSE(monitor_->ShouldTrigger(filter, callbackData));

    rect = RectI(0, 0, 0, 0);
    monitor_->InsertCurRectMap(id, rect);
    EXPECT_FALSE(monitor_->ShouldTrigger(filter, callbackData));
}

/*
 * @tc.name: CheckStatifyTest
 * @tc.desc: Test CheckStatify
 * @tc.type: FUNC
 */
HWTEST_F(SelfDrawingNodeMonitorTest, CheckStatifyTest, TestSize.Level1)
{
    RectFilter filter;
    filter.range.lowLimit.width = 1;
    filter.range.lowLimit.height = 1;
    filter.range.highLimit.width = 1;
    filter.range.highLimit.height = 1;
    RectI rect = RectI(0, 0, 0, 0);
    EXPECT_EQ(monitor_->CheckStatify(rect, filter), false);
    rect = RectI(1, 0, 0, 0);
    EXPECT_EQ(monitor_->CheckStatify(rect, filter), false);
    rect = RectI(1, 1, 0, 0);
    EXPECT_EQ(monitor_->CheckStatify(rect, filter), false);
    rect = RectI(1, 1, 1, 0);
    EXPECT_EQ(monitor_->CheckStatify(rect, filter), false);
    rect = RectI(1, 1, 1, 1);
    EXPECT_EQ(monitor_->CheckStatify(rect, filter), true);
    rect = RectI(2, 0, 0, 0);
    EXPECT_EQ(monitor_->CheckStatify(rect, filter), false);
    rect = RectI(1, 2, 0, 0);
    EXPECT_EQ(monitor_->CheckStatify(rect, filter), false);
    rect = RectI(1, 1, 2, 0);
    EXPECT_EQ(monitor_->CheckStatify(rect, filter), false);
    rect = RectI(1, 1, 1, 2);
    EXPECT_EQ(monitor_->CheckStatify(rect, filter), false);
}
} // namespace OHOS::Rosen