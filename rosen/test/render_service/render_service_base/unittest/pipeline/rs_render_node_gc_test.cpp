/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_node_gc.h"
#include "drawable/rs_render_node_shadow_drawable.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSRenderNodeGCTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderNodeGCTest::SetUpTestCase() {}
void RSRenderNodeGCTest::TearDownTestCase() {}
void RSRenderNodeGCTest::SetUp() {}
void RSRenderNodeGCTest::TearDown() {}

/**
 * @tc.name: NodeDestructor001
 * @tc.desc: test results of NodeDestructor
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeGCTest, NodeDestructor001, TestSize.Level1)
{
    auto ptr = std::make_shared<RSRenderNode>(0);
    RSRenderNodeGC::NodeDestructor(ptr.get());
    EXPECT_TRUE(ptr != nullptr);
}

/**
 * @tc.name: NodeDestructorInner001
 * @tc.desc: test results of NodeDestructorInner
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeGCTest, NodeDestructorInner001, TestSize.Level1)
{
    RSRenderNodeGC& node = RSRenderNodeGC::Instance();
    node.NodeDestructorInner(nullptr);
    EXPECT_TRUE(node.nodeBucket_.size() == 1);

    auto ptr = std::make_shared<RSRenderNode>(0);
    node.NodeDestructorInner(ptr.get());
    EXPECT_TRUE(ptr != nullptr);

    node.NodeDestructorInner(ptr.get());
    EXPECT_TRUE(ptr != nullptr);
}

/**
 * @tc.name: ReleaseNodeMemory001
 * @tc.desc: test results of ReleaseNodeMemory
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeMemory001, TestSize.Level1)
{
    RSRenderNodeGC& node = RSRenderNodeGC::Instance();
    std::queue<std::vector<RSRenderNode*>> tempQueue;
    node.nodeBucket_.swap(tempQueue);
    node.ReleaseNodeMemory();
    EXPECT_TRUE(node.nodeBucket_.size() == 0);

    auto ptrToNode = new RSRenderNode(0);
    node.NodeDestructorInner(ptrToNode);
    node.ReleaseNodeMemory();
    EXPECT_TRUE(node.nodeBucket_.size() == 0);
}

/**
 * @tc.name: ReleaseDrawableMemory001
 * @tc.desc: test results of ReleaseDrawableMemory
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseDrawableMemory001, TestSize.Level1)
{
    class ConcreteRSRenderNodeDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
    public:
        explicit ConcreteRSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
            : RSRenderNodeDrawableAdapter(std::move(node))
        {}
        void Draw(Drawing::Canvas& canvas) {}
    };
    RSRenderNodeGC& node = RSRenderNodeGC::Instance();
    node.ReleaseDrawableMemory();
    EXPECT_TRUE(node.drawableBucket_.size() == 0);

    std::shared_ptr<const RSRenderNode> otherNode = std::make_shared<const RSRenderNode>(0);
    DrawableV2::RSRenderNodeDrawableAdapter* ptrToNode = new ConcreteRSRenderNodeDrawableAdapter(otherNode);
    node.DrawableDestructorInner(ptrToNode);
    node.ReleaseDrawableMemory();
    EXPECT_TRUE(node.drawableBucket_.size() == 0);
}
} // namespace Rosen
} // namespace OHOS
