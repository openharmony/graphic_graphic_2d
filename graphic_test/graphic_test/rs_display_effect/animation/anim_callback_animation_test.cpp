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
#include "rs_graphic_test_text.h"

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

RSCanvasNode::SharedPtr FinishCallbackFunc()
{
    auto finishCallbackNode = RSCanvasNode::Create();
    finishCallbackNode->SetBounds({ 0, 1500, 500, 500 });
    finishCallbackNode->SetBackgroundColor(0xff00ff00);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetText("finish callback suc");
    float frontSize = 50;
    textModifier->SetFontSize(frontSize);
    finishCallbackNode->AddModifier(textModifier);
    return finishCallbackNode;
}

RSCanvasNode::SharedPtr RepeatCallbackFunc(int countNum)
{
    auto repeatCallbackNode = RSCanvasNode::Create();
    repeatCallbackNode->SetBounds({ 0, 1500 + countNum, 500, 500 });
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetText("repeat callback num:" + std::to_string(countNum));
    float frontSize = 50;
    textModifier->SetFontSize(frontSize);
    repeatCallbackNode->AddModifier(textModifier);
    return repeatCallbackNode;
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_FinishCallback_Test_1)
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
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_FinishCallback_Test_1 animation finish callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_RepeatCallback_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    int repeatCount = 0; // repeatCount 0
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    protocol.SetRepeatCount(repeatCount);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_RepeatCallback_Test_1 animation finish callback" << std::endl;
        },
        [this]() {
            static int countNum = 0;
            auto repeatCallbackNode = RepeatCallbackFunc(++countNum);
            GetRootNode()->AddChild(repeatCallbackNode);
            RegisterNode(repeatCallbackNode);
            std::cout << "Animation_RepeatCallback_Test_1 animation repeat callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_RepeatCallback_Test_2)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    int repeatCount = 1; // repeatCount 1
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    protocol.SetRepeatCount(repeatCount);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_RepeatCallback_Test_2 animation finish callback" << std::endl;
        },
        [this]() {
            static int countNum = 0;
            auto repeatCallbackNode = RepeatCallbackFunc(++countNum);
            GetRootNode()->AddChild(repeatCallbackNode);
            RegisterNode(repeatCallbackNode);
            std::cout << "Animation_RepeatCallback_Test_2 animation repeat callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_RepeatCallback_Test_3)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    int repeatCount = 3; // repeatCount 3
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    protocol.SetRepeatCount(repeatCount);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_RepeatCallback_Test_3 animation finish callback" << std::endl;
        },
        [this]() {
            static int countNum = 0;
            auto repeatCallbackNode = RepeatCallbackFunc(++countNum);
            GetRootNode()->AddChild(repeatCallbackNode);
            RegisterNode(repeatCallbackNode);
            std::cout << "Animation_RepeatCallback_Test_3 animation repeat callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_RepeatCallback_Test_4)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    int repeatCount = 10; // repeatCount 10
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    protocol.SetRepeatCount(repeatCount);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_RepeatCallback_Test_4 animation finish callback" << std::endl;
        },
        [this]() {
            static int countNum = 0;
            auto repeatCallbackNode = RepeatCallbackFunc(++countNum);
            GetRootNode()->AddChild(repeatCallbackNode);
            RegisterNode(repeatCallbackNode);
            std::cout << "Animation_RepeatCallback_Test_4 animation repeat callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_OpenImplicitAnimation_Test_1)
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
    // just OpenImplicitAnimation
    RSNode::OpenImplicitAnimation(protocol, timingCurve, [this]() {
        std::cout << "Animation_OpenImplicitAnimation_Test_1 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_CloseImplicitAnimation_Test_1)
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
    // just CloseImplicitAnimation
    RSNode::CloseImplicitAnimation();
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_OpenImplicitAnimation_CloseImplicitAnimation_Test_1)
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
    // OpenImplicitAnimation & CloseImplicitAnimation
    RSNode::OpenImplicitAnimation(protocol, timingCurve, [this]() {
        auto finishCallbackNode = FinishCallbackFunc();
        GetRootNode()->AddChild(finishCallbackNode);
        RegisterNode(finishCallbackNode);
        std::cout << "Animation_OpenImplicitAnimation_CloseImplicitAnimation_Test_1 animation finish callback"
                  << std::endl;
    });
    animationCustomModifier->SetPosition(500);
    RSNode::CloseImplicitAnimation();
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_AddKeyFrame_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    float keyFrameFractionOne = 0.25f;
    float keyFrameFractionTwo = 0.75f;
    float keyFrameFractionThree = 1.0f;
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::OpenImplicitAnimation(protocol, timingCurve, [this]() {
        auto finishCallbackNode = FinishCallbackFunc();
        GetRootNode()->AddChild(finishCallbackNode);
        RegisterNode(finishCallbackNode);
        std::cout << "Animation_AddKeyFrame_Test_1 animation finish callback" << std::endl;
    });
    RSNode::AddKeyFrame(keyFrameFractionOne, RSAnimationTimingCurve::EASE_IN, [&]() {
        animationCustomModifier->SetPosition(500);
    });
    RSNode::AddKeyFrame(keyFrameFractionTwo, RSAnimationTimingCurve::LINEAR, [&]() {
        animationCustomModifier->SetPosition(700);
    });
    RSNode::AddKeyFrame(keyFrameFractionThree, RSAnimationTimingCurve::EASE_OUT, [&]() {
        animationCustomModifier->SetPosition(900);
    });
    RSNode::CloseImplicitAnimation();
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_AddDurationKeyFrame_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    float keyFrameDurationOne = 100;
    float keyFrameDurationTwo = 200;
    float keyFrameDurationThree = 300;
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::OpenImplicitAnimation(protocol, timingCurve, [this]() {
        auto finishCallbackNode = FinishCallbackFunc();
        GetRootNode()->AddChild(finishCallbackNode);
        RegisterNode(finishCallbackNode);
        std::cout << "Animation_AddDurationKeyFrame_Test_1 animation finish callback" << std::endl;
    });
    RSNode::AddDurationKeyFrame(keyFrameDurationOne, RSAnimationTimingCurve::EASE_IN, [&]() {
        animationCustomModifier->SetPosition(500);
    });
    RSNode::AddDurationKeyFrame(keyFrameDurationTwo, RSAnimationTimingCurve::LINEAR, [&]() {
        animationCustomModifier->SetPosition(700);
    });
    RSNode::AddDurationKeyFrame(keyFrameDurationThree, RSAnimationTimingCurve::EASE_OUT, [&]() {
        animationCustomModifier->SetPosition(900);
    });
    RSNode::CloseImplicitAnimation();
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_PahAnimation_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    std::string  ANIMATION_PATH = "M0 0 L300 200 L300 500 Z";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    testNode->SetMotionPathOption(motionPathOption);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_PahAnimation_Test_1 animation finish callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_PahAnimation_Test_2)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    std::string  ANIMATION_PATH = "Mstart.x start.y L300 200 L300 500 Lend.x endy";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    testNode->SetMotionPathOption(motionPathOption);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_PahAnimation_Test_2 animation finish callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_PahAnimation_Test_3)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    std::string  ANIMATION_PATH = "Mstart.x start.y L200 200 L200 500 L300 100 L300 500 Lend.x endy";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    testNode->SetMotionPathOption(motionPathOption);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_PahAnimation_Test_3 animation finish callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_PahAnimation_Test_4)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    std::string  ANIMATION_PATH = "Mstart.x start.y L200 200 L200 500 L300 100 L300 500 Lend.x endy";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    motionPathOption->SetBeginFraction(-1.0f); // beginFraction -1
    motionPathOption->SetEndFraction(-1.0f); // endFraction -1
    testNode->SetMotionPathOption(motionPathOption);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_PahAnimation_Test_4 animation finish callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_PahAnimation_Test_5)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    std::string  ANIMATION_PATH = "Mstart.x start.y L200 200 L200 500 L300 100 L300 500 Lend.x endy";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    motionPathOption->SetBeginFraction(0); // beginFraction 0
    motionPathOption->SetEndFraction(0); // endFraction 0
    testNode->SetMotionPathOption(motionPathOption);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_PahAnimation_Test_5 animation finish callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_PahAnimation_Test_6)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    std::string  ANIMATION_PATH = "Mstart.x start.y L200 200 L200 500 L300 100 L300 500 Lend.x endy";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    motionPathOption->SetBeginFraction(0.3f); // beginFraction 0.3
    motionPathOption->SetEndFraction(0.3f); // endFraction 0.3
    testNode->SetMotionPathOption(motionPathOption);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_PahAnimation_Test_6 animation finish callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_PahAnimation_Test_7)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    std::string  ANIMATION_PATH = "Mstart.x start.y L200 200 L200 500 L300 100 L300 500 Lend.x endy";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    motionPathOption->SetBeginFraction(0.8f); // beginFraction 0.8
    motionPathOption->SetEndFraction(0.8f); // endFraction 0.8
    testNode->SetMotionPathOption(motionPathOption);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_PahAnimation_Test_7 animation finish callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_PahAnimation_Test_8)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    std::string  ANIMATION_PATH = "Mstart.x start.y L200 200 L200 500 L300 100 L300 500 Lend.x endy";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    motionPathOption->SetBeginFraction(1.0f); // beginFraction 1
    motionPathOption->SetEndFraction(1.0f); // endFraction 1
    testNode->SetMotionPathOption(motionPathOption);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_PahAnimation_Test_8 animation finish callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_PahAnimation_Test_9)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    std::string  ANIMATION_PATH = "Mstart.x start.y L200 200 L200 500 L300 100 L300 500 Lend.x endy";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    motionPathOption->SetBeginFraction(2.0f); // beginFraction 2
    motionPathOption->SetEndFraction(2.0f); // endFraction 2
    testNode->SetMotionPathOption(motionPathOption);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_PahAnimation_Test_9 animation finish callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_PahAnimation_Test_10)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    std::string  ANIMATION_PATH = "Mstart.x start.y L200 200 L200 500 L300 100 L300 500 Lend.x endy";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    motionPathOption->SetRotationMode(RotationMode::ROTATE_AUTO);
    testNode->SetMotionPathOption(motionPathOption);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_PahAnimation_Test_10 animation finish callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_PahAnimation_Test_11)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    std::string  ANIMATION_PATH = "Mstart.x start.y L200 200 L200 500 L300 100 L300 500 Lend.x endy";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    motionPathOption->SetRotationMode(RotationMode::ROTATE_AUTO_REVERSE);
    testNode->SetMotionPathOption(motionPathOption);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_PahAnimation_Test_11 animation finish callback" << std::endl;
        });
}

GRAPHIC_TEST(AnimationTest, ANIMATION_TEST, Animation_PahAnimation_Test_12)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, 500, 500 });
    auto animationCustomModifier = std::make_shared<AnimationCustomModifier>();
    testNode->AddModifier(animationCustomModifier);
    animationCustomModifier->SetPosition(100);
    animationCustomModifier->SetTimeInterval(8.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    std::string  ANIMATION_PATH = "Mstart.x start.y L200 200 L200 500 L300 100 L300 500 Lend.x endy";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    motionPathOption->SetPathNeedAddOrigin(true);
    testNode->SetMotionPathOption(motionPathOption);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(
        protocol, timingCurve, [&]() { animationCustomModifier->SetPosition(500); },
        [this]() {
            auto finishCallbackNode = FinishCallbackFunc();
            GetRootNode()->AddChild(finishCallbackNode);
            RegisterNode(finishCallbackNode);
            std::cout << "Animation_PahAnimation_Test_12 animation finish callback" << std::endl;
        });
}

} // namespace OHOS::Rosen