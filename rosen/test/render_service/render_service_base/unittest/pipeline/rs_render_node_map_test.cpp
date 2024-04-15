/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_node_map.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderNodeMapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSRenderNodeMapTest::SetUpTestCase() {}
void RSRenderNodeMapTest::TearDownTestCase() {}
void RSRenderNodeMapTest::SetUp() {}
void RSRenderNodeMapTest::TearDown() {}

/**
 * @tc.name: ObtainScreenLockWindowNodeIdTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeMapTest, ObtainScreenLockWindowNodeIdTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = nullptr;
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.ObtainScreenLockWindowNodeId(surfaceNode);
}

} // namespace OHOS::Rosen