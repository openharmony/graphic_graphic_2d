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
#include "drawable/rs_root_render_node_drawable.h"
#include "pipeline/rs_root_render_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSRootRenderNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRootRenderNodeDrawableTest::SetUpTestCase() {}
void RSRootRenderNodeDrawableTest::TearDownTestCase() {}
void RSRootRenderNodeDrawableTest::SetUp() {}
void RSRootRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateRootRenderNodeDrawableTest
 * @tc.desc: Test If RootRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST(RSRootRenderNodeDrawableTest, CreateRootRenderNodeDrawable, TestSize.Level1)
{
    NodeId id = 1;
    auto renderNode = std::make_shared<RSRootRenderNode>(id);
    auto drawable = RSRootRenderNodeDrawable::OnGenerate(renderNode);
    ASSERT_NE(drawable, nullptr);
}
}
