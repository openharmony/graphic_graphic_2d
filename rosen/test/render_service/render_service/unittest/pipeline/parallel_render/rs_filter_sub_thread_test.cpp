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

#include "pipeline/parallel_render/rs_filter_sub_thread.h"
#include "render/rs_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsFilterSubThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsFilterSubThreadTest::SetUpTestCase() {}
void RsFilterSubThreadTest::TearDownTestCase() {}
void RsFilterSubThreadTest::SetUp() {}
void RsFilterSubThreadTest::TearDown() {}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: Test RsFilterSubThreadTest.PostTaskTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsFilterSubThreadTest, PostTaskTest, TestSize.Level1)
{
    auto renderContext = new RenderContext();
    ASSERT_TRUE(renderContext != nullptr);
    renderContext->InitializeEglContext();
    auto curThread = std::make_shared<RSFilterSubThread>(renderContext);
    curThread->PostTask([] {});
    curThread->Start();
    curThread->PostTask([] {});
    delete renderContext;
    renderContext = nullptr;
    usleep(1000 * 1000); // 1000 * 1000us
}

/**
 * @tc.name: CreateShareEglContextTest
 * @tc.desc: Test RsFilterSubThreadTest.CreateShareEglContextTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsFilterSubThreadTest, CreateShareEglContextTest, TestSize.Level1)
{
    auto curThread1 = std::make_shared<RSFilterSubThread>(nullptr);
    curThread1->CreateShareEglContext();
    auto renderContext = new RenderContext();
    ASSERT_TRUE(renderContext != nullptr);
    renderContext->InitializeEglContext();
    auto curThread2 = std::make_shared<RSFilterSubThread>(renderContext);
    curThread2->CreateShareEglContext();
    delete renderContext;
    renderContext = nullptr;
}

/**
 * @tc.name: DestroyShareEglContextgTest
 * @tc.desc: Test RsFilterSubThreadTest.DestroyShareEglContextgTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsFilterSubThreadTest, DestroyShareEglContextTest, TestSize.Level1)
{
    auto curThread1 = std::make_shared<RSFilterSubThread>(nullptr);
    curThread1->DestroyShareEglContext();
    auto renderContext = new RenderContext();
    ASSERT_TRUE(renderContext != nullptr);
    renderContext->InitializeEglContext();
    auto curThread2 = std::make_shared<RSFilterSubThread>(renderContext);
    curThread2->DestroyShareEglContext();
    delete renderContext;
    renderContext = nullptr;
}

/**
 * @tc.name: CreateShareGrContextTest
 * @tc.desc: Test RsFilterSubThreadTest.CreateShareGrContextTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsFilterSubThreadTest, CreateShareGrContextTest, TestSize.Level1)
{
    auto curThread = std::make_shared<RSFilterSubThread>(nullptr);
    curThread->CreateShareGrContext();
}

/**
 * @tc.name: RSFilterTaskTest
 * @tc.desc: Test RsFilterSubThreadTest.RSFilterTaskTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsFilterSubThreadTest, RSFilterTaskTest, TestSize.Level1)
{
    class RSFilterCacheTask : public RSFilter::RSFilterTask {
    public:
        bool InitSurface(Drawing::GPUContext* grContext) override
        {
            return true;
        }
        bool Render() override
        {
            return true;
        }
    };
    RSFilterCacheTask task;
    Drawing::GPUContext* grContext = nullptr;
    ASSERT_TRUE(task.InitSurface(grContext));
    ASSERT_TRUE(task.Render());
}
} // namespace OHOS::Rosen