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
#include "include/command/rs_node_showing_command.h"

#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSShowingNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShowingNodeCommandTest::SetUpTestCase() {}
void RSShowingNodeCommandTest::TearDownTestCase() {}
void RSShowingNodeCommandTest::SetUp() {}
void RSShowingNodeCommandTest::TearDown() {}

/**
 * @tc.name: RSShowingNodeCommandTest001
 * @tc.desc: Marshalling and Unmarshalling test.
 * @tc.type: FUNC
 */
HWTEST_F(RSShowingNodeCommandTest, RSShowingNodeCommandTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingNodeCommandTest RSShowingNodeCommandTest001 start";
    NodeId id = static_cast<NodeId>(-1);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, 0, RSRenderPropertyType::PROPERTY_FLOAT);

    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(id, property);
    EXPECT_TRUE(task != nullptr);
    Parcel parcel;
    task->Marshalling(parcel);

    auto copy = RSNodeGetShowingPropertyAndCancelAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(copy == nullptr);

    Parcel parcel1;
    Parcel parcel2;
    auto result = task->ReadFromParcel(parcel1);
    result = task->CheckHeader(parcel2);
    EXPECT_TRUE(result == false);
    GTEST_LOG_(INFO) << "RSShowingNodeCommandTest RSShowingNodeCommandTest001 end";
}

/**
 * @tc.name: RSShowingNodeCommandTest002
 * @tc.desc: Process and GetProperty test.
 * @tc.type: FUNC
 */
HWTEST_F(RSShowingNodeCommandTest, RSShowingNodeCommandTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingNodeCommandTest RSShowingNodeCommandTest002 start";
    NodeId id = static_cast<NodeId>(-1);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);

    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(id, property);
    EXPECT_TRUE(task != nullptr);
    RSContext context;
    task->Process(context);
    auto result = task->GetProperty();
    EXPECT_TRUE(result != nullptr);
    GTEST_LOG_(INFO) << "RSShowingNodeCommandTest RSShowingNodeCommandTest002 end";
}
} // namespace OHOS::Rosen
