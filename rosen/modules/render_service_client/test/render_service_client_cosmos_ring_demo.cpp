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

#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <pthread.h>
#include <string>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "event_handler.h"
#include "event_runner.h"
#include "image_source.h"
#include "pixel_map.h"

#include "animation/rs_animation.h"
#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "render/rs_image.h"
#include "screen_manager/screen_types.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction_handler.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

namespace OHOS::Rosen {
namespace {
using AppExecFwk::EventHandler;
using AppExecFwk::EventRunner;

constexpr float EFFECT_NODE_SIZE = 350.0f;
constexpr float CENTER_RATIO = 0.5f;
constexpr int32_t ANIMATION_DURATION_MS = 2000;
constexpr int32_t ANIMATION_REPEAT_INFINITE = -1;
constexpr int32_t DEMO_DURATION_SECONDS = 30;
constexpr int32_t DEMO_LOG_INTERVAL_SECONDS = 10;
constexpr int32_t DEMO_INSTANCE_ID = 0;
constexpr uint32_t BACKGROUND_COLOR_WHITE = 0xFFFFFFFF;
constexpr uint32_t EFFECT_COLOR_TRANSPARENT = 0x00000000;
const std::string BACKGROUND_IMAGE_PATH = "/data/local/tmp/Images/AllWhite.jpg";

// Keep the reference graphic test's parameters; only progress starts at zero.
constexpr float RING_RADIUS = 87.f;
constexpr float RING_WIDTH = 30.f;
constexpr float RING_FEATHER = 14.7f;
constexpr float RING_NOISE_DENSITY = 25.5f;
constexpr int32_t RING_NOISE_PERIOD = 10;
constexpr float RING_NOISE_CONTRAST = 1.f;
constexpr float RING_NOISE2_DENSITY = 10.5f;
constexpr int32_t RING_NOISE2_PERIOD = 7;
constexpr float RING_NOISE2_CONTRAST = 0.6f;
constexpr int32_t RING_NOISE_SEED = 29;
constexpr float RING_CHROMA_STRENGTH = 0.69f;
const Vector4f RING_MID_COLOR = Vector4f(0x0B / 255.0f, 0x5C / 255.0f, 0xFF / 255.0f, 1.0f);
const Vector4f RING_DARK_COLOR = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
const Vector4f RING_BRIGHT_COLOR = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
constexpr bool RING_HL_ENABLE = true;
constexpr float RING_HL_OUTER_RADIUS = 97.f;
constexpr float RING_HL_INNER_RADIUS = 78.f;
constexpr float RING_HL_OUTER_WIDTH = 6.11f;
constexpr float RING_HL_INNER_WIDTH = 3.75f;
constexpr float RING_HL_DISPERSION_STRENGTH = 0.32f;
constexpr float RING_HLP1_OUTER_CENTER_ANGLE = 204.f;
constexpr float RING_HLP1_OUTER_ARC_SPAN = 220.f;
constexpr float RING_HLP1_OUTER_BRIGHTNESS = 1.f;
const Vector2f RING_HLP1_OUTER_DISP_SCALES = Vector2f(1.5f, 1.5f);
constexpr float RING_HLP1_INNER_CENTER_ANGLE = 270.f;
constexpr float RING_HLP1_INNER_ARC_SPAN = 260.f;
constexpr float RING_HLP1_INNER_BRIGHTNESS = 1.f;
const Vector2f RING_HLP1_INNER_DISP_SCALES = Vector2f(1.f, 1.f);
constexpr float RING_HLP2_OUTER_CENTER_ANGLE = 75.f;
constexpr float RING_HLP2_OUTER_ARC_SPAN = 210.f;
constexpr float RING_HLP2_OUTER_BRIGHTNESS = 1.f;
const Vector2f RING_HLP2_OUTER_DISP_SCALES = Vector2f(1.2f, 1.2f);
constexpr float RING_HLP2_INNER_CENTER_ANGLE = 70.f;
constexpr float RING_HLP2_INNER_ARC_SPAN = 190.f;
constexpr float RING_HLP2_INNER_BRIGHTNESS = 1.f;
const Vector2f RING_HLP2_INNER_DISP_SCALES = Vector2f(1.f, 1.f);
constexpr float RING_HL_RADIAL_FEATHER_PX = 10.f;
constexpr float RING_HL_TANGENT_FEATHER_DEG = 50.f;
constexpr float RING_HL_ANGULAR_FADE = 0.3f;
const Vector2f RING_GLOW_RADIUS = Vector2f(18.f, 90.f);
const Vector2f RING_GLOW_STRENGTH = Vector2f(0.12f, 0.43f);
const Vector2f RING_GLOW_THRESHOLD = Vector2f(-0.26f, 0.f);
constexpr float RING_GLOW_INTENSITY = 0.41f;
constexpr float RING_GLOW_SATURATION = 1.f;
constexpr float RING_GLOW_EXPOSURE = 6.59f;
constexpr float RING_PROGRESS = 0.0f;
constexpr int RING_ROTATION_TURNS = 1;
constexpr float RING_PARTICLE_COUNT = 65.f;
constexpr float RING_PARTICLE_SIZE = 3.7f;
constexpr float RING_PARTICLE_DISPERSION_STRENGTH = 0.01f;
constexpr float RING_PARTICLE_SCALE = 1.f;
constexpr float RING_PARTICLE_INNER_RADIUS = 65.f;
constexpr float RING_PARTICLE_OUTER_RADIUS = 113.f;
constexpr float RING_PARTICLE_PATH_LENGTH = 45.f;
const Vector4f RING_PARTICLE_MID_COLOR = Vector4f(0x0B / 255.0f, 0x5C / 255.0f, 0xFF / 255.0f, 1.0f);
constexpr float RING_PARTICLE_TINT_STRENGTH = 0.55f;
constexpr float RING_PARTICLE_BRIGHTNESS = 2.04f;
constexpr float RING_PARTICLE_OPACITY = 1.f;
constexpr float RING_PARTICLE_BIRTH_PEAK = 0.15f;

void InitRingParameters(RSNGCosmosRing& shader)
{
    shader.Setter<CosmosRingRadiusTag>(RING_RADIUS);
    shader.Setter<CosmosRingWidthTag>(RING_WIDTH);
    shader.Setter<CosmosRingFeatherTag>(RING_FEATHER);
    shader.Setter<CosmosRingNoiseDensityTag>(RING_NOISE_DENSITY);
    shader.Setter<CosmosRingNoisePeriodTag>(RING_NOISE_PERIOD);
    shader.Setter<CosmosRingNoiseContrastTag>(RING_NOISE_CONTRAST);
    shader.Setter<CosmosRingNoise2DensityTag>(RING_NOISE2_DENSITY);
    shader.Setter<CosmosRingNoise2PeriodTag>(RING_NOISE2_PERIOD);
    shader.Setter<CosmosRingNoise2ContrastTag>(RING_NOISE2_CONTRAST);
    shader.Setter<CosmosRingNoiseSeedTag>(RING_NOISE_SEED);
    shader.Setter<CosmosRingChromaStrengthTag>(RING_CHROMA_STRENGTH);
    shader.Setter<CosmosRingMidColorTag>(RING_MID_COLOR);
    shader.Setter<CosmosRingDarkColorTag>(RING_DARK_COLOR);
    shader.Setter<CosmosRingBrightColorTag>(RING_BRIGHT_COLOR);
    shader.Setter<CosmosRingProgressTag>(RING_PROGRESS);
    shader.Setter<CosmosRingRotationTurnsTag>(RING_ROTATION_TURNS);
}

void InitHighlightParameters(RSNGCosmosRing& shader)
{
    shader.Setter<CosmosRingHLEnableTag>(RING_HL_ENABLE);
    shader.Setter<CosmosRingHLOuterRadiusTag>(RING_HL_OUTER_RADIUS);
    shader.Setter<CosmosRingHLInnerRadiusTag>(RING_HL_INNER_RADIUS);
    shader.Setter<CosmosRingHLOuterWidthTag>(RING_HL_OUTER_WIDTH);
    shader.Setter<CosmosRingHLInnerWidthTag>(RING_HL_INNER_WIDTH);
    shader.Setter<CosmosRingHLDispersionStrengthTag>(RING_HL_DISPERSION_STRENGTH);
    shader.Setter<CosmosRingHLP1OuterCenterAngleTag>(RING_HLP1_OUTER_CENTER_ANGLE);
    shader.Setter<CosmosRingHLP1OuterArcSpanTag>(RING_HLP1_OUTER_ARC_SPAN);
    shader.Setter<CosmosRingHLP1OuterBrightnessTag>(RING_HLP1_OUTER_BRIGHTNESS);
    shader.Setter<CosmosRingHLP1OuterDispScalesTag>(RING_HLP1_OUTER_DISP_SCALES);
    shader.Setter<CosmosRingHLP1InnerCenterAngleTag>(RING_HLP1_INNER_CENTER_ANGLE);
    shader.Setter<CosmosRingHLP1InnerArcSpanTag>(RING_HLP1_INNER_ARC_SPAN);
    shader.Setter<CosmosRingHLP1InnerBrightnessTag>(RING_HLP1_INNER_BRIGHTNESS);
    shader.Setter<CosmosRingHLP1InnerDispScalesTag>(RING_HLP1_INNER_DISP_SCALES);
    shader.Setter<CosmosRingHLP2OuterCenterAngleTag>(RING_HLP2_OUTER_CENTER_ANGLE);
    shader.Setter<CosmosRingHLP2OuterArcSpanTag>(RING_HLP2_OUTER_ARC_SPAN);
    shader.Setter<CosmosRingHLP2OuterBrightnessTag>(RING_HLP2_OUTER_BRIGHTNESS);
    shader.Setter<CosmosRingHLP2OuterDispScalesTag>(RING_HLP2_OUTER_DISP_SCALES);
    shader.Setter<CosmosRingHLP2InnerCenterAngleTag>(RING_HLP2_INNER_CENTER_ANGLE);
    shader.Setter<CosmosRingHLP2InnerArcSpanTag>(RING_HLP2_INNER_ARC_SPAN);
    shader.Setter<CosmosRingHLP2InnerBrightnessTag>(RING_HLP2_INNER_BRIGHTNESS);
    shader.Setter<CosmosRingHLP2InnerDispScalesTag>(RING_HLP2_INNER_DISP_SCALES);
    shader.Setter<CosmosRingHLRadialFeatherPxTag>(RING_HL_RADIAL_FEATHER_PX);
    shader.Setter<CosmosRingHLTangentFeatherDegTag>(RING_HL_TANGENT_FEATHER_DEG);
    shader.Setter<CosmosRingHLAngularFadeTag>(RING_HL_ANGULAR_FADE);
}

void InitGlowParameters(RSNGCosmosRing& shader)
{
    shader.Setter<CosmosRingGlowRadiusTag>(RING_GLOW_RADIUS);
    shader.Setter<CosmosRingGlowStrengthTag>(RING_GLOW_STRENGTH);
    shader.Setter<CosmosRingGlowThresholdTag>(RING_GLOW_THRESHOLD);
    shader.Setter<CosmosRingGlowIntensityTag>(RING_GLOW_INTENSITY);
    shader.Setter<CosmosRingGlowSaturationTag>(RING_GLOW_SATURATION);
    shader.Setter<CosmosRingGlowExposureTag>(RING_GLOW_EXPOSURE);
}

void InitParticleParameters(RSNGCosmosRing& shader)
{
    shader.Setter<CosmosRingParticleCountTag>(RING_PARTICLE_COUNT);
    shader.Setter<CosmosRingParticleSizeTag>(RING_PARTICLE_SIZE);
    shader.Setter<CosmosRingParticleDispersionStrengthTag>(RING_PARTICLE_DISPERSION_STRENGTH);
    shader.Setter<CosmosRingParticleScaleTag>(RING_PARTICLE_SCALE);
    shader.Setter<CosmosRingParticleInnerRadiusTag>(RING_PARTICLE_INNER_RADIUS);
    shader.Setter<CosmosRingParticleOuterRadiusTag>(RING_PARTICLE_OUTER_RADIUS);
    shader.Setter<CosmosRingParticlePathLengthTag>(RING_PARTICLE_PATH_LENGTH);
    shader.Setter<CosmosRingParticleMidColorTag>(RING_PARTICLE_MID_COLOR);
    shader.Setter<CosmosRingParticleTintStrengthTag>(RING_PARTICLE_TINT_STRENGTH);
    shader.Setter<CosmosRingParticleBrightnessTag>(RING_PARTICLE_BRIGHTNESS);
    shader.Setter<CosmosRingParticleOpacityTag>(RING_PARTICLE_OPACITY);
    shader.Setter<CosmosRingParticleBirthPeakTag>(RING_PARTICLE_BIRTH_PEAK);
}

std::shared_ptr<Media::PixelMap> DecodeBackgroundImage()
{
    uint32_t errorCode = 0;
    auto imageSource = Media::ImageSource::CreateImageSource(BACKGROUND_IMAGE_PATH, Media::SourceOptions(), errorCode);
    if (imageSource == nullptr || errorCode != 0) {
        std::cerr << "Failed to open " << BACKGROUND_IMAGE_PATH << ", error: " << errorCode << std::endl;
        return nullptr;
    }
    Media::DecodeOptions options;
    options.allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC;
    std::shared_ptr<Media::PixelMap> pixelMap = imageSource->CreatePixelMap(options, errorCode);
    if (pixelMap == nullptr || errorCode != 0) {
        std::cerr << "Failed to decode " << BACKGROUND_IMAGE_PATH << ", error: " << errorCode << std::endl;
        return nullptr;
    }
    return pixelMap;
}

class CosmosRingDemo final {
public:
    CosmosRingDemo() = default;
    ~CosmosRingDemo() = default;
    CosmosRingDemo(const CosmosRingDemo&) = delete;
    CosmosRingDemo& operator=(const CosmosRingDemo&) = delete;
    CosmosRingDemo(CosmosRingDemo&&) = delete;
    CosmosRingDemo& operator=(CosmosRingDemo&&) = delete;

    void Start(const std::shared_ptr<EventHandler>& handler)
    {
        if (!InitScreen() || !InitDirector(handler) || !InitScene() || !InitBackground() || !InitEffect()) {
            Stop();
            return;
        }
        surfaceNode_->AttachToDisplay(screenId_);
        surfaceAttached_ = true;
        director_->FlushModifier();
        director_->SendMessages();

        RSAnimationTimingProtocol protocol(ANIMATION_DURATION_MS);
        protocol.SetRepeatCount(ANIMATION_REPEAT_INFINITE);
        protocol.SetAutoReverse(false);
        protocol.SetDirection(true);
        // The bound shader and Animate must use the same UI context and thread.
        animations_ = RSNode::Animate(uiContext_, protocol, RSAnimationTimingCurve::LINEAR,
            [shader = ringShader_]() { shader->Setter<CosmosRingProgressTag>(1.0f); });
        if (animations_.empty()) {
            std::cerr << "Failed to start CosmosRing animation" << std::endl;
            Stop();
            return;
        }
        director_->SendMessages();
        running_ = true;
    }

    bool IsRunning() const
    {
        return running_;
    }

    void Stop()
    {
        running_ = false;
        auto transaction = uiContext_ ? uiContext_->GetRSTransaction() : nullptr;
        for (const auto& animation : animations_) {
            if (animation != nullptr) {
                animation->Finish();
            }
        }
        animations_.clear();
        if (surfaceAttached_) {
            surfaceNode_->DetachToDisplay(screenId_);
            surfaceAttached_ = false;
        }
        if (director_ != nullptr) {
            director_->SendMessages();
        }
        effectNode_.reset();
        backgroundNode_.reset();
        rootNode_.reset();
        surfaceNode_.reset();
        ringShader_.reset();
        if (director_ != nullptr) {
            director_->SendMessages();
            director_->SetUITaskRunner({}, DEMO_INSTANCE_ID, true);
        }
        // Director destruction queues the final lifecycle commands on this context.
        director_.reset();
        if (transaction != nullptr) {
            transaction->FlushImplicitTransaction();
        }
        uiContext_.reset();
    }

private:
    bool InitScreen()
    {
        auto& interfaces = RSInterfaces::GetInstance();
        screenId_ = interfaces.GetDefaultScreenId();
        if (screenId_ == INVALID_SCREEN_ID) {
            std::cerr << "No default screen is available" << std::endl;
            return false;
        }
        const auto mode = interfaces.GetScreenActiveMode(screenId_);
        screenWidth_ = static_cast<float>(mode.GetScreenWidth());
        screenHeight_ = static_cast<float>(mode.GetScreenHeight());
        if (screenWidth_ < EFFECT_NODE_SIZE || screenHeight_ < EFFECT_NODE_SIZE) {
            std::cerr << "Invalid screen size: " << screenWidth_ << "x" << screenHeight_ << std::endl;
            return false;
        }
        return true;
    }

    bool InitDirector(const std::shared_ptr<EventHandler>& handler)
    {
        auto token = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId_);
        if (token == nullptr) {
            std::cerr << "Failed to connect to Render Service" << std::endl;
            return false;
        }
        director_ = RSUIDirector::Create(token);
        if (director_ == nullptr) {
            std::cerr << "Failed to create UI director" << std::endl;
            return false;
        }
        uiContext_ = director_->GetRSUIContext();
        if (uiContext_ == nullptr || uiContext_->GetRSTransaction() == nullptr) {
            std::cerr << "Failed to create UI context or transaction" << std::endl;
            return false;
        }
        std::weak_ptr<EventHandler> weakHandler = handler;
        director_->SetUITaskRunner(
            [weakHandler](const std::function<void()>& task, uint32_t delay) {
                if (auto taskHandler = weakHandler.lock()) {
                    taskHandler->PostTask(task, "CosmosRingUITask", delay);
                }
            },
            DEMO_INSTANCE_ID, true);
        return true;
    }

    bool InitScene()
    {
        RSSurfaceNodeConfig config;
        config.SurfaceNodeName = "cosmos_ring_demo";
        surfaceNode_ = RSSurfaceNode::Create(config, RSSurfaceNodeType::APP_WINDOW_NODE, true, false, uiContext_);
        rootNode_ = std::static_pointer_cast<RSRootNode>(RSRootNode::Create(false, false, uiContext_));
        if (surfaceNode_ == nullptr || rootNode_ == nullptr) {
            std::cerr << "Failed to create surface or root node" << std::endl;
            return false;
        }
        surfaceNode_->SetBounds(0, 0, screenWidth_, screenHeight_);
        surfaceNode_->SetFrame(0, 0, screenWidth_, screenHeight_);
        surfaceNode_->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
        surfaceNode_->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
        rootNode_->SetBounds(0, 0, screenWidth_, screenHeight_);
        rootNode_->SetFrame(0, 0, screenWidth_, screenHeight_);
        rootNode_->SetBackgroundColor(BACKGROUND_COLOR_WHITE);
        director_->SetRSSurfaceNode(surfaceNode_);
        director_->SetRSRootNode(rootNode_);
        return true;
    }

    bool InitBackground()
    {
        auto pixelMap = DecodeBackgroundImage();
        if (pixelMap == nullptr) {
            return false;
        }
        backgroundNode_ = RSCanvasNode::Create(false, false, uiContext_);
        if (backgroundNode_ == nullptr) {
            std::cerr << "Failed to create background node" << std::endl;
            return false;
        }
        auto image = std::make_shared<RSImage>();
        image->SetPixelMap(pixelMap);
        image->SetImageFit(static_cast<int>(ImageFit::FILL));
        backgroundNode_->SetBounds(0, 0, screenWidth_, screenHeight_);
        backgroundNode_->SetFrame(0, 0, screenWidth_, screenHeight_);
        backgroundNode_->SetBgImageSize(screenWidth_, screenHeight_);
        backgroundNode_->SetBgImage(image);
        rootNode_->AddChild(backgroundNode_);
        return true;
    }

    bool InitEffect()
    {
        effectNode_ = RSCanvasNode::Create(false, false, uiContext_);
        if (effectNode_ == nullptr) {
            std::cerr << "Failed to create CosmosRing node" << std::endl;
            return false;
        }
        ringShader_ = std::make_shared<RSNGCosmosRing>();
        InitRingParameters(*ringShader_);
        InitHighlightParameters(*ringShader_);
        InitGlowParameters(*ringShader_);
        InitParticleParameters(*ringShader_);
        const float positionX = (screenWidth_ - EFFECT_NODE_SIZE) * CENTER_RATIO;
        const float positionY = (screenHeight_ - EFFECT_NODE_SIZE) * CENTER_RATIO;
        effectNode_->SetBounds(positionX, positionY, EFFECT_NODE_SIZE, EFFECT_NODE_SIZE);
        effectNode_->SetFrame(positionX, positionY, EFFECT_NODE_SIZE, EFFECT_NODE_SIZE);
        effectNode_->SetBackgroundColor(EFFECT_COLOR_TRANSPARENT);
        effectNode_->SetBackgroundNGShader(ringShader_);
        rootNode_->AddChild(effectNode_);
        return true;
    }

    ScreenId screenId_ = INVALID_SCREEN_ID;
    float screenWidth_ = 0.0f;
    float screenHeight_ = 0.0f;
    bool surfaceAttached_ = false;
    bool running_ = false;
    std::shared_ptr<RSUIDirector> director_;
    std::shared_ptr<RSUIContext> uiContext_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::shared_ptr<RSRootNode> rootNode_;
    RSCanvasNode::SharedPtr backgroundNode_;
    RSCanvasNode::SharedPtr effectNode_;
    std::shared_ptr<RSNGCosmosRing> ringShader_;
    std::vector<std::shared_ptr<RSAnimation>> animations_;
};

bool PrepareExitSignals(sigset_t& exitSignals)
{
    const pid_t shellPid = getppid();
    if (sigemptyset(&exitSignals) != 0 || sigaddset(&exitSignals, SIGINT) != 0 ||
        sigaddset(&exitSignals, SIGTERM) != 0 || sigaddset(&exitSignals, SIGHUP) != 0) {
        std::cerr << "Failed to prepare exit signals: " << std::strerror(errno) << std::endl;
        return false;
    }
    // Block before starting the UI thread so the main thread handles all exit signals.
    const int result = pthread_sigmask(SIG_BLOCK, &exitSignals, nullptr);
    if (result != 0) {
        std::cerr << "Failed to block exit signals: " << std::strerror(result) << std::endl;
        return false;
    }
    // HDC kills its device-side shell when the host terminal disconnects.
    if (prctl(PR_SET_PDEATHSIG, static_cast<unsigned long>(SIGTERM), 0UL, 0UL, 0UL) != 0) {
        std::cerr << "Failed to monitor launching shell: " << std::strerror(errno) << std::endl;
        return false;
    }
    // Parent death before prctl registration does not generate the signal.
    if (shellPid <= 1 || getppid() != shellPid) {
        std::cerr << "Launching shell exited before demo initialization" << std::endl;
        return false;
    }
    return true;
}

bool WaitForDemoExit(const sigset_t& exitSignals)
{
    const auto startTime = std::chrono::steady_clock::now();
    std::cout << "CosmosRing demo running (" << ANIMATION_DURATION_MS << " ms per cycle), auto-exit after "
              << DEMO_DURATION_SECONDS << " seconds; status every " << DEMO_LOG_INTERVAL_SECONDS << " seconds."
              << std::endl;
    for (int32_t elapsedSeconds = DEMO_LOG_INTERVAL_SECONDS; elapsedSeconds <= DEMO_DURATION_SECONDS;
         elapsedSeconds += DEMO_LOG_INTERVAL_SECONDS) {
        const auto nextLogTime = startTime + std::chrono::seconds(elapsedSeconds);
        auto now = std::chrono::steady_clock::now();
        while (now < nextLogTime) {
            const auto remaining = std::chrono::duration_cast<std::chrono::nanoseconds>(nextLogTime - now);
            const auto remainingSeconds = std::chrono::duration_cast<std::chrono::seconds>(remaining);
            const timespec timeout = { static_cast<time_t>(remainingSeconds.count()),
                static_cast<long>((remaining - remainingSeconds).count()) };
            const int signalNumber = sigtimedwait(&exitSignals, nullptr, &timeout);
            if (signalNumber >= 0) {
                std::cout << "Exit signal received: " << signalNumber << "; stopping CosmosRing demo" << std::endl;
                return true;
            }
            const int error = errno;
            if (error != EAGAIN && error != EINTR) {
                std::cerr << "Failed to wait for demo exit: " << std::strerror(error) << std::endl;
                return false;
            }
            now = std::chrono::steady_clock::now();
        }
        std::cout << "CosmosRing demo elapsed: " << elapsedSeconds << "/" << DEMO_DURATION_SECONDS << " seconds"
                  << std::endl;
    }
    std::cout << "Runtime limit reached; stopping CosmosRing demo" << std::endl;
    return true;
}

int RunDemo(const sigset_t& exitSignals)
{
    auto runner = EventRunner::Create("CosmosRingDemo");
    if (runner == nullptr) {
        std::cerr << "Failed to create UI event runner" << std::endl;
        return EXIT_FAILURE;
    }
    auto handler = std::make_shared<EventHandler>(runner);
    auto demo = std::make_shared<CosmosRingDemo>();
    const bool started = handler->PostSyncTask([demo, handler]() { demo->Start(handler); }, "StartCosmosRingDemo");
    if (!started || !demo->IsRunning()) {
        std::cerr << "CosmosRing demo initialization failed" << std::endl;
        return EXIT_FAILURE;
    }
    const bool waited = WaitForDemoExit(exitSignals);
    const bool stopped = handler->PostSyncTask([demo]() { demo->Stop(); }, "StopCosmosRingDemo");
    if (!stopped) {
        std::cerr << "Failed to dispatch CosmosRing cleanup" << std::endl;
    } else {
        std::cout << "CosmosRing demo cleanup complete" << std::endl;
    }
    handler->RemoveAllEvents();
    return waited && stopped ? EXIT_SUCCESS : EXIT_FAILURE;
}
} // namespace
} // namespace OHOS::Rosen

int main()
{
    sigset_t exitSignals;
    if (!OHOS::Rosen::PrepareExitSignals(exitSignals)) {
        return EXIT_FAILURE;
    }
    return OHOS::Rosen::RunDemo(exitSignals);
}
