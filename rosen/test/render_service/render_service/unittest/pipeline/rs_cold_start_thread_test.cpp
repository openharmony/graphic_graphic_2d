/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_cold_start_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSColdStartThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColdStartThreadTest::SetUpTestCase() {}
void RSColdStartThreadTest::TearDownTestCase() {}
void RSColdStartThreadTest::SetUp() {}
void RSColdStartThreadTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, CreateAndDestroy001, TestSize.Level1)
{
    NodeId id = 1;
    auto sp = std::make_shared<RSSurfaceRenderNode>(id);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(sp);
    RSColdStartThread thread { surfaceRenderNode, id };
    thread.PostTask([] {});
    thread.Stop();
}

/**
 * @tc.name: RSColdStartManager002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, RSColdStartManager002, TestSize.Level1)
{
    RSColdStartManager::Instance().StartColdStartThreadIfNeed(nullptr);
}

/**
 * @tc.name: RSColdStartManager003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, RSColdStartManager003, TestSize.Level1)
{
    NodeId id = 5;
    auto sp = std::make_shared<RSSurfaceRenderNode>(id);
    RSColdStartManager::Instance().StartColdStartThreadIfNeed(sp);
    RSColdStartManager::Instance().StartColdStartThreadIfNeed(sp);
}

/**
 * @tc.name: IsColdStartThreadIdleTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, IsColdStartThreadIdleTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSColdStartManager rsColdStartManager;
    auto isColdStartThreadIdle = rsColdStartManager.IsColdStartThreadIdle(nodeId);
    EXPECT_FALSE(isColdStartThreadIdle);

    auto sp = std::make_shared<RSSurfaceRenderNode>(nodeId);
    RSColdStartManager::Instance().StartColdStartThreadIfNeed(sp);
    auto isColdStartThreadIdle002 = rsColdStartManager.IsColdStartThreadIdle(nodeId);
    EXPECT_FALSE(isColdStartThreadIdle002);
}

/**
 * @tc.name: CheckColdStartMap001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, CheckColdStartMap001, TestSize.Level1)
{
    RSRenderNodeMap nodeMap;
    RSColdStartManager rsColdStartManager;
    rsColdStartManager.CheckColdStartMap(nodeMap);
}

/**
 * @tc.name: CheckColdStartMap002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, CheckColdStartMap002, TestSize.Level1)
{
    NodeId id1 = 5;
    NodeId id2 = 0;
    auto sp1 = std::make_shared<RSSurfaceRenderNode>(id1);
    auto sp2 = std::make_shared<RSSurfaceRenderNode>(id2);
    RSColdStartManager::Instance().StartColdStartThreadIfNeed(sp1);
    RSColdStartManager::Instance().StartColdStartThreadIfNeed(sp2);
    RSRenderNodeMap nodeMap;
    RSColdStartManager rsColdStartManager;
    rsColdStartManager.CheckColdStartMap(nodeMap);
}

/**
 * @tc.name: StopColdStartThreadTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, StopColdStartThreadTest, TestSize.Level1)
{
    NodeId id1 = 5;
    NodeId id2 = 0;

    auto sp1 = std::make_shared<RSSurfaceRenderNode>(id1);
    auto sp2 = std::make_shared<RSSurfaceRenderNode>(id2);

    RSColdStartManager::Instance().StartColdStartThreadIfNeed(sp1);
    
    RSColdStartManager::Instance().StopColdStartThread(id2);
    RSColdStartManager::Instance().StopColdStartThread(id1);
}

/**
 * @tc.name: PostPlayBackTask001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, PostPlayBackTask001, TestSize.Level1)
{
    NodeId id = 5;
    int height = 0;
    int width = 0;
    RSColdStartManager rsColdStartManager;
#ifndef USE_ROSEN_DRAWING
    auto drawCmdList = std::make_shared<DrawCmdList>(width, height);
#else
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(width, height);
#endif
    rsColdStartManager.PostPlayBackTask(id, drawCmdList, width, height);

    auto sp = std::make_shared<RSSurfaceRenderNode>(id);
    RSColdStartManager::Instance().StartColdStartThreadIfNeed(sp);

    rsColdStartManager.PostPlayBackTask(id, drawCmdList, (float)width, (float)height);
}

/**
 * @tc.name: PostPlayBackTask002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, PostPlayBackTask002, TestSize.Level1)
{
    NodeId id = 5;
    int height = 0.0;
    int width = 0.0;
    auto sp = std::make_shared<RSSurfaceRenderNode>(id);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(sp);
    RSColdStartThread thread { surfaceRenderNode, id };
#ifndef USE_ROSEN_DRAWING
    auto drawCmdList = std::make_shared<DrawCmdList>(width, height);
#else
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(width, height);
#endif
    thread.PostPlayBackTask(drawCmdList, (float)width, (float)height);

// #ifdef RS_ENABLE_GL
//     EGLContext context;
//     thread.Run(context);
// #else 
//     thread.Run();
// #endif
//     thread.PostPlayBackTask(drawCmdList, (float)width, (float)height);
}

/**
 * @tc.name: IsIdleTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, IsIdleTest, TestSize.Level1)
{
    NodeId id = 5;
    auto sp = std::make_shared<RSSurfaceRenderNode>(id);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(sp);
    RSColdStartThread thread { surfaceRenderNode, id };
    ASSERT_EQ(false, thread.IsIdle());
}

/**
 * @tc.name: DestroyColdStartThreadTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, DestroyColdStartThreadTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(node);
    RSColdStartThread thread { surfaceRenderNode, nodeId };
    RSColdStartManager rsColdStartManager;
    rsColdStartManager.DestroyColdStartThread(nodeId);
}

} // namespace OHOS::Rosen
