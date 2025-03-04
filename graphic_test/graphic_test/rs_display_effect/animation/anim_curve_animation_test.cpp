/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "anim_custom_modifier_test.h"
#include "rs_graphic_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class AnimationTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSurfaceBounds({ 0, 0, screenWidth, screenHeight });
    }
};

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 500, 500});
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timeingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timeingCurve, [&]() {
        animationCustomModifier->SetPosition(500);
    }, []() {
        std::cout << "Animation_Curve_Test_1 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_2)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 500, 500});
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timeingCurve = RSAnimationTimingCurve::SPRING;
    RSNode::Animate(protocol, timeingCurve, [&]() {
        animationCustomModifier->SetPosition(500);
    }, []() {
        std::cout << "Animation_Curve_Test_2 animation finish callback" << std::endl;
    });
}
} // namespace OHOS::Rosen