/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <event_handler.h>
#include <iostream>
#include <surface.h>

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "window.h"

#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_property_modifier.h"
#include "render/rs_border.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_display_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

std::shared_ptr<RSNode> rootNode;
std::vector<std::shared_ptr<RSCanvasNode>> nodes;

void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    std::cout << "rs app demo Init Rosen Backend!" << std::endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(SK_ColorYELLOW);

    nodes.emplace_back(RSCanvasNode::Create());
    nodes[0]->SetBounds(10, 10, 100, 100);
    nodes[0]->SetFrame(10, 10, 100, 100);
    nodes[0]->SetBackgroundColor(SK_ColorBLUE);

    rootNode->AddChild(nodes[0], -1);

    rootNode->AddChild(nodes[0], -1);
    rsUiDirector->SetRoot(rootNode->GetId());
}

class NodeModifier : public RSNodeModifier {
public:
    NodeModifier() = default;
    virtual ~NodeModifier() = default;

    void Modify(RSNode& target) const override
    {
        target.SetTranslate(translate_->Get());
    }

    void SetTranslate(Vector2f translate)
    {
        if (translate_ == nullptr) {
            translate_ = std::make_shared<RSAnimatableProperty<Vector2f>>(translate);
            AttachProperty(translate_);
        } else {
            translate_->Set(translate);
        }
    }

    Vector2f GetTranslate() const
    {
        return currentTranslateValue_;
    }

    bool SetTranslateCallBack()
    {
        if (!translate_) {
            return false;
        }

        // add callback for each frame
        translate_->SetUpdateCallback([&](Vector2f vec) { currentTranslateValue_ = vec; });
        return true;
    }

    Vector2f currentTranslateValue_ = Vector2f(0.0f, 0.0f);

private:
    std::shared_ptr<RSAnimatableProperty<Vector2f>> translate_;
};

int main()
{
    int cnt = 0;

    // Init demo env
    std::cout << "rs app demo start!" << std::endl;
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({ 0, 0, 720, 1280 });
    auto window = Window::Create("app_demo", option);
    window->Show();
    auto rect = window->GetRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        std::cout << "rs app demo create window failed: " << rect.width_ << " " << rect.height_ << std::endl;
        window->Hide();
        window->Destroy();
        window = Window::Create("app_demo", option);
        window->Show();
        rect = window->GetRect();
    }
    std::cout << "rs app demo create window " << rect.width_ << " " << rect.height_ << std::endl;
    auto surfaceNode = window->GetSurfaceNode();

    // Build rosen renderThread & create nodes
    std::cout << "rs app demo stage " << cnt++ << std::endl;
    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    rsUiDirector->SetUITaskRunner(
        [handler](const std::function<void()>& task, uint32_t delay) { handler->PostTask(task); });
    runner->Run();
    RSTransaction::FlushImplicitTransaction();
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);

    std::cout << "rs app demo stage " << cnt++ << std::endl;

    // simulate sliding trajecity
    int pointsNum = 30;
    vector<Vector2f> handPoint(pointsNum);
    for (int i = 0; i < handPoint.size(); i++) {
        handPoint[i].data_[0] = 0.0f;
        handPoint[i].data_[1] = 0.5f * i * i;
    }

    // simulate sliding stage
    for (auto vec : handPoint) {
        nodes[0]->SetTranslate(vec);
        rsUiDirector->SendMessages();
        usleep(30000);
    }

    // calculate sliding velocity
    auto velocity = (handPoint[pointsNum - 1] - handPoint[pointsNum - 2]) * (1.0f / (30000.0f * 1e-6));

    // calculate final points
    Vector2f finalTrans = handPoint[pointsNum - 1] + velocity * 0.5;

    std::cout << "rs app demo stage " << cnt++ << std::endl;

    auto nodeModifier = std::make_shared<NodeModifier>();
    nodes[0]->AddModifier(nodeModifier);

    // init property
    nodes[0]->SetTranslate(handPoint[pointsNum - 1]);

    RSAnimationTimingProtocol protocol;
    // duration is not effective when the curve is interpolatingSpring
    protocol.SetDuration(3000);

    auto timingCurve = RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, 225.0f, 30.0f, 2.0f);

    // create property animation
    RSNode::Animate(
        protocol, timingCurve,
        [&]() {
            std::cout << "the end tranlate of sliding animation is " << finalTrans.data_[0] << ", "
                      << finalTrans.data_[1] << std::endl;
            nodeModifier->SetTranslate(finalTrans);
        },
        []() { std::cout << "sliding animation finish callback" << std::endl; });

    int64_t startNum = 80825861106;
    int64_t interruptNum = 80825861106 * 15;

    bool hasRunningAnimation = true;
    bool stopSignal = true;

    while (hasRunningAnimation) {
        hasRunningAnimation = rsUiDirector->FlushAnimation(startNum);
        rsUiDirector->FlushModifier();
        rsUiDirector->SendMessages();
        std::cout << "the current translate is " << nodeModifier->GetTranslate().data_[0] << ", "
                  << nodeModifier->GetTranslate().data_[1] << endl;

        // simulate to stop animation by gesture
        if ((startNum >= interruptNum) && stopSignal) {
            std::cout << "stop sliding animation in current translate " << nodeModifier->GetTranslate().data_[0] << ", "
                      << nodeModifier->GetTranslate().data_[1] << endl;

            // set duration to 0
            protocol.SetDuration(0);

            // create interrupt animation
            RSNode::Animate(
                protocol, RSAnimationTimingCurve::EASE_IN_OUT,
                [&]() { nodeModifier->SetTranslate(nodeModifier->GetTranslate()); },
                [&]() {
                    std::cout << "interrupt animation finish callback, the end translate is "
                              << nodeModifier->GetTranslate().data_[0] << ", " << nodeModifier->GetTranslate().data_[1]
                              << std::endl;
                });

            stopSignal = false;
        }
        startNum += 100000000;
        usleep(100000);
    }

    // dump property via modifiers
    std::cout << "rs app demo stage " << cnt++ << std::endl;
    std::cout << nodes[0]->GetStagingProperties().Dump() << std::endl;
    std::cout << "rs app demo end!" << std::endl;
    sleep(3);

    window->Hide();
    window->Destroy();
    return 0;
}