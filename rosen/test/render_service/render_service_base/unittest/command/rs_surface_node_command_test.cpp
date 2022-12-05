/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "include/command/rs_surface_node_command.h"
#include "include/pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceNodeCommandTest::SetUpTestCase() {}
void RSSurfaceNodeCommandTest::TearDownTestCase() {}
void RSSurfaceNodeCommandTest::SetUp() {}
void RSSurfaceNodeCommandTest::TearDown() {}

/**
 * @tc.name: TestRSSurfaceNodeCommand002
 * @tc.desc: ConnectToNodeInRenderService test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand002, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(context, id);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(*context2, id2);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand003
 * @tc.desc: SetContextClipRegion test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand003, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SkRect clipRect;
    SurfaceNodeCommandHelper::SetContextClipRegion(context, id, clipRect);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContextClipRegion(*context2, id2, clipRect);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand004
 * @tc.desc: SetSecurityLayer test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand004, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    bool isSecurityLayer = false;
    SurfaceNodeCommandHelper::SetSecurityLayer(context, id, isSecurityLayer);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetSecurityLayer(*context2, id2, isSecurityLayer);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand005
 * @tc.desc: SetColorSpace test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand005, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    ColorGamut colorSpace = ColorGamut::COLOR_GAMUT_INVALID;
    SurfaceNodeCommandHelper::SetColorSpace(context, id, colorSpace);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetColorSpace(*context2, id2, colorSpace);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand006
 * @tc.desc: UpdateSurfaceDefaultSize test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand006, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    float width = 0;
    float height = 0;
    SurfaceNodeCommandHelper::UpdateSurfaceDefaultSize(context, id, width, height);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::UpdateSurfaceDefaultSize(*context2, id2, width, height);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand007
 * @tc.desc: ConnectToNodeInRenderService test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand007, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(context, id);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(*context2, id2);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand008
 * @tc.desc: SetCallbackForRenderThreadRefresh test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand008, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    std::function<void(void)> callback;
    SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh(context, id, callback);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh(*context2, id2, callback);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand009
 * @tc.desc: SetContextBounds test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand009, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    Vector4f bounds;
    SurfaceNodeCommandHelper::SetContextBounds(context, id, bounds);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContextBounds(*context2, id2, bounds);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand010
 * @tc.desc: SetAbilityBGAlpha test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand010, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    uint8_t alpha = static_cast<uint8_t>(0);
    SurfaceNodeCommandHelper::SetAbilityBGAlpha(context, id, alpha);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetAbilityBGAlpha(*context2, id2, alpha);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand011
 * @tc.desc: UpdateParentWithoutTransition test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand011, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    NodeId parentId = static_cast<NodeId>(-2);
    SurfaceNodeCommandHelper::UpdateParentWithoutTransition(context, id, parentId);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    NodeId parentId2 = 11;
    SurfaceNodeCommandHelper::Create(*context2, parentId2);
    SurfaceNodeCommandHelper::UpdateParentWithoutTransition(*context2, id2, parentId2);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand012
 * @tc.desc: SetIsNotifyUIBufferAvailable test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand012, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    bool available = false;
    SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable(context, id, available);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable(*context2, id2, available);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand013
 * @tc.desc: SetSurfaceNodeType test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand013, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    RSSurfaceNodeType type = RSSurfaceNodeType::DEFAULT;
    SurfaceNodeCommandHelper::SetSurfaceNodeType(context, id, type);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetSurfaceNodeType(*context2, id2, type);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand015
 * @tc.desc: SetContextAlpha test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand015, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    float alpha = 0;
    SurfaceNodeCommandHelper::SetContextAlpha(context, id, alpha);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContextAlpha(*context2, id2, alpha);
}

/**
 * @tc.name: SetContextMatrix001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetContextMatrix001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    SkMatrix matrix;
    SurfaceNodeCommandHelper::SetContextMatrix(context, id, matrix);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
}

/**
 * @tc.name: SetAppFreeze001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetAppFreeze001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    bool isAppFreeze = false;
    SurfaceNodeCommandHelper::SetAppFreeze(context, id, isAppFreeze);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetAppFreeze(*context2, id2, isAppFreeze);
}

/**
 * @tc.name: SetContainerWindow001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetContainerWindow001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    bool hasContainerWindow = false;
    float density = 0.1;
    SurfaceNodeCommandHelper::SetContainerWindow(context, id, hasContainerWindow, density);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContainerWindow(*context2, id2, hasContainerWindow, density);
}
} // namespace OHOS::Rosen
