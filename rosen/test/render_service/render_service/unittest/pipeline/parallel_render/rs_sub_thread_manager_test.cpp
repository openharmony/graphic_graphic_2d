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
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsSubThreadManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsSubThreadManagerTest::SetUpTestCase() {}
void RsSubThreadManagerTest::TearDownTestCase() {}
void RsSubThreadManagerTest::SetUp() {}
void RsSubThreadManagerTest::TearDown() {}

/**
 * @tc.name: StartTest
 * @tc.desc: Test RsSubThreadManagerTest.StartTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadManagerTest, StartTest, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    ASSERT_TRUE(rsSubThreadManager->threadList_.empty());
    rsSubThreadManager->Start(nullptr);
    auto renderContext = new RenderContext();
    rsSubThreadManager->Start(renderContext);
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    rsSubThreadManager->threadList_.push_back(curThread);
    ASSERT_FALSE(rsSubThreadManager->threadList_.empty());
    rsSubThreadManager->Start(nullptr);
    delete renderContext;
    renderContext = nullptr;
}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: Test RsSubThreadManagerTest.PostTaskTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadManagerTest, PostTaskTest, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    rsSubThreadManager->PostTask([]{}, 5);
    auto renderContext = new RenderContext();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    rsSubThreadManager->threadList_.push_back(curThread);
    rsSubThreadManager->PostTask([]{}, 1);
    delete renderContext;
    renderContext = nullptr;
}

/**
 * @tc.name: SubmitSubThreadTaskTest
 * @tc.desc: Test RsSubThreadManagerTest.SubmitSubThreadTaskTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadManagerTest, SubmitSubThreadTaskTest, TestSize.Level1)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> list;
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    rsSubThreadManager->SubmitSubThreadTask(nullptr, list);
}

/**
 * @tc.name: AddToReleaseQueue
 * @tc.desc: Test RsSubThreadManagerTest.AddToReleaseQueue
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadManagerTest, AddToReleaseQueue, TestSize.Level1)
{
#ifdef USE_ROSEN_DRAWING
    const Drawing::ImageInfo info =
    Drawing::ImageInfo{200, 200, Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE }; // image size 200*200
    auto surface(Drawing::Surface::MakeRaster(info));
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    ASSERT_NE(rsSubThreadManager, nullptr);
    rsSubThreadManager->AddToReleaseQueue(std::move(surface), 10); // 10 is invalid
#endif
}
} // namespace OHOS::Rosen