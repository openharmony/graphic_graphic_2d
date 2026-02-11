/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "pipeline/rs_surface_render_node_utils.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class RSSurfaceRenderNodeUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSSurfaceRenderNodeUtilsTest::SetUpTestCase() {}
void RSSurfaceRenderNodeUtilsTest::TearDownTestCase() {}
void RSSurfaceRenderNodeUtilsTest::SetUp() {}
void RSSurfaceRenderNodeUtilsTest::TearDown() {}

/**
 * @tc.name: IntersectHwcDamageWithTest
 * @tc.desc: Test the static method IntersectHwcDamageWith
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeUtilsTest, IntersectHwcDamageWithTest, TestSize.Level1)
{
    RectI rect { 0, 0, 50, 50 };
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    EXPECT_FALSE(RSSurfaceRenderNodeUtils::IntersectHwcDamageWith(*node, rect));

    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node->SetHardwareEnabled(true);
    auto tempHandler = node->surfaceHandler_;
    node->surfaceHandler_ = nullptr;
    EXPECT_FALSE(RSSurfaceRenderNodeUtils::IntersectHwcDamageWith(*node, rect));

    node->surfaceHandler_ = tempHandler;
    EXPECT_FALSE(RSSurfaceRenderNodeUtils::IntersectHwcDamageWith(*node, rect));

    node->surfaceHandler_->SetCurrentFrameBufferConsumed();
    EXPECT_FALSE(RSSurfaceRenderNodeUtils::IntersectHwcDamageWith(*node, rect));
}

} // namespace Rosen
} // namespace OHOS