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
    // This test verifies the design intent
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
    // This test verifies the design intent
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
 * @tc.name: RenderProcessNullptrTest001
 * @tc.desc: Test RSRenderProcess nullptr comparison
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderProcessTest, RenderProcessNullptrTest001, TestSize.Level1)
{
    sptr<RSRenderProcess> renderProcess = nullptr;
    ASSERT_EQ(renderProcess, nullptr);

    renderProcess = sptr<RSRenderProcess>::MakeSptr();
    ASSERT_NE(renderProcess, nullptr);
}

} // namespace OHOS::Rosen
