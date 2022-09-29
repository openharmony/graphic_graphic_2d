/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include <surface.h>

#include "include/core/SkBitmap.h"
#include "include/core/SkColor.h"
#include "include/core/SkImage.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkTileMode.h"
#include "modifier/rs_extended_modifier.h"
#include "wm/window.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
#include "render/rs_border.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_root_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"
#include "animation/rs_ui_animation_manager.h"
#include "modifier/rs_property_modifier.h"

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
    rootNode->SetBackgroundColor(SK_ColorRED);

    nodes.emplace_back(RSCanvasNode::Create());
    nodes[0]->SetBounds(100, 100, 300, 200);
    nodes[0]->SetFrame(100, 100, 300, 200);
    nodes[0]->SetBackgroundColor(SK_ColorBLUE);

    rootNode->AddChild(nodes[0], -1);
    rsUiDirector->SetRoot(rootNode->GetId());
}

class MyData : public RSAnimatableArithmetic<MyData> {
public:
    MyData() : data(0.f) {}
    explicit MyData(const float num) : data(num) {}
    virtual ~MyData() = default;

    MyData Add(const MyData& value) const override
    {
        float res = data + value.data;
        return MyData(res);
    }
    MyData Minus(const MyData& value) const override
    {
        float res = data - value.data;
        return MyData(res);
    }
    MyData Multiply(const float scale) const override
    {
        float res = data * scale;
        return MyData(res);
    }
    bool IsEqual(const MyData& value) const override
    {
        return data == value.data;
    }

    float data;
};

class MyModifier : public RSOverlayStyleModifier {
public:
    explicit MyModifier(const std::shared_ptr<RSPropertyBase> property)
        : RSOverlayStyleModifier(property) {}
    virtual ~MyModifier() = default;
    void Draw(RSDrawingContext& context) const override
    {
        SkBitmap bitmap;
        bitmap.allocN32Pixels(100, 100);
        bitmap.eraseColor(0xffff7f3f);
        bitmap.erase(0xff3fff7f, SkIRect::MakeWH(50, 50));
        bitmap.erase(0xffff3f7f, SkIRect::MakeXYWH(50, 50, 50, 50));
        SkPaint p;
        p.setShader(bitmap.makeShader(SkTileMode::kRepeat, SkTileMode::kRepeat));
        auto animatableProperty = std::static_pointer_cast<RSAnimatableProperty<MyData>>(property_);
        p.setAlphaf(animatableProperty->Get().data);
        std::cout<<"MyModifier Draw property get  "<<animatableProperty->Get().data<<std::endl;
        context.canvas->drawRect(SkRect::MakeWH(context.width, context.height), p);
    }
};

int main()
{
    // Init demo env
    int cnt = 0;
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
    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    std::cout << "rs app demo stage " << cnt++ << std::endl;

    // change property in nodes [setter using modifier]
    rootNode->SetBackgroundColor(SK_ColorYELLOW);
    std::cout << "rs app demo stage " << cnt++ << std::endl;
    nodes[0]->SetBounds({ 0.0f, 0.0f, 100.f, 100.f });
    rsUiDirector->SendMessages();
    sleep(2);

    nodes[0]->SetFrame(100, 300, 600, 800);
    nodes[0]->SetBorderColor(SK_ColorBLACK);
    nodes[0]->SetBorderWidth(20.f);
    nodes[0]->SetBorderStyle((uint32_t)BorderStyle::SOLID);

    // directly add modifier
    nodes[0]->SetPivot(0.f, 0.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(Vector4f(100, 100, 600, 800));
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    nodes[0]->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(1);

    std::cout << "rs app demo stage " << cnt++ << std::endl;

    // add custom drawing modifier
    auto mydata = MyData(1.0f);
    auto myproperty = std::make_shared<RSAnimatableProperty<MyData>>(mydata);
    auto mymodifier = std::make_shared<MyModifier>(myproperty);
    nodes[0]->AddModifier(mymodifier);

    rsUiDirector->SendMessages();
    sleep(3);

    // custom drawing modifier with anim
    std::cout << "rs app demo stage " << cnt++ << std::endl;
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(5000);
    RSNode::OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, []() {
        std::cout<<"animation finished"<<endl;
    });
    auto mydata2 = MyData(0.1f);
    myproperty->Set(mydata2);
    RSNode::CloseImplicitAnimation();
    rsUiDirector->SendMessages();

    int64_t startNum = 80825861106;
    bool hasRunningAnimation = true;
    while (hasRunningAnimation) {
        hasRunningAnimation = rsUiDirector->RunningCustomAnimation(startNum);
        rsUiDirector->SendMessages();
        startNum += 100000000;
        usleep(100000);
    }

    // dump property via modifiers
    std::cout << "rs app demo stage " << cnt++ << std::endl;
    std::cout << "print: --------- " << nodes[0]->GetStagingProperties().Dump() << std::endl;
    sleep(3);

    std::cout << "rs app demo end!" << std::endl;
    window->Hide();
    window->Destroy();
    return 0;
}
