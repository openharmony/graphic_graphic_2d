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

#include "point_custom_modifier_test.h"
#include "rs_graphic_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class AnimationProtocolTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

    void RegisterTestNode(std::shared_ptr<Modifier> modifier)
    {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
        testNode->AddModifier(modifier);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, ConstantProtocolTest01)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol = RSAnimationTimingProtocol::DEFAULT;
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_1 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, ConstantProtocolTest02)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol = RSAnimationTimingProtocol::IMMEDIATE;
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetDurationTest01)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(-100);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetDurationTest02)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(0);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetDurationTest03)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetStartDelayTest01)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetStartDelay(-400);
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetStartDelayTest02)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetStartDelay(-100);
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetStartDelayTest03)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetStartDelay(0);
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetStartDelayTest04)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetStartDelay(400);
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetSpeedTest01)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetSpeed(0.5);
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetSpeedTest02)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetSpeed(1);
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetSpeedTest03)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetSpeed(2);
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetRepeatCountTest01)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetRepeatCount(0);
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetRepeatCountTest02)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetRepeatCount(1);
    protocol.SetDuration(300);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetDirectionTest01)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDirection(false);
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetDirectionTest02)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDirection(true);
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetAutoReverseTest01)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetRepeatCount(2);
    protocol.SetAutoReverse(false);
    protocol.SetDuration(400);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetAutoReverseTest02)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetRepeatCount(2);
    protocol.SetAutoReverse(true);
    protocol.SetDuration(400);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetFinishCallbackTypeTest01)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetFinishCallbackType(FinishCallbackType::TIME_SENSITIVE);
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetFinishCallbackTypeTest02)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetFinishCallbackType(FinishCallbackType::TIME_INSENSITIVE);
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}

GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, SetFinishCallbackTypeTest03)
{
    auto pointMoidifier = std::make_shared<PointCustomModifier>();
    RegisterTestNode(pointMoidifier);
    pointMoidifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetFinishCallbackType(FinishCallbackType::LOGICALLY);
    protocol.SetDuration(800);
    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(protocol, timingCurve, [&]() {
        pointMoidifier->SetPosition(1000);
    }, []() {
        std::cout << "Animation_Protocol_Test_2 animation finish callback" << std::endl;
    });
}
} // namespace OHOS::Rosen