/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "limit_number.h"

#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "transaction/rs_transaction_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderThreadTest::SetUpTestCase() {}
void RSRenderThreadTest::TearDownTestCase() {}
void RSRenderThreadTest::SetUp() {}
void RSRenderThreadTest::TearDown() {}

/**
 * @tc.name: Detach001
 * @tc.desc: test results of Detach
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, Detach001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Detach
     */
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRenderThread::Instance().Detach(nodeId);
}

/**
 * @tc.name: Detach002
 * @tc.desc: test results of Detach
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, Detach002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Detach
     */
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSCanvasRenderNode node(nodeId);
    RSRenderThread::Instance().Detach(node.GetId());
}

/**
 * @tc.name: RecvTransactionData001
 * @tc.desc: test results of Detach
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, RecvTransactionData001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. PrepareChildren
     */
    std::unique_ptr<RSTransactionData> data = std::make_unique<RSTransactionData>();
    RSRenderThread::Instance().RecvTransactionData(data);
}

/**
 * @tc.name: PostPreTask001
 * @tc.desc: test results of PostPreTask
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, PostPreTask001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. PrepareChildren
     */
    RSRenderThread::Instance().PostPreTask();
}

/**
 * @tc.name: UpdateWindowStatus001
 * @tc.desc: test results of PostPreTask
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, UpdateWindowStatus001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. PrepareChildren
     */
    RSRenderThread::Instance().UpdateWindowStatus(false);
    RSRenderThread::Instance().UpdateWindowStatus(true);
}
} // namespace OHOS::Rosen