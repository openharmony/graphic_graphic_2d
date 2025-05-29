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

#include "dm/display_manager.h"
#include "event_handler.h"
#include "rs_trace.h"
#include "securec.h"

#include "draw/canvas.h"
#include "image/bitmap.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;
uint64_t screenId = 0;
const Rosen::RSAnimationTimingCurve ANIMATION_TIMING_CURVE =
    Rosen::RSAnimationTimingCurve::CreateCubicCurve(0.2f, 0.0f, 0.2f, 1.0f);

namespace {

void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height,
    std::vector<std::shared_ptr<RSCanvasNode>>& nodes, std::shared_ptr<RSNode>& rootNode)
{
    std::cout << "rs multi instance demo Init Rosen Backend!" << std::endl;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    rootNode = RSRootNode::Create(false, false, rsUIContext);
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_YELLOW);

    nodes.emplace_back(RSCanvasNode::Create(false, false, rsUIContext));
    nodes[0]->SetBounds(0, 0, 100, 100);
    nodes[0]->SetFrame(0, 0, 100, 100);
    nodes[0]->SetBackgroundColor(Drawing::Color::COLOR_BLUE);

    rootNode->AddChild(nodes[0], -1);

    nodes.emplace_back(RSCanvasNode::Create(false, false, rsUIContext));
    nodes[1]->SetBounds(0, 200, 100, 100);
    nodes[1]->SetFrame(0, 200, 100, 100);
    nodes[1]->SetBackgroundColor(Drawing::Color::COLOR_GREEN);

    rootNode->AddChild(nodes[0], -1);
    rootNode->AddChild(nodes[1], -1);
    rsUiDirector->SetRoot(rootNode->GetId());
}

class SubThread {
public:
    SubThread(uint32_t threadIndex) : threadIndex_(threadIndex) {}
    ~SubThread() = default;
    void Start()
    {
        std::string name = "SubThread" + std::to_string(threadIndex_);
        runner_ = AppExecFwk::EventRunner::Create(name);
        handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    }
    void PostTask(const std::function<void()>& task, const std::string& name = std::string())
    {
        if (handler_) {
            handler_->PostImmediateTask(task, name);
        }
    }

private:
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    uint32_t threadIndex_ = 0;
};

bool InitSurface()
{
    SubThread subThread1 = SubThread(1);
    SubThread subThread2 = SubThread(2);
    subThread1.Start();
    subThread2.Start();
    std::shared_ptr<RSUIDirector> rsUiDirector;
    std::shared_ptr<RSUIDirector> rsUiDirector2;
    std::shared_ptr<RSNode> rootNode;
    std::vector<std::shared_ptr<RSCanvasNode>> nodes;
    std::shared_ptr<RSSurfaceNode> surfaceNode;
    // stage 1
    std::function<void()> task = [&rsUiDirector, &nodes, &rootNode, &surfaceNode]() {
        std::cout << "Stage 1" << std::endl;
        rsUiDirector = RSUIDirector::Create();
        rsUiDirector->Init(true, true);
        auto uiContext = rsUiDirector->GetRSUIContext();
        if (uiContext) {
            std::cout << "stage 1 RSUIDirector::Create success" << std::endl;
            std::cout << "stage 1 get RSUIContext by RSUIDirector success" << std::endl;
            std::cout << "stage 1 RSUIContext token is " << uiContext->GetToken() << std::endl;
        } else {
            std::cout << "RSUIContext::Create failed" << std::endl;
        }
        RSSurfaceNodeConfig surfaceNodeConfig;
        surfaceNodeConfig.SurfaceNodeName = "AppMain_Window";
        RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
        std::cout << "RSSurfaceNode::Create" << std::endl;
        surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType, true, false, uiContext);
        std::cout << "Create node by RSUIContext success" << std::endl;
        surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
        surfaceNode->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
        surfaceNode->SetBounds(0, 0, 600, 600);
        surfaceNode->SetBackgroundColor(SK_ColorRED);
        rsUiDirector->SetRSSurfaceNode(surfaceNode);
        Init(rsUiDirector, 300, 300, nodes, rootNode);
        surfaceNode->AttachToDisplay(screenId);
        auto transaction = uiContext->GetRSTransaction();
        if (!transaction) {
            std::cout << "!transaction" << std::endl;
            return;
        }
        transaction->FlushImplicitTransaction();
        std::cout << "find node by nodeMap" << std::endl;
        auto node11 = RSNodeMap::Instance().GetNode(rootNode->GetId());
        std::string result = !!node11 ? "true" : "false";
        std::cout << "find node by global nodeMap result is " << result << std::endl;
        auto node32 = uiContext->GetNodeMap().GetNode(rootNode->GetId());
        result = !!node32 ? "true" : "false";
        std::cout << "find node by nodeMapnew result is " << result << std::endl;
        // change property in nodes [setter using modifier]
        sleep(2);
        std::cout << "change Node properties " << std::endl;
        nodes[0]->SetBounds(0, 0, 200, 200);
        nodes[0]->SetFrame(0, 0, 200, 200);
        nodes[0]->SetBorderColor(Drawing::Color::COLOR_BLACK);
        nodes[0]->SetBorderWidth(10);
        nodes[0]->SetBorderStyle((uint32_t)BorderStyle::SOLID);
        std::cout << "send message by local transaction" << std::endl;
        transaction->FlushImplicitTransaction();
        sleep(3);
        std::cout << "window1 animation start" << std::endl;
        auto propertyCallback = [&]() {
            std::cout << "window1 animation1 " << std::endl;
            nodes[0]->SetBounds(100, 0, 100, 100);
            nodes[0]->SetFrame(100, 0, 100, 100);
            nodes[0]->SetBackgroundColor(Drawing::Color::COLOR_RED);
        };
        auto finishCallback = [&]() {};
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(1000);
        RSNode::Animate(uiContext, protocol, ANIMATION_TIMING_CURVE, propertyCallback, finishCallback);
        transaction->FlushImplicitTransaction();
        sleep(6);
        auto propertyCallback2 = [&]() {
            std::cout << "window1 animation2" << std::endl;
            nodes[0]->SetBounds(0, 0, 200, 200);
            nodes[0]->SetFrame(0, 0, 200, 200);
            nodes[0]->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
        };
        auto finishCallback2 = [&]() {};
        RSNode::Animate(uiContext, protocol, ANIMATION_TIMING_CURVE, propertyCallback2, finishCallback2);
        transaction->FlushImplicitTransaction();
        sleep(2);
        std::cout << "window1 hide" << std::endl;
        surfaceNode->SetVisible(false);
        transaction->FlushImplicitTransaction();
    };
    subThread1.PostTask(task, "stage 1");
    sleep(8);
    // stage 2
    std::shared_ptr<RSNode> rootNode2;
    std::vector<std::shared_ptr<RSCanvasNode>> nodes2;
    std::shared_ptr<RSSurfaceNode> surfaceNode2;
    std::function<void()> task2 = [&rsUiDirector2, &nodes2, &rootNode2, &surfaceNode2, &nodes, &rootNode,
                                      &rsUiDirector]() {
        std::cout << " " << std::endl;
        std::cout << "Stage 2" << std::endl;
        rsUiDirector2 = RSUIDirector::Create();
        rsUiDirector2->Init(true, true);
        auto uiContext = rsUiDirector2->GetRSUIContext();
        if (uiContext) {
            std::cout << "stage 2 RSUIDirector::Create success" << std::endl;
            std::cout << "stage 2 get RSUIContext by RSUIDirector success" << std::endl;
            std::cout << "stage 2 RSUIContext token is " << uiContext->GetToken() << std::endl;
            std::cout << "stage 2 token is not same as stage 1" << std::endl;
        } else {
            std::cout << "RSUIContext::Create failed" << std::endl;
        }
        // stage 2 find node
        std::cout << "find node in thread B" << std::endl;
        auto node32 = uiContext->GetNodeMap().GetNode(rootNode->GetId());
        std::string result = !!node32 ? "true" : "false";
        // change property by thread2
        nodes[0]->SetBounds(0, 0, 400, 200);
        nodes[0]->SetFrame(0, 0, 400, 200);
        nodes[0]->SetBorderColor(Drawing::Color::COLOR_RED);
        nodes[0]->SetBorderWidth(10);
        nodes[0]->SetBorderStyle((uint32_t)BorderStyle::SOLID);
        std::cout << "modifier property in thread 2 failed" << std::endl;
        auto transaction = uiContext->GetRSTransaction();
        transaction->FlushImplicitTransaction();
        RSSurfaceNodeConfig surfaceNodeConfig;
        surfaceNodeConfig.SurfaceNodeName = "AppMain_Window2";
        RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
        std::cout << "stage 2 RSSurfaceNode::Create" << std::endl;
        surfaceNode2 = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType, true, false, uiContext);
        surfaceNode2->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
        surfaceNode2->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
        surfaceNode2->SetBounds(700, 700, 600, 600);
        surfaceNode2->SetBackgroundColor(SK_ColorRED);
        rsUiDirector2->SetRSSurfaceNode(surfaceNode2);
        Init(rsUiDirector2, 400, 400, nodes2, rootNode2);
        surfaceNode2->AttachToDisplay(screenId);
        auto uiContext2 = rsUiDirector->GetRSUIContext();
        uiContext2->GetRSTransaction()->FlushImplicitTransaction();
        sleep(1);
        std::cout << "send Message By window1 flush failed" << std::endl;
        transaction->FlushImplicitTransaction();
        sleep(2);
        std::cout << "window2 animation start" << std::endl;
        auto propertyCallback = [&]() {
            std::cout << "window2 animation " << std::endl;
            nodes2[0]->SetBounds(100, 0, 100, 100);
            nodes2[0]->SetFrame(100, 0, 100, 100);
            nodes2[0]->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
        };
        auto finishCallback = [&]() {};
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(1000);
        RSNode::Animate(uiContext, protocol, ANIMATION_TIMING_CURVE, propertyCallback, finishCallback);
        transaction->FlushImplicitTransaction();
        sleep(3);
        auto propertyCallback2 = [&]() {
            std::cout << "window2 animation2" << std::endl;
            nodes2[0]->SetBounds(0, 0, 200, 200);
            nodes2[0]->SetFrame(0, 0, 200, 200);
            nodes2[0]->SetBackgroundColor(Drawing::Color::COLOR_RED);
        };
        auto finishCallback2 = [&]() {};
        RSNode::Animate(uiContext2, protocol, ANIMATION_TIMING_CURVE, propertyCallback2, finishCallback2);
        transaction->FlushImplicitTransaction();
        std::cout << "window2 animation2 failed because uiContext1 can not effect window2" << std::endl;
        sleep(3);
        std::cout << "window2 hide" << std::endl;
        surfaceNode2->SetVisible(false);
        transaction->FlushImplicitTransaction();
    };
    subThread2.PostTask(task2, "stage 2");
    sleep(10);

    // stage 3
    std::shared_ptr<RSNode> rootNode3;
    std::vector<std::shared_ptr<RSCanvasNode>> nodes3;
    std::shared_ptr<RSSurfaceNode> surfaceNode3;
    std::shared_ptr<RSNode> rootNode4;
    std::vector<std::shared_ptr<RSCanvasNode>> nodes4;
    std::shared_ptr<RSSurfaceNode> surfaceNode4;
    std::shared_ptr<RSUIDirector> rsUiDirector3;
    std::shared_ptr<RSUIDirector> rsUiDirector4;
    std::function<void()> task3 = [&rsUiDirector3, &rootNode3, &rootNode4, &surfaceNode3, &surfaceNode4, &nodes3,
                                      &rsUiDirector4, &nodes4] {
        std::cout << " " << std::endl;
        std::cout << "stage 3 " << std::endl;
        rsUiDirector3 = RSUIDirector::Create();
        rsUiDirector3->Init(true, true);
        auto uiContext3 = rsUiDirector3->GetRSUIContext();
        if (uiContext3) {
            std::cout << "stage 3 RSUIDirector 1 success" << std::endl;
            std::cout << "stage 3 RSUIContext token is " << uiContext3->GetToken() << std::endl;
        } else {
            std::cout << "RSUIContext::Create failed" << std::endl;
        }
        rsUiDirector4 = RSUIDirector::Create();
        rsUiDirector4->Init(true, true);
        auto uiContext4 = rsUiDirector4->GetRSUIContext();
        if (uiContext4) {
            std::cout << "stage 3 RSUIDirector 2 success" << std::endl;
            std::cout << "stage 3 RSUIContext 2 token is " << uiContext4->GetToken() << std::endl;
            std::cout << "stage 3 RsUIContext 2 token is not same as RSUIContext token" << std::endl;
        } else {
            std::cout << "RSUIContext::Create failed" << std::endl;
        }
        RSSurfaceNodeConfig surfaceNodeConfig;
        surfaceNodeConfig.SurfaceNodeName = "AppMain_Window3";
        RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
        std::cout << "stage 3 RSSurfaceNode::Create" << std::endl;
        surfaceNode3 = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType, true, false, uiContext3);
        surfaceNode3->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
        surfaceNode3->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
        surfaceNode3->SetBounds(0, 1200, 600, 600);
        surfaceNode3->SetBackgroundColor(SK_ColorRED);
        rsUiDirector3->SetRSSurfaceNode(surfaceNode3);
        Init(rsUiDirector3, 400, 400, nodes3, rootNode3);
        surfaceNode3->AttachToDisplay(screenId);
        RSSurfaceNodeConfig surfaceNodeConfig2;
        surfaceNodeConfig2.SurfaceNodeName = "AppMain_Window4";
        RSSurfaceNodeType surfaceNodeType2 = RSSurfaceNodeType::APP_WINDOW_NODE;
        surfaceNode4 = RSSurfaceNode::Create(surfaceNodeConfig2, surfaceNodeType2, true, false, uiContext4);
        surfaceNode4->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
        surfaceNode4->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
        surfaceNode4->SetBounds(900, 1200, 500, 500);
        surfaceNode4->SetBackgroundColor(SK_ColorRED);
        rsUiDirector4->SetRSSurfaceNode(surfaceNode4);
        Init(rsUiDirector4, 300, 300, nodes4, rootNode4);
        surfaceNode4->AttachToDisplay(screenId);
        auto transaction3 = uiContext3->GetRSTransaction();
        transaction3->FlushImplicitTransaction();
        std::cout << "window 4 message flush failed because uicontext3 can not flush window4" << std::endl;
        sleep(2);
        auto transaction4 = uiContext4->GetRSTransaction();
        transaction4->FlushImplicitTransaction();
        std::cout << "window 4 message flush success" << std::endl;
        sleep(2);
        auto propertyCallback = [&]() {
            std::cout << "window3 animation" << std::endl;
            nodes3[0]->SetBounds(100, 0, 100, 100);
            nodes3[0]->SetFrame(100, 0, 100, 100);
            nodes3[0]->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
        };
        auto finishCallback = [&]() {};
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(1000);
        RSNode::Animate(uiContext3, protocol, ANIMATION_TIMING_CURVE, propertyCallback, finishCallback);
        auto propertyCallback2 = [&]() {
            std::cout << "window4 animation" << std::endl;
            nodes4[0]->SetBounds(100, 0, 100, 100);
            nodes4[0]->SetFrame(100, 0, 100, 100);
            nodes4[0]->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
        };
        auto finishCallback4 = [&]() {};
        RSNode::Animate(uiContext3, protocol, ANIMATION_TIMING_CURVE, propertyCallback2, finishCallback4);
        transaction3->FlushImplicitTransaction();
        transaction4->FlushImplicitTransaction();
        std::cout << "window 4 can not do animation" << std::endl;
        sleep(2);
        nodes3[1]->AddChild(nodes4[1], -1);
        std::cout << "window 3 window 4 can't add tree" << std::endl;
        sleep(2);
        std::cout << "window3 hide" << std::endl;
        surfaceNode3->SetVisible(false);
        transaction3->FlushImplicitTransaction();
        sleep(1);
        std::cout << "window4 hide" << std::endl;
        surfaceNode4->SetVisible(false);
        transaction4->FlushImplicitTransaction();
    };
    subThread1.PostTask(task3, "stage 3");
    sleep(10);
    return true;
}
} // namespace

int main()
{
    std::cout << "rs multi instance demo start!" << std::endl;
    InitSurface();
    std::cout << "rs multi instance demo end!" << std::endl;
    return 0;
}