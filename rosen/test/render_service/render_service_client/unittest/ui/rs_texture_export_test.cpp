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
#include "pipeline/rs_render_thread.h"
#include "ui/rs_texture_export.h"
#include "limit_number.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"

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
    rootNode->isTextureExportNode_ = false;
    SurfaceId surfaceId = 0;
    RSTextureExport text(rootNode, surfaceId);
    RSRenderThread::Instance().thread_ = std::make_unique<std::thread>([]{});
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
    rootNode->isTextureExportNode_ = true;
    RSUINodeType type = rootNode->GetType();
    type = RSUINodeType::ROOT_NODE;
    SurfaceId surfaceId = 0;
    RSTextureExport text(rootNode, surfaceId);
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
    std::shared_ptr<RSNode> rootNode = std::make_shared<RSNode>(isRenderServiceNode);
    rootNode->isTextureExportNode_ = true;
    RSUINodeType type = rootNode->GetType();
    type = RSUINodeType::UNKNOW;
    SurfaceId surfaceId = 0;
    RSTextureExport text(rootNode, surfaceId);
    bool res = text.DoTextureExport();
    ASSERT_TRUE(res == true);
}

/**
 * @tc.name: UpdateBufferInfo
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSTextureExportTest, UpdateBufferInfo, TestSize.Level1)
{
    bool isRenderServiceNode = false;
    std::shared_ptr<RSNode> rootNode = std::make_shared<RSNode>(isRenderServiceNode);
    rootNode->isTextureExportNode_ = false;
    SurfaceId surfaceId = 0;
    RSTextureExport text(rootNode, surfaceId);
    float x = 1.0;
    float y = 2.0;
    float width = 3.0;
    float height = 4.0;
    text.virtualRootNode_ = RSRootNode::Create(false, true);
    text.UpdateBufferInfo(x, y, width, height);
    ASSERT_NE(rootNode, nullptr);
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
}