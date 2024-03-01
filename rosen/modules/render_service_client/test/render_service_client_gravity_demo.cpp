/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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


#include "property/rs_properties_def.h"
#include "refbase.h"
#include "render_context/render_context.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "wm/window.h"

namespace OHOS {
namespace Rosen {
namespace Test {
#define RS_GRAVITY_CASE_STR(value) case value: return #value
const char* GravityString(Gravity gravity)
{
    switch (gravity) {
        RS_GRAVITY_CASE_STR(Gravity::BOTTOM);
        RS_GRAVITY_CASE_STR(Gravity::BOTTOM_LEFT);
        RS_GRAVITY_CASE_STR(Gravity::BOTTOM_RIGHT);
        RS_GRAVITY_CASE_STR(Gravity::LEFT);
        RS_GRAVITY_CASE_STR(Gravity::RIGHT);
        RS_GRAVITY_CASE_STR(Gravity::TOP_LEFT);
        RS_GRAVITY_CASE_STR(Gravity::TOP_RIGHT);
        RS_GRAVITY_CASE_STR(Gravity::CENTER);
        RS_GRAVITY_CASE_STR(Gravity::TOP);
        RS_GRAVITY_CASE_STR(Gravity::RESIZE);
        RS_GRAVITY_CASE_STR(Gravity::RESIZE_ASPECT);
        RS_GRAVITY_CASE_STR(Gravity::RESIZE_ASPECT_TOP_LEFT);
        RS_GRAVITY_CASE_STR(Gravity::RESIZE_ASPECT_BOTTOM_RIGHT);
        RS_GRAVITY_CASE_STR(Gravity::RESIZE_ASPECT_FILL);
        RS_GRAVITY_CASE_STR(Gravity::RESIZE_ASPECT_FILL_TOP_LEFT);
        RS_GRAVITY_CASE_STR(Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT);
        default: return "Unknown";
    }
}

// we can make this demo and run it on the device,
// to visualize the surface's all gravity effects in the window area,
// as its enum name indicates.
class RsSurfaceNodeGravityTestDemo {
public:
    RsSurfaceNodeGravityTestDemo()
    {
#ifdef ACE_ENABLE_GPU
        renderContext_ = std::make_unique<RenderContext>();
        renderContext_->InitializeEglContext();
#endif // ACE_ENABLE_GPU

        sptr<WindowOption> option(new WindowOption());
        option->SetDisplayId(0); // default displayId is 0
        option->SetWindowRect({0, 0, 600, 600}); // test window size is 600*600
        option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
        option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
        option->SetWindowName("gravity_demo");
        window_ = Window::Create(option->GetWindowName(), option);
        if (window_ != nullptr) {
            surfaceNode_ = window_->GetSurfaceNode();
            surfaceNode_->SetBounds(0, 0, 600, 600); // surface bounds is window rect.
            surfaceNode_->SetBackgroundColor(0xffff0000); // set background color to red.
            RSTransaction::FlushImplicitTransaction();
            window_->Show();
        } else {
            std::cout << "Failed to create window!" << std::endl;
        }

        availableGravities_ = {
            Gravity::CENTER, Gravity::TOP, Gravity::BOTTOM, Gravity::LEFT, Gravity::RIGHT, Gravity::TOP_LEFT,
            Gravity::TOP_RIGHT, Gravity::BOTTOM_LEFT, Gravity::BOTTOM_RIGHT, Gravity::RESIZE, Gravity::RESIZE_ASPECT,
            Gravity::RESIZE_ASPECT_TOP_LEFT, Gravity::RESIZE_ASPECT_BOTTOM_RIGHT, Gravity::RESIZE_ASPECT_FILL,
            Gravity::RESIZE_ASPECT_FILL_TOP_LEFT, Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT, Gravity::DEFAULT
        };
    }

    ~RsSurfaceNodeGravityTestDemo() noexcept
    {
        if (window_ != nullptr) {
            window_->Hide();
            window_->Destroy();
        }
    }

    void Run()
    {
        if (surfaceNode_ == nullptr) {
            return;
        }

        auto rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode_);
        if (rsSurface == nullptr) {
            return;
        }

#ifdef ACE_ENABLE_GPU
        if (renderContext_ != nullptr) {
            rsSurface->SetRenderContext(renderContext_.get());
        }
#endif // ACE_ENABLE_GPU

        std::size_t gravityIdx = 0;
        // test 50 frames.
        for (int i = 0; i < 50; ++i) {
            if (gravityIdx >= availableGravities_.size()) {
                gravityIdx = 0;
            }
            auto gravity = availableGravities_[gravityIdx++];
            surfaceNode_->SetFrameGravity(gravity);
            RSTransaction::FlushImplicitTransaction();

            // make the surfaceBuffer(400*400) smaller than the window size to test gravity.
            auto frame = rsSurface->RequestFrame(400, 400);
            if (frame == nullptr) {
                std::cout << "Failed to create frame!" << std::endl;
                return;
            }
            auto canvas = frame->GetCanvas();
            if (canvas == nullptr) {
                std::cout << "Failed to create canvas!" << std::endl;
                return;
            }
            canvas->Clear(Drawing::Color::COLOR_WHITE);
            std::cout << "Drawing does not support TextBlob" << std::endl;
            frame->SetDamageRegion(0, 0, 400, 400);
            rsSurface->FlushFrame(frame);

            sleep(2);
        }
    }

private:
#ifdef ACE_ENABLE_GPU
    std::unique_ptr<RenderContext> renderContext_;
#endif // ACE_ENABLE_GPU

    sptr<Window> window_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::vector<Gravity> availableGravities_;
};
} // namespace Test
} // namespace Rosen
} // namespace OHOS

int main()
{
    std::cout << "rs gravity demo start!" << std::endl;
    OHOS::Rosen::Test::RsSurfaceNodeGravityTestDemo demo;
    demo.Run();
    std::cout << "rs gravity demo end!" << std::endl;
    return 0;
}
