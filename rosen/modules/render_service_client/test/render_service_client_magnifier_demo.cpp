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
#include "effect/rs_render_shape_base.h"
#include "image_source.h"
#include "pixel_map.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/property/include/rs_ui_shape_base.h"
#include "wm/window.h"

using namespace OHOS;
using namespace OHOS::Rosen;
constexpr uint64_t SCREEN_ID = 0;
namespace {
const std::string DEFAULT_IMAGE_PATH = "/data/local/tmp/bg_test.jpg";

constexpr int WINDOW_OFFSET = 200;
constexpr int WINDOW_WIDTH = 1500;
constexpr int WINDOW_HEIGHT = 2000;

constexpr float MAGNIFIER_FACTOR = 1.4f;
constexpr float MAGNIFIER_WIDTH = 450.0f;
constexpr float MAGNIFIER_HEIGHT = 250.0f;
constexpr float MAGNIFIER_CORNER_RADIUS = 250.0f;
constexpr float MAGNIFIER_BORDER_WIDTH = 3.0f;
constexpr uint32_t MAGNIFIER_BORDER_COLOR = 0xFFFFFF80;
constexpr float MAGNIFIER_SHADOW_SIZE = 80.0f;
constexpr float MAGNIFIER_SHADOW_STRENGTH = 0.05f;

constexpr float WAIT_TIME = 5.0f;
constexpr float ANIMATION_TIME = 2000.0f;
constexpr float ANIMATION_INTERVAL = 1000.0f;

const std::vector<RRect> RRECT_PARAMS = {
    RRect{RectT<float>{200, 600, MAGNIFIER_WIDTH, MAGNIFIER_HEIGHT}, MAGNIFIER_CORNER_RADIUS, MAGNIFIER_CORNER_RADIUS},
    RRect{RectT<float>{500, 600, 250, 300}, MAGNIFIER_CORNER_RADIUS, MAGNIFIER_CORNER_RADIUS},
    RRect{RectT<float>{200, 600, MAGNIFIER_WIDTH, MAGNIFIER_HEIGHT}, MAGNIFIER_CORNER_RADIUS, MAGNIFIER_CORNER_RADIUS},
};


std::shared_ptr<Media::PixelMap> DecodePixelMap(
    const std::string& pathName, const Media::AllocatorType& allocatorType)
{
    uint32_t errCode = 0;
    std::unique_ptr<Media::ImageSource> imageSource =
        Media::ImageSource::CreateImageSource(pathName, Media::SourceOptions(), errCode);
    if (imageSource == nullptr || errCode != 0) {
        std::cout << "DecodePixelMap: create image source failed, imageSource: " << imageSource <<
         ", errCode: " << errCode << std::endl;
        return nullptr;
    }

    Media::DecodeOptions decodeOpt;
    decodeOpt.allocatorType = allocatorType;
    std::shared_ptr<Media::PixelMap> pixelmap = imageSource->CreatePixelMap(decodeOpt, errCode);
    if (pixelmap == nullptr || errCode != 0) {
        std::cout << "DecodePixelMap: create pixel map failed, pixelmap: " << pixelmap <<
         ", errCode: " << errCode << std::endl;
        return nullptr;
    }
    return pixelmap;
}

using ShapeCreator = std::function<std::shared_ptr<RSNGShapeBase>()>;

static std::unordered_map<RSNGEffectType, ShapeCreator> creatorShape = {

    {RSNGEffectType::SDF_RRECT_SHAPE,
     [] {
         return std::make_shared<RSNGSDFRRectShape>();
    }},
};

std::shared_ptr<RSNGMagnifierFilter> CreateDefaultMagnifierFilter()
{
    auto magnifierFilter = std::make_shared<RSNGMagnifierFilter>();
    magnifierFilter->Setter<MagnifierFactorTag>(MAGNIFIER_FACTOR);
    magnifierFilter->Setter<MagnifierWidthTag>(MAGNIFIER_WIDTH);
    magnifierFilter->Setter<MagnifierHeightTag>(MAGNIFIER_HEIGHT);
    magnifierFilter->Setter<MagnifierCornerRadiusTag>(MAGNIFIER_CORNER_RADIUS);
    magnifierFilter->Setter<MagnifierBorderWidthTag>(MAGNIFIER_BORDER_WIDTH);
    magnifierFilter->Setter<MagnifierOuterContourColor1Tag>(RSColor(MAGNIFIER_BORDER_COLOR));
    magnifierFilter->Setter<MagnifierShadowSizeTag>(MAGNIFIER_SHADOW_SIZE);
    magnifierFilter->Setter<MagnifierShadowStrengthTag>(MAGNIFIER_SHADOW_STRENGTH);
    return magnifierFilter;
}

std::shared_ptr<RSNGShapeBase> CreateShape(RSNGEffectType type)
{
    auto it = creatorShape.find(type);
    return it != creatorShape.end() ? it->second() : nullptr;
}

std::shared_ptr<RSNGShapeBase> InitSDFRRectShape(RRect rrect)
{
    std::cout << "InitSDFRRectShape" << std::endl;
    auto rootShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
    if (!rootShape) {
        std::cout << "Failed to create SDF RRect shape" << std::endl;
        return nullptr;
    }
    auto sdfRRectRootShape = std::static_pointer_cast<RSNGSDFRRectShape>(rootShape);
    sdfRRectRootShape->Setter<SDFRRectShapeRRectTag>(rrect);
    return rootShape;
}

class MagnifierTestDemo {
public:
    MagnifierTestDemo()
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

        if (!InitBackgroundNode()) {
            std::cout << "Failed to init background node!" << std::endl;
            return;
        }

        // create canvasNode
        canvasNode_ = CreateDefaultCanvasNode();
        if (canvasNode_ == nullptr) {
            std::cout << "Failed to create union node!" << std::endl;
            return;
        }
        rootNode_->AddChild(canvasNode_, -1);

        // flush transaction
        transaction_ = uiContext_->GetRSTransaction();
        if (transaction_ == nullptr) {
            std::cout << "rs transaction is nullptr" << std::endl;
            return;
        }

        transaction_->FlushImplicitTransaction();
        std::cout << "rs magnifier demo initSuccess" << std::endl;
        initSuccess_ = true;
    }

    bool InitSurfaceNode()
    {
        if (rsUiDirector_ == nullptr) {
            return false;
        }
        RSSurfaceNodeConfig surfaceNodeConfig;
        surfaceNodeConfig.SurfaceNodeName = "magnifier_demo";
        RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
        std::cout << "RSSurfaceNode::Create" << std::endl;
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
        rsUiDirector_->SetRSSurfaceNode(surfaceNode_);
        return true;
    }

    bool InitRootNode()
    {
        if (rsUiDirector_ == nullptr) {
            return false;
        }
        rootNode_ = RSRootNode::Create(false, false, uiContext_);
        if (rootNode_ == nullptr) {
            std::cout << "Failed to create root node!" << std::endl;
            return false;
        }
        rootNode_->SetBounds(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        rootNode_->SetFrame(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        rsUiDirector_->SetRSRootNode(rootNode_->ReinterpretCastTo<RSRootNode>());
        return true;
    }

    bool InitBackgroundNode()
    {
        if (rootNode_ == nullptr) {
            return false;
        }
        backgroundNode_ = RSCanvasNode::Create(false, false, uiContext_);
        if (backgroundNode_ == nullptr) {
            std::cout << "Failed to create background node" << std::endl;
            return false;
        }
        backgroundNode_->SetBounds(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        backgroundNode_->SetFrame(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        backgroundNode_->SetBgImageWidth(WINDOW_WIDTH);
        backgroundNode_->SetBgImageHeight(WINDOW_HEIGHT);

        std::shared_ptr<Media::PixelMap> bgPixelMap =
            DecodePixelMap(DEFAULT_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
        if (bgPixelMap == nullptr) {
            std::cout << "Failed to decode background image" << std::endl;
            return false;
        }
        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetPixelMap(bgPixelMap);
        backgroundNode_->SetBgImage(rosenImage);
        rootNode_->AddChild(backgroundNode_, -1);
        return true;
    }

    ~MagnifierTestDemo() noexcept
    {
        if (initSuccess_) {
            surfaceNode_->DetachToDisplay(SCREEN_ID);
            transaction_->FlushImplicitTransaction();
        }
    }

    RSCanvasNode::SharedPtr CreateDefaultCanvasNode()
    {
        auto canvasNode = RSCanvasNode::Create(false, false, uiContext_);
        if (canvasNode == nullptr) {
            return nullptr;
        }
        canvasNode->SetBounds(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        canvasNode->SetFrame(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        canvasNode->SetClipToBounds(true);
        return canvasNode;
    }

    void TestMagnifier()
    {
        if (!initSuccess_) {
            std::cout << "rs magnifier demo init failed, can't run" << std::endl;
            return;
        }
        // build SDFShapeTree
        auto rRectShape = InitSDFRRectShape(RRECT_PARAMS[0]);
        if (!rRectShape) {
            std::cout << "Failed to create SDF RRect shape" << std::endl;
            return;
        }
        canvasNode_->SetSDFShape(rRectShape);
        canvasNode_->SetMaterialNGFilter(CreateDefaultMagnifierFilter());
        transaction_->FlushImplicitTransaction();
        rsUiDirector_->SendMessages();
        rsUiDirector_->FlushModifier();
        std::cout << "test magnifier success update" << std::endl;
        RSAnimationTimingProtocol protocol;
        auto timingCurve = RSAnimationTimingCurve::LINEAR;
        protocol.SetDuration(ANIMATION_TIME);
        RSNode::OpenImplicitAnimation(uiContext_, protocol, timingCurve, [this]() {
            std::cout << "animation finish callback" << std::endl;
        });
        for (size_t index = 1; index < RRECT_PARAMS.size(); index++) {
            auto rRectShapeNow = InitSDFRRectShape(RRECT_PARAMS[index]);
            if (!rRectShapeNow) {
                std::cout << "Failed to create SDF RRect shape for animation frame " << index << std::endl;
                continue;
            }
            RSNode::AddDurationKeyFrame(uiContext_, ANIMATION_INTERVAL, timingCurve, [&]() {
                canvasNode_->SetSDFShape(rRectShapeNow);
            });
        }
        RSNode::CloseImplicitAnimation(uiContext_);
        transaction_->FlushImplicitTransaction();
        sleep(WAIT_TIME);
    }

    void Run()
    {
        TestMagnifier();
    }

private:
    std::shared_ptr<RSUIDirector> rsUiDirector_;
    std::shared_ptr<RSUIContext> uiContext_;
    std::shared_ptr<RSTransactionHandler> transaction_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::shared_ptr<RSNode> rootNode_;
    RSCanvasNode::SharedPtr backgroundNode_;
    RSCanvasNode::SharedPtr canvasNode_;
    bool initSuccess_ = false;
};
} // namespace

int main()
{
    {
        MagnifierTestDemo demo;
        demo.Run();
    }
    return 0;
}