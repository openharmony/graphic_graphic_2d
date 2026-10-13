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

#include <gtest/gtest.h>

#include "transaction/rs_render_pipeline_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr uint64_t CLEAN_DELEGATE_TEST_ID = 123;

class RSPipelineClientCleanDelegateTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
    static inline std::shared_ptr<RSRenderPipelineClient> rsClient = nullptr;
};

void RSPipelineClientCleanDelegateTest::SetUpTestCase()
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    rsClient = std::make_shared<RSRenderPipelineClient>(connectToRenderRemote);
    ASSERT_NE(rsClient, nullptr);
}

/**
 * @tc.name: CleanDelegateModeBufferCmd001
 * @tc.desc: CleanDelegateModeBufferCmd returns false when no render connection is available
 * @tc.type: FUNC
 */
HWTEST_F(RSPipelineClientCleanDelegateTest, CleanDelegateModeBufferCmd001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    // rsClient is constructed without a remote object, no connection is registered in
    // RSRenderServiceConnectHub for its token mask id
    EXPECT_FALSE(rsClient->CleanDelegateModeBufferCmd(CLEAN_DELEGATE_TEST_ID));
}
} // namespace Rosen
} // namespace OHOS
