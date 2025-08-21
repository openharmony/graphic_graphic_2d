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

#include <memory>
#include "gtest/gtest.h"
#include "limit_number.h"

#include "pipeline/main_thread/rs_render_service_connection.h"
#include "pipeline/rs_test_util.h"
#include "platform/ohos/rs_render_service_connect_hub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderServiceConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceConnectionTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSRenderServiceConnectionTest::TearDownTestCase() {}
void RSRenderServiceConnectionTest::SetUp() {}
void RSRenderServiceConnectionTest::TearDown() {}

/**
 * @tc.name: GetMemoryGraphic001
 * @tc.desc: GetMemoryGraphic
 * @tc.type: FUNC
 * @tc.require:issueI590LM
 */
HWTEST_F(RSRenderServiceConnectionTest, GetMemoryGraphic001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    auto rsRenderServiceConnection = new RSRenderServiceConnection(
        0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    MemoryGraphic mem1;
    rsRenderServiceConnection->GetMemoryGraphic(123, mem1);
    ASSERT_EQ(mem1.GetGpuMemorySize(), 0);
    MemoryGraphic mem2;
    rsRenderServiceConnection->GetMemoryGraphic(0, mem2);
    ASSERT_EQ(mem2.GetGpuMemorySize(), 0);
}

/**
 * @tc.name: GetMemoryGraphic002
 * @tc.desc: GetMemoryGraphic
 * @tc.type: FUNC
 * @tc.require:issueI590LM
 */
HWTEST_F(RSRenderServiceConnectionTest, GetMemoryGraphic002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    auto rsRenderServiceConnection = new RSRenderServiceConnection(
        0, nullptr, mainThread, CreateOrGetScreenManager(), token->AsObject(), nullptr);
    std::vector<MemoryGraphic> memoryGraphics;
    rsRenderServiceConnection->GetMemoryGraphics(memoryGraphics);
    ASSERT_EQ(memoryGraphics.size(), 0);
}
} // namespace OHOS::Rosen