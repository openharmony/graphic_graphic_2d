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

#include "animation/rs_atlas_info_animation.h"
#include "animation/rs_animation_timing_protocol.h"
#include "common/rs_atlas_info.h"
#include "modifier/rs_animatable_arithmetic.h"
#include "modifier/rs_property.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSAtlasInfoAnimationTest : public testing::Test {
public:
    std::shared_ptr<RSUIContext> CreateRSUIContext();
    const AtlasInfo START_INFO = []() { AtlasInfo i; i.frameIndex = 0.0f; i.totalFrame = 6; return i; }();
    const AtlasInfo END_INFO = []() { AtlasInfo i; i.frameIndex = 5.0f; i.totalFrame = 6; return i; }();
};

std::shared_ptr<RSUIContext> RSAtlasInfoAnimationTest::CreateRSUIContext()
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    return rsUIContext;
}

/**
 * @tc.name: OnUpdateStagingValueNotFirstStart001
 * @tc.desc: Verify OnUpdateStagingValue returns early when isFirstStart is false.
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasInfoAnimationTest, OnUpdateStagingValueNotFirstStart001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<AtlasInfo>>(START_INFO);
    auto startProperty = std::make_shared<RSAnimatableProperty<AtlasInfo>>(START_INFO);
    auto endProperty = std::make_shared<RSAnimatableProperty<AtlasInfo>>(END_INFO);
    auto rsUIContext = CreateRSUIContext();
    auto animation = std::make_shared<RSAtlasInfoAnimation>(rsUIContext, property, startProperty, endProperty);
    animation->OnUpdateStagingValue(false); // isFirstStart=false -> early return
    // stagingValue_ unchanged.
    EXPECT_FLOAT_EQ(property->Get().frameIndex, 0.0f);
}

/**
 * @tc.name: OnUpdateStagingValueNullProperty001
 * @tc.desc: Verify OnUpdateStagingValue returns early when property_ is null.
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasInfoAnimationTest, OnUpdateStagingValueNullProperty001, TestSize.Level1)
{
    auto startProperty = std::make_shared<RSAnimatableProperty<AtlasInfo>>(START_INFO);
    auto endProperty = std::make_shared<RSAnimatableProperty<AtlasInfo>>(END_INFO);
    auto rsUIContext = CreateRSUIContext();
    auto animation = std::make_shared<RSAtlasInfoAnimation>(rsUIContext, nullptr, startProperty, endProperty);
    animation->OnUpdateStagingValue(true); // property_ null -> early return, no crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: OnUpdateStagingValueNoAutoReverse001
 * @tc.desc: Verify OnUpdateStagingValue sets property to endValue when autoReverse is off.
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasInfoAnimationTest, OnUpdateStagingValueNoAutoReverse001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<AtlasInfo>>(START_INFO);
    auto startProperty = std::make_shared<RSAnimatableProperty<AtlasInfo>>(START_INFO);
    auto endProperty = std::make_shared<RSAnimatableProperty<AtlasInfo>>(END_INFO);
    auto rsUIContext = CreateRSUIContext();
    auto animation = std::make_shared<RSAtlasInfoAnimation>(rsUIContext, property, startProperty, endProperty);
    animation->SetAutoReverse(false);
    animation->SetRepeatCount(1);
    animation->OnUpdateStagingValue(true);
    EXPECT_FLOAT_EQ(property->Get().frameIndex, 5.0f);
}

/**
 * @tc.name: OnUpdateStagingValueAutoReverseEvenRepeat001
 * @tc.desc: Verify OnUpdateStagingValue sets property to startValue when autoReverse and even repeatCount.
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasInfoAnimationTest, OnUpdateStagingValueAutoReverseEvenRepeat001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<AtlasInfo>>(START_INFO);
    auto startProperty = std::make_shared<RSAnimatableProperty<AtlasInfo>>(START_INFO);
    auto endProperty = std::make_shared<RSAnimatableProperty<AtlasInfo>>(END_INFO);
    auto rsUIContext = CreateRSUIContext();
    auto animation = std::make_shared<RSAtlasInfoAnimation>(rsUIContext, property, startProperty, endProperty);
    animation->SetAutoReverse(true);
    animation->SetRepeatCount(2); // even -> targetValue = startValue_
    animation->OnUpdateStagingValue(true);
    EXPECT_FLOAT_EQ(property->Get().frameIndex, 0.0f);
}

/**
 * @tc.name: OnUpdateStagingValueAutoReverseOddRepeat001
 * @tc.desc: Verify OnUpdateStagingValue sets property to endValue when autoReverse and odd repeatCount.
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasInfoAnimationTest, OnUpdateStagingValueAutoReverseOddRepeat001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<AtlasInfo>>(START_INFO);
    auto startProperty = std::make_shared<RSAnimatableProperty<AtlasInfo>>(START_INFO);
    auto endProperty = std::make_shared<RSAnimatableProperty<AtlasInfo>>(END_INFO);
    auto rsUIContext = CreateRSUIContext();
    auto animation = std::make_shared<RSAtlasInfoAnimation>(rsUIContext, property, startProperty, endProperty);
    animation->SetAutoReverse(true);
    animation->SetRepeatCount(3); // odd -> targetValue = endValue_
    animation->OnUpdateStagingValue(true);
    EXPECT_FLOAT_EQ(property->Get().frameIndex, 5.0f);
}

/**
 * @tc.name: UpdateToRenderAtlasInfoCacheMiss001
 * @tc.desc: First UpdateToRender<AtlasInfo> call hits cache miss -> full IPC path.
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasInfoAnimationTest, UpdateToRenderAtlasInfoCacheMiss001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<AtlasInfo>>(START_INFO);
    // No node attached; UPDATE_TO_RENDER early-returns but g_lastSentAtlas cache is still populated.
    property->UpdateToRender(START_INFO, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
    // stagingValue_ unchanged by UpdateToRender.
    EXPECT_FLOAT_EQ(property->Get().frameIndex, 0.0f);
}

/**
 * @tc.name: UpdateToRenderAtlasInfoCacheHit001
 * @tc.desc: Second UpdateToRender<AtlasInfo> with same context -> lightweight frameIndex IPC path.
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasInfoAnimationTest, UpdateToRenderAtlasInfoCacheHit001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<AtlasInfo>>(START_INFO);
    property->UpdateToRender(START_INFO, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
    // Same context (totalFrame=6), only frameIndex differs -> cache hit branch.
    AtlasInfo next = START_INFO;
    next.frameIndex = 4.0f;
    property->UpdateToRender(next, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
    EXPECT_FLOAT_EQ(property->Get().frameIndex, 0.0f);
}

/**
 * @tc.name: UpdateToRenderAtlasInfoContextChanged001
 * @tc.desc: UpdateToRender<AtlasInfo> with changed context -> cache miss, full IPC path.
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasInfoAnimationTest, UpdateToRenderAtlasInfoContextChanged001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<AtlasInfo>>(START_INFO);
    property->UpdateToRender(START_INFO, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
    // Different context (totalFrame differs) -> HasSameContext false -> cache miss full path.
    AtlasInfo changed = START_INFO;
    changed.frameIndex = 2.0f;
    changed.totalFrame = 10; // context field changed
    property->UpdateToRender(changed, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
    EXPECT_FLOAT_EQ(property->Get().frameIndex, 0.0f);
}

/**
 * @tc.name: RSPropertyAtlasInfoDestructorNoCrash001
 * @tc.desc: RSProperty<AtlasInfo> destructor erases g_lastSentAtlas entry without crash.
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasInfoAnimationTest, RSPropertyAtlasInfoDestructorNoCrash001, TestSize.Level1)
{
    {
        auto property = std::make_shared<RSAnimatableProperty<AtlasInfo>>(START_INFO);
        property->UpdateToRender(START_INFO, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
    } // destructor runs here, should erase g_lastSentAtlas[id_] without crash.
    EXPECT_TRUE(true);
}
} // namespace Rosen
} // namespace OHOS
