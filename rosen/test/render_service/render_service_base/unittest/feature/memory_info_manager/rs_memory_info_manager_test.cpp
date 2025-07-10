/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "feature/memory_info_manager/rs_memory_info_manager.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSMemoryInfoManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMemoryInfoManagerTest::SetUpTestCase() {}
void RSMemoryInfoManagerTest::TearDownTestCase() {}
void RSMemoryInfoManagerTest::SetUp() {}
void RSMemoryInfoManagerTest::TearDown() {}

#ifdef RS_MEMORY_INFO_MANAGER
/**
 * @tc.name: SetAndGetGlobalRootNodeStatusChangeFlagTest001
 * @tc.desc: test results of SetAndGetGlobalRootNodeStatusChangeFlag
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryInfoManagerTest, SetAndGetGlobalRootNodeStatusChangeFlagTest001, TestSize.Level1)
{
    MemoryTrack::Instance().SetGlobalRootNodeStatusChangeFlag(true);
    EXPECT_EQ(MemoryTrack::Instance().GetGlobalRootNodeStatusChangeFlag(), true);
}

/**
 * @tc.name: SetSurfaceMemoryInfoTest001
 * @tc.desc: test results of SetSurfaceMemoryInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryInfoManagerTest, SetSurfaceMemoryInfoTest001, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::DEFAULT;
    RSSurfaceRenderNodeConfig = { .id = id, .nodeType = type };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    auto buffer = SurfaceBuffer::Create();
    node->GetRSSurfaceHandler()->buffer_.buffer = buffer;
    MemoryTrack::Instance().SetGlobalRootNodeStatusChangeFlag(true);
    RSMemoryInfoManager::SetSurfaceMemoryInfo(false, node->GetRSSurfaceHandler());
    RSMemoryInfoManager::SetSurfaceMemoryInfo(true, node->GetRSSurfaceHandler());
    EXPECT_NE(node->GetRSSurfaceHandler()->GetBuffer(), nullptr);
}

/**
 * @tc.name: SetImageMemoryInfoTest001
 * @tc.desc: test results of SetImageMemoryInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryInfoManagerTest, SetImageMemoryInfoTest001, TestSize.Level1)
{
    auto pixelMap =std::make_shared<Media::PixelMap>();
    pixelMap->SetPixelsAddr(NULL, NULL, 0, Media::AllocatorType::DMA_ALLOC, NULL);
    RSMemoryInfoManager::SetImageMemoryInfo(pixelMap);
    EXPECT_NE(pixelMap, nullptr);
}

/**
 * @tc.name: SetImageMemoryInfoTest001
 * @tc.desc: test results of SetImageMemoryInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryInfoManagerTest, SetImageMemoryInfoTest001, TestSize.Level1)
{
    auto pixelMap =std::make_shared<Media::PixelMap>();
    pixelMap->SetPixelsAddr(NULL, NULL, 0, Media::AllocatorType::DMA_ALLOC, NULL);
    RSMemoryInfoManager::SetImageMemoryInfo(pixelMap);
    EXPECT_NE(pixelMap, nullptr);
}

/**
 * @tc.name: RecordNodeOnTreeStatusTest001
 * @tc.desc: test results of RecordNodeOnTreeStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryInfoManagerTest, RecordNodeOnTreeStatusTest001 TestSize.Level1)
{
    MemoryTrack::Instance().SetGlobalRootNodeStatusChangeFlag(false);
    RSMemoryInfoManager::RecordNodeOnTreeStatus(true, 1111, 1111);
    EXPECT_EQ(MemoryTrack::Instance().globalRootNodeStatusChangeFlag, true);
    MemoryTrack::Instance().SetGlobalRootNodeStatusChangeFlag(false);
    RSMemoryInfoManager::RecordNodeOnTreeStatus(true, 1111, 1112);
    EXPECT_EQ(MemoryTrack::Instance().globalRootNodeStatusChangeFlag, false);
}

/**
 * @tc.name:  ResetRootNodeStatusChangeFlagTest001
 * @tc.desc: test results of  ResetRootNodeStatusChangeFlag
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryInfoManagerTest, ResetRootNodeStatusChangeFlagTest001 TestSize.Level1)
{
    MemoryTrack::Instance().SetGlobalRootNodeStatusChangeFlag(true);
    RSMemoryInfoManager::ResetRootNodeStatusChangeFlag(1111, 1111);
    EXPECT_EQ(MemoryTrack::Instance().globalRootNodeStatusChangeFlag, false);
    MemoryTrack::Instance().SetGlobalRootNodeStatusChangeFlag(true);
    RSMemoryInfoManager::ResetRootNodeStatusChangeFlag(1111, 1112);
    EXPECT_EQ(MemoryTrack::Instance().globalRootNodeStatusChangeFlag, true);
}
#endif
} // namespace Rosen
} // namespace OHOS
