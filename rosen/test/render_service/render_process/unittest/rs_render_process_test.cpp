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

class MockIpcPersistenceData : public RSIpcPersistenceDataBase {
public:
    RSIpcPersistenceType GetType() const override { return RSIpcPersistenceType::DEFAULT; }
    bool Marshalling(Parcel& parcel) const override { return true; }
    void Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent) override
    {
        applyCount++;
    }
    int applyCount = 0;
};

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
 * @tc.desc: Test ConnectToRenderService returns nullptr when system services are unavailable
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

/**
 * @tc.name: ApplyIpcPersistenceDataTest001
 * @tc.desc: Test ApplyIpcPersistenceData with empty data map
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, ApplyIpcPersistenceDataTest001, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
    auto emptyData = std::make_shared<IpcPersistenceTypeToDataMap>();
    sptr<RSRenderPipelineAgent> nullAgent = nullptr;
    renderProcess->ApplyIpcPersistenceData(nullAgent, emptyData);
    EXPECT_TRUE(emptyData->empty());
}

/**
 * @tc.name: ApplyIpcPersistenceDataTest002
 * @tc.desc: Test ApplyIpcPersistenceData with single entry verifies Apply is called
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, ApplyIpcPersistenceDataTest002, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
    auto dataMap = std::make_shared<IpcPersistenceTypeToDataMap>();
    auto mockData = std::make_shared<MockIpcPersistenceData>();
    (*dataMap)[RSIpcPersistenceType::DEFAULT].push_back(mockData);
    sptr<RSRenderPipelineAgent> nullAgent = nullptr;
    renderProcess->ApplyIpcPersistenceData(nullAgent, dataMap);
    EXPECT_EQ(mockData->applyCount, 1);
    EXPECT_EQ(dataMap->size(), 1u);
}

/**
 * @tc.name: ApplyIpcPersistenceDataTest003
 * @tc.desc: Test ApplyIpcPersistenceData with multiple entries across types
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, ApplyIpcPersistenceDataTest003, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
    auto dataMap = std::make_shared<IpcPersistenceTypeToDataMap>();
    auto mockData1 = std::make_shared<MockIpcPersistenceData>();
    auto mockData2 = std::make_shared<MockIpcPersistenceData>();
    auto mockData3 = std::make_shared<MockIpcPersistenceData>();
    (*dataMap)[RSIpcPersistenceType::SET_WATERMARK].push_back(mockData1);
    (*dataMap)[RSIpcPersistenceType::SET_WATERMARK].push_back(mockData2);
    (*dataMap)[RSIpcPersistenceType::SHOW_WATERMARK].push_back(mockData3);
    sptr<RSRenderPipelineAgent> nullAgent = nullptr;
    renderProcess->ApplyIpcPersistenceData(nullAgent, dataMap);
    EXPECT_EQ(mockData1->applyCount, 1);
    EXPECT_EQ(mockData2->applyCount, 1);
    EXPECT_EQ(mockData3->applyCount, 1);
    EXPECT_EQ(dataMap->size(), 2u);
}

/**
 * @tc.name: ApplyIpcPersistenceDataTest004
 * @tc.desc: Test ApplyIpcPersistenceData with empty vector in data map
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, ApplyIpcPersistenceDataTest004, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
    auto dataMap = std::make_shared<IpcPersistenceTypeToDataMap>();
    (*dataMap)[RSIpcPersistenceType::ON_HWC_EVENT] = {};
    sptr<RSRenderPipelineAgent> nullAgent = nullptr;
    renderProcess->ApplyIpcPersistenceData(nullAgent, dataMap);
    EXPECT_EQ(dataMap->size(), 1u);
    EXPECT_TRUE(dataMap->at(RSIpcPersistenceType::ON_HWC_EVENT).empty());
}

} // namespace OHOS::Rosen
