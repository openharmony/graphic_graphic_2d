/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "rs_animation_test_utils.h"

#include "animation/rs_path_animation.h"
#include "modifier_ng/appearance/rs_background_filter_modifier.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;

class RSPathAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSUIContext> rsUIContext;
};

void RSPathAnimationTest::SetUpTestCase() {}
void RSPathAnimationTest::TearDownTestCase() {}

void RSPathAnimationTest::SetUp()
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
}

void RSPathAnimationTest::TearDown() {}

/**
 * @tc.name: SetRotationTest001
 * @tc.desc: Verify the SetRotation
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, SetRotationTest001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, startProperty, endProperty);
    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    EXPECT_EQ(node->GetModifierCreatedBySetter(ModifierNG::RSModifierType::TRANSFORM), nullptr);
    pathAnimation->SetRotation(node, 1.0f);

    node->modifiersNGCreatedBySetter_.emplace(
        ModifierNG::RSModifierType::TRANSFORM, std::make_shared<ModifierNG::RSBackgroundFilterModifier>());
    EXPECT_NE(node->GetModifierCreatedBySetter(ModifierNG::RSModifierType::TRANSFORM), nullptr);
    pathAnimation->SetRotation(node, 1.0f);
}

/**
 * @tc.name: RebuildInRender001
 * @tc.desc: Verify RebuildInRender with null animation path
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, RebuildInRender001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest RebuildInRender001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, startProperty, endProperty);
    pathAnimation->SetRebuildParam({0.5f, false});
    pathAnimation->RebuildInRender();
    GTEST_LOG_(INFO) << "RSPathAnimationTest RebuildInRender001 end";
}

/**
 * @tc.name: PreProcessPathNullValues001
 * @tc.desc: Verify PreProcessPath handles null startValue/endValue
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, PreProcessPathNullValues001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest PreProcessPathNullValues001 start";
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, nullptr, ANIMATION_PATH, nullptr, nullptr);
    auto result = pathAnimation->PreProcessPath(ANIMATION_PATH, nullptr, nullptr);
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "RSPathAnimationTest PreProcessPathNullValues001 end";
}

/**
 * @tc.name: InitInterpolationValueNullStartEnd001
 * @tc.desc: Verify InitInterpolationValue handles null startValue/endValue
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, InitInterpolationValueNullStartEnd001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest InitInterpolationValueNullStartEnd001 start";
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, nullptr, ANIMATION_PATH, startProperty, endProperty);
    pathAnimation->startValue_ = nullptr;
    pathAnimation->endValue_ = nullptr;
    pathAnimation->InitInterpolationValue();
    EXPECT_EQ(pathAnimation->byValue_, nullptr);
    GTEST_LOG_(INFO) << "RSPathAnimationTest InitInterpolationValueNullStartEnd001 end";
}

/**
 * @tc.name: CreateRenderAnimationNullValues001
 * @tc.desc: Verify CreateRenderAnimation returns nullptr when originValue/startValue/endValue are null
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, CreateRenderAnimationNullValues001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest CreateRenderAnimationNullValues001 start";
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, nullptr, ANIMATION_PATH, nullptr, nullptr);
    pathAnimation->originValue_ = nullptr;
    pathAnimation->startValue_ = nullptr;
    pathAnimation->endValue_ = nullptr;
    auto result = pathAnimation->CreateRenderAnimation(nullptr);
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "RSPathAnimationTest CreateRenderAnimationNullValues001 end";
}

/**
 * @tc.name: CreateRenderAnimationStartValueNull001
 * @tc.desc: Verify CreateRenderAnimation returns nullptr when only startValue is null
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, CreateRenderAnimationStartValueNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest CreateRenderAnimationStartValueNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto pathAnimation = std::make_shared<RSPathAnimation>(rsUIContext, property, path);
    pathAnimation->originValue_ = property;
    pathAnimation->startValue_ = nullptr;
    pathAnimation->endValue_ = endProperty;
    auto result = pathAnimation->CreateRenderAnimation(nullptr);
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "RSPathAnimationTest CreateRenderAnimationStartValueNull001 end";
}

/**
 * @tc.name: CreateRenderAnimationEndValueNull001
 * @tc.desc: Verify CreateRenderAnimation returns nullptr when only endValue is null
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, CreateRenderAnimationEndValueNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest CreateRenderAnimationEndValueNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto pathAnimation = std::make_shared<RSPathAnimation>(rsUIContext, property, path);
    pathAnimation->originValue_ = property;
    pathAnimation->startValue_ = startProperty;
    pathAnimation->endValue_ = nullptr;
    auto result = pathAnimation->CreateRenderAnimation(nullptr);
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "RSPathAnimationTest CreateRenderAnimationEndValueNull001 end";
}

/**
 * @tc.name: CreateRenderAnimationAllNonNull001
 * @tc.desc: Verify CreateRenderAnimation returns valid animation when all values are non-null
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, CreateRenderAnimationAllNonNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest CreateRenderAnimationAllNonNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto canvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto pathAnimation = std::make_shared<RSPathAnimation>(rsUIContext, property, path);
    pathAnimation->target_ = canvasNode;
    pathAnimation->originValue_ = property;
    pathAnimation->startValue_ = startProperty;
    pathAnimation->endValue_ = endProperty;
    auto result = pathAnimation->CreateRenderAnimation(canvasNode);
    EXPECT_NE(result, nullptr);
    GTEST_LOG_(INFO) << "RSPathAnimationTest CreateRenderAnimationAllNonNull001 end";
}

/**
 * @tc.name: OnStartAnimationNull001
 * @tc.desc: Verify OnStart handles CreateRenderAnimation returning nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, OnStartAnimationNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest OnStartAnimationNull001 start";
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, nullptr, ANIMATION_PATH, nullptr, nullptr);
    pathAnimation->originValue_ = nullptr;
    pathAnimation->startValue_ = nullptr;
    pathAnimation->endValue_ = nullptr;
    pathAnimation->OnStart();
    EXPECT_FALSE(pathAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSPathAnimationTest OnStartAnimationNull001 end";
}

/**
 * @tc.name: OnStartAnimationNullWithTargetAndPath001
 * @tc.desc: Verify OnStart handles CreateRenderAnimation nullptr with valid target and path
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, OnStartAnimationNullWithTargetAndPath001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest OnStartAnimationNullWithTargetAndPath001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto canvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto pathAnimation = std::make_shared<RSPathAnimation>(rsUIContext, property, path);
    pathAnimation->target_ = canvasNode;
    pathAnimation->property_ = nullptr;
    pathAnimation->originValue_ = nullptr;
    pathAnimation->startValue_ = nullptr;
    pathAnimation->endValue_ = nullptr;
    pathAnimation->OnStart();
    EXPECT_FALSE(pathAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSPathAnimationTest OnStartAnimationNullWithTargetAndPath001 end";
}

/**
 * @tc.name: RebuildInRenderAnimationNull001
 * @tc.desc: Verify RebuildInRender handles CreateRenderAnimation returning nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, RebuildInRenderAnimationNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest RebuildInRenderAnimationNull001 start";
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, nullptr, ANIMATION_PATH, startProperty, endProperty);
    pathAnimation->originValue_ = nullptr;
    pathAnimation->startValue_ = nullptr;
    pathAnimation->endValue_ = nullptr;
    pathAnimation->RebuildInRender();
    EXPECT_FALSE(pathAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSPathAnimationTest RebuildInRenderAnimationNull001 end";
}

/**
 * @tc.name: RebuildInRenderAnimationNullWithTarget001
 * @tc.desc: Verify RebuildInRender handles animation nullptr with valid target
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, RebuildInRenderAnimationNullWithTarget001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest RebuildInRenderAnimationNullWithTarget001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto canvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto pathAnimation = std::make_shared<RSPathAnimation>(rsUIContext, property, path);
    pathAnimation->target_ = canvasNode;
    pathAnimation->originValue_ = nullptr;
    pathAnimation->startValue_ = nullptr;
    pathAnimation->endValue_ = nullptr;
    pathAnimation->SetRebuildParam({0.5f, false});
    pathAnimation->RebuildInRender();
    EXPECT_FLOAT_EQ(pathAnimation->GetRebuildParam().fraction, 0.5f);
    EXPECT_FALSE(pathAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSPathAnimationTest RebuildInRenderAnimationNullWithTarget001 end";
}

/**
 * @tc.name: OnStartPropertyNullIsNeedPath001
 * @tc.desc: Verify OnStart skips AddPathAnimation when property_ is null and isNeedPath_ is true
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, OnStartPropertyNullIsNeedPath001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest OnStartPropertyNullIsNeedPath001 start";
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, nullptr, ANIMATION_PATH, startProperty, endProperty);
    pathAnimation->isNeedPath_ = true;
    pathAnimation->property_ = nullptr;
    pathAnimation->OnStart();
    EXPECT_FALSE(pathAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSPathAnimationTest OnStartPropertyNullIsNeedPath001 end";
}

/**
 * @tc.name: GetType001
 * @tc.desc: Verify GetType returns PATH
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, GetType001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, startProperty, endProperty);
    EXPECT_EQ(pathAnimation->GetType(), RSAnimationType::PATH);
}

/**
 * @tc.name: SetRotation002
 * @tc.desc: Verify SetRotation sets value via TRANSFORM modifier when property not in properties_ map
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, SetRotation002, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, startProperty, endProperty);
    auto node = RSCanvasNode::Create(false, false, rsUIContext);

    node->SetRotation(0.f);
    auto modifier = node->GetModifierCreatedBySetter(ModifierNG::RSModifierType::TRANSFORM);
    ASSERT_NE(modifier, nullptr);
    auto rotationProp = modifier->GetProperty(ModifierNG::RSPropertyType::ROTATION);
    ASSERT_NE(rotationProp, nullptr);
    pathAnimation->rotationId_ = rotationProp->GetId();
    // Remove from properties_ to force the modifier path (line 284 true)
    node->properties_.erase(pathAnimation->rotationId_);

    pathAnimation->SetRotation(node, 45.0f);
    auto floatProp = std::static_pointer_cast<RSProperty<float>>(
        modifier->GetProperty(ModifierNG::RSPropertyType::ROTATION));
    EXPECT_FLOAT_EQ(floatProp->stagingValue_, 45.0f);
}

/**
 * @tc.name: PreProcessPathVector4f001
 * @tc.desc: Verify PreProcessPath and InitNeedPath with Vector4f properties
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, PreProcessPathVector4f001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(0.f, 0.f, 0.f, 0.f));
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(0.f, 0.f, 0.f, 0.f));
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(1.f, 1.f, 0.f, 0.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, startValue, endValue);
    // Constructor already calls InitNeedPath with Vector4f → line 352 true
    auto result = pathAnimation->PreProcessPath(ANIMATION_PATH, startValue, endValue);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: PreProcessPathUnsupportedType001
 * @tc.desc: Verify PreProcessPath returns null for unsupported property type (float)
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, PreProcessPathUnsupportedType001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, startValue, endValue);
    auto result = pathAnimation->PreProcessPath(ANIMATION_PATH, startValue, endValue);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: PreProcessPathMixedType001
 * @tc.desc: Verify PreProcessPath when startValue is Vector2f but endValue is Vector4f
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, PreProcessPathMixedType001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, nullptr, nullptr);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(1.f, 1.f, 0.f, 0.f));
    auto result = pathAnimation->PreProcessPath(ANIMATION_PATH, startValue, endValue);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: InitNeedPathMixedType001
 * @tc.desc: Verify InitNeedPath when startValue is Vector4f but endValue is Vector2f
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, InitNeedPathMixedType001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(0.f));
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(0.f, 0.f, 0.f, 0.f));
    auto endValue = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, startValue, endValue);
    // Constructor calls InitNeedPath: startVector4f non-null, endVector4f null → line 352 false
    EXPECT_FALSE(pathAnimation->isAdditive_);
}

/**
 * @tc.name: InitInterpolationVector2f001
 * @tc.desc: Verify InitInterpolationVector2f returns false for type mismatch
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, InitInterpolationVector2f001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto startValue = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endValue = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, startValue, endValue);

    // startValue not Vector2f → startVector2f null → short-circuit false
    auto v4start = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(0.f));
    EXPECT_FALSE(pathAnimation->InitInterpolationVector2f(v4start, endValue));

    // startValue is Vector2f but endValue not → endVector2f null → false
    auto v4end = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(1.f));
    EXPECT_FALSE(pathAnimation->InitInterpolationVector2f(startValue, v4end));
}

/**
 * @tc.name: InitInterpolationVector2f002
 * @tc.desc: Verify InitInterpolationVector2f originValue and needAddOrigin_ branches
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, InitInterpolationVector2f002, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto startValue = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    auto endValue = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, startValue, endValue);

    // Branch 1: originValue_ is null → originVector2f null → short-circuit false
    pathAnimation->originValue_ = nullptr;
    pathAnimation->needAddOrigin_ = true;
    EXPECT_TRUE(pathAnimation->InitInterpolationVector2f(startValue, endValue));

    // Branch 2: originValue_ is Vector2f but needAddOrigin_ is false
    pathAnimation->originValue_ = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.5f, 0.5f));
    pathAnimation->needAddOrigin_ = false;
    EXPECT_TRUE(pathAnimation->InitInterpolationVector2f(startValue, endValue));

    // Branch 3: originValue_ is Vector2f and needAddOrigin_ is true
    pathAnimation->needAddOrigin_ = true;
    EXPECT_TRUE(pathAnimation->InitInterpolationVector2f(startValue, endValue));
}

/**
 * @tc.name: InitInterpolationVector4f001
 * @tc.desc: Verify InitInterpolationVector4f returns false for type mismatch
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, InitInterpolationVector4f001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(0.f));
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(0.f, 0.f, 0.f, 0.f));
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(1.f, 1.f, 0.f, 0.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, startValue, endValue);

    // startValue not Vector4f → startVector4f null → short-circuit false
    auto v2start = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    EXPECT_FALSE(pathAnimation->InitInterpolationVector4f(v2start, endValue));

    // startValue is Vector4f but endValue not → endVector4f null → false
    auto v2end = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f));
    EXPECT_FALSE(pathAnimation->InitInterpolationVector4f(startValue, v2end));
}

/**
 * @tc.name: InitInterpolationVector4f002
 * @tc.desc: Verify InitInterpolationVector4f originValue and needAddOrigin_ branches
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, InitInterpolationVector4f002, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(0.f));
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(0.f, 0.f, 0.f, 0.f));
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(1.f, 1.f, 0.f, 0.f));
    auto pathAnimation =
        std::make_shared<RSPathAnimation>(rsUIContext, property, ANIMATION_PATH, startValue, endValue);

    // Branch 1: originValue_ is null → originVector4f null → short-circuit false
    pathAnimation->originValue_ = nullptr;
    pathAnimation->needAddOrigin_ = true;
    EXPECT_TRUE(pathAnimation->InitInterpolationVector4f(startValue, endValue));

    // Branch 2: originValue_ is Vector4f but needAddOrigin_ is false
    pathAnimation->originValue_ = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(0.5f));
    pathAnimation->needAddOrigin_ = false;
    EXPECT_TRUE(pathAnimation->InitInterpolationVector4f(startValue, endValue));

    // Branch 3: originValue_ is Vector4f and needAddOrigin_ is true
    pathAnimation->needAddOrigin_ = true;
    EXPECT_TRUE(pathAnimation->InitInterpolationVector4f(startValue, endValue));
}

} // namespace Rosen
} // namespace OHOS
