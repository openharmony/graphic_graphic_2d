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

#ifdef RS_ENABLE_UNI_RENDER
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
 * @tc.name: UpdateBufferInfoTest001
 * @tc.desc: Branch A - virtualRootNode_ is nullptr, function returns early
 * @tc.type: FUNC
 */
HWTEST_F(RSTextureExportTest, UpdateBufferInfoTest001, TestSize.Level1)
{
    bool isRenderServiceNode = false;
    std::shared_ptr<RSNode> rootNode = std::make_shared<RSNode>(isRenderServiceNode);
    SurfaceId surfaceId = 0;
    RSTextureExport text(rootNode, surfaceId);
    text.virtualRootNode_ = nullptr;
    text.UpdateBufferInfo(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(text.virtualRootNode_, nullptr);
}

/**
 * @tc.name: UpdateBufferInfoTest002
 * @tc.desc: Branch B - virtualRootNode_ non-null, SetBounds/SetFrame with negated x,y
 * @tc.type: FUNC
 */
HWTEST_F(RSTextureExportTest, UpdateBufferInfoTest002, TestSize.Level1)
{
    bool isRenderServiceNode = false;
    std::shared_ptr<RSNode> rootNode = std::make_shared<RSNode>(isRenderServiceNode);
    SurfaceId surfaceId = 0;
    RSTextureExport text(rootNode, surfaceId);
    text.virtualRootNode_ = RSRootNode::Create(false, true);
    ASSERT_NE(text.virtualRootNode_, nullptr);
    float x = 1.0f;
    float y = 2.0f;
    float width = 3.0f;
    float height = 4.0f;
    text.UpdateBufferInfo(x, y, width, height);
    auto bounds = text.virtualRootNode_->GetStagingProperties().GetBounds();
    EXPECT_FLOAT_EQ(bounds.x_, -x);
    EXPECT_FLOAT_EQ(bounds.y_, -y);
    EXPECT_FLOAT_EQ(bounds.z_, width);
    EXPECT_FLOAT_EQ(bounds.w_, height);
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
#endif
}