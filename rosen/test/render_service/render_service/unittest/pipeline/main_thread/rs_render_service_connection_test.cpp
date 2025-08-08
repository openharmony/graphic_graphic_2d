/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    MemoryGraphic mem1;
    renderService->GetMemoryGraphic(123, mem1);
    ASSERT_EQ(mem1.GetGpuMemorySize(), 0);
    MemoryGraphic mem2;
    renderService->GetMemoryGraphic(0, mem2);
    ASSERT_EQ(mem2.GetGpuMemorySize(), 0);
}
} // namespace OHOS::Rosen