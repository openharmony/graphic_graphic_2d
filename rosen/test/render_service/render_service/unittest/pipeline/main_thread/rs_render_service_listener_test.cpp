/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <memory>
#include "gtest/gtest.h"
#include "limit_number.h"

#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderServiceListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceListenerTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSRenderServiceListenerTest::TearDownTestCase() {}
void RSRenderServiceListenerTest::SetUp() {}
void RSRenderServiceListenerTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc: Create listener with RSSurfaceRenderNode.
 * @tc.type: FUNC
 * @tc.require:issueI590LM
 */
HWTEST_F(RSRenderServiceListenerTest, CreateAndDestroy001, TestSize.Level1)
{
    // nullptr test
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::shared_ptr<RSRenderServiceListener> rsListener = std::make_shared<RSRenderServiceListener>(wp);
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: OnTunnelHandleChange001
 * @tc.desc: Test OnTunnelHandleChange of invalid and valid listener.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, OnTunnelHandleChange001, TestSize.Level1)
{
    // nullptr test and early return
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::shared_ptr<RSRenderServiceListener> rsListener = std::make_shared<RSRenderServiceListener>(wp);
    rsListener->OnTunnelHandleChange();

    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    rsListener = std::make_shared<RSRenderServiceListener>(node);
    rsListener->OnTunnelHandleChange();
    ASSERT_EQ(node->GetTunnelHandleChange(), true);
}

/**
 * @tc.name: OnCleanCache001
 * @tc.desc: Test OnCleanCache of invalid and valid listener.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, OnCleanCache001, TestSize.Level1)
{
    // nullptr test and early return
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::shared_ptr<RSRenderServiceListener> rsListener = std::make_shared<RSRenderServiceListener>(wp);
    uint32_t bufSeqNum = 0;
    rsListener->OnCleanCache(&bufSeqNum);
    
    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    rsListener = std::make_shared<RSRenderServiceListener>(node);
    rsListener->OnCleanCache(&bufSeqNum);
    ASSERT_EQ(node->GetRSSurfaceHandler()->GetAvailableBufferCount(), 0);
    ASSERT_TRUE(bufSeqNum >= 0);
}

/**
* @tc.name: ForceRefresh001
* @tc.desc: Test ForceRefresh
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSRenderServiceListenerTest, ForceRefresh001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    std::shared_ptr<RSRenderServiceListener> rsListener = std::make_shared<RSRenderServiceListener>(node);
    rsListener = std::make_shared<RSRenderServiceListener>(node);

    ASSERT_FALSE(node->IsLayerTop());
    ASSERT_FALSE(node->IsTopLayerForceRefresh());
    rsListener->ForceRefresh(node);

    node->SetLayerTop(true);
    ASSERT_TRUE(node->IsLayerTop());
    ASSERT_TRUE(node->IsTopLayerForceRefresh());
    rsListener->ForceRefresh(node);

    node->SetLayerTop(true, false);
    ASSERT_TRUE(node->IsLayerTop());
    ASSERT_FALSE(node->IsTopLayerForceRefresh());
    rsListener->ForceRefresh(node);

    node->SetLayerTop(false, true);
    ASSERT_FALSE(node->IsLayerTop());
    ASSERT_TRUE(node->IsTopLayerForceRefresh());
    rsListener->ForceRefresh(node);
}

/**
 * @tc.name: OnBufferAvailable001
 * @tc.desc: Test OnBufferAvailable of invalid and valid listener.
 * @tc.type: FUNC
 * @tc.require: issueI590LM
 */
HWTEST_F(RSRenderServiceListenerTest, OnBufferAvailable001, TestSize.Level1)
{
    // nullptr test and early return
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::shared_ptr<RSRenderServiceListener> rsListener = std::make_shared<RSRenderServiceListener>(wp);
    rsListener->OnBufferAvailable();

    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    rsListener = std::make_shared<RSRenderServiceListener>(node);
    node->SetIsNotifyUIBufferAvailable(false);
    rsListener->OnBufferAvailable();
    ASSERT_EQ(node->IsNotifyUIBufferAvailable(), true);

    node->SetIsNotifyUIBufferAvailable(true);
    rsListener->OnBufferAvailable();
    ASSERT_EQ(node->IsNotifyUIBufferAvailable(), true);
}
} // namespace OHOS::Rosen
