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

#include <cfloat>
#include <chrono>
#include <climits>
#include <cmath>
#include <iostream>

#include "rs_trace.h"

#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_interactive_implict_animator.h"
#include "animation/rs_keyframe_animation.h"
#include "animation/rs_path_animation.h"
#include "animation/rs_steps_interpolator.h"
#include "common/rs_vector2.h"
#include "modifier/rs_property.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;
constexpr uint64_t screenId = 0;

namespace {
/**
 * @brief Reset all node properties to default values
 * @param nodes Vector of canvas nodes to reset
 */
void ResetAllNodes(std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    for (auto& node : nodes) {
        node->SetTranslate(Vector2f(0.0f, 0.0f));
        node->SetScale(Vector2f(1.0f, 1.0f));
        node->SetRotation(0.0f);
        node->SetAlpha(1.0f);
    }
}

void InitNodes(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height,
    std::vector<std::shared_ptr<RSCanvasNode>>& nodes, std::shared_ptr<RSNode>& rootNode)
{
    std::cout << "Init nodes" << std::endl;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    rootNode = RSRootNode::Create(false, false, rsUIContext);
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);

    // Create three nodes with different colors (vertically aligned)
    nodes.emplace_back(RSCanvasNode::Create(false, false, rsUIContext));
    nodes[0]->SetBounds(100, 50, 200, 200);
    nodes[0]->SetFrame(100, 50, 200, 200);
    nodes[0]->SetBackgroundColor(SK_ColorBLUE);

    nodes.emplace_back(RSCanvasNode::Create(false, false, rsUIContext));
    nodes[1]->SetBounds(100, 350, 200, 200);
    nodes[1]->SetFrame(100, 350, 200, 200);
    nodes[1]->SetBackgroundColor(SK_ColorRED);

    nodes.emplace_back(RSCanvasNode::Create(false, false, rsUIContext));
    nodes[2]->SetBounds(100, 650, 200, 200);
    nodes[2]->SetFrame(100, 650, 200, 200);
    nodes[2]->SetBackgroundColor(SK_ColorGREEN);

    rootNode->AddChild(nodes[0], -1);
    rootNode->AddChild(nodes[1], -1);
    rootNode->AddChild(nodes[2], -1);
    rsUiDirector->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
}

void GroupAnimationTestOne(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case one: Test child animations with durations exceeding group animator duration"
              << std::endl;
    std::cout << "Group animator duration: 1000ms" << std::endl;
    std::cout << "Node 0 duration: 800ms (shorter than group)" << std::endl;
    std::cout << "Node 1 duration: 2000ms (longer than group)" << std::endl;
    std::cout << "Node 2 duration: 2500ms (longer than group)" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with 1000ms duration
    auto protocol = RSAnimationTimingProtocol(1000);
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack([]() { std::cout << "Group animation finished!" << std::endl; });

    // Add animation with 800ms duration for node 0 (shorter than group duration)
    // Use AddAnimation to allow independent timing
    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(800); // 800ms duration
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add animation with 2000ms duration for node 1 (longer than group duration)
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(2000); // 2000ms duration
        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Add animation with 2500ms duration for node 2 (longer than group duration)
    auto callback2 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol2(2500); // 2500ms duration
        RSNode::Animate(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback2);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout
        << "Group animation started: group=1000ms, node0=800ms, node1=2000ms, node2=2500ms, waiting for completion..."
        << std::endl;
    sleep(3);
}

void GroupAnimationTestTwo(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout
        << "Group Animation case two: Group animation with delay - group delay 500ms, child delays 300ms/500ms/800ms"
        << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with 500ms start delay
    auto protocol = RSAnimationTimingProtocol(1600);
    protocol.SetStartDelay(500); // Group delay 500ms
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack([]() { std::cout << "Group animation with delays finished!" << std::endl; });

    // Add animation with 300ms delay for node 0
    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(300); // 300ms duration
        protocol0.SetStartDelay(300);             // 300ms delay
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add animation with 500ms delay for node 1
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(1000); // 1000ms duration
        protocol1.SetStartDelay(500);              // 500ms delay
        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Add animation with 800ms delay for node 2
    auto callback2 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol2(800); // 800ms duration
        protocol2.SetStartDelay(800);             // 800ms delay
        RSNode::Animate(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback2);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Group animation with delays started: group delay 500ms, node0 delay 300ms, node1 delay 500ms, node2 "
                 "delay 800ms, waiting for completion..."
              << std::endl;
    sleep(4);
}

void GroupAnimationTestThree(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case three: Different child speeds with group speed 2" << std::endl;
    std::cout << "Group speed: 2.0" << std::endl;
    std::cout << "Node 0 speed: 0.5 (effective: 2.0 x 0.5 = 1.0)" << std::endl;
    std::cout << "Node 1 speed: 1.0 (effective: 2.0 x 1.0 = 2.0)" << std::endl;
    std::cout << "Node 2 speed: 1.5 (effective: 2.0 x 1.5 = 3.0)" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with speed 2.0
    auto protocol = RSAnimationTimingProtocol(4000);
    protocol.SetSpeed(2.0f); // Group speed = 2.0
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack(
        []() { std::cout << "Group animation with different speeds finished!" << std::endl; });

    // Add animation with speed 0.5 for node 0 (slowest)
    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(1000);
        protocol0.SetSpeed(0.5f); // Child speed = 0.5
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add animation with speed 1.0 for node 1 (medium)
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(1000);
        protocol1.SetSpeed(1.0f); // Child speed = 1.0
        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Add animation with speed 1.5 for node 2 (fastest)
    auto callback2 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol2(1000);
        protocol2.SetSpeed(1.5f); // Child speed = 1.5
        RSNode::Animate(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback2);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Group animation with different speeds started, node0: speed=0.5, node1: speed=1.0, node2: speed=1.5, "
                 "group speed=2.0, waiting for completion..."
              << std::endl;
    sleep(3);
}

void GroupAnimationTestFour(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case four: Child animations with durations exceeding group duration, repeatCount = 3"
              << std::endl;
    std::cout << "Group animator duration: 1000ms, repeatCount = 3" << std::endl;
    std::cout << "Node 0 duration: 800ms (shorter than group)" << std::endl;
    std::cout << "Node 1 duration: 2000ms (longer than group)" << std::endl;
    std::cout << "Node 2 duration: 2500ms (longer than group)" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with 1000ms duration and repeatCount = 3
    auto protocol = RSAnimationTimingProtocol(1000);
    protocol.SetRepeatCount(3); // Repeat 3 times
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack(
        []() { std::cout << "Group animation with repeatCount=3 finished!" << std::endl; });

    // Add animation with 800ms duration for node 0 (shorter than group duration)
    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(800);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add animation with 2000ms duration for node 1 (longer than group duration)
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(2000);
        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Add animation with 2500ms duration for node 2 (longer than group duration)
    auto callback2 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol2(2500);
        RSNode::Animate(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback2);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Group animation started: group=1000ms repeat=3, node0=800ms, node1=2000ms, node2=2500ms, waiting for "
                 "completion..."
              << std::endl;
    sleep(4);
}

void GroupAnimationTestFive(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case five: Child animations with durations exceeding group duration, repeatCount = "
                 "-1 (infinite)"
              << std::endl;
    std::cout << "Group animator duration: 1000ms, repeatCount = -1 (infinite loop)" << std::endl;
    std::cout << "Node 0 duration: 800ms (shorter than group)" << std::endl;
    std::cout << "Node 1 duration: 2000ms (longer than group)" << std::endl;
    std::cout << "Node 2 duration: 2500ms (longer than group)" << std::endl;
    std::cout << "Note: This test will run indefinitely until manually interrupted" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with 1000ms duration and repeatCount = -1 (infinite)
    auto protocol = RSAnimationTimingProtocol(1000);
    protocol.SetRepeatCount(-1); // Infinite loop
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback (will never be called for infinite loop)
    groupAnimator->SetFinishCallBack(
        []() { std::cout << "Group animation with repeatCount=-1 finished!" << std::endl; });

    // Add animation with 800ms duration for node 0 (shorter than group duration)
    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(800);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add animation with 2000ms duration for node 1 (longer than group duration)
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(2000);
        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Add animation with 2500ms duration for node 2 (longer than group duration)
    auto callback2 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol2(2500);
        RSNode::Animate(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback2);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Group animation started: group=1000ms repeat=-1 (infinite), node0=800ms, node1=2000ms, node2=2500ms"
              << std::endl;
    std::cout << "Running for 5 seconds to demonstrate infinite loop behavior..." << std::endl;
    sleep(5);
}

void GroupAnimationTestSix(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case six: Mix of delay and repeatCount" << std::endl;
    std::cout << "Group animator: duration=1600ms, startDelay=500ms, repeatCount=3" << std::endl;
    std::cout << "Node 0: duration=300ms, startDelay=300ms" << std::endl;
    std::cout << "Node 1: duration=1000ms, startDelay=500ms" << std::endl;
    std::cout << "Node 2: duration=800ms, startDelay=800ms" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with startDelay 500ms and repeatCount 3
    auto protocol = RSAnimationTimingProtocol(1600);
    protocol.SetStartDelay(500); // Group delay 500ms
    protocol.SetRepeatCount(3);  // Repeat 3 times
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Record group animation start time
    auto groupStartTime = std::chrono::steady_clock::now();

    // Set finish callback with duration measurement
    groupAnimator->SetFinishCallBack([groupStartTime]() {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
        std::cout << "Group animation with delay and repeatCount finished! Duration: " << duration << "ms" << std::endl;
    });

    // Add animation with 300ms duration and 300ms delay for node 0
    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(300);
        protocol0.SetStartDelay(300); // 300ms delay
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add animation with 1000ms duration and 500ms delay for node 1
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(1000);
        protocol1.SetStartDelay(500); // 500ms delay
        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Add animation with 800ms duration and 800ms delay for node 2
    auto callback2 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol2(800);
        protocol2.SetStartDelay(800); // 800ms delay
        RSNode::Animate(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback2);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Group animation started: group delay=500ms, repeat=3, node0 delay=300ms, node1 delay=500ms, node2 "
                 "delay=800ms, waiting for completion..."
              << std::endl;
    sleep(7);
}

void GroupAnimationTestSeven(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case seven: All child animations shorter than group duration, repeatCount = 3"
              << std::endl;
    std::cout << "Group animator duration: 1000ms, repeatCount = 3" << std::endl;
    std::cout << "Node 0 duration: 500ms (shorter than group)" << std::endl;
    std::cout << "Node 1 duration: 600ms (shorter than group)" << std::endl;
    std::cout << "Node 2 duration: 700ms (shorter than group)" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with 1000ms duration and repeatCount = 3
    auto protocol = RSAnimationTimingProtocol(1000);
    protocol.SetRepeatCount(3); // Repeat 3 times
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack(
        []() { std::cout << "Group animation with repeatCount=3 finished!" << std::endl; });

    // Add animation with 500ms duration for node 0 (shorter than group duration)
    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(500);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add animation with 600ms duration for node 1 (shorter than group duration)
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(600);
        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Add animation with 700ms duration for node 2 (shorter than group duration)
    auto callback2 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol2(700);
        RSNode::Animate(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback2);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Group animation started: group=1000ms repeat=3, node0=500ms, node1=600ms, node2=700ms, waiting for "
                 "completion..."
              << std::endl;
    sleep(4);
}

void GroupAnimationTestEight(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case eight: All child animations longer than group duration, repeatCount = 3"
              << std::endl;
    std::cout << "Group animator duration: 1000ms, repeatCount = 3" << std::endl;
    std::cout << "Node 0 duration: 1200ms (longer than group)" << std::endl;
    std::cout << "Node 1 duration: 1500ms (longer than group)" << std::endl;
    std::cout << "Node 2 duration: 1800ms (longer than group)" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with 1000ms duration and repeatCount = 3
    auto protocol = RSAnimationTimingProtocol(1000);
    protocol.SetRepeatCount(3); // Repeat 3 times
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack(
        []() { std::cout << "Group animation with repeatCount=3 finished!" << std::endl; });

    // Add animation with 1200ms duration for node 0 (longer than group duration)
    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(1200);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add animation with 1500ms duration for node 1 (longer than group duration)
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(1500);
        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Add animation with 1800ms duration for node 2 (longer than group duration)
    auto callback2 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol2(1800);
        RSNode::Animate(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback2);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Group animation started: group=1000ms repeat=3, node0=1200ms, node1=1500ms, node2=1800ms, waiting "
                 "for completion..."
              << std::endl;
    sleep(4);
}

void GroupAnimationTestNine(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case nine: Child animations with durations exceeding group duration, repeatCount = "
                 "3, autoReverse = true"
              << std::endl;
    std::cout << "Group animator duration: 1000ms, repeatCount = 3, autoReverse = true" << std::endl;
    std::cout << "Node 0 duration: 800ms (shorter than group)" << std::endl;
    std::cout << "Node 1 duration: 2000ms (longer than group)" << std::endl;
    std::cout << "Node 2 duration: 2500ms (longer than group)" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with 1000ms duration, repeatCount = 3, and autoReverse = true
    auto protocol = RSAnimationTimingProtocol(1000);
    protocol.SetRepeatCount(3);    // Repeat 3 times
    protocol.SetAutoReverse(true); // Enable auto reverse
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack(
        []() { std::cout << "Group animation with repeatCount=3, autoReverse=true finished!" << std::endl; });

    // Add animation with 800ms duration for node 0 (shorter than group duration)
    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(800);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add animation with 2000ms duration for node 1 (longer than group duration)
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(2000);
        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Add animation with 2500ms duration for node 2 (longer than group duration)
    auto callback2 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol2(2500);
        RSNode::Animate(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback2);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Group animation started: group=1000ms repeat=3 autoReverse=true, node0=800ms, node1=2000ms, "
                 "node2=2500ms, waiting for completion..."
              << std::endl;
    sleep(4);
}

void GroupAnimationTestTen(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case ten: Two group animations with different delays" << std::endl;
    std::cout << "Group 1: delay 300ms, Group 2: delay 800ms" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Create first group animator with 300ms start delay
    auto protocol1 = RSAnimationTimingProtocol(1000);
    protocol1.SetStartDelay(300); // Group 1 delay 300ms
    auto groupAnimatorWeak1 =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol1, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator1 = groupAnimatorWeak1.lock();

    if (!groupAnimator1) {
        std::cout << "Failed to create group animator 1" << std::endl;
        return;
    }

    // Set finish callback for group 1
    groupAnimator1->SetFinishCallBack([]() { std::cout << "Group animation 1 (300ms delay) finished!" << std::endl; });

    // Add animation for node 0 to group 1
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(1000); // 1000ms duration
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator1->AddAnimation(callback1);

    // Create second group animator with 800ms start delay
    auto protocol2 = RSAnimationTimingProtocol(1000);
    protocol2.SetStartDelay(800); // Group 2 delay 800ms
    auto groupAnimatorWeak2 =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator2 = groupAnimatorWeak2.lock();

    if (!groupAnimator2) {
        std::cout << "Failed to create group animator 2" << std::endl;
        return;
    }

    // Set finish callback for group 2
    groupAnimator2->SetFinishCallBack([]() { std::cout << "Group animation 2 (800ms delay) finished!" << std::endl; });

    // Add animation for node 1 to group 2
    auto callback2 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(1000); // 1000ms duration
        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator2->AddAnimation(callback2);

    // Start both group animations
    groupAnimator1->StartAnimation();
    groupAnimator2->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Two group animations started: Group 1 delay 300ms, Group 2 delay 800ms, waiting for completion..."
              << std::endl;
    sleep(3);
}

void GroupAnimationTestEleven(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case eleven: Test Pause/Continue commands on group animation" << std::endl;
    std::cout << "Group animator duration: 2000ms" << std::endl;
    std::cout << "Node 0 duration: 2000ms" << std::endl;
    std::cout << "Test sequence: Start -> 1000ms -> Pause -> 1000ms -> Continue -> Wait for completion" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    auto protocol = RSAnimationTimingProtocol(2000);
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    groupAnimator->SetFinishCallBack(
        []() { std::cout << "Group animation with Pause/Continue finished!" << std::endl; });

    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(2000);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    std::cout << "Group animation started, running for 500ms..." << std::endl;
    sleep(1);

    std::cout << "Pausing animation..." << std::endl;
    groupAnimator->PauseAnimation();
    transaction->FlushImplicitTransaction();
    sleep(1);

    std::cout << "Continuing animation..." << std::endl;
    groupAnimator->ContinueAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Animation resumed, waiting for completion..." << std::endl;
    sleep(2);
}

void GroupAnimationTestTwelve(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case twelve: Test Reverse command on group animation" << std::endl;
    std::cout << "Group animator duration: 2000ms" << std::endl;
    std::cout << "Node 0 duration: 2000ms" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    auto protocol = RSAnimationTimingProtocol(2000);
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    groupAnimator->SetFinishCallBack([]() { std::cout << "Group animation with Reverse finished!" << std::endl; });

    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(2000);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    std::cout << "Group animation started, running for 1000ms..." << std::endl;
    sleep(1);

    std::cout << "Reversing animation..." << std::endl;
    groupAnimator->PauseAnimation();
    groupAnimator->ReverseAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Animation reversed, waiting for completion..." << std::endl;
    sleep(2);
}

void GroupAnimationTestThirteen(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case thirteen: Test Finish command on group animation: Finish at END" << std::endl;
    std::cout << "Group animator duration: 2000ms" << std::endl;
    std::cout << "Node 0 duration: 2000ms" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    auto protocol = RSAnimationTimingProtocol(2000);
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    groupAnimator->SetFinishCallBack([]() { std::cout << "Group animation with Finish finished!" << std::endl; });

    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(2000);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    std::cout << "Group animation started, running for 500ms..." << std::endl;
    sleep(1);

    std::cout << "Finishing animation at END position..." << std::endl;
    groupAnimator->FinishAnimation(RSInteractiveAnimationPosition::END);
    transaction->FlushImplicitTransaction();

    std::cout << "Animation finished, waiting for callback..." << std::endl;
    sleep(1);
}

void GroupAnimationTestFourteen(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case Fourteen: Child animations with durations exceeding group duration, repeatCount "
                 "= 3 and pause&resume"
              << std::endl;
    std::cout << "Group animator duration: 1000ms, repeatCount = 3" << std::endl;
    std::cout << "Group animator pause at 900ms sleep 1000ms and resume" << std::endl;
    std::cout << "Node 0 duration: 800ms (shorter than group)" << std::endl;
    std::cout << "Node 1 duration: 2000ms (longer than group)" << std::endl;
    std::cout << "Node 2 duration: 2500ms (longer than group)" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with 1000ms duration and repeatCount = 3
    auto protocol = RSAnimationTimingProtocol(1000);
    protocol.SetRepeatCount(3); // Repeat 3 times
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack(
        []() { std::cout << "Group animation with repeatCount=3 finished!" << std::endl; });

    // Add animation with 800ms duration for node 0 (shorter than group duration)
    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(800);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add animation with 2000ms duration for node 1 (longer than group duration)
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(2000);
        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Add animation with 2500ms duration for node 2 (longer than group duration)
    auto callback2 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol2(2500);
        RSNode::Animate(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback2);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    // sleep 900ms
    usleep(900000);

    std::cout << "Pausing animation..." << std::endl;
    groupAnimator->PauseAnimation();
    transaction->FlushImplicitTransaction();
    sleep(1);

    std::cout << "Continuing animation..." << std::endl;
    groupAnimator->ContinueAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Group animation started: group=1000ms repeat=3, node0=800ms, node1=2000ms, node2=2500ms, waiting for "
                 "completion..."
              << std::endl;
    sleep(4);
}

void GroupAnimationTestFifteen(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout
        << "Group Animation case Fifteen: Child animations with durations exceeding group duration, repeatCount = "
           "3&autoReverse=true and pause&resume"
        << std::endl;
    std::cout << "Group animator duration: 1000ms, repeatCount = 3&autoReverse=true" << std::endl;
    std::cout << "Group animator pause at 900ms sleep 1000ms and resume" << std::endl;
    std::cout << "Node 0 duration: 800ms (shorter than group)" << std::endl;
    std::cout << "Node 1 duration: 2000ms (longer than group)" << std::endl;
    std::cout << "Node 2 duration: 2500ms (longer than group)" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with 1000ms duration and repeatCount = 3&autoReverse=true
    auto protocol = RSAnimationTimingProtocol(1000);
    protocol.SetRepeatCount(3);    // Repeat 3 times
    protocol.SetAutoReverse(true); // Enable auto reverse
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack(
        []() { std::cout << "Group animation with repeatCount=3 finished!" << std::endl; });

    // Add animation with 800ms duration for node 0 (shorter than group duration)
    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(800);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add animation with 2000ms duration for node 1 (longer than group duration)
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(2000);
        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Add animation with 2500ms duration for node 2 (longer than group duration)
    auto callback2 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol2(2500);
        RSNode::Animate(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback2);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    // sleep 900ms
    usleep(900000);

    std::cout << "Pausing animation..." << std::endl;
    groupAnimator->PauseAnimation();
    transaction->FlushImplicitTransaction();
    sleep(1);

    std::cout << "Continuing animation..." << std::endl;
    groupAnimator->ContinueAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Group animation started: group=1000ms repeat=3, node0=800ms, node1=2000ms, node2=2500ms, waiting for "
                 "completion..."
              << std::endl;
    sleep(4);
}

void GroupAnimationTestSixteen(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Comparison between two groups of animations: one with delay and one without delay" << std::endl;
    std::cout << "Group 1: delay 500ms, Group 2: delay 0ms" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Create first group animator with 300ms start delay
    auto protocol1 = RSAnimationTimingProtocol(1000);
    protocol1.SetStartDelay(500); // Group 1 delay 500ms
    auto groupAnimatorWeak1 =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol1, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator1 = groupAnimatorWeak1.lock();

    if (!groupAnimator1) {
        std::cout << "Failed to create group animator 1" << std::endl;
        return;
    }

    // Set finish callback for group 1
    groupAnimator1->SetFinishCallBack([]() { std::cout << "Group animation 1 (500ms delay) finished!" << std::endl; });

    // Add animation for node 0 to group 1
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(1000); // 1000ms duration
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator1->AddAnimation(callback1);

    // Create second group animator with 800ms start delay
    auto protocol2 = RSAnimationTimingProtocol(1000);
    auto groupAnimatorWeak2 =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator2 = groupAnimatorWeak2.lock();

    if (!groupAnimator2) {
        std::cout << "Failed to create group animator 2" << std::endl;
        return;
    }

    // Set finish callback for group 2
    groupAnimator2->SetFinishCallBack([]() { std::cout << "Group animation 2 (0ms delay) finished!" << std::endl; });

    // Add animation for node 1 to group 2
    auto callback2 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(1000); // 1000ms duration
        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator2->AddAnimation(callback2);

    // Start both group animations
    groupAnimator1->StartAnimation();
    groupAnimator2->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Comparison between two groups of animations: one with delay and one without delay, waiting for "
                 "completion..."
              << std::endl;
    sleep(3);
}

void GroupAnimationTestSeventeen(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Test 17: Pause group animation during startDelay (500ms delay, pause at 200ms, continue after 1s)"
              << std::endl;

    // Reset all properties
    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Record group animation start time
    auto groupStartTime = std::chrono::steady_clock::now();
    auto groupStartTimeMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(groupStartTime.time_since_epoch()).count();
    std::cout << "Group animation start time: " << groupStartTimeMs << "ms" << std::endl;

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Create group animator with 500ms start delay and 1000ms duration
    auto protocol = RSAnimationTimingProtocol(1000);
    protocol.SetStartDelay(500); // 500ms start delay
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack([groupStartTime]() {
        auto endTime = std::chrono::steady_clock::now();
        auto endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
        std::cout << "Group animation finished! End time: " << endTimeMs << "ms, Duration: " << duration << "ms"
                  << std::endl;
    });

    // Add animation for node 0
    auto callback = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(1000); // 1000ms duration
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback);

    // Start group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    std::cout << "Group animation 17 started with 500ms delay" << std::endl;

    // Wait 200ms (still in startDelay period)
    usleep(200000);
    std::cout << "At 200ms: Pausing group animation (still in startDelay)" << std::endl;
    groupAnimator->PauseAnimation();
    transaction->FlushImplicitTransaction();

    // Wait 1 second, then continue
    sleep(1);
    std::cout << "At 1200ms: Continuing group animation" << std::endl;
    groupAnimator->ContinueAnimation();
    transaction->FlushImplicitTransaction();

    // Wait for animation to complete
    std::cout << "Test 17: Waiting for animation to complete..." << std::endl;
    sleep(3);
    std::cout << "Test 17: Completed" << std::endl;
}

void GroupAnimationTestEighteen(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case eighteen: Test Finish command on group animation: Finish at START" << std::endl;
    std::cout << "Group animator duration: 2000ms" << std::endl;
    std::cout << "Node 0 duration: 2000ms" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    auto protocol = RSAnimationTimingProtocol(2000);
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    groupAnimator->SetFinishCallBack([]() { std::cout << "Group animation with Finish finished!" << std::endl; });

    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(2000);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    std::cout << "Group animation started, running for 500ms..." << std::endl;
    sleep(1);

    std::cout << "Finishing animation at START position..." << std::endl;
    groupAnimator->FinishAnimation(RSInteractiveAnimationPosition::START);
    transaction->FlushImplicitTransaction();

    std::cout << "Animation finished, waiting for callback..." << std::endl;
    sleep(1);
}

void GroupAnimationTestNinteen(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case Ninteen: Test Finish command on group animation: Finish at CURRENT" << std::endl;
    std::cout << "Group animator duration: 2000ms" << std::endl;
    std::cout << "Node 0 duration: 2000ms" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    auto protocol = RSAnimationTimingProtocol(2000);
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    groupAnimator->SetFinishCallBack([]() { std::cout << "Group animation with Finish finished!" << std::endl; });

    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(2000);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    std::cout << "Group animation started, running for 500ms..." << std::endl;
    sleep(1);

    std::cout << "Finishing animation at CURRENT position..." << std::endl;
    groupAnimator->FinishAnimation(RSInteractiveAnimationPosition::CURRENT);
    transaction->FlushImplicitTransaction();

    std::cout << "Animation finished, waiting for callback..." << std::endl;
    sleep(1);
}

void GroupAnimationTestTwenty(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout
        << "Group Animation case Twenty: Test Finish command on group animation: Finish at END when group at delay time"
        << std::endl;
    std::cout << "Group animator duration: 2000ms delay 500ms" << std::endl;
    std::cout << "Node 0 duration: 2000ms" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    auto protocol = RSAnimationTimingProtocol(2000);
    protocol.SetStartDelay(500); // Group delay 500ms
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    groupAnimator->SetFinishCallBack([]() { std::cout << "Group animation with Finish finished!" << std::endl; });

    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(2000);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    std::cout << "Group animation delay for 300ms..." << std::endl;
    // sleep 300ms
    usleep(300000);

    std::cout << "Finishing animation at END position..." << std::endl;
    groupAnimator->FinishAnimation(RSInteractiveAnimationPosition::END);
    transaction->FlushImplicitTransaction();

    std::cout << "Animation finished, waiting for callback..." << std::endl;
    sleep(1);
}

void GroupAnimationTestTwentyOne(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case TwentyOne: Test Finish command on group animation: Finish at END when child "
                 "animation at 'groupWating' state"
              << std::endl;
    std::cout << "Group animator duration: 2000ms" << std::endl;
    std::cout << "Node 0 duration: 800ms" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    auto protocol = RSAnimationTimingProtocol(2000);
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    groupAnimator->SetFinishCallBack([]() { std::cout << "Group animation with Finish finished!" << std::endl; });

    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(800);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    std::cout << "Group animation started, running for 500ms..." << std::endl;
    sleep(1);

    std::cout << "Finishing animation at END position..." << std::endl;
    groupAnimator->FinishAnimation(RSInteractiveAnimationPosition::END);
    transaction->FlushImplicitTransaction();

    std::cout << "Animation finished, waiting for callback..." << std::endl;
    sleep(1);
}

void GroupAnimationTestTwentyTwo(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case TwentyTwo: Test Finish command on group animation: Finish at END when child "
                 "animation at 'pause' state"
              << std::endl;
    std::cout << "Group animator duration: 2000ms" << std::endl;
    std::cout << "Node 0 duration: 800ms" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    auto protocol = RSAnimationTimingProtocol(2000);
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    groupAnimator->SetFinishCallBack([]() { std::cout << "Group animation with Finish finished!" << std::endl; });

    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(800);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    std::cout << "Group animation started, running for 500ms..." << std::endl;
    // sleep 300ms
    usleep(300000);
    std::cout << "Pausing animation..." << std::endl;
    groupAnimator->PauseAnimation();
    transaction->FlushImplicitTransaction();

    sleep(1);
    std::cout << "Finishing animation at END position..." << std::endl;
    groupAnimator->FinishAnimation(RSInteractiveAnimationPosition::END);
    transaction->FlushImplicitTransaction();

    std::cout << "Animation finished, waiting for callback..." << std::endl;
    sleep(1);
}

void GroupAnimationTestTwentyThree(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case TwentyThree: Test Group animation finishCallback" << std::endl;
    std::cout << "Group animator duration: 2000ms" << std::endl;
    std::cout << "Node 0 duration: 800ms (shorter than group)" << std::endl;
    std::cout << "Node 1 duration: 2000ms (longer than group)" << std::endl;
    std::cout << "Node 2 duration: 2500ms (longer than group)" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Record group animation start time
    auto groupStartTime = std::chrono::steady_clock::now();
    auto groupStartTimeMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(groupStartTime.time_since_epoch()).count();
    std::cout << "Group animation start time: " << groupStartTimeMs << "ms" << std::endl;

    // Create a group animator with 1000ms duration
    auto protocol = RSAnimationTimingProtocol(2000);
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack([groupStartTime]() {
        auto endTime = std::chrono::steady_clock::now();
        auto endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
        std::cout << "Group animation finished! End time: " << endTimeMs << "ms, Duration: " << duration << "ms"
                  << std::endl;
    });

    // Add animation with 800ms duration for node 0 (shorter than group duration)
    // Use AddAnimation to allow independent timing
    auto callback0 = [nodes, rsUIContext, groupStartTime]() {
        RSAnimationTimingProtocol protocol0(800); // 800ms duration
        RSNode::Animate(
            rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); },
            [groupStartTime]() {
                auto endTime = std::chrono::steady_clock::now();
                auto endTimeMs =
                    std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
                std::cout << "Node 0 animation finished! End time: " << endTimeMs << "ms, Duration: " << duration
                          << "ms" << std::endl;
            });
    };
    groupAnimator->AddAnimation(callback0);

    // Add animation with 2000ms duration for node 1 (longer than group duration)
    auto callback1 = [nodes, rsUIContext, groupStartTime]() {
        RSAnimationTimingProtocol protocol1(2000); // 2000ms duration
        RSNode::Animate(
            rsUIContext, protocol1, RSAnimationTimingCurve::LINEAR,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); },
            [groupStartTime]() {
                auto endTime = std::chrono::steady_clock::now();
                auto endTimeMs =
                    std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
                std::cout << "Node 1 animation finished! End time: " << endTimeMs << "ms, Duration: " << duration
                          << "ms" << std::endl;
            });
    };
    groupAnimator->AddAnimation(callback1);

    // Add animation with 2500ms duration for node 2 (longer than group duration)
    auto callback2 = [nodes, rsUIContext, groupStartTime]() {
        RSAnimationTimingProtocol protocol2(2500); // 2500ms duration
        RSNode::Animate(
            rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); },
            [groupStartTime]() {
                auto endTime = std::chrono::steady_clock::now();
                auto endTimeMs =
                    std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
                std::cout << "Node 2 animation finished! End time: " << endTimeMs << "ms, Duration: " << duration
                          << "ms" << std::endl;
            });
    };
    groupAnimator->AddAnimation(callback2);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout
        << "Group animation started: group=2000ms, node0=800ms, node1=2000ms, node2=2500ms, waiting for completion..."
        << std::endl;
    sleep(3);
}

void GroupAnimationTestTwentyFour(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case TwentyFour: Test different animation types - Spring, Keyframe, and Steps"
              << std::endl;
    std::cout << "Group animator duration: 2000ms" << std::endl;
    std::cout << "Node 0: Spring animation (physical spring)" << std::endl;
    std::cout << "Node 1: Keyframe animation with multiple keyframes" << std::endl;
    std::cout << "Node 2: Steps animation with 5 steps" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Record group animation start time
    auto groupStartTime = std::chrono::steady_clock::now();
    auto groupStartTimeMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(groupStartTime.time_since_epoch()).count();
    std::cout << "Group animation start time: " << groupStartTimeMs << "ms" << std::endl;

    // Create a group animator with 2000ms duration
    auto protocol = RSAnimationTimingProtocol(2000);
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack([groupStartTime]() {
        auto endTime = std::chrono::steady_clock::now();
        auto endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
        std::cout << "Group animation finished! End time: " << endTimeMs << "ms, Duration: " << duration << "ms"
                  << std::endl;
    });

    // Add Spring animation for node 0 (physical spring)
    // Spring animation uses spring model to determine duration
    auto callback0 = [nodes, rsUIContext, groupStartTime]() {
        RSAnimationTimingProtocol protocol0(2000);
        // Create spring curve with response and dampingRatio
        auto springCurve = RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f);
        RSNode::Animate(
            rsUIContext, protocol0, springCurve, [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); },
            [groupStartTime]() {
                auto endTime = std::chrono::steady_clock::now();
                auto endTimeMs =
                    std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
                std::cout << "Node 0 (Spring) animation finished! End time: " << endTimeMs
                          << "ms, Duration: " << duration << "ms" << std::endl;
            });
    };
    groupAnimator->AddAnimation(callback0);

    // Add Keyframe animation for node 1
    // Keyframe animation allows multiple control points
    auto callback1 = [nodes, rsUIContext, groupStartTime]() {
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(2000);
        RSNode::OpenImplicitAnimation(protocol, RSAnimationTimingCurve::DEFAULT, [groupStartTime]() {
            auto endTime = std::chrono::steady_clock::now();
            auto endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
            std::cout << "Node 1 (Keyframe) animation finished! End time: " << endTimeMs << "ms, Duration: " << duration
                      << "ms" << std::endl;
        });
        RSNode::AddKeyFrame(
            0.0f, RSAnimationTimingCurve::EASE_IN, [&]() { nodes[1]->SetTranslate(Vector2f(0.0f, 0.0f)); });
        RSNode::AddKeyFrame(
            0.3f, RSAnimationTimingCurve::EASE_IN, [&]() { nodes[1]->SetTranslate(Vector2f(200.0f, 0.0f)); });
        RSNode::AddKeyFrame(
            0.7f, RSAnimationTimingCurve::EASE_OUT, [&]() { nodes[1]->SetTranslate(Vector2f(400.0f, 0.0f)); });
        RSNode::AddKeyFrame(
            1.0f, RSAnimationTimingCurve::EASE_IN_OUT, [&]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
        RSNode::CloseImplicitAnimation();
    };
    groupAnimator->AddAnimation(callback1);

    // Add Steps animation for node 2
    // Steps animation divides animation into discrete steps
    auto callback2 = [nodes, rsUIContext, groupStartTime]() {
        RSAnimationTimingProtocol protocol2(2000);
        // Create steps curve with 5 steps, position at START
        auto stepsCurve = RSAnimationTimingCurve::CreateStepsCurve(5, StepsCurvePosition::START);
        RSNode::Animate(
            rsUIContext, protocol2, stepsCurve, [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); },
            [groupStartTime]() {
                auto endTime = std::chrono::steady_clock::now();
                auto endTimeMs =
                    std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
                std::cout << "Node 2 (Steps) animation finished! End time: " << endTimeMs
                          << "ms, Duration: " << duration << "ms" << std::endl;
            });
    };
    groupAnimator->AddAnimation(callback2);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Group animation started: Spring, Keyframe, Steps animations, waiting for completion..." << std::endl;
    sleep(3);
}

// ============================================================================
// Boundary Value and Exception Test Cases for RSAnimationTimingProtocol
// ============================================================================

/**
 * @tc.name: GroupAnimationTestTwentyFive
 * @tc.desc: Test duration boundary values: negative and zero (exception values only)
 * @tc.type: FUNC
 */
void GroupAnimationTestTwentyFive(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "========================================" << std::endl;
    std::cout << "Test 25: Duration Boundary Values Test" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Testing duration with: -100 (negative), 0 (zero)" << std::endl;

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Reset nodes
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Test 1: Negative duration (-100)
    RSAnimationTimingProtocol protocol1;
    protocol1.SetDuration(-100); // Negative duration
    auto groupAnimatorWeak1 =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol1, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator1 = groupAnimatorWeak1.lock();

    if (!groupAnimator1) {
        std::cout << "  [Duration=-100] Failed to create group animator (expected)" << std::endl;
    } else {
        std::cout << "  [Duration=-100] Unexpectedly created group animator" << std::endl;
    }

    // Test 2: Zero duration
    RSAnimationTimingProtocol protocol2;
    protocol2.SetDuration(0); // Zero duration
    auto groupAnimatorWeak2 =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator2 = groupAnimatorWeak2.lock();

    if (!groupAnimator2) {
        std::cout << "  [Duration=0] Failed to create group animator (expected)" << std::endl;
    } else {
        std::cout << "  [Duration=0] Unexpectedly created group animator" << std::endl;
    }

    std::cout << "Duration boundary test completed" << std::endl;
}

/**
 * @tc.name: GroupAnimationTestTwentySix
 * @tc.desc: Test startDelay boundary values: negative (exception values only)
 * @tc.type: FUNC
 */
void GroupAnimationTestTwentySix(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "========================================" << std::endl;
    std::cout << "Test 26: StartDelay Boundary Values Test" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Testing startDelay with: -1000 (negative)" << std::endl;

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Reset nodes
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Record group animation start time
    auto groupStartTime = std::chrono::steady_clock::now();
    auto groupStartTimeMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(groupStartTime.time_since_epoch()).count();
    std::cout << "Group animation start time: " << groupStartTimeMs << "ms" << std::endl;

    // Test: Negative startDelay (-1000)
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(2000);
    protocol.SetStartDelay(-1000);
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack([groupStartTime]() {
        auto endTime = std::chrono::steady_clock::now();
        auto endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
        std::cout << "Group animation finished! End time: " << endTimeMs << "ms, Duration: " << duration << "ms"
                  << std::endl;
    });

    std::cout << "Group animator created: startDelay corrected from -1000" << std::endl;

    // Add animation with 2000ms duration for node 0
    // Use AddAnimation to allow independent timing
    auto callback0 = [nodes, rsUIContext, groupStartTime]() {
        RSAnimationTimingProtocol protocol0(2000); // 2000ms duration
        RSNode::Animate(
            rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); },
            [groupStartTime]() {
                auto endTime = std::chrono::steady_clock::now();
                auto endTimeMs =
                    std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
                std::cout << "Node 0 animation finished! End time: " << endTimeMs << "ms, Duration: " << duration
                          << "ms" << std::endl;
            });
    };
    groupAnimator->AddAnimation(callback0);
    groupAnimator->StartAnimation();

    transaction->FlushImplicitTransaction();
    std::cout << "All startDelay boundary test animations started, waiting 3 seconds..." << std::endl;
    sleep(3);
}

/**
 * @tc.name: GroupAnimationTestTwentySeven
 * @tc.desc: Test speed boundary values: zero and negative (exception values only)
 * @tc.type: FUNC
 */
void GroupAnimationTestTwentySeven(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "========================================" << std::endl;
    std::cout << "Test 27: Speed Boundary Values Test" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Testing speed with: 0.0 (zero), -1.0 (negative)" << std::endl;

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Reset nodes
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Test 1: Zero speed (will be fixed to 1.0f)
    RSAnimationTimingProtocol protocol1;
    protocol1.SetDuration(2000);
    protocol1.SetSpeed(0.0f); // Zero speed - will be fixed to 1.0f
    auto groupAnimatorWeak1 =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol1, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator1 = groupAnimatorWeak1.lock();

    if (!groupAnimator1) {
        std::cout << "Failed to create group animator 1" << std::endl;
        return;
    }

    std::cout << "Group animator 1 created successfully with speed fixed from 0.0 to 1.0f" << std::endl;

    groupAnimator1->AddImplictAnimation([nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });

    // Test 2: Negative speed (will be fixed to 1.0f)
    RSAnimationTimingProtocol protocol2;
    protocol2.SetDuration(2000);
    protocol2.SetSpeed(-1.0f); // Negative speed - will be fixed to 1.0f
    auto groupAnimatorWeak2 =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator2 = groupAnimatorWeak2.lock();

    if (!groupAnimator2) {
        std::cout << "Failed to create group animator 2" << std::endl;
        return;
    }

    std::cout << "Group animator 2 created successfully with speed fixed from -1.0 to 1.0f" << std::endl;

    groupAnimator2->AddImplictAnimation([nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });

    groupAnimator1->StartAnimation();
    groupAnimator2->StartAnimation();

    transaction->FlushImplicitTransaction();
    std::cout << "All speed boundary test animations started, waiting 3 seconds..." << std::endl;
    sleep(3);
}

/**
 * @tc.name: GroupAnimationTestTwentyEight
 * @tc.desc: Test repeatCount boundary values: other negatives (except -1), zero
 * @tc.type: FUNC
 */
void GroupAnimationTestTwentyEight(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "========================================" << std::endl;
    std::cout << "Test 28: RepeatCount Boundary Values Test" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Testing repeatCount with: -5 (other negative), 0 (zero)" << std::endl;

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Reset nodes
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Test 1: repeatCount = -5 (other negative value, will be fixed to 1)
    RSAnimationTimingProtocol protocol1;
    protocol1.SetDuration(500);
    protocol1.SetRepeatCount(-5); // Negative but not -1, will be fixed to 1
    auto groupAnimatorWeak1 =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol1, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator1 = groupAnimatorWeak1.lock();

    if (!groupAnimator1) {
        std::cout << "Failed to create group animator 1" << std::endl;
        return;
    }

    std::cout << "Group animator 1 created successfully with repeatCount fixed from -5 to 1" << std::endl;

    groupAnimator1->AddImplictAnimation([nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });

    // Test 2: repeatCount = 0 (will be fixed to 1)
    RSAnimationTimingProtocol protocol2;
    protocol2.SetDuration(500);
    protocol2.SetRepeatCount(0); // No repeat, will be fixed to 1
    auto groupAnimatorWeak2 =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator2 = groupAnimatorWeak2.lock();

    if (!groupAnimator2) {
        std::cout << "Failed to create group animator 2" << std::endl;
        return;
    }

    std::cout << "Group animator 2 created successfully with repeatCount fixed from 0 to 1" << std::endl;

    groupAnimator2->AddImplictAnimation([nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });

    groupAnimator1->StartAnimation();
    groupAnimator2->StartAnimation();

    transaction->FlushImplicitTransaction();
    std::cout << "All repeatCount boundary test animations started, waiting 3 seconds..." << std::endl;
    sleep(3);
}

/**
 * @tc.name: GroupAnimationTestTwentyNine
 * @tc.desc: Test special float values for speed: NaN, Infinity
 * @tc.type: FUNC
 */
void GroupAnimationTestTwentyNine(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "========================================" << std::endl;
    std::cout << "Test 29: Special Float Values Test" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Testing speed with: NaN, +Infinity, -Infinity" << std::endl;

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Reset nodes
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Test 1: speed = NaN (will be fixed to 1.0f)
    RSAnimationTimingProtocol protocol1;
    protocol1.SetDuration(2000);
    protocol1.SetSpeed(NAN); // Not a Number, will be fixed to 1.0f
    auto groupAnimatorWeak1 =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol1, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator1 = groupAnimatorWeak1.lock();

    if (!groupAnimator1) {
        std::cout << "Failed to create group animator 1" << std::endl;
        return;
    }

    std::cout << "Group animator 1 created successfully with speed fixed from NaN to 1.0f" << std::endl;

    groupAnimator1->AddImplictAnimation([nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });

    // Test 2: speed = +Infinity (will be fixed to 1.0f)
    RSAnimationTimingProtocol protocol2;
    protocol2.SetDuration(2000);
    protocol2.SetSpeed(INFINITY); // Positive infinity, will be fixed to 1.0f
    auto groupAnimatorWeak2 =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol2, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator2 = groupAnimatorWeak2.lock();

    if (!groupAnimator2) {
        std::cout << "Failed to create group animator 2" << std::endl;
        return;
    }

    std::cout << "Group animator 2 created successfully with speed fixed from +Infinity to 1.0f" << std::endl;

    groupAnimator2->AddImplictAnimation([nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });

    // Test 3: speed = -Infinity (will be fixed to 1.0f)
    RSAnimationTimingProtocol protocol3;
    protocol3.SetDuration(2000);
    protocol3.SetSpeed(-INFINITY); // Negative infinity, will be fixed to 1.0f
    auto groupAnimatorWeak3 =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol3, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator3 = groupAnimatorWeak3.lock();

    if (!groupAnimator3) {
        std::cout << "Failed to create group animator 3" << std::endl;
        return;
    }

    std::cout << "Group animator 3 created successfully with speed fixed from -Infinity to 1.0f" << std::endl;

    groupAnimator3->AddImplictAnimation([nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });

    groupAnimator1->StartAnimation();
    groupAnimator2->StartAnimation();
    groupAnimator3->StartAnimation();

    transaction->FlushImplicitTransaction();
    std::cout << "All special float value test animations started, waiting 3 seconds..." << std::endl;
    sleep(3);
}

/**
 * @tc.name: GroupAnimationTestThirty
 * @tc.desc: Test multiple properties animation on each node
 * @tc.type: FUNC
 */
void GroupAnimationTestThirty(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "========================================" << std::endl;
    std::cout << "Test 30: Multiple Properties Animation" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Testing multiple properties on each node:" << std::endl;
    std::cout << "  Node 0: Translate + Scale + Alpha" << std::endl;
    std::cout << "  Node 1: Translate + Rotation + Bounds" << std::endl;
    std::cout << "  Node 2: Scale + Alpha + Bounds" << std::endl;

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Reset nodes
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with 2000ms duration
    RSAnimationTimingProtocol protocol(2000);
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    std::cout << "Group animator created successfully" << std::endl;

    // Node 0: Translate + Scale + Alpha (3 properties)
    groupAnimator->AddImplictAnimation([nodes]() {
        nodes[0]->SetTranslate(Vector2f(300.0f, 0.0f)); // Move right
        nodes[0]->SetScale(Vector2f(1.5f, 1.5f));       // Scale up
        nodes[0]->SetAlpha(0.5f);                       // Fade to half opacity
    });

    // Node 1: Translate + Rotation + Bounds (3 properties)
    groupAnimator->AddImplictAnimation([nodes]() {
        nodes[1]->SetTranslate(Vector2f(300.0f, 0.0f)); // Move right
        nodes[1]->SetRotation(90.0f);                   // Rotate 90 degrees
        nodes[1]->SetBounds(150, 350, 250, 250);        // Increase bounds size
    });

    // Node 2: Scale + Alpha + Bounds (3 properties)
    groupAnimator->AddImplictAnimation([nodes]() {
        nodes[2]->SetScale(Vector2f(0.5f, 0.5f)); // Scale down
        nodes[2]->SetAlpha(0.3f);                 // Fade to low opacity
        nodes[2]->SetBounds(150, 650, 100, 100);  // Decrease bounds size
    });

    groupAnimator->StartAnimation();

    transaction->FlushImplicitTransaction();
    std::cout << "Multiple properties animation started, waiting 3 seconds..." << std::endl;
    sleep(3);
}

/**
 * @tc.name: GroupAnimationTestThirtyOne
 * @tc.desc: Test animation behavior when app goes to background
 * @tc.type: FUNC
 */
void GroupAnimationTestThirtyOne(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "========================================" << std::endl;
    std::cout << "Test 31: Animation Pause on Background" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Testing that animation stops when app goes to background" << std::endl;

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Reset nodes
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with 4000ms duration (long enough to observe background behavior)
    RSAnimationTimingProtocol protocol(4000);
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    std::cout << "Group animator created with 4000ms duration" << std::endl;

    // Add translation animation to node 0
    groupAnimator->AddImplictAnimation([nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });

    // Record group animation start time
    auto groupStartTime = std::chrono::steady_clock::now();
    auto groupStartTimeMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(groupStartTime.time_since_epoch()).count();
    std::cout << "Group animation start time: " << groupStartTimeMs << "ms" << std::endl;

    // Set finish callback
    groupAnimator->SetFinishCallBack([groupStartTime]() {
        auto endTime = std::chrono::steady_clock::now();
        auto endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
        std::cout << "Group animation finished! End time: " << endTimeMs << "ms, Duration: " << duration << "ms"
                  << std::endl;
    });

    // Start animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    std::cout << "Animation started, waiting 1 second before going to background..." << std::endl;
    sleep(1);

    // Get surface node and set to background
    auto surfaceNode = rsUiDirector->GetRSSurfaceNode();
    if (surfaceNode) {
        std::cout << "Setting app to BACKGROUND state..." << std::endl;
        surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::BACKGROUND);
        transaction->FlushImplicitTransaction();
        std::cout << "App is now in BACKGROUND" << std::endl;
    } else {
        std::cout << "Failed to get surface node" << std::endl;
        return;
    }

    // Wait and observe if animation continues
    std::cout << "Waiting 3 seconds to check if animation continues..." << std::endl;
    sleep(3);

    // Restore to foreground and try to continue
    std::cout << "Restoring app to FOREGROUND state..." << std::endl;
    surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
    transaction->FlushImplicitTransaction();
    std::cout << "App restored to FOREGROUND" << std::endl;

    std::cout << "Test completed. Animation should have paused when in background." << std::endl;
}

/**
 * @tc.name: GroupAnimationTestThirtyTwo
 * @tc.desc: Test group animation with one finite child and one infinite child (repeatCount=-1)
 * @tc.type: FUNC
 */
void GroupAnimationTestThirtyTwo(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "========================================" << std::endl;
    std::cout << "Test 32: Group Animation with Infinite Child and Background" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Testing group animation with one finite child and one infinite child (repeatCount=-1)" << std::endl;
    std::cout << "Expected: Group completes based on its duration, infinite child stops when app goes to background"
              << std::endl;

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Record group animation start time
    auto groupStartTime = std::chrono::steady_clock::now();
    auto groupStartTimeMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(groupStartTime.time_since_epoch()).count();
    std::cout << "Group animation start time: " << groupStartTimeMs << "ms" << std::endl;

    // Create a group animator with 10000ms duration
    auto protocol = RSAnimationTimingProtocol(10000);
    auto rsUIContext_local = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext_local, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack([groupStartTime]() {
        auto endTime = std::chrono::steady_clock::now();
        auto endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
        std::cout << "Group animation finished! End time: " << endTimeMs << "ms, Duration: " << duration << "ms"
                  << std::endl;
    });

    // Add first child animation with finite repeatCount (repeatCount=1, no repeat)
    auto callback0 = [nodes, rsUIContext_local, groupStartTime]() {
        RSAnimationTimingProtocol protocol0(2000);
        protocol0.SetRepeatCount(1); // Finite: play once
        RSNode::Animate(
            rsUIContext_local, protocol0, RSAnimationTimingCurve::EASE_IN_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); },
            [groupStartTime]() {
                auto endTime = std::chrono::steady_clock::now();
                auto endTimeMs =
                    std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
                std::cout << "Node 0 (Finite, repeatCount=1) animation finished! End time: " << endTimeMs
                          << "ms, Duration: " << duration << "ms" << std::endl;
            });
    };
    groupAnimator->AddAnimation(callback0);

    // Add second child animation with infinite repeatCount (repeatCount=-1)
    auto callback1 = [nodes, rsUIContext_local, groupStartTime]() {
        RSAnimationTimingProtocol protocol1(500);
        protocol1.SetRepeatCount(-1); // Infinite: loop forever
        RSNode::Animate(
            rsUIContext_local, protocol1, RSAnimationTimingCurve::EASE_IN_OUT,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); },
            [groupStartTime]() {
                auto endTime = std::chrono::steady_clock::now();
                auto endTimeMs =
                    std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
                std::cout << "Node 1 (Infinite, repeatCount=-1) animation finished! End time: " << endTimeMs
                          << "ms, Duration: " << duration << "ms" << std::endl;
            });
    };
    groupAnimator->AddAnimation(callback1);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    std::cout << "Animation started, waiting 1 second before going to background..." << std::endl;
    sleep(1);

    // Get surface node and set to background
    auto surfaceNode = rsUiDirector->GetRSSurfaceNode();
    if (surfaceNode) {
        std::cout << "Setting app to BACKGROUND state..." << std::endl;
        surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::BACKGROUND);
        transaction->FlushImplicitTransaction();
        std::cout << "App is now in BACKGROUND" << std::endl;
    } else {
        std::cout << "Failed to get surface node" << std::endl;
        return;
    }

    // Wait and observe if animations continue
    std::cout << "Waiting 3 seconds to check if animations continue..." << std::endl;
    sleep(3);

    // Restore to foreground
    std::cout << "Restoring app to FOREGROUND state..." << std::endl;
    surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
    transaction->FlushImplicitTransaction();
    std::cout << "App restored to FOREGROUND" << std::endl;
    sleep(5);

    std::cout << "Test completed. Group animation should have finished, and infinite child should stop when app went "
                 "to background."
              << std::endl;
    std::cout << "Expected behavior:" << std::endl;
    std::cout << "  - Group animation finishes after ~2000ms (its duration)" << std::endl;
    std::cout << "  - Finite child (Node 0) finishes with the group" << std::endl;
    std::cout << "  - Infinite child (Node 1) stops when app goes to BACKGROUND" << std::endl;
}

/**
 * @tc.name: GroupAnimationTestThirtyThree
 * @tc.desc: Test group animation with duration and delay, verify total time calculation
 * @tc.type: FUNC
 */
void GroupAnimationTestThirtyThree(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "========================================" << std::endl;
    std::cout << "Test 33: Group Animation with Delay and Time Measurement" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Testing group animation with duration=2000ms, startDelay=500ms" << std::endl;
    std::cout << "Expected total time: 500ms (delay) + 2000ms (duration) = 2500ms" << std::endl;

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Record group animation start time
    auto groupStartTime = std::chrono::steady_clock::now();
    auto groupStartTimeMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(groupStartTime.time_since_epoch()).count();
    std::cout << "Group animation start time: " << groupStartTimeMs << "ms" << std::endl;

    // Create group animator with 2000ms duration and 500ms start delay
    auto protocol = RSAnimationTimingProtocol(2000);
    protocol.SetStartDelay(500); // 500ms start delay
    auto rsUIContext_local = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext_local, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    std::cout << "Group animator created with duration=2000ms, startDelay=500ms" << std::endl;

    // Set finish callback with time measurement
    groupAnimator->SetFinishCallBack([groupStartTime]() {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - groupStartTime).count();
        std::cout << "========================================" << std::endl;
        std::cout << "Group animation finished!" << std::endl;
        std::cout << "  Total duration: " << duration << "ms" << std::endl;
        std::cout << "  Expected: 500ms (delay) + 2000ms (duration) = 2500ms" << std::endl;
        std::cout << "========================================" << std::endl;
    });

    // Add one child animation for node 0
    auto callback0 = [nodes, rsUIContext_local]() {
        RSAnimationTimingProtocol protocol0(2000); // 2000ms duration
        RSNode::Animate(rsUIContext_local, protocol0, RSAnimationTimingCurve::EASE_IN_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    std::cout << "Added 1 child animation for Node 0 with duration=2000ms" << std::endl;

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Group animation started at " << groupStartTimeMs << "ms" << std::endl;
    std::cout << "Waiting for completion (expected ~2500ms)..." << std::endl;
    sleep(3);

    std::cout << "Test completed. Check finish callback output for time measurement." << std::endl;
}

/**
 * @tc.name: GroupAnimationTestThirtyFour
 * @tc.desc: Test normal animation (not group animation) with pause/continue during delay
 * @tc.type: FUNC
 */
void GroupAnimationTestThirtyFour(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "========================================" << std::endl;
    std::cout << "Test 34: Normal Animation with Pause/Continue During Delay" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Testing normal animation (NOT group animation):" << std::endl;
    std::cout << "  duration=2000ms, startDelay=500ms" << std::endl;
    std::cout << "  Pause at 200ms (during startDelay)" << std::endl;
    std::cout << "  Wait 1s, then Continue" << std::endl;
    std::cout << "Expected duration: 2000ms + 500ms(delay) + 1000ms(pause) = 3500ms" << std::endl;

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Record animation start time
    auto animationStartTime = std::chrono::steady_clock::now();
    auto animationStartTimeMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(animationStartTime.time_since_epoch()).count();
    std::cout << "Animation start time: " << animationStartTimeMs << "ms" << std::endl;

    // Create normal animation (not group animation) with 2000ms duration and 500ms start delay
    RSAnimationTimingProtocol protocol(2000);
    protocol.SetStartDelay(500); // 500ms start delay

    // Create animation with finish callback
    // Note: RSNode::Animate returns std::vector<std::shared_ptr<RSAnimation>>
    auto animations = RSNode::Animate(
        rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT,
        [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); },
        [animationStartTime]() {
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - animationStartTime).count();
            std::cout << "========================================" << std::endl;
            std::cout << "Normal animation finished!" << std::endl;
            std::cout << "  Actual duration: " << duration << "ms" << std::endl;
            std::cout << "  Expected duration analysis:" << std::endl;
            std::cout << "    - startDelay: 500ms (200ms consumed, 300ms remaining)" << std::endl;
            std::cout << "    - Pause at 200ms: pauses startDelay timer (300ms remaining)" << std::endl;
            std::cout << "    - Pause duration: 1000ms" << std::endl;
            std::cout << "    - Continue at 1200ms: resumes startDelay" << std::endl;
            std::cout << "    - Remaining startDelay: 300ms" << std::endl;
            std::cout << "    - duration: 2000ms" << std::endl;
            std::cout << "  Total: 500ms + 2000ms + 1000ms = 3500ms" << std::endl;
            std::cout << "========================================" << std::endl;
        });

    // Get the first animation from the vector
    if (animations.empty()) {
        std::cout << "Failed to create animation" << std::endl;
        return;
    }
    auto animation = animations[0];

    // Flush transaction to start animation
    transaction->FlushImplicitTransaction();
    std::cout << "Animation started at " << animationStartTimeMs << "ms" << std::endl;

    // Wait 200ms (still in startDelay period)
    usleep(200000); // 200ms = 200000us
    std::cout << "At 200ms: Pausing animation (still in startDelay)..." << std::endl;

    // Pause the animation
    animation->Pause();
    transaction->FlushImplicitTransaction();

    // Wait 1 second, then continue
    sleep(1);
    std::cout << "At 1200ms: Continuing animation..." << std::endl;

    // Continue the animation
    animation->Resume();
    transaction->FlushImplicitTransaction();

    // Wait for animation to complete
    std::cout << "Waiting for animation to complete..." << std::endl;
    sleep(3);

    std::cout << "Test completed. Check finish callback for actual duration measurement." << std::endl;
}

void GroupAnimationTestThirtyFive(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes,
    std::shared_ptr<RSNode>& rootNode)
{
    std::cout << "========================================" << std::endl;
    std::cout << "Test 35: Node destruction during group animation" << std::endl;
    std::cout << "========================================" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create group animator: duration=3000ms, delay=500ms
    auto protocol = RSAnimationTimingProtocol(3000);
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimator =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT).lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    auto groupStartTime = std::chrono::steady_clock::now();
    groupAnimator->SetFinishCallBack([groupStartTime]() {
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - groupStartTime)
                .count();
        std::cout << "Group animation finished! Duration: " << duration << "ms" << std::endl;
    });

    // Add child animation for node 0
    groupAnimator->AddAnimation([nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(2500);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    });

    // Start animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    std::cout << "Group animation started, waiting 1500ms..." << std::endl;
    sleep(1);
    usleep(500000); // 500ms

    // Destroy node[0] step by step
    auto nodeId = nodes[0]->GetId();
    std::cout << "Destroying node[0] (id:" << nodeId << ")..." << std::endl;
    std::cout << "  Initial ref count: " << nodes[0].use_count() << std::endl;

    std::cout << "  1. RemoveChild from rootNode: " << nodes[0].use_count();
    rootNode->RemoveChild(nodes[0]);
    transaction->FlushImplicitTransaction();
    std::cout << " -> " << nodes[0].use_count() << std::endl;

    std::cout << "  2. RemoveFromTree: " << nodes[0].use_count();
    nodes[0]->RemoveFromTree();
    std::cout << " -> " << nodes[0].use_count() << std::endl;

    std::cout << "  3. Release reference: " << nodes[0].use_count() << " -> 0" << std::endl;
    nodes[0] = nullptr;
    transaction->FlushImplicitTransaction();

    std::cout << "Node destroyed, waiting for group animation to complete..." << std::endl;
    sleep(2);

    std::cout << "Test completed. Recreating node[0]..." << std::endl;
    nodes[0] = RSCanvasNode::Create(false, false, rsUIContext);
    nodes[0]->SetBounds(100, 50, 200, 200);
    nodes[0]->SetFrame(100, 50, 200, 200);
    nodes[0]->SetBackgroundColor(SK_ColorBLUE);
    rootNode->AddChild(nodes[0], -1);
    transaction->FlushImplicitTransaction();
    std::cout << "Node[0] recreated." << std::endl;
    sleep(2);
}

void GroupAnimationTestThirtySix(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Test 36: Group animation with repeat=3, control child animations during first round" << std::endl;
    std::cout << "Group animator: duration=2000ms, repeat=3" << std::endl;
    std::cout << "Child 1 (node[0]): duration=2000ms, will pause at 500ms" << std::endl;
    std::cout << "Child 2 (node[1]): duration=2000ms, will finish at 500ms" << std::endl;
    std::cout << "Child 3 (node[2]): duration=2000ms" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Create group animator with repeat=3
    auto protocol = RSAnimationTimingProtocol(2000);
    protocol.SetRepeatCount(3);
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    groupAnimator->SetFinishCallBack([]() { std::cout << "Test 36: Group animation finished!" << std::endl; });

    // Add child animation 1 for node[0] and save reference
    std::vector<std::shared_ptr<RSAnimation>> childAnims1;
    auto callback0 = [nodes, rsUIContext, &childAnims1]() {
        RSAnimationTimingProtocol protocol(2000); // 2000ms duration
        childAnims1 = RSNode::Animate(rsUIContext, protocol, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add child animation 2 for node[1] and save reference
    std::vector<std::shared_ptr<RSAnimation>> childAnims2;
    auto callback1 = [nodes, rsUIContext, &childAnims2]() {
        RSAnimationTimingProtocol protocol(2000); // 2000ms duration
        childAnims2 = RSNode::Animate(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Add child animation 3 for node[2] and save reference
    std::vector<std::shared_ptr<RSAnimation>> childAnims3;
    auto callback2 = [nodes, rsUIContext, &childAnims3]() {
        RSAnimationTimingProtocol protocol(2000); // 2000ms duration
        childAnims3 = RSNode::Animate(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback2);

    // Start group animation (this will execute the callbacks and create child animations)
    std::cout << "Starting group animation..." << std::endl;
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    // Wait for 500ms (during first round of repeat=3)
    usleep(500000);

    std::cout << "At 500ms: Controlling child animations..." << std::endl;
    std::cout << "  childAnims1 size: " << childAnims1.size() << std::endl;
    std::cout << "  childAnims2 size: " << childAnims2.size() << std::endl;

    // Control child animations
    if (!childAnims1.empty() && childAnims1[0]) {
        std::cout << "Pausing child animation 1..." << std::endl;
        childAnims1[0]->Pause();
    } else {
        std::cout << "childAnims1 is empty or null!" << std::endl;
    }

    if (!childAnims2.empty() && childAnims2[0]) {
        std::cout << "Finishing child animation 2..." << std::endl;
        childAnims2[0]->Finish();
    } else {
        std::cout << "childAnims2 is empty or null!" << std::endl;
    }

    transaction->FlushImplicitTransaction();

    // Wait to observe behavior
    std::cout << "Waiting for group animation to complete..." << std::endl;
    sleep(5);
}

void GroupAnimationTestThirtySeven(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Test 37: Group animation with repeat=3, autoReverse=true, control child animations" << std::endl;
    std::cout << "Group animator: duration=2000ms, repeat=3, autoReverse=true" << std::endl;
    std::cout << "Child 1 (node[0]): duration=2000ms, will pause at 500ms" << std::endl;
    std::cout << "Child 2 (node[1]): duration=2000ms, will finish at 500ms" << std::endl;
    std::cout << "Child 3 (node[2]): duration=2000ms (normal)" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Create group animator with repeat=3 and autoReverse=true
    auto protocol = RSAnimationTimingProtocol(2000);
    protocol.SetRepeatCount(3);
    protocol.SetAutoReverse(true); // Key difference: autoReverse enabled
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    groupAnimator->SetFinishCallBack(
        []() { std::cout << "Test 37: Group animation with autoReverse finished!" << std::endl; });

    // Add child animation 1 for node[0] and save reference
    std::vector<std::shared_ptr<RSAnimation>> childAnims1;
    auto callback0 = [nodes, rsUIContext, &childAnims1]() {
        RSAnimationTimingProtocol protocol(2000);
        childAnims1 = RSNode::Animate(rsUIContext, protocol, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add child animation 2 for node[1] and save reference
    std::vector<std::shared_ptr<RSAnimation>> childAnims2;
    auto callback1 = [nodes, rsUIContext, &childAnims2]() {
        RSAnimationTimingProtocol protocol(2000);
        childAnims2 = RSNode::Animate(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Add child animation 3 for node[2] and save reference
    std::vector<std::shared_ptr<RSAnimation>> childAnims3;
    auto callback2 = [nodes, rsUIContext, &childAnims3]() {
        RSAnimationTimingProtocol protocol(2000);
        childAnims3 = RSNode::Animate(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[2]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback2);

    // Start group animation
    std::cout << "Starting group animation with autoReverse..." << std::endl;
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    // Wait for 500ms (during first round, forward direction)
    usleep(500000);

    std::cout << "At 500ms (forward): Controlling child animations..." << std::endl;
    std::cout << "  childAnims1 size: " << childAnims1.size() << std::endl;
    std::cout << "  childAnims2 size: " << childAnims2.size() << std::endl;

    // Control child animations
    if (!childAnims1.empty() && childAnims1[0]) {
        std::cout << "Pausing child animation 1..." << std::endl;
        childAnims1[0]->Pause();
    } else {
        std::cout << "childAnims1 is empty or null!" << std::endl;
    }

    if (!childAnims2.empty() && childAnims2[0]) {
        std::cout << "Finishing child animation 2..." << std::endl;
        childAnims2[0]->Finish();
    } else {
        std::cout << "childAnims2 is empty or null!" << std::endl;
    }

    transaction->FlushImplicitTransaction();

    // Wait to observe autoReverse behavior
    std::cout << "Waiting for group animation to complete..." << std::endl;
    std::cout << "Expected: Each repeat will play forward then reverse" << std::endl;
    sleep(8); // Longer wait due to autoReverse (2x duration per repeat)
}

void GroupAnimationTestThirtyEight(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Test 38: Group animation with two path animations" << std::endl;
    std::cout << "Group animator: duration=1000ms" << std::endl;
    std::cout << "Path animation 1 (node[0]): duration=800ms (shorter than group)" << std::endl;
    std::cout << "Path animation 2 (node[1]): duration=1500ms (longer than group)" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    auto rsUIContext = rsUiDirector->GetRSUIContext();

    // Create group animator with 1000ms duration
    RSAnimationTimingProtocol groupProtocol;
    groupProtocol.SetDuration(1000); // 1000ms group duration
    groupProtocol.SetRepeatCount(1); // Run once
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, groupProtocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    groupAnimator->SetFinishCallBack(
        []() { std::cout << "Test 38: Group animation with path animations finished!" << std::endl; });

    // Add first path animation with 800ms duration (shorter than group 1000ms)
    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(800); // 800ms duration (shorter than group)
        std::string animationPath0 = "M0 0 L300 200 L300 500 Z";
        auto motionPathOption0 = std::make_shared<RSMotionPathOption>(animationPath0);
        nodes[0]->SetMotionPathOption(motionPathOption0);

        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(300.0f, 500.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Add second path animation with 1500ms duration (longer than group 1000ms)
    auto callback1 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol1(1500); // 1500ms duration (longer than group)
        std::string animationPath1 = "M0 0 L400 300 L400 700 Z";
        auto motionPathOption1 = std::make_shared<RSMotionPathOption>(animationPath1);
        nodes[1]->SetMotionPathOption(motionPathOption1);

        RSNode::Animate(rsUIContext, protocol1, RSAnimationTimingCurve::EASE_IN,
            [nodes]() { nodes[1]->SetTranslate(Vector2f(400.0f, 700.0f)); });
    };
    groupAnimator->AddAnimation(callback1);

    // Start group animation
    std::cout << "Starting group animation with two path animations..." << std::endl;
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();

    // Wait for animations to complete
    std::cout << "Waiting for animations to complete..." << std::endl;
    std::cout << "Expected behavior:" << std::endl;
    std::cout << "  - Path animation 1 (800ms) will complete before group (1000ms)" << std::endl;
    std::cout << "  - Path animation 2 (1500ms) will be cut off when group completes at 1000ms" << std::endl;
    sleep(5);
}

void GroupAnimationTestThirtyNine(std::shared_ptr<RSUIDirector> rsUiDirector,
    std::shared_ptr<RSTransactionHandler> transaction, std::vector<std::shared_ptr<RSCanvasNode>>& nodes)
{
    std::cout << "Group Animation case ThirtyNine: Child animations with durations exceeding group duration, repeatCount = "
                 "4, autoReverse = true"
              << std::endl;
    std::cout << "Group animator duration: 1000ms, repeatCount = 4, autoReverse = true" << std::endl;
    std::cout << "Node 0 duration: 800ms (shorter than group)" << std::endl;

    // Reset all properties
    ResetAllNodes(nodes);
    transaction->FlushImplicitTransaction();

    // Create a group animator with 1000ms duration, repeatCount = 4, and autoReverse = true
    auto protocol = RSAnimationTimingProtocol(1000);
    protocol.SetRepeatCount(4);    // Repeat 4 times
    protocol.SetAutoReverse(true); // Enable auto reverse
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto groupAnimatorWeak =
        RSInteractiveImplictAnimator::CreateGroup(rsUIContext, protocol, RSAnimationTimingCurve::EASE_IN_OUT);
    auto groupAnimator = groupAnimatorWeak.lock();

    if (!groupAnimator) {
        std::cout << "Failed to create group animator" << std::endl;
        return;
    }

    // Set finish callback
    groupAnimator->SetFinishCallBack(
        []() { std::cout << "Group animation with repeatCount=4, autoReverse=true finished!" << std::endl; });

    // Add animation with 800ms duration for node 0 (shorter than group duration)
    auto callback0 = [nodes, rsUIContext]() {
        RSAnimationTimingProtocol protocol0(800);
        RSNode::Animate(rsUIContext, protocol0, RSAnimationTimingCurve::EASE_OUT,
            [nodes]() { nodes[0]->SetTranslate(Vector2f(500.0f, 0.0f)); });
    };
    groupAnimator->AddAnimation(callback0);

    // Start the group animation
    groupAnimator->StartAnimation();
    transaction->FlushImplicitTransaction();
    
    // Print Node 0 current translation value after starting animation
    auto currentTranslate = nodes[0]->GetStagingProperties().GetTranslate();
    std::cout << "Node 0 current translation value after starting animation: (" 
              << currentTranslate.x_ << ", " << currentTranslate.y_ << ")" << std::endl;

    std::cout << "Group animation started: group=1000ms repeat=4 autoReverse=true, node0=800ms, waiting for completion..."
              << std::endl;
    sleep(6);
}

bool StartTest(int testCase = -1)
{
    // Create rsUIContext
    std::shared_ptr<RSUIDirector> rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init(true, true);
    auto uiContext = rsUiDirector->GetRSUIContext();
    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    rsUiDirector->SetUITaskRunner(
        [handler](const std::function<void()>& task, uint32_t delay) { handler->PostTask(task, delay); }, 1000, true);
    runner->Run();

    if (uiContext) {
        std::cout << "RSUIDirector::Create success" << std::endl;
        std::cout << "get RSUIContext by RSUIDirector success" << std::endl;
        std::cout << "RSUIContext token is " << uiContext->GetToken() << std::endl;
    } else {
        std::cout << "RSUIContext::Create failed" << std::endl;
        return false;
    }

    // Create rsSurfaceNode
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "GroupAnimation_Window";
    RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    std::cout << "RSSurfaceNode::Create" << std::endl;
    std::shared_ptr<RSSurfaceNode> surfaceNode =
        RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType, true, false, uiContext);
    std::cout << "Create node by RSUIContext success" << std::endl;
    surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
    surfaceNode->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    surfaceNode->SetBounds(0, 0, 1200, 1200);
    surfaceNode->SetBackgroundColor(SK_ColorWHITE);
    rsUiDirector->SetRSSurfaceNode(surfaceNode);

    // Create subNode
    std::shared_ptr<RSNode> rootNode;
    std::vector<std::shared_ptr<RSCanvasNode>> nodes;
    InitNodes(rsUiDirector, 1000, 1000, nodes, rootNode);
    surfaceNode->AttachToDisplay(screenId);

    // Flush transaction
    auto transaction = uiContext->GetRSTransaction();
    if (!transaction) {
        std::cout << "!transaction" << std::endl;
        return false;
    }
    transaction->FlushImplicitTransaction();
    std::cout << "transaction success" << std::endl;
    sleep(2);

    // Run specific test case or all test cases
    switch (testCase) {
        case 1:
            GroupAnimationTestOne(rsUiDirector, transaction, nodes);
            break;
        case 2:
            GroupAnimationTestTwo(rsUiDirector, transaction, nodes);
            break;
        case 3:
            GroupAnimationTestThree(rsUiDirector, transaction, nodes);
            break;
        case 4:
            GroupAnimationTestFour(rsUiDirector, transaction, nodes);
            break;
        case 5:
            GroupAnimationTestFive(rsUiDirector, transaction, nodes);
            break;
        case 6:
            GroupAnimationTestSix(rsUiDirector, transaction, nodes);
            break;
        case 7:
            GroupAnimationTestSeven(rsUiDirector, transaction, nodes);
            break;
        case 8:
            GroupAnimationTestEight(rsUiDirector, transaction, nodes);
            break;
        case 9:
            GroupAnimationTestNine(rsUiDirector, transaction, nodes);
            break;
        case 10:
            GroupAnimationTestTen(rsUiDirector, transaction, nodes);
            break;
        case 11:
            GroupAnimationTestEleven(rsUiDirector, transaction, nodes);
            break;
        case 12:
            GroupAnimationTestTwelve(rsUiDirector, transaction, nodes);
            break;
        case 13:
            GroupAnimationTestThirteen(rsUiDirector, transaction, nodes);
            break;
        case 14:
            GroupAnimationTestFourteen(rsUiDirector, transaction, nodes);
            break;
        case 15:
            GroupAnimationTestFifteen(rsUiDirector, transaction, nodes);
            break;
        case 16:
            GroupAnimationTestSixteen(rsUiDirector, transaction, nodes);
            break;
        case 17:
            GroupAnimationTestSeventeen(rsUiDirector, transaction, nodes);
            break;
        case 18:
            GroupAnimationTestEighteen(rsUiDirector, transaction, nodes);
            break;
        case 19:
            GroupAnimationTestNinteen(rsUiDirector, transaction, nodes);
            break;
        case 20:
            GroupAnimationTestTwenty(rsUiDirector, transaction, nodes);
            break;
        case 21:
            GroupAnimationTestTwentyOne(rsUiDirector, transaction, nodes);
            break;
        case 22:
            GroupAnimationTestTwentyTwo(rsUiDirector, transaction, nodes);
            break;
        case 23:
            GroupAnimationTestTwentyThree(rsUiDirector, transaction, nodes);
            break;
        case 24:
            GroupAnimationTestTwentyFour(rsUiDirector, transaction, nodes);
            break;
        case 25:
            GroupAnimationTestTwentyFive(rsUiDirector, transaction, nodes);
            break;
        case 26:
            GroupAnimationTestTwentySix(rsUiDirector, transaction, nodes);
            break;
        case 27:
            GroupAnimationTestTwentySeven(rsUiDirector, transaction, nodes);
            break;
        case 28:
            GroupAnimationTestTwentyEight(rsUiDirector, transaction, nodes);
            break;
        case 29:
            GroupAnimationTestTwentyNine(rsUiDirector, transaction, nodes);
            break;
        case 30:
            GroupAnimationTestThirty(rsUiDirector, transaction, nodes);
            break;
        case 31:
            GroupAnimationTestThirtyOne(rsUiDirector, transaction, nodes);
            break;
        case 32:
            GroupAnimationTestThirtyTwo(rsUiDirector, transaction, nodes);
            break;
        case 33:
            GroupAnimationTestThirtyThree(rsUiDirector, transaction, nodes);
            break;
        case 34:
            GroupAnimationTestThirtyFour(rsUiDirector, transaction, nodes);
            break;
        case 35:
            GroupAnimationTestThirtyFive(rsUiDirector, transaction, nodes, rootNode);
            break;
        case 36:
            GroupAnimationTestThirtySix(rsUiDirector, transaction, nodes);
            break;
        case 37:
            GroupAnimationTestThirtySeven(rsUiDirector, transaction, nodes);
            break;
        case 38:
            GroupAnimationTestThirtyEight(rsUiDirector, transaction, nodes);
            break;
        case 39:
            GroupAnimationTestThirtyNine(rsUiDirector, transaction, nodes);
            break;

        default:
            // Run all test cases
            GroupAnimationTestOne(rsUiDirector, transaction, nodes);
            GroupAnimationTestTwo(rsUiDirector, transaction, nodes);
            GroupAnimationTestThree(rsUiDirector, transaction, nodes);
            GroupAnimationTestFour(rsUiDirector, transaction, nodes);
            GroupAnimationTestFive(rsUiDirector, transaction, nodes);
            GroupAnimationTestSix(rsUiDirector, transaction, nodes);
            GroupAnimationTestSeven(rsUiDirector, transaction, nodes);
            GroupAnimationTestEight(rsUiDirector, transaction, nodes);
            GroupAnimationTestNine(rsUiDirector, transaction, nodes);
            GroupAnimationTestTen(rsUiDirector, transaction, nodes);
            GroupAnimationTestEleven(rsUiDirector, transaction, nodes);
            GroupAnimationTestTwelve(rsUiDirector, transaction, nodes);
            GroupAnimationTestThirteen(rsUiDirector, transaction, nodes);
            GroupAnimationTestFourteen(rsUiDirector, transaction, nodes);
            GroupAnimationTestFifteen(rsUiDirector, transaction, nodes);
            GroupAnimationTestSixteen(rsUiDirector, transaction, nodes);
            GroupAnimationTestSeventeen(rsUiDirector, transaction, nodes);
            GroupAnimationTestEighteen(rsUiDirector, transaction, nodes);
            GroupAnimationTestNinteen(rsUiDirector, transaction, nodes);
            GroupAnimationTestTwenty(rsUiDirector, transaction, nodes);
            GroupAnimationTestTwentyOne(rsUiDirector, transaction, nodes);
            GroupAnimationTestTwentyTwo(rsUiDirector, transaction, nodes);
            GroupAnimationTestTwentyThree(rsUiDirector, transaction, nodes);
            GroupAnimationTestTwentyFour(rsUiDirector, transaction, nodes);
            GroupAnimationTestTwentyFive(rsUiDirector, transaction, nodes);
            GroupAnimationTestTwentySix(rsUiDirector, transaction, nodes);
            GroupAnimationTestTwentySeven(rsUiDirector, transaction, nodes);
            GroupAnimationTestTwentyEight(rsUiDirector, transaction, nodes);
            GroupAnimationTestTwentyNine(rsUiDirector, transaction, nodes);
            GroupAnimationTestThirty(rsUiDirector, transaction, nodes);
            GroupAnimationTestThirtyOne(rsUiDirector, transaction, nodes);
            GroupAnimationTestThirtyTwo(rsUiDirector, transaction, nodes);
            GroupAnimationTestThirtyThree(rsUiDirector, transaction, nodes);
            GroupAnimationTestThirtyFour(rsUiDirector, transaction, nodes);
            GroupAnimationTestThirtyFive(rsUiDirector, transaction, nodes, rootNode);
            GroupAnimationTestThirtySix(rsUiDirector, transaction, nodes);
            GroupAnimationTestThirtySeven(rsUiDirector, transaction, nodes);
            GroupAnimationTestThirtyEight(rsUiDirector, transaction, nodes);
            GroupAnimationTestThirtyNine(rsUiDirector, transaction, nodes);
            break;
    }

    sleep(2);
    return true;
}
} // namespace

int main()
{
    int testCase = -1;
    std::cout << "========================================" << std::endl;
    std::cout << "Group Animation Demo Test Cases" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. Child animations with durations exceeding group duration" << std::endl;
    std::cout << "2. Group animation with delays and child animations with delays" << std::endl;
    std::cout << "3. Different child speeds with group speed 2" << std::endl;
    std::cout << "4. Child animations exceeding group duration, repeatCount=3" << std::endl;
    std::cout << "5. Child animations exceeding group duration, repeatCount=-1 (infinite)" << std::endl;
    std::cout << "6. Mix of delay and repeatCount" << std::endl;
    std::cout << "7. All child animations shorter than group duration, repeatCount=3" << std::endl;
    std::cout << "8. All child animations longer than group duration, repeatCount=3" << std::endl;
    std::cout << "9. Child animations exceeding group duration, repeatCount=3, autoReverse=true" << std::endl;
    std::cout << "10. Two group animations with different delays" << std::endl;
    std::cout << "11. Test Pause/Continue commands on group animation" << std::endl;
    std::cout << "12. Test Reverse command on group animation" << std::endl;
    std::cout << "13. Test Finish command on group animation: Finish at END" << std::endl;
    std::cout << "14. Test Pause/Continue commands on group animation with repeat" << std::endl;
    std::cout << "15. Test Pause/Continue commands on group animation with repeat&autoReverse=true" << std::endl;
    std::cout << "16. Comparison between two groups of animations: one with delay and one without delay" << std::endl;
    std::cout << "17. Test that the grouped animation receives a pause command during its delay period." << std::endl;
    std::cout << "18. Test Finish command on group animation: Finish at START" << std::endl;
    std::cout << "19. Test Finish command on group animation: Finish at CURRENT" << std::endl;
    std::cout << "20. Test that the grouped animation receives a Finish command during its delay period" << std::endl;
    std::cout << "21. Test that the child animations of a grouped animation receive a Finish command during the "
                 "groupWaiting period."
              << std::endl;
    std::cout << "22. Test that the grouped animation receives a finish command while paused." << std::endl;
    std::cout << "23. Test Group animation finishCallback" << std::endl;
    std::cout << "24. Test different animation types - Spring, Keyframe, and Steps" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Boundary Value and Exception Test Cases:" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "25. Test duration boundary values: negative, zero" << std::endl;
    std::cout << "26. Test startDelay boundary values: negative" << std::endl;
    std::cout << "27. Test speed boundary values: zero, negative" << std::endl;
    std::cout << "28. Test repeatCount boundary values: other negatives (except -1), zero" << std::endl;
    std::cout << "29. Test special float values: NaN, Infinity" << std::endl;
    std::cout << "30. Test multiple properties animation on each node" << std::endl;
    std::cout << "31. Test animation behavior when app goes to background" << std::endl;
    std::cout << "32. Test group animation with infinite child (repeatCount=-1) and background behavior" << std::endl;
    std::cout << "33. Test group animation with delay and time measurement (duration=2000ms, delay=500ms)" << std::endl;
    std::cout << "34. Test normal animation with pause/continue during delay" << std::endl;
    std::cout << "35. Test group animation with node destruction during animation" << std::endl;
    std::cout << "36. Test group animation (repeat=3) controlling child animations during first round" << std::endl;
    std::cout << "37. Test group animation (repeat=3, autoReverse=true) controlling child animations" << std::endl;
    std::cout << "38. Test group animation with two path animations (800ms and 1500ms)" << std::endl;
    std::cout << "39. Child animations exceeding group duration, repeatCount=4, autoReverse=true (single child)" << std::endl;
    std::cout << "========================================" << std::endl;

    std::cout << "0. Run all test cases" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Please enter test case number (0-39): ";
    std::cin >> testCase;

    if (testCase < 0 || testCase > 39) {
        std::cout << "Invalid input, running all test cases..." << std::endl;
        testCase = 0;
    }

    StartTest(testCase);
    return 0;
}
