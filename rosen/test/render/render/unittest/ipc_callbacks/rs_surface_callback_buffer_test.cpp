/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ipc_callbacks/rs_surface_buffer_callback.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSSurfaceCallbackBufferTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceCallbackBufferTest::SetUpTestCase() {}
void RSSurfaceCallbackBufferTest::TearDownTestCase() {}
void RSSurfaceCallbackBufferTest::SetUp() {}
void RSSurfaceCallbackBufferTest::TearDown() {}

/**
 * @tc.name: ConstructorTest
 * @tc.desc: Verify the Constructor
 * @tc.type:FUNC
 * @tc.require: issueIB2AHG
 */
HWTEST_F(RSSurfaceCallbackBufferTest, Constructor, TestSize.Level1)
{
    bool flag = false;
    DefaultSurfaceBufferCallbackFuncs funcs = {
        .OnFinish = [&flag](const FinishCallbackRet&) { flag = true; }
    };
    RSDefaultSurfaceBufferCallback rsDefaultSurfaceBufferCallback(funcs);
    EXPECT_TRUE(rsDefaultSurfaceBufferCallback.finishCallback_ != nullptr);
    EXPECT_FALSE(flag);
}
} // namespace Rosen
} // namespace OHOS
