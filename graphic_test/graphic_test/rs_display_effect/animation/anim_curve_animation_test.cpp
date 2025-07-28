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
        SetScreenSize(screenWidth, screenHeight);
    }
};

/*
 * @tc.name: Animation_Curve_Test_1
 * @tc.desc: Test the built-in animation curve EASE_IN_OUT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        []() { std::cout << "Animation_Curve_Test_1 animation finish callback" << std::endl; });
}

/*
 * @tc.name: Animation_Curve_Test_2
 * @tc.desc: Test the built-in animation curve SPRING
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_2)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::SPRING;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        []() { std::cout << "Animation_Curve_Test_2 animation finish callback" << std::endl; });
}

/*
 * @tc.name: Animation_Curve_Test_3
 * @tc.desc: Test the built-in animation curve LINEAR
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_3)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto lineaModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(lineaModifier);
    lineaModifier->SetTimeInterval(1.0f);
    lineaModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    RSNode::Animate(protocol, timingCurve, [&]() {
        lineaModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_3 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_4
 * @tc.desc: Test the built-in animation curve LINEAR
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_4)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto lineaModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(lineaModifier);
    lineaModifier->SetTimeInterval(3.0f);
    lineaModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    RSNode::Animate(protocol, timingCurve, [&]() {
        lineaModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_4 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_5
 * @tc.desc: Test the built-in animation curve LINEAR
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_5)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto lineaModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(lineaModifier);
    lineaModifier->SetTimeInterval(5.0f);
    lineaModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    RSNode::Animate(protocol, timingCurve, [&]() {
        lineaModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_5 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_6
 * @tc.desc: Test the built-in animation curve LINEAR
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_6)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto lineaModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(lineaModifier);
    lineaModifier->SetTimeInterval(7.0f);
    lineaModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(700);
    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    RSNode::Animate(protocol, timingCurve, [&]() {
        lineaModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_6 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_7
 * @tc.desc: Test the built-in animation curve LINEAR
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_7)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto lineaModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(lineaModifier);
    lineaModifier->SetTimeInterval(9.0f);
    lineaModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(900);
    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    RSNode::Animate(protocol, timingCurve, [&]() {
        lineaModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_7 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_8
 * @tc.desc: Test the built-in animation curve EASE
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_8)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeModifier);
    easeModifier->SetTimeInterval(1.0f);
    easeModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    auto timingCurve = RSAnimationTimingCurve::EASE;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_8 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_9
 * @tc.desc: Test the built-in animation curve EASE
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_9)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeModifier);
    easeModifier->SetTimeInterval(3.0f);
    easeModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    auto timingCurve = RSAnimationTimingCurve::EASE;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_9 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_10
 * @tc.desc: Test the built-in animation curve EASE
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_10)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeModifier);
    easeModifier->SetTimeInterval(5.0f);
    easeModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    auto timingCurve = RSAnimationTimingCurve::EASE;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_10 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_11
 * @tc.desc: Test the built-in animation curve EASE
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_11)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeModifier);
    easeModifier->SetTimeInterval(7.0f);
    easeModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(700);
    auto timingCurve = RSAnimationTimingCurve::EASE;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_11 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_12
 * @tc.desc: Test the built-in animation curve EASE
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_12)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeModifier);
    easeModifier->SetTimeInterval(9.0f);
    easeModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(900);
    auto timingCurve = RSAnimationTimingCurve::EASE;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_12 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_13
 * @tc.desc: Test the built-in animation curve EASE_IN
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_13)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeInModifier);
    easeInModifier->SetTimeInterval(1.0f);
    easeInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_13 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_14
 * @tc.desc: Test the built-in animation curve EASE_IN
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_14)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeInModifier);
    easeInModifier->SetTimeInterval(3.0f);
    easeInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_14 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_15
 * @tc.desc: Test the built-in animation curve EASE_IN
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_15)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeInModifier);
    easeInModifier->SetTimeInterval(5.0f);
    easeInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_15 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_16
 * @tc.desc: Test the built-in animation curve EASE_IN
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_16)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeInModifier);
    easeInModifier->SetTimeInterval(7.0f);
    easeInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(700);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_16 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_17
 * @tc.desc: Test the built-in animation curve EASE_IN
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_17)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeInModifier);
    easeInModifier->SetTimeInterval(9.0f);
    easeInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(900);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_17 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_18
 * @tc.desc: Test the built-in animation curve EASE_OUT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_18)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeOutInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeOutInModifier);
    easeOutInModifier->SetTimeInterval(1.0f);
    easeOutInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    auto timingCurve = RSAnimationTimingCurve::EASE_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeOutInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_18 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_19
 * @tc.desc: Test the built-in animation curve EASE_OUT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_19)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeOutInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeOutInModifier);
    easeOutInModifier->SetTimeInterval(3.0f);
    easeOutInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    auto timingCurve = RSAnimationTimingCurve::EASE_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeOutInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_19 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_20
 * @tc.desc: Test the built-in animation curve EASE_OUT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_20)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeOutInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeOutInModifier);
    easeOutInModifier->SetTimeInterval(5.0f);
    easeOutInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    auto timingCurve = RSAnimationTimingCurve::EASE_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeOutInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_20 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_21
 * @tc.desc: Test the built-in animation curve EASE_OUT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_21)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeOutInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeOutInModifier);
    easeOutInModifier->SetTimeInterval(7.0f);
    easeOutInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(700);
    auto timingCurve = RSAnimationTimingCurve::EASE_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeOutInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_21 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_22
 * @tc.desc: Test the built-in animation curve EASE_OUT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_22)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeOutInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeOutInModifier);
    easeOutInModifier->SetTimeInterval(9.0f);
    easeOutInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(900);
    auto timingCurve = RSAnimationTimingCurve::EASE_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeOutInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_22 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_23
 * @tc.desc: Test the built-in animation curve EASE_IN_OUT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_23)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeInOutInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeInOutInModifier);
    easeInOutInModifier->SetTimeInterval(1.0f);
    easeInOutInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeInOutInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_23 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_24
 * @tc.desc: Test the built-in animation curve EASE_IN_OUT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_24)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeInOutInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeInOutInModifier);
    easeInOutInModifier->SetTimeInterval(3.0f);
    easeInOutInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeInOutInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_24 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_25
 * @tc.desc: Test the built-in animation curve EASE_IN_OUT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_25)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeInOutInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeInOutInModifier);
    easeInOutInModifier->SetTimeInterval(5.0f);
    easeInOutInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeInOutInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_25 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_26
 * @tc.desc: Test the built-in animation curve EASE_IN_OUT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_26)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeInOutInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeInOutInModifier);
    easeInOutInModifier->SetTimeInterval(7.0f);
    easeInOutInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(700);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeInOutInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_26 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_27
 * @tc.desc: Test the built-in animation curve EASE_IN_OUT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_27)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto easeInOutInModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(easeInOutInModifier);
    easeInOutInModifier->SetTimeInterval(9.0f);
    easeInOutInModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(900);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        easeInOutInModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_27 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_28
 * @tc.desc: Test the built-in animation curve DEFAULT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_28)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto defaultModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(defaultModifier);
    defaultModifier->SetTimeInterval(1.0f);
    defaultModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    auto timingCurve = RSAnimationTimingCurve::DEFAULT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        defaultModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_28 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_29
 * @tc.desc: Test the built-in animation curve DEFAULT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_29)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto defaultModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(defaultModifier);
    defaultModifier->SetTimeInterval(3.0f);
    defaultModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    auto timingCurve = RSAnimationTimingCurve::DEFAULT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        defaultModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_29 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_30
 * @tc.desc: Test the built-in animation curve DEFAULT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_30)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto defaultModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(defaultModifier);
    defaultModifier->SetTimeInterval(5.0f);
    defaultModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    auto timingCurve = RSAnimationTimingCurve::DEFAULT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        defaultModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_30 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_31
 * @tc.desc: Test the built-in animation curve DEFAULT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_31)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto defaultModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(defaultModifier);
    defaultModifier->SetTimeInterval(7.0f);
    defaultModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(700);
    auto timingCurve = RSAnimationTimingCurve::DEFAULT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        defaultModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_31 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_32
 * @tc.desc: Test the built-in animation curve DEFAULT
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_32)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto defaultModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(defaultModifier);
    defaultModifier->SetTimeInterval(9.0f);
    defaultModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(900);
    auto timingCurve = RSAnimationTimingCurve::DEFAULT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        defaultModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_32 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_33
 * @tc.desc: Test the built-in animation curve SPRING
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_33)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto springModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(springModifier);
    springModifier->SetTimeInterval(1.0f);
    springModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    auto timingCurve = RSAnimationTimingCurve::SPRING;
    RSNode::Animate(protocol, timingCurve, [&]() {
        springModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_33 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_34
 * @tc.desc: Test the built-in animation curve SPRING
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_34)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto springModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(springModifier);
    springModifier->SetTimeInterval(3.0f);
    springModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    auto timingCurve = RSAnimationTimingCurve::SPRING;
    RSNode::Animate(protocol, timingCurve, [&]() {
        springModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_34 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_35
 * @tc.desc: Test the built-in animation curve SPRING
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_35)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto springModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(springModifier);
    springModifier->SetTimeInterval(5.0f);
    springModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    auto timingCurve = RSAnimationTimingCurve::SPRING;
    RSNode::Animate(protocol, timingCurve, [&]() {
        springModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_35 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_36
 * @tc.desc: Test the built-in animation curve SPRING
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_36)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto springModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(springModifier);
    springModifier->SetTimeInterval(7.0f);
    springModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(700);
    auto timingCurve = RSAnimationTimingCurve::SPRING;
    RSNode::Animate(protocol, timingCurve, [&]() {
        springModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_36 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_37
 * @tc.desc: Test the built-in animation curve SPRING
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_37)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto springModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(springModifier);
    springModifier->SetTimeInterval(9.0f);
    springModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(900);
    auto timingCurve = RSAnimationTimingCurve::SPRING;
    RSNode::Animate(protocol, timingCurve, [&]() {
        springModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_37 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_38
 * @tc.desc: Test the built-in animation curve INTERACTIVE_SPRING
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_38)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto interactiveSpringModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(interactiveSpringModifier);
    interactiveSpringModifier->SetTimeInterval(1.0f);
    interactiveSpringModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    auto timingCurve = RSAnimationTimingCurve::INTERACTIVE_SPRING;
    RSNode::Animate(protocol, timingCurve, [&]() {
        interactiveSpringModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_38 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_39
 * @tc.desc: Test the built-in animation curve INTERACTIVE_SPRING
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_39)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto interactiveSpringModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(interactiveSpringModifier);
    interactiveSpringModifier->SetTimeInterval(3.0f);
    interactiveSpringModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    auto timingCurve = RSAnimationTimingCurve::INTERACTIVE_SPRING;
    RSNode::Animate(protocol, timingCurve, [&]() {
        interactiveSpringModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_39 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_40
 * @tc.desc: Test the built-in animation curve INTERACTIVE_SPRING
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_40)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto interactiveSpringModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(interactiveSpringModifier);
    interactiveSpringModifier->SetTimeInterval(5.0f);
    interactiveSpringModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    auto timingCurve = RSAnimationTimingCurve::INTERACTIVE_SPRING;
    RSNode::Animate(protocol, timingCurve, [&]() {
        interactiveSpringModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_40 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_41
 * @tc.desc: Test the built-in animation curve INTERACTIVE_SPRING
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_41)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto interactiveSpringModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(interactiveSpringModifier);
    interactiveSpringModifier->SetTimeInterval(7.0f);
    interactiveSpringModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(700);
    auto timingCurve = RSAnimationTimingCurve::INTERACTIVE_SPRING;
    RSNode::Animate(protocol, timingCurve, [&]() {
        interactiveSpringModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_41 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_42
 * @tc.desc: Test the built-in animation curve INTERACTIVE_SPRING
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_42)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto interactiveSpringModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(interactiveSpringModifier);
    interactiveSpringModifier->SetTimeInterval(9.0f);
    interactiveSpringModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(900);
    auto timingCurve = RSAnimationTimingCurve::INTERACTIVE_SPRING;
    RSNode::Animate(protocol, timingCurve, [&]() {
        interactiveSpringModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_42 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_43
 * @tc.desc: Test the default animation curve
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_43)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto timeCurveModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(timeCurveModifier);
    timeCurveModifier->SetTimeInterval(1.0f);
    timeCurveModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    auto timingCurve = RSAnimationTimingCurve();
    RSNode::Animate(protocol, timingCurve, [&]() {
        timeCurveModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_43 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_44
 * @tc.desc: Test the default animation curve
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_44)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto timeCurveModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(timeCurveModifier);
    timeCurveModifier->SetTimeInterval(3.0f);
    timeCurveModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    auto timingCurve = RSAnimationTimingCurve();
    RSNode::Animate(protocol, timingCurve, [&]() {
        timeCurveModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_44 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_45
 * @tc.desc: Test the default animation curve
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_45)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto timeCurveModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(timeCurveModifier);
    timeCurveModifier->SetTimeInterval(5.0f);
    timeCurveModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    auto timingCurve = RSAnimationTimingCurve();
    RSNode::Animate(protocol, timingCurve, [&]() {
        timeCurveModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_45 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_46
 * @tc.desc: Test the default animation curve
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_46)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto timeCurveModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(timeCurveModifier);
    timeCurveModifier->SetTimeInterval(7.0f);
    timeCurveModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(700);
    auto timingCurve = RSAnimationTimingCurve();
    RSNode::Animate(protocol, timingCurve, [&]() {
        timeCurveModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_46 animation finish callback" << std::endl;
    });
}

/*
 * @tc.name: Animation_Curve_Test_47
 * @tc.desc: Test the default animation curve
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_Curve_Test_47)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 1200, 2000});
    auto timeCurveModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(timeCurveModifier);
    timeCurveModifier->SetTimeInterval(9.0f);
    timeCurveModifier->SetPosition(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(900);
    auto timingCurve = RSAnimationTimingCurve();
    RSNode::Animate(protocol, timingCurve, [&]() {
        timeCurveModifier->SetPosition(1050);
    }, []() {
        std::cout << "Animation_Curve_Test_47 animation finish callback" << std::endl;
    });
}
} // namespace OHOS::Rosen