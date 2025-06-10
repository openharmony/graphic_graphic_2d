/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <vector>

#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkImage.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "wm/window.h"

#include "accesstoken_kit.h"
#ifdef SUPPORT_ACCESS_TOKEN
#include "nativetoken_kit.h"
#include "token_setproc.h"
#endif
#include "animation/rs_transition.h"
#include "animation/rs_transition_effect.h"
#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_property_modifier.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_display_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::Rosen;

constexpr int64_t START_NUMBER = 181154000809;
constexpr int64_t INCREASE_NUMBER = 10000000;
constexpr int64_t SLEEP_TIME = 10000;
constexpr int NUMBER_HALF = 2;
constexpr int NUMBER_TWO = 2;
constexpr int CLIP_NUMBER = 10;
constexpr int COLOR_VALUE_MAX = 255;
constexpr int WINDOW_WIDTH = 720;
constexpr int WINDOW_HEIGHT = 1280;
constexpr int ANIMATION_DURATION = 2000;
constexpr int ANIMATION_DELAY_TIME = 500;
std::shared_ptr<RSNode> rootNode;
std::vector<std::shared_ptr<RSCanvasNode>> nodes;
std::shared_ptr<RSUIDirector> rsUiDirector;

class TransitionModifier1 : public RSTransitionModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        if (!radius_ || !backgroundColor_ || !alpha_) {
            SkRect rect = SkRect::MakeXYWH(0, 0, 0, 0);
            SkPaint p;
            context.canvas->drawRect(rect, p);
            return;
        }
        context.canvas->save();
        SkPath path;
        path.addCircle(context.width / NUMBER_HALF, context.height / NUMBER_HALF,
            radius_->Get() * sqrt(pow(context.width / NUMBER_HALF, NUMBER_TWO) +
            pow(context.height / NUMBER_HALF, NUMBER_TWO)));
        context.canvas->clipPath(path);
        SkRect rect = SkRect::MakeXYWH(0, 0, context.width, context.height);
        SkPaint p;
        p.setColor(backgroundColor_->Get().AsArgbInt());
        p.setAlphaf(alpha_->Get());
        context.canvas->drawRect(rect, p);
        context.canvas->restore();
    }

    void SetRadius(float radius)
    {
        if (radius_ == nullptr) {
            radius_ = std::make_shared<RSAnimatableProperty<float>>(radius);
            AttachProperty(radius_);
        } else {
            radius_->Set(radius);
        }
    }

    void SetBackgroundColor(Color backgroundColor)
    {
        if (backgroundColor_ == nullptr) {
            backgroundColor_ = std::make_shared<RSAnimatableProperty<Color>>(backgroundColor);
            AttachProperty(backgroundColor_);
        } else {
            backgroundColor_->Set(backgroundColor);
        }
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

    void Active() override
    {
        // should set property of before transition-in
        SetRadius(0);
        SetBackgroundColor(Color(COLOR_VALUE_MAX, 0, 0));
        SetAlpha(0);
    }

    void Identity() override
    {
        // should set property of after transition-in
        SetRadius(1);
        SetBackgroundColor(Color(0, 0, COLOR_VALUE_MAX));
        SetAlpha(1);
    }

private:
    std::shared_ptr<RSAnimatableProperty<float>> radius_;
    std::shared_ptr<RSAnimatableProperty<Color>> backgroundColor_;
    std::shared_ptr<RSAnimatableProperty<float>> alpha_;
};

class TransitionModifier2 : public RSTransitionModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        float radius = 0;
        if (radius_) {
            radius = radius_->Get();
        }
        context.canvas->save();
        SkPath path;
        path.addCircle(0, 0, radius * sqrt(pow(context.width, NUMBER_TWO) + pow(context.height, NUMBER_TWO)));
        context.canvas->clipPath(path);
        SkRect rect = SkRect::MakeXYWH(0, 0, context.width, context.height);
        SkPaint p;
        p.setColor(backgroundColor_.AsArgbInt());
        context.canvas->drawRect(rect, p);
        context.canvas->restore();
    }

    void SetRadius(float radius)
    {
        if (radius_ == nullptr) {
            radius_ = std::make_shared<RSAnimatableProperty<float>>(radius);
            AttachProperty(radius_);
        } else {
            radius_->Set(radius);
        }
    }

    void SetBackgroundColor(Color backgroundColor)
    {
        backgroundColor_ = backgroundColor;
    }

    void Active() override
    {
        // should set property of before transition-in
        SetRadius(0);
    }

    void Identity() override
    {
        // should set property of after transition-in
        SetRadius(1);
    }

private:
    std::shared_ptr<RSAnimatableProperty<float>> radius_;
    Color backgroundColor_ = Color(COLOR_VALUE_MAX, 0, 0);
};

class TransitionModifier3 : public RSTransitionModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        float clipWidth = 0;
        if (clipWidth_) {
            clipWidth = clipWidth_->Get();
        }
        for (int i = 0; i < clipPieces_; i++) {
            context.canvas->save();
            SkRect clipRect = SkRect::MakeXYWH(context.width / clipPieces_ * i, 0,
                clipWidth * context.width / clipPieces_, context.height);
            context.canvas->clipRect(clipRect);
            SkRect rect = SkRect::MakeXYWH(0, 0, context.width, context.height);
            SkPaint p;
            p.setColor(backgroundColor_.AsArgbInt());
            context.canvas->drawRect(rect, p);
            context.canvas->restore();
        }
    }

    void SetClipWidth(float clipWidth)
    {
        if (clipWidth_ == nullptr) {
            clipWidth_ = std::make_shared<RSAnimatableProperty<float>>(clipWidth);
            AttachProperty(clipWidth_);
        } else {
            clipWidth_->Set(clipWidth);
        }
    }

    void SetBackgroundColor(Color backgroundColor)
    {
        backgroundColor_ = backgroundColor;
    }

    void Active() override
    {
        // should set property of before transition-in
        SetClipWidth(0);
    }

    void Identity() override
    {
        // should set property of after transition-in
        SetClipWidth(1);
    }

private:
    std::shared_ptr<RSAnimatableProperty<float>> clipWidth_;
    Color backgroundColor_ = Color(COLOR_VALUE_MAX, 0, 0);
    int clipPieces_ = CLIP_NUMBER;
};

class TransitionModifier4 : public RSTransitionModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        float clipWidth = 0;
        if (clipWidth_) {
            clipWidth = clipWidth_->Get();
        }
        for (int i = 0; i < clipPieces_; i++) {
            context.canvas->save();
            SkRect clipRect = SkRect::MakeXYWH(
                context.width / clipPieces_ * (i + 1) - clipWidth * context.width / clipPieces_, 0,
                clipWidth * context.width / clipPieces_, context.height);
            context.canvas->clipRect(clipRect);
            SkRect rect = SkRect::MakeXYWH(0, 0, context.width, context.height);
            SkPaint p;
            p.setColor(backgroundColor_.AsArgbInt());
            context.canvas->drawRect(rect, p);
            context.canvas->restore();
        }
    }

    void SetClipWidth(float clipWidth)
    {
        if (clipWidth_ == nullptr) {
            clipWidth_ = std::make_shared<RSAnimatableProperty<float>>(clipWidth);
            AttachProperty(clipWidth_);
        } else {
            clipWidth_->Set(clipWidth);
        }
    }

    void SetBackgroundColor(Color backgroundColor)
    {
        backgroundColor_ = backgroundColor;
    }

    void Active() override
    {
        // should set property of before transition-in
        SetClipWidth(0);
    }

    void Identity() override
    {
        // should set property of after transition-in
        SetClipWidth(1);
    }

private:
    std::shared_ptr<RSAnimatableProperty<float>> clipWidth_;
    Color backgroundColor_ = Color(COLOR_VALUE_MAX, 0, 0);
    int clipPieces_ = CLIP_NUMBER;
};

class TransitionModifier5 : public RSTransitionModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        float clipHeight = 0;
        if (clipHeight_) {
            clipHeight = clipHeight_->Get();
        }
        for (int i = 0; i < clipPieces_; i++) {
            context.canvas->save();
            SkRect clipRect = SkRect::MakeXYWH(0, context.height / clipPieces_ * i,
                context.width, clipHeight * context.height / clipPieces_);
            context.canvas->clipRect(clipRect);
            SkRect rect = SkRect::MakeXYWH(0, 0, context.width, context.height);
            SkPaint p;
            p.setColor(backgroundColor_.AsArgbInt());
            context.canvas->drawRect(rect, p);
            context.canvas->restore();
        }
    }

    void SetClipHeight(float clipHeight)
    {
        if (clipHeight_ == nullptr) {
            clipHeight_ = std::make_shared<RSAnimatableProperty<float>>(clipHeight);
            AttachProperty(clipHeight_);
        } else {
            clipHeight_->Set(clipHeight);
        }
    }

    void SetBackgroundColor(Color backgroundColor)
    {
        backgroundColor_ = backgroundColor;
    }

    void Active() override
    {
        // should set property of before transition-in
        SetClipHeight(0);
    }

    void Identity() override
    {
        // should set property of after transition-in
        SetClipHeight(1);
    }

private:
    std::shared_ptr<RSAnimatableProperty<float>> clipHeight_;
    Color backgroundColor_ = Color(COLOR_VALUE_MAX, 0, 0);
    int clipPieces_ = CLIP_NUMBER;
};

void Transition1()
{
    nodes.emplace_back(RSCanvasNode::Create());
    nodes[0]->SetBounds({ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});
    // define custom modifier for transition
    auto transitionModifier1 = std::make_shared<TransitionModifier1>();
    auto transitionModifier2 = std::make_shared<TransitionModifier2>();
    transitionModifier2->SetBackgroundColor(Color(0, 0, COLOR_VALUE_MAX));
    // add the modifier to node
    nodes[0]->AddModifier(transitionModifier1);
    nodes[0]->AddModifier(transitionModifier2);
    // create transition effect
    auto transitionInEffect = RSTransitionEffect::Create()->Custom(transitionModifier1);
    auto transitionOutEffect = RSTransitionEffect::Create()->Custom(transitionModifier2);
    nodes[0]->SetTransitionEffect(RSTransitionEffect::Asymmetric(transitionInEffect, transitionOutEffect));
    // define animation protocol
    RSAnimationTimingProtocol protocol;
    // set duration as 2000 millisecond
    protocol.SetDuration(ANIMATION_DURATION);
    // create transition-in animation
    RSNode::Animate(protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
        rootNode->AddChild(nodes[0]);
    }, []() {
        std::cout << "nodes0 appears" << std::endl;
    });
    uint64_t timeStamp = START_NUMBER;
    bool hasRunningAnimation = true;
    while (hasRunningAnimation) {
        hasRunningAnimation = rsUiDirector->FlushAnimation(timeStamp);
        rsUiDirector->FlushModifier();
        rsUiDirector->SendMessages();
        timeStamp += INCREASE_NUMBER;
        usleep(SLEEP_TIME);
    }
    nodes[0]->RemoveModifier(transitionModifier1);
}

void Transition2()
{
    nodes.emplace_back(RSCanvasNode::Create());
    nodes[1]->SetBounds({ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});
    // define custom modifier for transition
    auto transitionModifier2 = std::make_shared<TransitionModifier2>();
    auto transitionModifier3 = std::make_shared<TransitionModifier3>();
    transitionModifier2->SetBackgroundColor(Color(0, COLOR_VALUE_MAX, 0));
    transitionModifier3->SetBackgroundColor(Color(0, COLOR_VALUE_MAX, 0));
    // add the modifier to node
    nodes[1]->AddModifier(transitionModifier2);
    nodes[1]->AddModifier(transitionModifier3);
    // create transition effect
    auto transitionInEffect = RSTransitionEffect::Create()->Custom(transitionModifier2);
    auto transitionOutEffect = RSTransitionEffect::Create()->Custom(transitionModifier3);
    nodes[1]->SetTransitionEffect(RSTransitionEffect::Asymmetric(transitionInEffect, transitionOutEffect));
    // define animation protocol
    RSAnimationTimingProtocol protocol;
    // set duration as 2000 millisecond
    protocol.SetDuration(ANIMATION_DURATION);
    // create transition-out animation
    RSNode::Animate(protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
        rootNode->RemoveChild(nodes[0]);
    }, []() {
        std::cout << "nodes0 disappears" << std::endl;
    });
    protocol.SetStartDelay(ANIMATION_DELAY_TIME);
    // create transition-in animation
    RSNode::Animate(protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
        rootNode->AddChild(nodes[1]);
    }, []() {
        std::cout << "nodes1 appears" << std::endl;
    });
    uint64_t timeStamp = START_NUMBER;
    bool hasRunningAnimation = true;
    while (hasRunningAnimation) {
        hasRunningAnimation = rsUiDirector->FlushAnimation(timeStamp);
        rsUiDirector->FlushModifier();
        rsUiDirector->SendMessages();
        timeStamp += INCREASE_NUMBER;
        usleep(SLEEP_TIME);
    }
    nodes[1]->RemoveModifier(transitionModifier2);
}

void Transition3()
{
    nodes.emplace_back(RSCanvasNode::Create());
    nodes[NUMBER_TWO]->SetBounds({ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});
    // define custom modifier for transition
    auto transitionModifier4 = std::make_shared<TransitionModifier4>();
    auto transitionModifier5 = std::make_shared<TransitionModifier5>();
    transitionModifier4->SetBackgroundColor(Color(0, COLOR_VALUE_MAX, COLOR_VALUE_MAX));
    transitionModifier5->SetBackgroundColor(Color(0, COLOR_VALUE_MAX, COLOR_VALUE_MAX));
    // add the modifier to node
    nodes[NUMBER_TWO]->AddModifier(transitionModifier4);
    nodes[NUMBER_TWO]->AddModifier(transitionModifier5);
    // create transition effect
    auto transitionInEffect = RSTransitionEffect::Create()->Custom(transitionModifier4);
    auto transitionOutEffect = RSTransitionEffect::Create()->Custom(transitionModifier5);
    nodes[NUMBER_TWO]->SetTransitionEffect(RSTransitionEffect::Asymmetric(transitionInEffect, transitionOutEffect));
    // define animation protocol
    RSAnimationTimingProtocol protocol;
    // set duration as 2000 millisecond
    protocol.SetDuration(ANIMATION_DURATION);
    // create transition-out animation
    RSNode::Animate(protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
        rootNode->RemoveChild(nodes[1]);
    }, []() {
        std::cout << "nodes1 disappears" << std::endl;
    });
    // create transition-in animation
    RSNode::Animate(protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
        rootNode->AddChild(nodes[NUMBER_TWO]);
    }, []() {
        std::cout << "nodes2 appears" << std::endl;
    });
    uint64_t timeStamp = START_NUMBER;
    bool hasRunningAnimation = true;
    while (hasRunningAnimation) {
        hasRunningAnimation = rsUiDirector->FlushAnimation(timeStamp);
        rsUiDirector->FlushModifier();
        rsUiDirector->SendMessages();
        timeStamp += INCREASE_NUMBER;
        usleep(SLEEP_TIME);
    }
    nodes[NUMBER_TWO]->RemoveModifier(transitionModifier4);
}

void Transition4()
{
    // define animation protocol
    RSAnimationTimingProtocol protocol;
    // set duration as 2000 millisecond
    protocol.SetDuration(ANIMATION_DURATION);
    // create transition-out animation
    RSNode::Animate(protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
        rootNode->RemoveChild(nodes[NUMBER_TWO]);
    }, []() {
        std::cout << "nodes2 disappears" << std::endl;
    });
    uint64_t timeStamp = START_NUMBER;
    bool hasRunningAnimation = true;
    while (hasRunningAnimation) {
        hasRunningAnimation = rsUiDirector->FlushAnimation(timeStamp);
        rsUiDirector->FlushModifier();
        rsUiDirector->SendMessages();
        timeStamp += INCREASE_NUMBER;
        usleep(SLEEP_TIME);
    }
}

void InitNativeTokenInfo()
{
#ifdef SUPPORT_ACCESS_TOKEN
    uint64_t tokenId;
    const char *perms[1];
    perms[0] = "ohos.permission.SYSTEM_FLOAT_WINDOW";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "render_service_client_transition_demo",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    std::cout << "init native token for float window" << std::endl;
#endif
}

int main()
{
#ifdef SUPPORT_ACCESS_TOKEN
    std::cout << "transition demo start!" << std::endl;
    InitNativeTokenInfo();
    // create window
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT });
    auto window = Window::Create("transition", option);
    window->Show();
    sleep(1);
    auto rect = window->GetRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        std::cout << "create window failed: " << rect.width_ << " " << rect.height_ << std::endl;
        window->Hide();
        window->Destroy();
        window = Window::Create("transition_demo", option);
        window->Show();
        sleep(1);
        rect = window->GetRect();
    }
    std::cout << "create window " << rect.width_ << " " << rect.height_ << std::endl;
    auto surfaceNode = window->GetSurfaceNode();

    std::cout << "init rosen backend!" << std::endl;

    // init rosen backend
    rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    rsUiDirector->SetRSSurfaceNode(surfaceNode);

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, rect.width_, rect.height_);
    rootNode->SetFrame(0, 0, rect.width_, rect.height_);
    rootNode->SetBackgroundColor(SK_ColorWHITE);
    rsUiDirector->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());

    std::cout << "nodes[0] appearing." << std::endl;
    Transition1();
    sleep(NUMBER_TWO);
    std::cout << "nodes[0] disappearing and nodes[1] appearing." << std::endl;
    Transition2();
    sleep(NUMBER_TWO);
    std::cout << "nodes[1] disappearing and nodes[2] appearing." << std::endl;
    Transition3();
    sleep(NUMBER_TWO);
    std::cout << "nodes[2] disappearing." << std::endl;
    Transition4();
    sleep(NUMBER_TWO);

    std::cout << "transition demo end!" << std::endl;
    window->Hide();
    window->Destroy();
#endif
    return 0;
}
