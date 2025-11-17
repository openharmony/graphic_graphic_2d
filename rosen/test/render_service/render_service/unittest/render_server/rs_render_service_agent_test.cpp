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

#include "render_server/rs_render_service_agent.h"
#include "render_server/rs_render_service.h"
#include "common/rs_thread_handler.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderServiceAgentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceAgentTest::SetUpTestCase() {}
void RSRenderServiceAgentTest::TearDownTestCase() {}
void RSRenderServiceAgentTest::SetUp() {}
void RSRenderServiceAgentTest::TearDown() {}

/**
 * @tc.name: PostTaskImmediate001
 * @tc.desc: PostTaskImmediate Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceAgentTest, PostTaskImmediate001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*renderService);
    RSTaskMessage::RSTask task = []() -> void { return; };
    renderServiceAgent->PostTaskImmediate(task);
}

/**
 * @tc.name: PostSyncTaskImmediate001
 * @tc.desc: PostTaskImmediate Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceAgentTest, PostSyncTaskImmediate001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*renderService);
    RSTaskMessage::RSTask task = []() -> void { return; };
    renderServiceAgent->PostSyncTaskImmediate(task);
}
} // namespace OHOS::Rosen