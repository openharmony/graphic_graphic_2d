/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "pipeline/rs_display_render_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSDisplayRenderNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDisplayRenderNodeDrawableTest::SetUpTestCase() {}
void RSDisplayRenderNodeDrawableTest::TearDownTestCase() {}
void RSDisplayRenderNodeDrawableTest::SetUp() {}
void RSDisplayRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateDisplayRenderNodeDrawableTest
 * @tc.desc: Test If DisplayRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST(RSDisplayRenderNodeDrawableTest, CreateDisplayRenderNodeDrawable, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    NodeId id = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto drawable = RSDisplayRenderNodeDrawable::OnGenerate(displayNode);
    ASSERT_NE(drawable, nullptr);
}
}
