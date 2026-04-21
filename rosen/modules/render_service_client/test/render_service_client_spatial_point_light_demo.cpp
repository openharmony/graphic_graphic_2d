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

#include <iostream>

#include "draw/canvas.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"

using namespace OHOS;
using namespace OHOS::Rosen;

constexpr uint64_t SCREEN_ID = 0;

namespace {
constexpr int32_t WINDOW_OFFSET = 200;
constexpr int32_t WINDOW_WIDTH = 1500;
constexpr int32_t WINDOW_HEIGHT = 2000;

constexpr int32_t NODE_X = 200;
constexpr int32_t NODE_Y = 500;
constexpr int32_t NODE_WIDTH = 1000;
constexpr int32_t NODE_HEIGHT = 800;
constexpr int32_t HALF_DIVISOR = 2;

constexpr float DEFAULT_INTENSITY = 2.0f;
constexpr float DEFAULT_ATTENUATION = 0.3f;
constexpr float DEFAULT_LIGHT_Z = 100.0f;

constexpr float ANIMATION_FRAME_INTERVAL_US = 50000;
constexpr int32_t ANIMATION_FRAME_COUNT = 60;
constexpr int32_t MASK_SWITCH_INTERVAL = 3;
constexpr int32_t DEMO_INIT_WAIT_SECONDS = 2;
constexpr int32_t DEMO_ANIMATION_WAIT_SECONDS = 1;

const std::string SURFACE_NODE_NAME = "spatial_point_light_demo";

enum MaskType {
    MASK_RADIAL_GRADIENT = 0,
    MASK_RIPPLE,
    MASK_WAVE_GRADIENT,
    MASK_DOUBLE_RIPPLE,
    MASK_COUNT
};

std::shared_ptr<RSNGMaskBase> CreateRadialGradientMask()
{
    auto mask = std::make_shared<RSNGRadialGradientMask>();
    mask->Setter<RadialGradientMaskCenterTag>(Vector2f(0.5f, 0.5f));
    mask->Setter<RadialGradientMaskRadiusXTag>(0.4f);
    mask->Setter<RadialGradientMaskRadiusYTag>(0.4f);
    mask->Setter<RadialGradientMaskColorsTag>(std::vector<float>{1.0f, 0.0f});
    mask->Setter<RadialGradientMaskPositionsTag>(std::vector<float>{0.0f, 1.0f});
    return mask;
}

std::shared_ptr<RSNGMaskBase> CreateRippleMask()
{
    auto mask = std::make_shared<RSNGRippleMask>();
    mask->Setter<RippleMaskCenterTag>(Vector2f(0.5f, 0.5f));
    mask->Setter<RippleMaskRadiusTag>(5.0f);
    mask->Setter<RippleMaskWidthTag>(5.0f);
    mask->Setter<RippleMaskOffsetTag>(0.0f);
    return mask;
}

std::shared_ptr<RSNGMaskBase> CreateWaveGradientMask()
{
    auto mask = std::make_shared<RSNGWaveGradientMask>();
    mask->Setter<WaveGradientMaskWaveCenterTag>(Vector2f(0.5f, 0.5f));
    mask->Setter<WaveGradientMaskWaveWidthTag>(0.3f);
    mask->Setter<WaveGradientMaskPropagationRadiusTag>(0.6f);
    mask->Setter<WaveGradientMaskBlurRadiusTag>(0.15f);
    mask->Setter<WaveGradientMaskTurbulenceStrengthTag>(0.7f);
    return mask;
}

std::shared_ptr<RSNGMaskBase> CreateDoubleRippleMask()
{
    auto mask = std::make_shared<RSNGDoubleRippleMask>();
    mask->Setter<DoubleRippleMaskCenter1Tag>(Vector2f(0.3f, 0.3f));
    mask->Setter<DoubleRippleMaskCenter2Tag>(Vector2f(0.7f, 0.7f));
    mask->Setter<DoubleRippleMaskRadiusTag>(5.0f);
    mask->Setter<DoubleRippleMaskWidthTag>(5.0f);
    mask->Setter<DoubleRippleMaskTurbulenceTag>(0.5f);
    mask->Setter<DoubleRippleMaskHaloThicknessTag>(2.0f);
    return mask;
}

std::shared_ptr<RSNGMaskBase> CreateMaskByType(MaskType type)
{
    switch (type) {
        case MASK_RADIAL_GRADIENT:
            return CreateRadialGradientMask();
        case MASK_RIPPLE:
            return CreateRippleMask();
        case MASK_WAVE_GRADIENT:
            return CreateWaveGradientMask();
        case MASK_DOUBLE_RIPPLE:
            return CreateDoubleRippleMask();
        default:
            return CreateRadialGradientMask();
    }
}

std::string GetMaskName(MaskType type)
{
    switch (type) {
        case MASK_RADIAL_GRADIENT:
            return "RadialGradientMask";
        case MASK_RIPPLE:
            return "RippleMask";
        case MASK_WAVE_GRADIENT:
            return "WaveGradientMask";
        case MASK_DOUBLE_RIPPLE:
            return "DoubleRippleMask";
        default:
            return "Unknown";
    }
}

class SpatialPointLightDemo {
public:
    SpatialPointLightDemo()
    {
        rsUiDirector_ = RSUIDirector::Create();
        if (rsUiDirector_ == nullptr) {
            std::cout << "Failed to create ui director!" << std::endl;
            return;
        }
        rsUiDirector_->Init(true, true);
        uiContext_ = rsUiDirector_->GetRSUIContext();
        if (uiContext_ == nullptr) {
            std::cout << "ui context is nullptr!" << std::endl;
            return;
        }

        if (!InitSurfaceNode()) {
            std::cout << "Failed to init surface node!" << std::endl;
            return;
        }

        if (!InitRootNode()) {
            std::cout << "Failed to init root node!" << std::endl;
            return;
        }

        if (!InitCanvasNode()) {
            std::cout << "Failed to init canvas node!" << std::endl;
            return;
        }

        transaction_ = uiContext_->GetRSTransaction();
        if (transaction_ == nullptr) {
            std::cout << "rs transaction is nullptr" << std::endl;
            return;
        }

        transaction_->FlushImplicitTransaction();
        std::cout << "SpatialPointLight demo init success" << std::endl;
        initSuccess_ = true;
    }

    bool InitSurfaceNode()
    {
        RSSurfaceNodeConfig surfaceNodeConfig;
        surfaceNodeConfig.SurfaceNodeName = SURFACE_NODE_NAME;
        RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
        surfaceNode_ = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType, true, false, uiContext_);
        if (!surfaceNode_) {
            std::cout << "Failed to create surface node!" << std::endl;
            return false;
        }
        surfaceNode_->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
        surfaceNode_->SetBounds(0, WINDOW_OFFSET, WINDOW_WIDTH, WINDOW_HEIGHT);
        surfaceNode_->SetFrame(0, WINDOW_OFFSET, WINDOW_WIDTH, WINDOW_HEIGHT);
        surfaceNode_->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
        surfaceNode_->SetBackgroundColor(SK_ColorWHITE);
        surfaceNode_->AttachToDisplay(SCREEN_ID);
        surfaceNodeAttached_ = true;
        rsUiDirector_->SetRSSurfaceNode(surfaceNode_);
        return true;
    }

    bool InitRootNode()
    {
        rootNode_ = RSRootNode::Create(false, false, uiContext_);
        if (rootNode_ == nullptr) {
            std::cout << "Failed to create root node!" << std::endl;
            return false;
        }
        rootNode_->SetBounds(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        rootNode_->SetFrame(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        rootNode_->SetBackgroundColor(0xFF000000);
        rsUiDirector_->SetRSRootNode(rootNode_->ReinterpretCastTo<RSRootNode>());
        return true;
    }

    bool InitCanvasNode()
    {
        canvasNode_ = RSCanvasNode::Create(false, false, uiContext_);
        if (canvasNode_ == nullptr) {
            std::cout << "Failed to create canvas node!" << std::endl;
            return false;
        }
        canvasNode_->SetBounds(NODE_X, NODE_Y, NODE_WIDTH, NODE_HEIGHT);
        canvasNode_->SetFrame(NODE_X, NODE_Y, NODE_WIDTH, NODE_HEIGHT);
        canvasNode_->SetBackgroundColor(0xFF808080);

        UpdateShader(DEFAULT_INTENSITY, Vector3f(NODE_X + NODE_WIDTH / HALF_DIVISOR,
            NODE_Y + NODE_HEIGHT / HALF_DIVISOR, DEFAULT_LIGHT_Z),
            Vector4f(1.0f, 1.0f, 1.0f, 1.0f), DEFAULT_ATTENUATION, MASK_RADIAL_GRADIENT);

        rootNode_->AddChild(canvasNode_, -1);
        return true;
    }

    void UpdateShader(float intensity, const Vector3f& position,
        const Vector4f& color, float attenuation, MaskType maskType)
    {
        auto shader = std::make_shared<RSNGSpatialPointLight>();
        shader->Setter<SpatialPointLightLightIntensityTag>(intensity);
        shader->Setter<SpatialPointLightLightPositionTag>(position);
        shader->Setter<SpatialPointLightLightColorTag>(color);
        shader->Setter<SpatialPointLightAttenuationTag>(attenuation);
        shader->Setter<SpatialPointLightMaskTag>(CreateMaskByType(maskType));
        canvasNode_->SetBackgroundNGShader(shader);
    }

    ~SpatialPointLightDemo() noexcept
    {
        if (surfaceNodeAttached_) {
            surfaceNode_->DetachToDisplay(SCREEN_ID);
            if (transaction_) {
                transaction_->FlushImplicitTransaction();
            }
        }
    }

    void FlushAndSend()
    {
        transaction_->FlushImplicitTransaction();
        rsUiDirector_->SendMessages();
        rsUiDirector_->FlushModifier();
    }

    void RunIntensityAnimation()
    {
        std::cout << "Running intensity animation..." << std::endl;
        float phase = 0.0f;
        for (int frame = 0; frame < ANIMATION_FRAME_COUNT; frame++) {
            phase += 0.1f;
            float intensity = 0.5f + 2.0f * (0.5f + 0.5f * sin(phase));
            UpdateShader(intensity,
                Vector3f(NODE_X + NODE_WIDTH / HALF_DIVISOR, NODE_Y + NODE_HEIGHT / HALF_DIVISOR, DEFAULT_LIGHT_Z),
                Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
                DEFAULT_ATTENUATION,
                currentMask_);
            FlushAndSend();
            usleep(ANIMATION_FRAME_INTERVAL_US);
        }
        std::cout << "Intensity animation done" << std::endl;
    }

    void RunPositionAnimation()
    {
        std::cout << "Running position animation..." << std::endl;
        float centerX = NODE_X + NODE_WIDTH / HALF_DIVISOR;
        float centerY = NODE_Y + NODE_HEIGHT / HALF_DIVISOR;
        float radius = 200.0f;
        for (int frame = 0; frame < ANIMATION_FRAME_COUNT; frame++) {
            float angle = frame * 0.1f;
            float x = centerX + radius * cos(angle);
            float y = centerY + radius * sin(angle);
            float z = DEFAULT_LIGHT_Z + 50.0f * sin(angle * 0.5f);
            UpdateShader(DEFAULT_INTENSITY,
                Vector3f(x, y, z),
                Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
                DEFAULT_ATTENUATION,
                currentMask_);
            FlushAndSend();
            usleep(ANIMATION_FRAME_INTERVAL_US);
        }
        std::cout << "Position animation done" << std::endl;
    }

    void RunColorAnimation()
    {
        std::cout << "Running color animation..." << std::endl;
        float phase = 0.0f;
        for (int frame = 0; frame < ANIMATION_FRAME_COUNT; frame++) {
            phase += 0.05f;
            float r = 0.5f + 0.5f * sin(phase);
            float g = 0.5f + 0.5f * sin(phase + 2.094f);
            float b = 0.5f + 0.5f * sin(phase + 4.189f);
            UpdateShader(DEFAULT_INTENSITY,
                Vector3f(NODE_X + NODE_WIDTH / HALF_DIVISOR, NODE_Y + NODE_HEIGHT / HALF_DIVISOR, DEFAULT_LIGHT_Z),
                Vector4f(r, g, b, 1.0f),
                DEFAULT_ATTENUATION,
                currentMask_);
            FlushAndSend();
            usleep(ANIMATION_FRAME_INTERVAL_US);
        }
        std::cout << "Color animation done" << std::endl;
    }

    void SwitchMask(MaskType newMask)
    {
        currentMask_ = newMask;
        std::cout << "Switching to mask: " << GetMaskName(currentMask_) << std::endl;
        UpdateShader(DEFAULT_INTENSITY,
            Vector3f(NODE_X + NODE_WIDTH / HALF_DIVISOR, NODE_Y + NODE_HEIGHT / HALF_DIVISOR, DEFAULT_LIGHT_Z),
            Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
            DEFAULT_ATTENUATION,
            currentMask_);
        FlushAndSend();
    }

    void RunMaskSwitchDemo()
    {
        std::cout << "Running mask switch demo..." << std::endl;
        for (int i = 0; i < MASK_COUNT; i++) {
            SwitchMask(static_cast<MaskType>(i));
            sleep(MASK_SWITCH_INTERVAL);
        }
        std::cout << "Mask switch demo done" << std::endl;
    }

    void Run()
    {
        if (!initSuccess_) {
            std::cout << "SpatialPointLight demo init failed, can't run" << std::endl;
            return;
        }

        std::cout << "=== SpatialPointLight Demo Start ===" << std::endl;
        FlushAndSend();
        sleep(DEMO_INIT_WAIT_SECONDS);

        RunIntensityAnimation();
        sleep(DEMO_ANIMATION_WAIT_SECONDS);

        RunPositionAnimation();
        sleep(DEMO_ANIMATION_WAIT_SECONDS);

        RunColorAnimation();
        sleep(DEMO_ANIMATION_WAIT_SECONDS);

        RunMaskSwitchDemo();
        sleep(DEMO_ANIMATION_WAIT_SECONDS);

        std::cout << "=== SpatialPointLight Demo End ===" << std::endl;
    }

private:
    std::shared_ptr<RSUIDirector> rsUiDirector_;
    std::shared_ptr<RSUIContext> uiContext_;
    std::shared_ptr<RSTransactionHandler> transaction_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::shared_ptr<RSNode> rootNode_;
    RSCanvasNode::SharedPtr canvasNode_;
    MaskType currentMask_ = MASK_RADIAL_GRADIENT;
    bool initSuccess_ = false;
    bool surfaceNodeAttached_ = false;
};
} // namespace

int main()
{
    {
        SpatialPointLightDemo demo;
        demo.Run();
    }
    return 0;
}