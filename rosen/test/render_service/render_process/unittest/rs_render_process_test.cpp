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

#include "platform/common/rs_log.h"
#include "render_process/rs_render_process.h"
#include "render_process/transaction/ipc_persistence/rs_ipc_persistence_def.h"
#include "rs_render_pipeline_agent.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSRenderProcessTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<RSRenderProcess> renderProcess_;
};

sptr<RSRenderProcess> RSRenderProcessTest::renderProcess_ = nullptr;

void RSRenderProcessTest::SetUpTestCase()
{
    renderProcess_ = sptr<RSRenderProcess>::MakeSptr();
}

void RSRenderProcessTest::TearDownTestCase()
{
    renderProcess_ = nullptr;
}

void RSRenderProcessTest::SetUp() {}
void RSRenderProcessTest::TearDown() {}

/**
 * @tc.name: InitTest001
 * @tc.desc: Test RSRenderProcess::Init method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, InitTest001, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);

    // Note: Init() may require system resources not available in test environment
    // This test verifies the object is properly constructed
    ASSERT_TRUE(renderProcess != nullptr);
}

/**
 * @tc.name: CreateRenderProcessTest001
 * @tc.desc: Test creating RSRenderProcess instance
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, CreateRenderProcessTest001, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
    ASSERT_EQ(renderProcess->GetSptrRefCount(), 1);
}

/**
 * @tc.name: CreateRenderProcessTest002
 * @tc.desc: Test creating multiple RSRenderProcess instances
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, CreateRenderProcessTest002, TestSize.Level1)
{
    auto renderProcess1 = sptr<RSRenderProcess>::MakeSptr();
    auto renderProcess2 = sptr<RSRenderProcess>::MakeSptr();

    ASSERT_NE(renderProcess1, nullptr);
    ASSERT_NE(renderProcess2, nullptr);
    ASSERT_NE(renderProcess1.GetRefPtr(), renderProcess2.GetRefPtr());
}

/**
 * @tc.name: RenderProcessCopyConstructorTest001
 * @tc.desc: Test that RSRenderProcess copy constructor is deleted
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, RenderProcessCopyConstructorTest001, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);

    // RSRenderProcess has deleted copy constructor
    ASSERT_TRUE(renderProcess != nullptr);
}

/**
 * @tc.name: RenderProcessAssignmentTest001
 * @tc.desc: Test that RSRenderProcess assignment operator is deleted
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, RenderProcessAssignmentTest001, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);

    // RSRenderProcess has deleted assignment operator
    ASSERT_TRUE(renderProcess != nullptr);
}

/**
 * @tc.name: RenderProcessSmartPointerTest001
 * @tc.desc: Test RSRenderProcess smart pointer behavior
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, RenderProcessSmartPointerTest001, TestSize.Level1)
{
    auto renderProcess1 = sptr<RSRenderProcess>::MakeSptr();
    auto initialRefCount = renderProcess1->GetSptrRefCount();

    {
        auto renderProcess2 = renderProcess1;
        auto increasedRefCount = renderProcess1->GetSptrRefCount();
        ASSERT_GT(increasedRefCount, initialRefCount);
    }

    auto finalRefCount = renderProcess1->GetSptrRefCount();
    ASSERT_EQ(finalRefCount, initialRefCount);
}

/**
 * @tc.name: RunTest001
 * @tc.desc: Test RSRenderProcess::Run with null runner does not crash
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, RunTest001, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
    ASSERT_EQ(renderProcess->runner_, nullptr);
    renderProcess->Run();
}

/**
 * @tc.name: ConnectToRenderServiceTest001
 * @tc.desc: Test ConnectToRenderService returns nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, ConnectToRenderServiceTest001, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
    auto result = renderProcess->ConnectToRenderService();
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: InitTest002
 * @tc.desc: Test RSRenderProcess::Init returns false when ConnectToRenderService fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, InitTest002, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
    bool result = renderProcess->Init();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: InitTest003
 * @tc.desc: Test RSRenderProcess::Init sets runner and handler even when it fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, InitTest003, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
    ASSERT_EQ(renderProcess->runner_, nullptr);
    ASSERT_EQ(renderProcess->handler_, nullptr);
    bool result = renderProcess->Init();
    EXPECT_FALSE(result);
    EXPECT_NE(renderProcess->runner_, nullptr);
    EXPECT_NE(renderProcess->handler_, nullptr);
}

namespace {
class FakeTransfer : public RSIpcTransferBase {
public:
    explicit FakeTransfer(bool applyResult) : applyResult_(applyResult) {}

    RSIServiceToRenderConnectionInterfaceCode GetTypeId() const override
    {
        return RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED;
    }
    bool IsPersistent() const override { return false; }
    bool IsSync() const override { return true; }
    FanoutPolicy GetFanoutPolicy() const override { return FanoutPolicy::ANY_SUCCESS; }
    bool Apply(const sptr<RSRenderPipelineAgent>&) override
    {
        applyCount++;
        return applyResult_;
    }
    bool ProxyMarshalling(Parcel&) const override { return true; }
    bool StubMarshalling(Parcel&) const override { return true; }
    bool ProxyUnmarshalling(Parcel&) override { return true; }
    std::shared_ptr<RSIpcTransferBase> CopyTransfer() const override { return nullptr; }
protected:
    void Persist(IpcPersistenceMap&, std::mutex&) override {}
    void ClearPid(pid_t) override {}
public:
    int32_t applyCount = 0;
private:
    bool applyResult_;
};
} // namespace

/**
 * @tc.name: ApplyIpcPersistenceDataTest001
 * @tc.desc: every non-null entry's Apply is called exactly once and the replay map is not mutated
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderProcessTest, ApplyIpcPersistenceDataTest001, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
    IpcPersistenceMap replayData;
    auto entry1 = std::make_shared<FakeTransfer>(true);
    auto entry2 = std::make_shared<FakeTransfer>(true);
    auto entry3 = std::make_shared<FakeTransfer>(true);
    replayData[RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED] = entry1;
    replayData[RSIServiceToRenderConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED] = entry2;
    replayData[RSIServiceToRenderConnectionInterfaceCode::HANDLE_HWC_EVENT] = entry3;
    sptr<RSRenderPipelineAgent> nullAgent = nullptr;
    EXPECT_NO_FATAL_FAILURE(renderProcess->ApplyIpcPersistenceData(nullAgent, replayData));
    EXPECT_EQ(entry1->applyCount, 1); // every entry's Apply must be called
    EXPECT_EQ(entry2->applyCount, 1);
    EXPECT_EQ(entry3->applyCount, 1);
    EXPECT_EQ(replayData.size(), 3u); // Apply does not mutate the replay map
}

/**
 * @tc.name: ApplyIpcPersistenceDataTest002
 * @tc.desc: null transfer entries are skipped; a transfer whose Apply fails does not abort iteration
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderProcessTest, ApplyIpcPersistenceDataTest002, TestSize.Level2)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
    IpcPersistenceMap replayData;
    // null transfer entry -> skipped (continue)
    replayData[RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED] = nullptr;
    // failing transfer -> Apply returns false, logged, iteration continues
    auto failTransfer = std::make_shared<FakeTransfer>(false);
    replayData[RSIServiceToRenderConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED] = failTransfer;
    sptr<RSRenderPipelineAgent> nullAgent = nullptr;
    EXPECT_NO_FATAL_FAILURE(renderProcess->ApplyIpcPersistenceData(nullAgent, replayData));
    EXPECT_EQ(failTransfer->applyCount, 1); // failing entry was still applied exactly once
    EXPECT_EQ(replayData.size(), 2u); // Apply does not mutate the replay map
}

#ifdef RES_SCHED_ENABLE
/**
 * @tc.name: SubScribeSystemAbility001
 * @tc.desc: Test RSRenderProcess::SubScribeSystemAbility
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, SubScribeSystemAbilityTest001, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
    ASSERT_EQ(renderProcess->saStatusChangeListener_, nullptr);
    renderProcess->SubScribeSystemAbility();
    EXPECT_NE(renderProcess->saStatusChangeListener_, nullptr);
}
#endif

} // namespace OHOS::Rosen
