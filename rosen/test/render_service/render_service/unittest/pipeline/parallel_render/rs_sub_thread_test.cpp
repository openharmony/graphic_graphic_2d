/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "pipeline/parallel_render/rs_sub_thread.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsSubThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsSubThreadTest::SetUpTestCase() {}
void RsSubThreadTest::TearDownTestCase() {}
void RsSubThreadTest::SetUp() {}
void RsSubThreadTest::TearDown() {}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: Test RsSubThreadTest.PostTaskTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, PostTaskTest, TestSize.Level1)
{
    auto renderContext = new RenderContext();
    ASSERT_TRUE(renderContext != nullptr);
    renderContext->InitializeEglContext();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    curThread->PostTask([]{});
    curThread->Start();
    curThread->PostTask([]{});
    delete renderContext;
    renderContext = nullptr;
}

/**
 * @tc.name: CreateShareEglContextTest
 * @tc.desc: Test RsSubThreadTest.CreateShareEglContextTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, CreateShareEglContextTest, TestSize.Level1)
{
    auto curThread1 = std::make_shared<RSSubThread>(nullptr, 0);
    curThread1->CreateShareEglContext();
    auto renderContext = new RenderContext();
    ASSERT_TRUE(renderContext != nullptr);
    renderContext->InitializeEglContext();
    auto curThread2 = std::make_shared<RSSubThread>(renderContext, 0);
    curThread2->CreateShareEglContext();
    delete renderContext;
    renderContext = nullptr;
}

/**
 * @tc.name: DestroyShareEglContextgTest
 * @tc.desc: Test RsSubThreadTest.DestroyShareEglContextgTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, DestroyShareEglContextTest, TestSize.Level1)
{
    auto curThread1 = std::make_shared<RSSubThread>(nullptr, 0);
    curThread1->DestroyShareEglContext();
    auto renderContext = new RenderContext();
    ASSERT_TRUE(renderContext != nullptr);
    renderContext->InitializeEglContext();
    auto curThread2 = std::make_shared<RSSubThread>(renderContext, 0);
    curThread2->DestroyShareEglContext();
    delete renderContext;
    renderContext = nullptr;
}

/**
 * @tc.name: CreateShareGrContextTest
 * @tc.desc: Test RsSubThreadTest.CreateShareGrContextTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, CreateShareGrContextTest, TestSize.Level1)
{
    auto curThread = std::make_shared<RSSubThread>(nullptr, 0);
    curThread->CreateShareGrContext();
}
} // namespace OHOS::Rosen