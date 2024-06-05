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

#include "drawable/rs_drawable.h"
#include "drawable/rs_property_drawable_background.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSDrawableTest::SetUpTestCase() {}
void RSDrawableTest::TearDownTestCase() {}
void RSDrawableTest::SetUp() {}
void RSDrawableTest::TearDown() {}

/**
 * @tc.name: CalculateDirtySlots
 * @tc.desc: Test CalculateDirtySlots
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlots, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    ModifierDirtyTypes dirtyTypes(std::string("001100111"));
    RSDrawable::Vec drawableVec;
    RSDrawable::CalculateDirtySlots(dirtyTypes, drawableVec);
    ModifierDirtyTypes dirtyTypesTwo(std::string("111111111"));
    std::optional<Vector4f> aiInvert = { Vector4f() };
    node.renderContent_->GetMutableRenderProperties().SetAiInvert(aiInvert);
    ASSERT_TRUE(node.GetRenderProperties().GetAiInvert());
    ASSERT_NE(RSDrawable::CalculateDirtySlots(dirtyTypesTwo, drawableVec).size(), 0);
}

/**
 * @tc.name: UpdateDirtySlots
 * @tc.desc: Test UpdateDirtySlots
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSDrawableTest, UpdateDirtySlots, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    RSDrawable::Vec drawableVec;
    std::unordered_set<RSDrawableSlot> dirtySlots;
    for (size_t i = 0; i < 33; ++i) {
        dirtySlots.insert(static_cast<RSDrawableSlot>(static_cast<size_t>(RSDrawableSlot::SAVE_ALL) + i));
    }
    RSDrawable::UpdateDirtySlots(node, drawableVec, dirtySlots);
    std::shared_ptr<RSShader> shader = RSShader::CreateRSShader();
    node.renderContent_->GetMutableRenderProperties().SetBackgroundShader(shader);
    for (auto& drawable : drawableVec) {
        drawable = DrawableV2::RSBackgroundShaderDrawable::OnGenerate(node);
        ASSERT_TRUE(drawable);
    }
    ASSERT_FALSE(RSDrawable::UpdateDirtySlots(node, drawableVec, dirtySlots));
}

/**
 * @tc.name: UpdateSaveRestore
 * @tc.desc: Test UpdateSaveRestore
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSDrawableTest, UpdateSaveRestore, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    RSDrawable::Vec drawableVec;
    uint8_t drawableVecStatus = 77;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_NE(drawableVecStatus, 77);
}
} // namespace OHOS::Rosen