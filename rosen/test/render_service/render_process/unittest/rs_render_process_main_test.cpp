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

#include "platform/common/rs_log.h"
#include "render_process/rs_render_process.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSRenderProcessMainTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderProcessMainTest::SetUpTestCase() {}
void RSRenderProcessMainTest::TearDownTestCase() {}
void RSRenderProcessMainTest::SetUp() {}
void RSRenderProcessMainTest::TearDown() {}

/**
 * @tc.name: CreateRenderProcessTest001
 * @tc.desc: Test creating RSRenderProcess instance
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessMainTest, CreateRenderProcessTest001, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
}

/**
 * @tc.name: CreateRenderProcessTest002
 * @tc.desc: Test creating multiple RSRenderProcess instances
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessMainTest, CreateRenderProcessTest002, TestSize.Level1)
{
    auto renderProcess1 = sptr<RSRenderProcess>::MakeSptr();
    auto renderProcess2 = sptr<RSRenderProcess>::MakeSptr();

    ASSERT_NE(renderProcess1, nullptr);
    ASSERT_NE(renderProcess2, nullptr);
    ASSERT_NE(renderProcess1.GetRefPtr(), renderProcess2.GetRefPtr());
}

/**
 * @tc.name: RenderProcessRefCountTest001
 * @tc.desc: Test RSRenderProcess reference counting
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessMainTest, RenderProcessRefCountTest001, TestSize.Level1)
{
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    auto refCount = renderProcess->GetSptrRefCount();

    ASSERT_GT(refCount, 0);

    sptr<RSRenderProcess> renderProcess2 = renderProcess;
    auto refCount2 = renderProcess->GetSptrRefCount();

    ASSERT_GT(refCount2, refCount);
}

} // namespace OHOS::Rosen
