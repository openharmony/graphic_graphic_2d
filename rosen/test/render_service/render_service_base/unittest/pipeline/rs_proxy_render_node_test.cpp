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

#include <gtest/gtest.h>

#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_thread_visitor.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSProxyRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSProxyRenderNodeTest::SetUpTestCase() {}
void RSProxyRenderNodeTest::TearDownTestCase() {}
void RSProxyRenderNodeTest::SetUp() {}
void RSProxyRenderNodeTest::TearDown() {}

/**
 * @tc.name: LifeCycel001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSProxyRenderNodeTest, LifeCycel001, TestSize.Level1)
{
    NodeId id = 10;
    NodeId targetId = 11;
    RSSurfaceRenderNodeConfig config;
    auto target = std::make_shared<RSSurfaceRenderNode>(config);
    auto node = std::make_shared<RSProxyRenderNode>(id, target, targetId);
    std::shared_ptr<RSNodeVisitor> visitor = std::make_shared<RSRenderThreadVisitor>();
    node->Prepare(visitor);
    node->Process(visitor);
    auto matrix = Drawing::Matrix();
    auto clipRegion = Drawing::Rect();
    node->SetContextMatrix(matrix);
    node->SetContextClipRegion(clipRegion);
    node->ResetContextVariableCache();
    node->SetContextMatrix(matrix);
    node->SetContextAlpha(0.0f);
    clipRegion = Drawing::Rect(0.f, 0.f, 10.f, 10.f);
    node->SetContextClipRegion(clipRegion);
}
} // namespace Rosen
} // namespace OHOS