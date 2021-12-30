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
#include "pipeline/rs_render_result.h"
#include "surface.h"
#include "ui/rs_node.h"
#include "ui/rs_ui_director.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIDirectorTest : public testing::Test {
public:
    static constexpr int g_normalInt_1 = 123;
    static constexpr int g_normalInt_2 = 34342;
    static constexpr int g_normalInt_3 = 3245;
    static constexpr int g_ExtremeInt_1 = 1;
    static constexpr int g_ExtremeInt_2 = -1;
    static constexpr int g_ExtremeInt_3 = 0;

    static constexpr uint64_t g_normalUInt64_1 = 123;
    static constexpr uint64_t g_normalUInt64_2 = 34342;
    static constexpr uint64_t g_normalUInt64_3 = 3245;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIDirectorTest::SetUpTestCase() {}
void RSUIDirectorTest::TearDownTestCase() {}
void RSUIDirectorTest::SetUp() {}
void RSUIDirectorTest::TearDown() {}

/**
 * @tc.name: SetSurfaceSize001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSUIDirectorTest, SetSurfaceSize001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SetSurfaceSize(g_normalInt_1, g_normalInt_2);
}

/**
 * @tc.name: SetSurfaceSize004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSUIDirectorTest, SetSurfaceSize002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SetSurfaceSize(std::numeric_limits<int>::max(), std::numeric_limits<int>::min());
}

/**
 * @tc.name: SetTimeStamp001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSUIDirectorTest, SetTimeStamp001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SetTimeStamp(g_normalUInt64_1);
}

/**
 * @tc.name: SetTimeStamp002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSUIDirectorTest, SetTimeStamp002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SetTimeStamp(-std::numeric_limits<uint64_t>::max());
}

/**
 * @tc.name: SetTimeStamp003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSUIDirectorTest, SetTimeStamp003, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SetTimeStamp(std::numeric_limits<int64_t>::min());
}

/**
 * @tc.name: SetPlatformSurface002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSUIDirectorTest, SetPlatformSurface002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    auto surfaceTemp = OHOS::Surface::CreateSurfaceAsConsumer();
    director->SetPlatformSurface(surfaceTemp);
}

/**
 * @tc.name: PlatformInit001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSUIDirectorTest, PlatformInit001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->Init();
}

/**
 * @tc.name: SetUITaskRunner001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSUIDirectorTest, SetUITaskRunner001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SetUITaskRunner([&](const auto& uiTaskRunner) {});
}

/**
 * @tc.name: DirectorSendMessages001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSUIDirectorTest, DirectorSendMessages001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SendMessages();
}

/**
 * @tc.name: UIDirectorSetRoot001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSUIDirectorTest, UIDirectorSetRoot001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    RSNode::SharedPtr testNode = RSNode::Create();
    director->SetRoot(testNode->GetId());
}

/**
 * @tc.name: UIDirectorTotal001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSUIDirectorTest, UIDirectorTotal001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    RSNode::SharedPtr rootNode = RSNode::Create();
    RSNode::SharedPtr child1 = RSNode::Create();
    RSNode::SharedPtr child2 = RSNode::Create();
    RSNode::SharedPtr child3 = RSNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);

    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();

    director->Init();

    director->SetSurfaceSize(800, 600);

    director->SetRoot(rootNode->GetId());

    director->SetTimeStamp(345);
    director->SetPlatformSurface(nullptr);
    auto surfaceTemp = OHOS::Surface::CreateSurfaceAsConsumer();
    director->SetPlatformSurface(surfaceTemp);

    director->SetUITaskRunner([&](const auto& uiTaskRunner) {});
    director->SendMessages();
}
} // namespace OHOS::Rosen
