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

#include <event_handler.h>
#include <iostream>
#include <surface.h>

#include "wm/window.h"

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
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_YELLOW);

    nodes.emplace_back(RSCanvasNode::Create());
    nodes[0]->SetBounds(0, 0, 100, 100);
    nodes[0]->SetFrame(0, 0, 100, 100);
    nodes[0]->SetBackgroundColor(Drawing::Color::COLOR_BLUE);

    rootNode->AddChild(nodes[0], -1);

    nodes.emplace_back(RSCanvasNode::Create());
    nodes[1]->SetBounds(0, 200, 200, 200);
    nodes[1]->SetFrame(0, 200, 200, 200);
    nodes[1]->SetBackgroundColor(Drawing::Color::COLOR_BLUE);

    rootNode->AddChild(nodes[0], -1);
    rootNode->AddChild(nodes[1], -1);
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
    MyModifier() = default;
    ~MyModifier() = default;
    void Draw(RSDrawingContext& context) const override
    {
        Drawing::Bitmap bitmap;
        bitmap.Build(100, 100, Drawing::BitmapFormat {
            Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_PREMUL});
        Drawing::Surface surface;
        surface.Bind(bitmap);
        auto tempCanvas = surface.GetCanvas();
        tempCanvas->Clear(0xffff3f7f);

        Drawing::Brush tempBrush;
        tempBrush.SetColor(Drawing::Color(0xff3fff7f));
        tempCanvas->AttachBrush(tempBrush);
        tempCanvas->DrawRect(Drawing::Rect(0, 0, 50, 50));

        tempBrush.SetColor(Drawing::Color(0xffff3f7f));
        tempCanvas->AttachBrush(tempBrush);
        tempCanvas->DrawRect(Drawing::Rect(50, 50, 100, 100));
        tempCanvas->DetachBrush();

        auto image = surface.GetImageSnapshot();
        if (image == nullptr) {
            return;
        }
        Drawing::SamplingOptions sampling;
        Drawing::Matrix matrix;
        Drawing::Brush brush;
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, Drawing::TileMode::REPEAT, Drawing::TileMode::REPEAT, sampling, matrix));
        auto animatableProperty = std::static_pointer_cast<RSAnimatableProperty<MyData>>(property_);
        brush.SetAlphaF(animatableProperty->Get().data);

        std::cout << "MyModifier Draw property get  " << animatableProperty->Get().data << std::endl;
        context.canvas->AttachBrush(brush);
        context.canvas->DrawRect(Drawing::Rect(0, 0, context.width, context.height));
        context.canvas->DetachBrush();
    }
};

class TransModifier : public RSGeometryTransModifier {
public:
    TransModifier() = default;
    ~TransModifier() = default;

    Drawing::Matrix GeometryEffect(float width, float height) const override
    {
        Drawing::Matrix matrix;
        if (distance_) {
            matrix.PreTranslate(distance_->Get(), distance_->Get());
            std::cout << "TransModifier GeometryEffect, distance:"<< distance_->Get() << std::endl;
        }

        return matrix;
    }

    void SetTrans(float distance)
    {
        if (distance_ == nullptr) {
            distance_ = std::make_shared<RSAnimatableProperty<float>>(distance);
            AttachProperty(distance_);
        } else {
            distance_->Set(distance);
        }
    }
private:
    std::shared_ptr<RSAnimatableProperty<float>> distance_;
};

class CustomModifier : public RSContentStyleModifier {
public:
    CustomModifier() = default;
    ~CustomModifier() = default;

    void Draw(RSDrawingContext& context) const override
    {
        if (!alpha_ || !width_ || !height_ || !backgroundColor_) {
            Drawing::Rect rect;
            Drawing::Brush brush;
            context.canvas->AttachBrush(brush);
            context.canvas->DrawRect(rect);
            context.canvas->DetachBrush();
            return;
        }
        Drawing::Rect rect(0, 0, width_->Get(), height_->Get());
        Drawing::Brush brush;
        brush.SetColor(backgroundColor_->Get().AsArgbInt());
        brush.SetAlphaF(alpha_->Get());
        context.canvas->AttachBrush(brush);
        context.canvas->DrawRect(rect);
        context.canvas->DetachBrush();

        std::cout << "Draw Get alpha_ " << alpha_->Get() << std::endl;
        std::cout << "Draw Get width_ " << width_->Get() << std::endl;
        std::cout << "Draw Get height_ " << height_->Get() << std::endl;
        std::cout << "Draw Get backgroundColor_ " << std::hex << backgroundColor_->Get().AsArgbInt() << std::endl;
    }

    void SetAlpha(float alpha)
    {
        if (alpha_ == nullptr) {
            alpha_ = std::make_shared<RSAnimatableProperty<float>>(alpha);
            AttachProperty(alpha_);
        } else {
            alpha_->Set(alpha);
        }
    }

    void SetWidth(float width)
    {
        if (width_ == nullptr) {
            width_ = std::make_shared<RSAnimatableProperty<float>>(width);
            AttachProperty(width_);
        } else {
            width_->Set(width);
        }
    }

    void SetHeight(float height)
    {
        if (height_ == nullptr) {
            height_ = std::make_shared<RSAnimatableProperty<float>>(height);
            AttachProperty(height_);
        } else {
            height_->Set(height);
        }
    }

    void SetBackgroundColor(Color color)
    {
        if (backgroundColor_ == nullptr) {
            backgroundColor_ = std::make_shared<RSAnimatableProperty<Color>>(color);
            AttachProperty(backgroundColor_);
        } else {
            backgroundColor_->Set(color);
        }
    }

private:
    std::shared_ptr<RSAnimatableProperty<float>> alpha_;
    std::shared_ptr<RSAnimatableProperty<float>> width_;
    std::shared_ptr<RSAnimatableProperty<float>> height_;
    std::shared_ptr<RSAnimatableProperty<Color>> backgroundColor_;
};

class NodeModifier : public RSNodeModifier {
public:
    NodeModifier() = default;
    virtual ~NodeModifier() = default;

    void Modify(RSNode& target) const override
    {
        target.SetAlpha(alpha_->Get());
        target.SetScale(scale_->Get());
        target.SetBackgroundColor(color_->Get().AsArgbInt());
    }

    void SetAlpha(float alpha)
    {
        if (alpha_ == nullptr) {
            alpha_ = std::make_shared<RSAnimatableProperty<float>>(alpha);
            AttachProperty(alpha_);
        } else {
            alpha_->Set(alpha);
        }
    }

    void SetScale(Vector2f scale)
    {
        if (scale_ == nullptr) {
            scale_ = std::make_shared<RSAnimatableProperty<Vector2f>>(scale);
            AttachProperty(scale_);
        } else {
            scale_->Set(scale);
        }
    }

    void SetColor(Color color)
    {
        if (color_ == nullptr) {
            color_ = std::make_shared<RSAnimatableProperty<Color>>(color);
            AttachProperty(color_);
        } else {
            color_->Set(color);
        }
    }

private:
    std::shared_ptr<RSAnimatableProperty<float>> alpha_;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scale_;
    std::shared_ptr<RSAnimatableProperty<Color>> color_;
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

    // change property in nodes [setter using modifier]
    std::cout << "rs app demo stage " << cnt++ << std::endl;
    nodes[0]->SetBounds(0, 0, 200, 200);
    nodes[0]->SetFrame(0, 0, 200, 200);
    nodes[0]->SetBorderColor(Drawing::Color::COLOR_BLACK);
    nodes[0]->SetBorderWidth(10);
    nodes[0]->SetBorderStyle((uint32_t)BorderStyle::SOLID);
    rsUiDirector->SendMessages();
    sleep(1);

    std::cout << "rs app demo stage " << cnt++ << std::endl;

    // multi-property modifier
    auto customModifier = std::make_shared<CustomModifier>();
    // add modifier to node
    nodes[0]->AddModifier(customModifier);
    // init property
    customModifier->SetAlpha(0);
    customModifier->SetWidth(0);
    customModifier->SetHeight(0);
    customModifier->SetBackgroundColor(Color(0, 0, 255));

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(3000);

    // create property animation
    RSNode::Animate(protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
        customModifier->SetAlpha(0.8);
        customModifier->SetWidth(720);
        customModifier->SetHeight(1280);
        customModifier->SetBackgroundColor(Color(255, 0, 0));
    }, []() {
        std::cout<<"custom animation 1 finish callback"<<std::endl;
    });

    int64_t startNum = 80825861106;
    bool hasRunningAnimation = true;
    while (hasRunningAnimation) {
        hasRunningAnimation = rsUiDirector->FlushAnimation(startNum);
        rsUiDirector->FlushModifier();
        rsUiDirector->SendMessages();
        startNum += 100000000;
        usleep(100000);
    }
    sleep(2);

    auto nodeModifier = std::make_shared<NodeModifier>();
    nodes[1]->AddModifier(nodeModifier);
    nodeModifier->SetAlpha(1);
    nodeModifier->SetScale(Vector2f(1.f, 1.f));
    nodeModifier->SetColor(Color(0, 255, 0));
    rsUiDirector->FlushAnimation(0);
    rsUiDirector->FlushModifier();
    rsUiDirector->SendMessages();
    sleep(3);

    // create property animation
    RSNode::Animate(protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
        nodeModifier->SetAlpha(0.2);
        nodeModifier->SetScale(Vector2f(3.f, 3.f));
        nodeModifier->SetColor(Color(255, 0, 255));
    }, []() {
        std::cout<<"custom animation 2 finish callback"<<std::endl;
    });

    hasRunningAnimation = true;
    while (hasRunningAnimation) {
        hasRunningAnimation = rsUiDirector->FlushAnimation(startNum);
        rsUiDirector->FlushModifier();
        rsUiDirector->SendMessages();
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
