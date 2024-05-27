/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "pipeline/rs_uifirst_manager.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSUifirstManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline RectI DEFAULT_RECT = {0, 0, 10, 10};
    static inline uint8_t GROUPED_BY_ANIM = 1;
    static inline NodeId INVALID_NODEID = 0xFFFF;
    static inline RSMainThread* mainThread_;
    static inline RSUifirstManager& uifirstManager_ = RSUifirstManager::Instance();
};

void RSUifirstManagerTest::SetUpTestCase()
{
    mainThread_ = RSMainThread::Instance();
    if (mainThread_) {
        uifirstManager_.mainThread_ = mainThread_;
    }
}

void RSUifirstManagerTest::TearDownTestCase() {}
void RSUifirstManagerTest::SetUp() {}
void RSUifirstManagerTest::TearDown() {}

/**
 * @tc.name: SetUifirstNodeEnableParam001
 * @tc.desc: Test SetUifirstNodeEnableParam, when node is leash window type
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, SetUifirstNodeEnableParam001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    uifirstManager_.SetUifirstNodeEnableParam(*surfaceNode, MultiThreadCacheType::NONE);
}

/**
 * @tc.name: SetUifirstNodeEnableParam002
 * @tc.desc: Test SetUifirstNodeEnableParam, with APP window child and FOREGROUND child
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, SetUifirstNodeEnableParam002, TestSize.Level1)
{
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    // create different children nodes
    NodeId id = 1;
    auto childNode1 = std::make_shared<RSSurfaceRenderNode>(id);
    auto childNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode2, nullptr);
    childNode2->SetSurfaceNodeType(RSSurfaceNodeType::FOREGROUND_SURFACE);
    auto childNode3 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode3, nullptr);
    childNode3->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    parentNode->AddChild(childNode1);
    parentNode->AddChild(childNode2);
    parentNode->AddChild(childNode3);
    parentNode->GenerateFullChildrenList();
    uifirstManager_.SetUifirstNodeEnableParam(*parentNode, MultiThreadCacheType::LEASH_WINDOW);
    // child2 uifirstParentFlag_ expected to be false
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(childNode2->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    ASSERT_FALSE(surfaceParams->GetParentUifirstNodeEnableParam());
    // child3 uifirstParentFlag_ expected to be true
    surfaceParams = static_cast<RSSurfaceRenderParams*>(childNode3->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    ASSERT_TRUE(surfaceParams->GetParentUifirstNodeEnableParam());
}

/**
 * @tc.name: MergeOldDirty001
 * @tc.desc: Test MergeOldDirty, preSurfaceCacheContentStatic_ is false, no dirty region
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, MergeOldDirty001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->preSurfaceCacheContentStatic_ = false;
    surfaceNode->oldDirty_ = RSUifirstManagerTest::DEFAULT_RECT;
    uifirstManager_.MergeOldDirty(*surfaceNode);
    if (surfaceNode->GetDirtyManager()) {
        ASSERT_TRUE(surfaceNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
    }
}

/**
 * @tc.name: MergeOldDirty002
 * @tc.desc: Test MergeOldDirty, preSurfaceCacheContentStatic_ is true, dirty region is not empty
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, MergeOldDirty002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->preSurfaceCacheContentStatic_ = true;
    surfaceNode->oldDirty_ = RSUifirstManagerTest::DEFAULT_RECT;
    uifirstManager_.MergeOldDirty(*surfaceNode);
    if (surfaceNode->GetDirtyManager()) {
        ASSERT_FALSE(surfaceNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
    }
}

/**
 * @tc.name: MergeOldDirty003
 * @tc.desc: Test MergeOldDirty, merge children dirty region
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, MergeOldDirty003, TestSize.Level1)
{
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->preSurfaceCacheContentStatic_ = true;
    parentNode->AddChild(surfaceNode);
    // add different children nodes
    auto childNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode, nullptr);
    childNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    childNode->oldDirty_= RSUifirstManagerTest::DEFAULT_RECT;
    surfaceNode->AddChild(childNode);
    surfaceNode->GenerateFullChildrenList();
    uifirstManager_.MergeOldDirty(*surfaceNode);
    if (childNode->GetDirtyManager()) {
        ASSERT_FALSE(childNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
    }
}

/**
 * @tc.name: RenderGroupUpdate001
 * @tc.desc: Test RenderGroupUpdate, when parent node is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, RenderGroupUpdate001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    auto surfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(drawable);
    uifirstManager_.RenderGroupUpdate(surfaceDrawable);
}

/**
 * @tc.name: RenderGroupUpdate002
 * @tc.desc: Test RenderGroupUpdate, when parent node is display node
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, RenderGroupUpdate002, TestSize.Level1)
{
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(++id);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->AddChild(surfaceNode);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    auto surfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(drawable);
    uifirstManager_.RenderGroupUpdate(surfaceDrawable);
}

/**
 * @tc.name: RenderGroupUpdate003
 * @tc.desc: Test RenderGroupUpdate, when parent node is surface node
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, RenderGroupUpdate003, TestSize.Level1)
{
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    auto childNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode, nullptr);
    parentNode->AddChild(childNode);
    parentNode->nodeGroupType_ = RSUifirstManagerTest::GROUPED_BY_ANIM;
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(childNode);
    auto surfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(drawable);
    uifirstManager_.RenderGroupUpdate(surfaceDrawable);
}

/**
 * @tc.name: ProcessForceUpdateNode001
 * @tc.desc: Test ProcessForceUpdateNode, input invalid nodeid
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, ProcessForceUpdateNode001, TestSize.Level1)
{
    ASSERT_NE(mainThread_, nullptr);
    uifirstManager_.pendingForceUpdateNode_.push_back(INVALID_NODEID);
    uifirstManager_.ProcessForceUpdateNode();
}

/**
 * @tc.name: ProcessForceUpdateNode002
 * @tc.desc: Test ProcessForceUpdateNode, when parent node is surface node
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, ProcessForceUpdateNode002, TestSize.Level1)
{
    ASSERT_NE(mainThread_, nullptr);
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    auto childNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode, nullptr);
    parentNode->AddChild(childNode);
    parentNode->GenerateFullChildrenList();
    mainThread_->context_->nodeMap.RegisterRenderNode(parentNode);
    uifirstManager_.pendingForceUpdateNode_.push_back(parentNode->GetId());
    uifirstManager_.ProcessForceUpdateNode();
}

/**
 * @tc.name: ProcessDoneNode
 * @tc.desc: Test ProcessDoneNode, subthreadProcessDoneNode_ is expected to be empty
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, ProcessDoneNode, TestSize.Level1)
{
    uifirstManager_.subthreadProcessDoneNode_.push_back(INVALID_NODEID);
    uifirstManager_.ProcessDoneNode();
    ASSERT_TRUE(uifirstManager_.subthreadProcessDoneNode_.empty());
}
}