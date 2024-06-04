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
#include <random>
#include <string>


#include "refbase.h"
#include "render_context/render_context.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
namespace Test {
constexpr int DEFAULT_DISPLAY_ID = 0;
constexpr int WINDOW_LEFT = 100;
constexpr int WINDOW_TOP = 200;
constexpr int WINDOW_WIDTH = 360;
constexpr int WINDOW_HEIGHT = 360;
constexpr int BUFFER_WIDTH = 360;
constexpr int BUFFER_HEIGHT = 360;
constexpr float MIN_SCALE = 0.5f;
constexpr float MAX_SCALE = 2.0f;

float GetRandomScale()
{
    static std::random_device seed;
    std::mt19937_64 gen(seed());
    std::uniform_real_distribution<> distribution(MIN_SCALE, MAX_SCALE);
    return distribution(gen);
}

// we can make this demo and run it on the device,
// to visualize the surface's scale effects.
class RsSurfaceNodeScaleTestDemo {
public:
    RsSurfaceNodeScaleTestDemo()
    {
#ifdef ACE_ENABLE_GPU
        renderContext_ = std::make_unique<RenderContext>();
        renderContext_->InitializeEglContext();
#endif // ACE_ENABLE_GPU

        sptr<WindowOption> option(new WindowOption());
        option->SetDisplayId(DEFAULT_DISPLAY_ID);
        option->SetWindowRect( {WINDOW_LEFT, WINDOW_TOP, WINDOW_WIDTH, WINDOW_HEIGHT} );
        option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
        option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
        option->SetWindowName("scale_demo");
        window_ = Window::Create(option->GetWindowName(), option);
        if (window_ != nullptr) {
            surfaceNode_ = window_->GetSurfaceNode();
            // surface bounds is window rect.
            surfaceNode_->SetBounds(WINDOW_LEFT, WINDOW_TOP, WINDOW_WIDTH, WINDOW_HEIGHT);
            RSTransaction::FlushImplicitTransaction();
            window_->Show();
        } else {
            std::cout << "Failed to create window!" << std::endl;
        }
    }

    ~RsSurfaceNodeScaleTestDemo() noexcept
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

        // test 50 frames.
        for (int i = 0; i < 50; ++i) {
            // get random scales and set them with pivot(start point or center point).
            float scaleX = GetRandomScale();
            float scaleY = GetRandomScale();
            surfaceNode_->SetScale(scaleX, scaleY);
            // default pivot: center
            float pivotX = 0.5f; // centerX: 0.5
            float pivotY = 0.5f; // centerY: 0.5
            std::string pivotInfo;
            // make pivot to start point if (i % 2 == 0)
            if (i % 2 == 0) {
                pivotX = 0.0f;
                pivotY = 0.0f;
                pivotInfo = "Pivot: start(0, 0)";
            }
            surfaceNode_->SetPivot(pivotX, pivotY);
            RSTransaction::FlushImplicitTransaction();

            auto frame = rsSurface->RequestFrame(BUFFER_WIDTH, BUFFER_HEIGHT);
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
            frame->SetDamageRegion(0, 0, BUFFER_WIDTH, BUFFER_HEIGHT);
            rsSurface->FlushFrame(frame);

            sleep(2); // sleep 2s
        }
    }

private:
#ifdef ACE_ENABLE_GPU
    std::unique_ptr<RenderContext> renderContext_;
#endif // ACE_ENABLE_GPU

    sptr<Window> window_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
};
} // namespace Test
} // namespace Rosen
} // namespace OHOS

int main()
{
    std::cout << "rs surface node scale demo start!" << std::endl;
    OHOS::Rosen::Test::RsSurfaceNodeScaleTestDemo demo;
    demo.Run();
    std::cout << "rs surface node scale demo end!" << std::endl;
    return 0;
}
