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

#include <memory>
#include "gtest/gtest.h"
#include "limit_number.h"

#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/rs_surface_render_node.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderServiceListener : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceListener::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSRenderServiceListener::TearDownTestCase() {}
void RSRenderServiceListener::SetUp() {}
void RSRenderServiceListener::TearDown() {}

/**
 * @tc.name: CreateAndDestroy01
 * @tc.desc: Create listener with RSSurfaceRenderNode.
 * @tc.type: FUNC
 * @tc.require:issueI590LM
 */
HWTEST_F(RSRenderServiceListener, CreateAndDestroy01, TestSize.Level1)
{
    // nullptr test
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::shared_ptr<RSRenderServiceListener> rsListener = std::make_shared<RSRenderServiceListener>(wp);
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: OnTunnelHandleChange01
 * @tc.desc: Test OnTunnelHandleChange of invalid and valid listener.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListener, OnTunnelHandleChange01, TestSize.Level1)
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
 * @tc.name: OnBufferAvailable01
 * @tc.desc: Test OnBufferAvailable of invalid and valid listener.
 * @tc.type: FUNC
 * @tc.require: issueI590LM
 */
HWTEST_F(RSRenderServiceListener, OnBufferAvailable01, TestSize.Level1)
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

/**
 * @tc.name: OnCleanCache01
 * @tc.desc: Test OnCleanCache of invalid and valid listener.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListener, OnCleanCache01, TestSize.Level1)
{
    // nullptr test and early return
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::shared_ptr<RSRenderServiceListener> rsListener = std::make_shared<RSRenderServiceListener>(wp);
    rsListener->OnCleanCache();
    
    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    rsListener = std::make_shared<RSRenderServiceListener>(node);
    rsListener->OnCleanCache();
    ASSERT_EQ(node->GetRSSurfaceHandler()->GetAvailableBufferCount(), 0);
}

} // namespace OHOS::Rosen
