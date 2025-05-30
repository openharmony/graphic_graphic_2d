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
#include "ui/rs_texture_export.h"
#include "limit_number.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif

using namespace testing;
using namespace testing::ext;
namespace OHOS::Rosen {
class RSTextureExportTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, -34.4f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTextureExportTest::SetUpTestCase() {}
void RSTextureExportTest::TearDownTestCase() {}
void RSTextureExportTest::SetUp() {}
void RSTextureExportTest::TearDown() {}


/**
 * @tc.name: DoTextureExport001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSTextureExportTest, DoTextureExport001, TestSize.Level1)
{
    bool isRenderServiceNode = false;
    std::shared_ptr<RSNode> rootNode = std::make_shared<RSNode>(isRenderServiceNode);
    SurfaceId surfaceId = 0;
    RSTextureExport text(rootNode, surfaceId);
    bool res = text.DoTextureExport();
    ASSERT_TRUE(res == true);
}

/**
 * @tc.name: DoTextureExport002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSTextureExportTest, DoTextureExport002, TestSize.Level1)
{
    bool isRenderServiceNode = false;
    std::shared_ptr<RSNode> rootNode = std::make_shared<RSNode>(isRenderServiceNode);
    SurfaceId surfaceId = 0;
    RSTextureExport text(rootNode, surfaceId);
    rootNode->isTextureExportNode_ = true;
    bool res = text.DoTextureExport();
    ASSERT_TRUE(res == true);
}

/**
 * @tc.name: DoTextureExport003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSTextureExportTest, DoTextureExport003, TestSize.Level1)
{
    bool isRenderServiceNode = false;
    std::shared_ptr<RSRootNode> RSNode = std::make_shared<RSRootNode>(isRenderServiceNode);
    SurfaceId surfaceId = 0;
    RSTextureExport text(RSNode, surfaceId);
    bool res = text.DoTextureExport();
    ASSERT_TRUE(res == true);
}

/**
 * @tc.name: StopTextureExport
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSTextureExportTest, StopTextureExport, TestSize.Level1)
{
    bool isRenderServiceNode = false;
    std::shared_ptr<RSNode> rootNode = std::make_shared<RSNode>(isRenderServiceNode);
    SurfaceId surfaceId = 0;
    RSTextureExport text(rootNode, surfaceId);
    text.StopTextureExport();
    ASSERT_NE(rootNode, nullptr);
}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: ClearContext001
 * @tc.desc: test ClearContext while virtualSurfaceNodeSet_ is empty
 * @tc.type:FUNC
 * @tc.require: issuesIC7U3T
 */
HWTEST_F(RSTextureExportTest, ClearContext001, TestSize.Level2)
{
    if (RSSystemProperties::IsUseVulkan()) {
        RSTextureExport text(nullptr, INVALID_NODEID);
        RSTextureExport::ClearContext();
        ASSERT_TRUE(RSTextureExport::virtualSurfaceNodeSet_.empty());
    }
}

/**
 * @tc.name: ClearContext002
 * @tc.desc: test ClearContext while virtualSurfaceNodeSet_ has nullptr
 * @tc.type:FUNC
 * @tc.require: issuesIC7U3T
 */
HWTEST_F(RSTextureExportTest, ClearContext002, TestSize.Level2)
{
    if (RSSystemProperties::IsUseVulkan()) {
        RSTextureExport text(nullptr, INVALID_NODEID);
        RSTextureExport::virtualSurfaceNodeSet_.insert(nullptr);
        RSTextureExport::ClearContext();
        ASSERT_FALSE(RSTextureExport::virtualSurfaceNodeSet_.empty());
    }
}

/**
 * @tc.name: ClearContext003
 * @tc.desc: test ClearContext while rsSurface is nullptr
 * @tc.type:FUNC
 * @tc.require: issuesIC7U3T
 */
HWTEST_F(RSTextureExportTest, ClearContext003, TestSize.Level2)
{
    if (RSSystemProperties::IsUseVulkan()) {
        bool isRenderServiceNode = false;
        std::shared_ptr<RSRootNode> rootNode = std::make_shared<RSRootNode>(isRenderServiceNode);
        SurfaceId surfaceId = 0;
        RSTextureExport text(rootNode, surfaceId);
        ASSERT_NE(text.virtualSurfaceNode_, nullptr);
        text.virtualSurfaceNode_->surface_ = nullptr;

        RSTextureExport::ClearContext();
        ASSERT_FALSE(RSTextureExport::virtualSurfaceNodeSet_.empty());
    }
}

/**
 * @tc.name: ClearContext004
 * @tc.desc: test ClearContext while rsSurface isn't nullptr
 * @tc.type:FUNC
 * @tc.require: issuesIC7U3T
 */
HWTEST_F(RSTextureExportTest, ClearContext004, TestSize.Level2)
{
    if (RSSystemProperties::IsUseVulkan()) {
        // create producer surface
        sptr<OHOS::IConsumerSurface> cSurface = IConsumerSurface::Create();
        ASSERT_NE(cSurface, nullptr);
        sptr<OHOS::IBufferProducer> producer = cSurface->GetProducer();
        sptr<OHOS::Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
        ASSERT_NE(pSurface, nullptr);
        
        bool isRenderServiceNode = false;
        std::shared_ptr<RSRootNode> rootNode = std::make_shared<RSRootNode>(isRenderServiceNode);
        SurfaceId surfaceId = 0;
        RSTextureExport text(rootNode, surfaceId);
        ASSERT_NE(text.virtualSurfaceNode_, nullptr);
        text.virtualSurfaceNode_->surface_ = std::make_shared<RSSurfaceOhosVulkan>(pSurface);

        RSTextureExport::ClearContext();
        ASSERT_FALSE(RSTextureExport::virtualSurfaceNodeSet_.empty());
    }
}
#endif
}