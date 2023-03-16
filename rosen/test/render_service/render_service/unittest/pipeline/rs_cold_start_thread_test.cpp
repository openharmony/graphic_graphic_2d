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
 * @tc.name: CreateAndDestroy002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, CreateAndDestroy002, TestSize.Level1)
{
    NodeId id = 2;
    int w = 1;
    int h = 1;
    auto sp = std::make_shared<RSSurfaceRenderNode>(id);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(sp);
    auto drawCmdList = std::make_shared<DrawCmdList>(w, h);
    RSColdStartThread thread { surfaceRenderNode, id };
    thread.PostPlayBackTask(drawCmdList, drawCmdList->GetWidth(), drawCmdList->GetHeight());
    thread.Stop();
}

/**
 * @tc.name: CreateAndDestroy003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, CreateAndDestroy003, TestSize.Level1)
{
    NodeId id = 3;
    int w = 1;
    int h = 1;
    auto sp = std::make_shared<RSSurfaceRenderNode>(id);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(sp);
    RSColdStartThread thread { surfaceRenderNode, id };
    thread.PostPlayBackTask(nullptr, w, h);
    thread.Stop();
}

/**
 * @tc.name: RSColdStartManager001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, RSColdStartManager001, TestSize.Level1)
{
    NodeId id = 4;
    int w = 2;
    int h = 2;
    auto sp = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawCmdList = std::make_shared<DrawCmdList>(w, h);
    RSColdStartManager::Instance().StartColdStartThreadIfNeed(sp);
    RSColdStartManager::Instance().PostPlayBackTask(
        sp->GetId(), drawCmdList, drawCmdList->GetWidth(), drawCmdList->GetHeight());
    RSColdStartManager::Instance().StopColdStartThread(sp->GetId());
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
 * @tc.name: RSColdStartManager004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColdStartThreadTest, RSColdStartManager004, TestSize.Level1)
{
    NodeId id = 6;
    int width = 1;
    int height = 1;
    auto sp = std::make_shared<RSSurfaceRenderNode>(id);
    RSColdStartManager::Instance().PostPlayBackTask(sp->GetId(), nullptr, width, height);
    RSColdStartManager::Instance().StopColdStartThread(sp->GetId());
}
} // namespace OHOS::Rosen
