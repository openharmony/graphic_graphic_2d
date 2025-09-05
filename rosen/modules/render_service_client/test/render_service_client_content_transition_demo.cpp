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

#include <iostream>

#include "rs_trace.h"

#include "draw/canvas.h"
#include "modifier_ng/custom/rs_content_style_modifier.h"
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
const int CANAS_WIDTH = 100;
const int CANAS_HEIGHT = 200;
const int SLEEP_TIME_US = 400000;
enum class ContentIndex : uint8_t {
    TYPE_EMPTY = 0,
    TYPE_ONE,
    TYPE_TWO,
    TYPE_THREE,
};
class ContentTransitionModifier : public ModifierNG::RSContentStyleModifier {
public:
    ContentTransitionModifier() = default;
    ~ContentTransitionModifier() = default;

    void Draw(ModifierNG::RSDrawingContext& context) const override
    {
        std::cout << "Draw contentIndex: " << static_cast<int>(contentIndex_) << std::endl;
        Drawing::Brush brush;
        switch (contentIndex_) {
            case ContentIndex::TYPE_EMPTY:
                return;
            case ContentIndex::TYPE_ONE: {
                // blue Rect
                Drawing::Rect rect1(0, 0, 500, 500);
                brush.SetColor(SK_ColorBLUE);
                context.canvas->AttachBrush(brush);
                context.canvas->DrawRect(rect1);
                context.canvas->DetachBrush();
                return;
            }
            case ContentIndex::TYPE_TWO: {
                // red circle
                brush.SetColor(SK_ColorRED);
                context.canvas->AttachBrush(brush);
                Drawing::Point centerpoint(CANAS_WIDTH, CANAS_HEIGHT);
                context.canvas->DrawCircle(centerpoint, CANAS_WIDTH);
                context.canvas->DetachBrush();
                return;
            }
            case ContentIndex::TYPE_THREE: {
                // yellow Rect
                Drawing::Rect rect1(0, 0, 500, 500);
                brush.SetColor(SK_ColorYELLOW);
                context.canvas->AttachBrush(brush);
                context.canvas->DrawRect(rect1);
                context.canvas->DetachBrush();
                return;
            }
            default:
                return;
        }
    }
    void SetContentIndex(ContentIndex index)
    {
        std::cout << "SetContentIndex: " << static_cast<int>(index) << std::endl;
        contentIndex_ = index;
    }

private:
    ContentIndex contentIndex_ = ContentIndex::TYPE_EMPTY;
};
void InitNodes(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height,
    std::vector<std::shared_ptr<RSCanvasNode>>& nodes, std::shared_ptr<RSNode>& rootNode)
{
    std::cout << "Init " << std::endl;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    rootNode = RSRootNode::Create(false, false, rsUIContext);
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);

    nodes.emplace_back(RSCanvasNode::Create(false, false, rsUIContext));
    nodes[0]->SetBounds(0, 0, width, height);
    nodes[0]->SetFrame(0, 0, width, height);

    rootNode->AddChild(nodes[0], -1);
    rsUiDirector->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
}

void ContentTransitionTestOne(std::shared_ptr<ContentTransitionModifier> contentTransitionModifier,
    std::shared_ptr<RSUIDirector> rsUiDirector, std::shared_ptr<RSTransactionHandler> transaction,
    std::shared_ptr<RSModifierManager> rsModifierManager)
{
    // Use the default parameters : duration 200ms
    std::cout << "Content Transition case one: default parameters:duration 200ms" << std::endl;
    contentTransitionModifier->SetContentTransitionParam(ContentTransitionType::OPACITY);

    // change content to type one with animation
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_ONE);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    sleep(3);
    // change content to type two with animation
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_TWO);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    sleep(3);
    // change content to type three with animation
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_THREE);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    sleep(3);
}

void ContentTransitionTestTwo(std::shared_ptr<ContentTransitionModifier> contentTransitionModifier,
    std::shared_ptr<RSUIDirector> rsUiDirector, std::shared_ptr<RSTransactionHandler> transaction,
    std::shared_ptr<RSModifierManager> rsModifierManager)
{
    // duration 800ms
    std::cout << "Content Transition case two:duration 800ms" << std::endl;
    auto protocol = RSAnimationTimingProtocol(800);
    contentTransitionModifier->SetContentTransitionParam(ContentTransitionType::OPACITY, protocol);

    // change content to type one with animation
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_ONE);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    sleep(3);
    // change content to type two with animation
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_TWO);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    sleep(3);
    // change content to type three with animation
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_THREE);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    sleep(3);
}

void ContentTransitionTestThree(std::shared_ptr<ContentTransitionModifier> contentTransitionModifier,
    std::shared_ptr<RSUIDirector> rsUiDirector, std::shared_ptr<RSTransactionHandler> transaction,
    std::shared_ptr<RSModifierManager> rsModifierManager)
{
    std::cout << "Content Transition case three: spring curve" << std::endl;
    auto protocol = RSAnimationTimingProtocol(800);
    auto curve = RSAnimationTimingCurve::SPRING;
    contentTransitionModifier->SetContentTransitionParam(ContentTransitionType::OPACITY, protocol, curve);

    // change content to type one with animation
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_ONE);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    sleep(1);
    // change content to type two with animation
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_TWO);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    sleep(1);
    // change content to type three with animation
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_THREE);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    sleep(3);
}

void ContentTransitionTestFour(std::shared_ptr<ContentTransitionModifier> contentTransitionModifier,
    std::shared_ptr<RSUIDirector> rsUiDirector, std::shared_ptr<RSTransactionHandler> transaction,
    std::shared_ptr<RSModifierManager> rsModifierManager)
{
    // The first animation is not over yet, start the second animation
    std::cout << "Content Transition case four: The first animation is not over yet, start the second animation"
              << std::endl;
    auto protocol = RSAnimationTimingProtocol(800);
    contentTransitionModifier->SetContentTransitionParam(ContentTransitionType::OPACITY, protocol);

    // change content to type one with animation
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_ONE);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    usleep(SLEEP_TIME_US);
    // change content to type two with animation
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_TWO);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    usleep(SLEEP_TIME_US);
    // change content to type three with animation
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_THREE);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    sleep(3);
}

void ContentTransitionTestFive(std::shared_ptr<ContentTransitionModifier> contentTransitionModifier,
    std::shared_ptr<RSUIDirector> rsUiDirector, std::shared_ptr<RSTransactionHandler> transaction,
    std::shared_ptr<RSModifierManager> rsModifierManager)
{
    // The first animation is not over yet, and there is a new drawlist that is refreshed in a non-animated form
    std::cout << "Content Transition case five: The animation is interrupted by a refresh" << std::endl;
    auto protocol = RSAnimationTimingProtocol(2000);
    contentTransitionModifier->SetContentTransitionParam(ContentTransitionType::OPACITY, protocol);

    // change content to type one with animation
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_ONE);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    usleep(SLEEP_TIME_US);
    // change content to type two with no animation
    contentTransitionModifier->SetContentTransitionParam(ContentTransitionType::IDENTITY);
    contentTransitionModifier->SetContentIndex(ContentIndex::TYPE_TWO);
    rsModifierManager->AddModifier(contentTransitionModifier);
    rsUiDirector->FlushModifier();
    transaction->FlushImplicitTransaction();
    sleep(3);
}

bool StartTest()
{
    // create rsUIContext
    std::shared_ptr<RSUIDirector> rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init(true, true);
    auto uiContext = rsUiDirector->GetRSUIContext();
    if (uiContext) {
        std::cout << "RSUIDirector::Create success" << std::endl;
        std::cout << "get RSUIContext by RSUIDirector success" << std::endl;
        std::cout << "RSUIContext token is " << uiContext->GetToken() << std::endl;
    } else {
        std::cout << "RSUIContext::Create failed" << std::endl;
    }

    // create rsSurfaceNode
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "AppMain_Window";
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

    // create subNode
    std::shared_ptr<RSNode> rootNode;
    std::vector<std::shared_ptr<RSCanvasNode>> nodes;
    InitNodes(rsUiDirector, 1000, 1000, nodes, rootNode);
    surfaceNode->AttachToDisplay(screenId);

    // flush transaction
    auto transaction = uiContext->GetRSTransaction();
    if (!transaction) {
        std::cout << "!transaction" << std::endl;
        return false;
    }
    transaction->FlushImplicitTransaction();
    std::cout << "transaction success" << std::endl;
    sleep(2);

    // create contentModifier for contentTransition
    auto contentTransitionModifier = std::make_shared<ContentTransitionModifier>();
    // add modifier to node
    nodes[0]->AddModifier(contentTransitionModifier);
    auto rsModifierManager = uiContext->GetRSModifierManager();

    // TEST_CASE_1:
    ContentTransitionTestOne(contentTransitionModifier, rsUiDirector, transaction, rsModifierManager);
    // TEST_CASE_2:
    ContentTransitionTestTwo(contentTransitionModifier, rsUiDirector, transaction, rsModifierManager);
    // TEST_CASE_3:
    ContentTransitionTestThree(contentTransitionModifier, rsUiDirector, transaction, rsModifierManager);
    // TEST_CASE_4:
    ContentTransitionTestFour(contentTransitionModifier, rsUiDirector, transaction, rsModifierManager);
    // TEST_CASE_5:
    ContentTransitionTestFive(contentTransitionModifier, rsUiDirector, transaction, rsModifierManager);
    sleep(3);

    return true;
}
} // namespace

int main()
{
    StartTest();
    return 0;
}
