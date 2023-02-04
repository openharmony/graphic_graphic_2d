/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <memory>
#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/parallel_render/rs_parallel_sub_thread.h"
#include "pipeline/parallel_render/rs_parallel_render_manager.h"
#include "render_context/render_context.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "pipeline/rs_render_node.h"
#include "screen_manager/rs_screen_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSParallelSubThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSParallelSubThreadTest::SetUpTestCase() {}
void RSParallelSubThreadTest::TearDownTestCase() {}
void RSParallelSubThreadTest::SetUp() {}
void RSParallelSubThreadTest::TearDown() {}

/**
 * @tc.name: StartSubThreadPrepareTest
 * @tc.desc: Test RSParallelSubThreadTest.StartSubThreadPrepareTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelSubThreadTest, StartSubThreadPrepareTest, TestSize.Level1)
{
    (void)system::SetParameter("rosen.prepareparallelrender.enabled", "0");
    int param = (int)RSSystemProperties::GetPrepareParallelRenderingEnabled();
    ASSERT_EQ(param, 0);

    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(100, displayConfig, rsContext->weak_from_this());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    std::vector<std::shared_ptr<RSSurfaceRenderNode>> rsSurfaceRenderNodeList;

    for (int i = 1; i <= 50; i++) {
        RSSurfaceRenderNodeConfig config;
        config.id = i;
        auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
        rsSurfaceRenderNodeList.push_back(rsSurfaceRenderNode);
        rsSurfaceRenderNode->SetSrcRect(RectI((i - 1) * 10, (i - 1) * 10, i * 10, i * 10));
        rsSurfaceRenderNode->SetFreeze(false);
        rsSurfaceRenderNode->SetSecurityLayer(true);
        rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, i);
    }
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    parallelRenderManager->SetParallelMode(true);
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    parallelRenderManager->CopyPrepareVisitorAndPackTask(*rsUniRenderVisitor, *rsDisplayRenderNode);
    parallelRenderManager->LoadBalanceAndNotify(TaskType::PREPARE_TASK);
    parallelRenderManager->WaitPrepareEnd(*rsUniRenderVisitor);
    parallelRenderManager->EndSubRenderThread();
}

/**
 * @tc.name: StartSubThreadProcessTest
 * @tc.desc: Test RSParallelSubThreadTest.StartSubThreadProcessTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelSubThreadTest, StartSubThreadProcessTest, TestSize.Level1)
{
    (void)system::SetParameter("rosen.prepareparallelrender.enabled", "0");
    int param = (int)RSSystemProperties::GetPrepareParallelRenderingEnabled();
    ASSERT_EQ(param, 0);

    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(100, displayConfig, rsContext->weak_from_this());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    std::vector<std::shared_ptr<RSSurfaceRenderNode>> rsSurfaceRenderNodeList;

    for (int i = 1; i <= 50; i++) {
        RSSurfaceRenderNodeConfig config;
        config.id = i;
        auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
        rsSurfaceRenderNodeList.push_back(rsSurfaceRenderNode);
        rsSurfaceRenderNode->SetSrcRect(RectI((i - 1) * 10, (i - 1) * 10, i * 10, i * 10));
        rsSurfaceRenderNode->SetFreeze(false);
        rsSurfaceRenderNode->SetSecurityLayer(true);
        rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, i);
    }
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    parallelRenderManager->SetParallelMode(true);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    parallelRenderManager->SetFrameSize(2560, 1600);
    parallelRenderManager->CopyVisitorAndPackTask(*rsUniRenderVisitor, *rsDisplayRenderNode);
    parallelRenderManager->LoadBalanceAndNotify(TaskType::PROCESS_TASK);
    parallelRenderManager->EndSubRenderThread();
}

/**
 * @tc.name: StartSubThreadOtherTest
 * @tc.desc: Test RSParallelSubThreadTest.StartSubThreadOtherTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelSubThreadTest, StartSubThreadOtherTest, TestSize.Level1)
{
    auto curThread = std::make_unique<RSParallelSubThread>(0);
    curThread->WaitReleaseFence();
    curThread->GetSharedContext();
    curThread->GetSkSurface();
    curThread->GetTexture();
}

} // namespace OHOS::Rosen