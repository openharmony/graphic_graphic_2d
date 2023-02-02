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

#include <cstdint>
#include <memory>
#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/parallel_render/rs_parallel_render_manager.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSParallelRenderManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSParallelRenderManagerTest::SetUpTestCase() {}
void RSParallelRenderManagerTest::TearDownTestCase() {}
void RSParallelRenderManagerTest::SetUp() {}
void RSParallelRenderManagerTest::TearDown() {}

/**
 * @tc.name: SetParallelModeTest
 * @tc.desc: Test RSParallelRenderManagerTest.SetParallelModeTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, SetParallelModeTest, TestSize.Level1)
{
    auto instance = RSParallelRenderManager::Instance();
    ParallelStatus status = instance->GetParallelRenderingStatus();
    ASSERT_EQ(ParallelStatus::OFF, status);

    instance->SetParallelMode(false);
    status = instance->GetParallelRenderingStatus();
    ASSERT_EQ(ParallelStatus::OFF, status);
    
    uint32_t threadNum = 3;
    instance->StartSubRenderThread(threadNum, nullptr);
    status = instance->GetParallelRenderingStatus();
    ASSERT_EQ(ParallelStatus::FIRSTFLUSH, status);

    for (uint32_t i = 0; i < threadNum; i++) {
        instance->ReadySubThreadNumIncrement();
    }
    status = instance->GetParallelRenderingStatus();
    ASSERT_EQ(ParallelStatus::ON, status);

    instance->GetParallelMode();
    instance->GetParallelModeSafe();
    bool mode = instance->GetParallelMode();
    ASSERT_EQ(true, mode);
}

/**
 * @tc.name: PackPrepareRenderTaskTest
 * @tc.desc: Test RSParallelRenderManagerTest.PackPrepareRenderTaskTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, PackPrepareRenderTaskTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config1;
    config1.id = 10;
    auto rsSurfaceRenderNode1 = std::make_shared<RSSurfaceRenderNode>(config1, rsContext->weak_from_this());
    rsSurfaceRenderNode1->SetSrcRect(RectI(0, 0, 10, 10));
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode1, TaskType::PREPARE_TASK);

    RSSurfaceRenderNodeConfig config2;
    config2.id = 20;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config2, rsContext->weak_from_this());
    rsSurfaceRenderNode2->SetSrcRect(RectI(20, 20, 10, 10));
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode2, TaskType::PREPARE_TASK);

    RSSurfaceRenderNodeConfig config3;
    config3.id = 30;
    auto rsSurfaceRenderNode3 = std::make_shared<RSSurfaceRenderNode>(config3, rsContext->weak_from_this());
    rsSurfaceRenderNode3->SetSrcRect(RectI(40, 40, 10, 10));
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode3, TaskType::PREPARE_TASK);

    RSParallelRenderManager::Instance()->CommitSurfaceNum(10);
}

/**
 * @tc.name: PackProcessRenderTaskTest
 * @tc.desc: Test RSParallelRenderManagerTest.PackProcessRenderTaskTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, PackProcessRenderTaskTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config1;
    config1.id = 10;
    auto rsSurfaceRenderNode1 = std::make_shared<RSSurfaceRenderNode>(config1, rsContext->weak_from_this());
    rsSurfaceRenderNode1->SetSrcRect(RectI(0, 0, 10, 10));
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode1, TaskType::PREPARE_TASK);

    RSSurfaceRenderNodeConfig config2;
    config2.id = 20;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config2, rsContext->weak_from_this());
    rsSurfaceRenderNode2->SetSrcRect(RectI(20, 20, 10, 10));
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode2, TaskType::PREPARE_TASK);

    RSSurfaceRenderNodeConfig config3;
    config3.id = 30;
    auto rsSurfaceRenderNode3 = std::make_shared<RSSurfaceRenderNode>(config3, rsContext->weak_from_this());
    rsSurfaceRenderNode3->SetSrcRect(RectI(40, 40, 10, 10));

    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode1, TaskType::PROCESS_TASK);
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode2, TaskType::PROCESS_TASK);
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode3, TaskType::PROCESS_TASK);

    SkCanvas skCanvas;
    auto canvas = std::make_shared<RSPaintFilterCanvas>(&skCanvas);
    RSParallelRenderManager::Instance()->MergeRenderResult(canvas);

    RSParallelRenderManager::Instance()->SetRenderTaskCost(0, 10, 2.1, TaskType::PROCESS_TASK);
    RSParallelRenderManager::Instance()->SetRenderTaskCost(1, 20, 3.2, TaskType::PROCESS_TASK);
    RSParallelRenderManager::Instance()->SetRenderTaskCost(2, 30, 10.1, TaskType::PROCESS_TASK);
}

/**
 * @tc.name: SetFrameSizeTest
 * @tc.desc: Test RSParallelRenderManagerTest.SetFrameSizeTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, SetFrameSizeTest, TestSize.Level1)
{
    int width = 2560;
    int height = 1600;
    RSParallelRenderManager::Instance()->SetFrameSize(width, height);
    int width0 = 0;
    int height0 = 0;
    RSParallelRenderManager::Instance()->GetFrameSize(width0, height0);
    ASSERT_EQ(width, width0);
    ASSERT_EQ(height, height0);
}

/**
 * @tc.name: SetParallelModeTest2
 * @tc.desc: Test RSParallelRenderManagerTest.SetParallelModeTest2
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, SetParallelModeTest2, TestSize.Level1)
{
    RSParallelRenderManager::Instance()->SetParallelMode(false);
    bool mode = RSParallelRenderManager::Instance()->GetParallelMode();
    ASSERT_EQ(false, mode);
}

} // namespace OHOS::Rosen