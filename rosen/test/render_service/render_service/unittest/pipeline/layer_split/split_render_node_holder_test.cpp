/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "pipeline/layer_split/split_render_node_holder.h"
#include "pipeline/layer_split/selector/opinc/rs_opinc_split_node_selector.h"
#include "pipeline/layer_split/splitter/opinc/rs_opinc_layer_splitter_planner.h"
#include "pipeline/layer_split/splitter/opinc/rs_opinc_layer_splitter_processor.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {

class SplitRenderNodeHolderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

HWTEST_F(SplitRenderNodeHolderTest, Constructor001, TestSize.Level1)
{
    auto holder = std::make_shared<SplitRenderNodeHolder>(nullptr);
    ASSERT_NE(holder, nullptr);
}

HWTEST_F(SplitRenderNodeHolderTest, GetParent001, TestSize.Level1)
{
    auto holder = std::make_shared<SplitRenderNodeHolder>(nullptr);
    ASSERT_NE(holder, nullptr);
    auto parent = holder->getParent();
    ASSERT_EQ(parent, nullptr);
}

HWTEST_F(SplitRenderNodeHolderTest, SetPlanner001, TestSize.Level1)
{
    auto holder = std::make_shared<SplitRenderNodeHolder>(nullptr);
    ASSERT_NE(holder, nullptr);
    auto planner = OpincSplitNodeSelector::GetInstance()->MakePlanner();
    holder->setPlanner(planner);
}

HWTEST_F(SplitRenderNodeHolderTest, SetProcessor001, TestSize.Level1)
{
    auto holder = std::make_shared<SplitRenderNodeHolder>(nullptr);
    ASSERT_NE(holder, nullptr);
    auto processor = OpincSplitNodeSelector::GetInstance()->MakeProcessor();
    holder->setProcessor(processor);
}

HWTEST_F(SplitRenderNodeHolderTest, Sync001, TestSize.Level1)
{
    auto holder = std::make_shared<SplitRenderNodeHolder>(nullptr);
    ASSERT_NE(holder, nullptr);
    holder->sync();
}

HWTEST_F(SplitRenderNodeHolderTest, Sync002, TestSize.Level1)
{
    auto holder = std::make_shared<SplitRenderNodeHolder>(nullptr);
    ASSERT_NE(holder, nullptr);
    auto planner = OpincSplitNodeSelector::GetInstance()->MakePlanner();
    auto processor = OpincSplitNodeSelector::GetInstance()->MakeProcessor();
    holder->setPlanner(planner);
    holder->setProcessor(processor);
    holder->sync();
}

HWTEST_F(SplitRenderNodeHolderTest, ToString001, TestSize.Level1)
{
    auto holder = std::make_shared<SplitRenderNodeHolder>(nullptr);
    ASSERT_NE(holder, nullptr);
    auto str = holder->toString();
    ASSERT_EQ(str, "Parent:null planner_:null");
}

HWTEST_F(SplitRenderNodeHolderTest, ToString002, TestSize.Level1)
{
    auto holder = std::make_shared<SplitRenderNodeHolder>(nullptr);
    ASSERT_NE(holder, nullptr);
    auto planner = OpincSplitNodeSelector::GetInstance()->MakePlanner();
    holder->setPlanner(planner);
    auto str = holder->toString();
    ASSERT_NE(str, "");
}

HWTEST_F(SplitRenderNodeHolderTest, ToString003, TestSize.Level1)
{
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    auto holder = std::make_shared<SplitRenderNodeHolder>(parentNode);
    ASSERT_NE(holder, nullptr);
    auto planner = OpincSplitNodeSelector::GetInstance()->MakePlanner();
    holder->setPlanner(planner);
    planner->SetOpIncParentNode(parentNode);
    auto str = holder->toString();
    ASSERT_NE(str, "");
}

} // namespace
} // namespace OHOS::Rosen