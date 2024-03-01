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
#include <surface.h>

#include "command/rs_base_node_command.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"

#include "pipeline/rs_render_result.h"
#include "pipeline/rs_render_thread.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "render_context/render_context.h"
// temporary debug
#include "foundation/graphic/graphic_2d/rosen/modules/render_service_base/src/platform/ohos/rs_surface_frame_ohos.h"
#include "foundation/graphic/graphic_2d/rosen/modules/render_service_base/src/platform/ohos/rs_surface_ohos.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

namespace OHOS::Rosen {
#ifdef ACE_ENABLE_GPU
    RenderContext* rc_ = nullptr;
#endif
constexpr int SURFACE_NODE_SIZE = 100;

namespace pipelineTestUtils {
    constexpr bool wrongExit = false;
    constexpr bool successExit = false;

    class ToDrawSurface {
    public:
        using drawFun = std::function<void(Drawing::Canvas&, Drawing::Brush&)>;
        ToDrawSurface()
        {
            // Do not hold it. Use it As ToDrawSurface::Sample().
        };

        inline ToDrawSurface& SetSurfaceNode(const std::shared_ptr<RSSurfaceNode> surfaceNode)
        {
            surfaceNode_ = surfaceNode;
            return *this;
        }

        inline ToDrawSurface& SetSurfaceNodeSize(Drawing::Rect surfaceGeometry)
        {
            surfaceGeometry_ = surfaceGeometry;
            return *this;
        }

        inline ToDrawSurface& SetBufferSize(int width, int height)
        {
            bufferSize_ = Drawing::Rect(0, 0, width, height);
            return *this;
        }

        inline ToDrawSurface& SetBufferSizeAuto()
        {
            bufferSize_ = surfaceGeometry_;
            return *this;
        }

        inline ToDrawSurface& SetBufferSize(Drawing::Rect bufferSize)
        {
            // bufferSize has no XY
            bufferSize_ = bufferSize;
            return *this;
        }

        inline ToDrawSurface& SetShapeColor(uint32_t color)
        {
            color_ = color;
            return *this;
        }

        inline ToDrawSurface& SetDraw(drawFun drawShape)
        {
            drawShape_ = drawShape;
            return *this;
        }

        bool Run()
        {
            if (surfaceNode_ == nullptr) {
                return false;
            }
            auto x = surfaceGeometry_.GetLeft();
            auto y = surfaceGeometry_.GetTop();
            auto width = surfaceGeometry_.GetWidth();
            auto height = surfaceGeometry_.GetHeight();
            surfaceNode_->SetBounds(x, y, width, height);
            std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode_);
            if (rsSurface == nullptr) {
                return wrongExit;
            }
#ifdef ACE_ENABLE_GPU
            // SetRenderContext must before rsSurface->RequestFrame, or it will failed.
            if (rc_) {
                rsSurface->SetRenderContext(rc_);
            } else {
                cout << "DrawSurface: RenderContext is nullptr\n";
            }
#endif
            auto framePtr = rsSurface->RequestFrame(bufferSize_.GetWidth(), bufferSize_.GetHeight());
            if (!framePtr) {
                // SetRenderContext must before rsSurface->RequestFrame,
                //      or frameptr will be nullptr.
                cout << "DrawSurface: frameptr is nullptr\n";
                return wrongExit;
            }
            auto canvas = framePtr->GetCanvas();
            if (!canvas) {
                cout << "DrawSurface: canvas is nullptr\n";
                return wrongExit;
            }
            canvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
            Drawing::Brush brush;
            brush.SetAntiAlias(true);
            brush.SetColor(color_);
            if (!drawShape_) {
                cout << "DrawSurface: drawShape_ is nullptr\n";
                return wrongExit;
            }
            drawShape_(*(canvas), brush);
            framePtr->SetDamageRegion(0, 0, surfaceGeometry_.GetWidth(), surfaceGeometry_.GetHeight());
            rsSurface->FlushFrame(framePtr);
            return successExit;
        }
    private:
        Drawing::Rect surfaceGeometry_ = {0.f, 0.f, 0.f, 0.f};
        Drawing::Rect bufferSize_ = {0.f, 0.f, 0.f, 0.f};
        drawFun drawShape_;
        uint32_t color_ = 0;
        std::shared_ptr<RSSurfaceNode> surfaceNode_ = nullptr;
    }; // class ToDrawSurface

    static std::shared_ptr<RSSurfaceNode> CreateSurface(int surfaceNodeX, int surfaceNodeY,
        int surfaceNodeWidth, int surfaceNodeHeight, uint32_t shapeColor)
    {
        RSSurfaceNodeConfig config;
        auto surfaceNode = RSSurfaceNode::Create(config);

        ToDrawSurface()
            .SetSurfaceNode(surfaceNode)
            .SetShapeColor(shapeColor)
            .SetSurfaceNodeSize(Drawing::Rect(surfaceNodeX, surfaceNodeY,
                surfaceNodeWidth + surfaceNodeX, surfaceNodeHeight + surfaceNodeY))
            .SetBufferSizeAuto()
            .SetDraw([&](Drawing::Canvas &canvas, Drawing::Brush &brush) -> void {
                canvas.AttachBrush(brush);
                canvas.drawRect(Drawing::Rect(0, 0, surfaceNodeWidth, surfaceNodeHeight));
                canvas.DetachBrush();
            })
            .Run();

        return surfaceNode;
    }

    static std::shared_ptr<RSDisplayNode> CreateDisplayNode(std::shared_ptr<RSSurfaceNode> surfaceNode)
    {
        RSDisplayNodeConfig displayConfig;
        RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(displayConfig);
        displayNode->AddChild(surfaceNode, -1);
        return displayNode;
    }
} // namespace pipelineTestUtils

// This test case is designed for temporary result demonstration.
// It could be removed after DMS implements this feature.
class RSDisplayModeTestCase {
public:
    inline static RSDisplayModeTestCase& GetInstance()
    {
        static RSDisplayModeTestCase c;
        return c;
    }

    void GetOrSetDisplayMode(RSDisplayNode::SharedPtr targetDisplayNode, RSDisplayNode::SharedPtr sourceDisplayNode)
    {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        while (true) {
            int testType = -1;
            int param = -1;

            cout << " ---------------------loop start------------------------" << endl;
            cout << "Input test type, 1: Get, 0: Set:, Others: End Test " << endl;
            cin >> testType;
            if (testType != 1 && testType != 0) {
                return;
            }
            if (testType == 0) {
                cout << "Input param, 1: is mirror, 0: not mirror: " << endl;
                cin >> param;
            }
            cout << " " << endl;

            if (testType == 1) {
                auto isMirrorDisplay = targetDisplayNode->IsMirrorDisplay() ? "is mirror" : "not mirror";
                cout << "Get display mode result: "<< isMirrorDisplay << endl;
            } else if (testType == 0) {
                bool isMirrored = param == 1 ? true : false;
                RSDisplayNodeConfig config = {0, isMirrored, sourceDisplayNode->GetId()};
                cout << "Set display mode "<< isMirrored << " , source node id " << config.mirrorNodeId << endl;
                targetDisplayNode->SetDisplayNodeMirrorConfig(config);
            }

            if (transactionProxy != nullptr) {
                transactionProxy->FlushImplicitTransaction();
            }
        }
        return;
    }

    void InitTestCase()
    {
        ScreenId id = RSInterfaces::GetInstance().GetDefaultScreenId();
        auto activeModeInfo = RSInterfaces::GetInstance().GetScreenActiveMode(id);

        screenWidth_ = activeModeInfo.GetScreenWidth();
        screenHeight_ = activeModeInfo.GetScreenHeight();
        std::cout << "Display " << id << " active mode info:\n";
        std::cout << "Width: " << screenWidth_ << ", Height: " << screenHeight_;

        RenderContextInit();
    }

    void TestCaseDefault()
    {
        auto sourceSurcaseNode = pipelineTestUtils::CreateSurface(SURFACE_NODE_SIZE, screenHeight_ * 0.2f,
            screenWidth_ * 0.4f, screenHeight_ * 0.3f, 0xff0000ff);
        auto sourceDisplayNode = pipelineTestUtils::CreateDisplayNode(sourceSurcaseNode);
        auto targetSurfaceNode = pipelineTestUtils::CreateSurface(SURFACE_NODE_SIZE, screenHeight_ * 0.6f,
            screenWidth_ * 0.6f, screenHeight_ * 0.3f, 0xffff0000);
        auto targetDisplayNode = pipelineTestUtils::CreateDisplayNode(targetSurfaceNode);

        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }

        GetOrSetDisplayMode(targetDisplayNode, sourceDisplayNode);

        sourceDisplayNode->RemoveFromTree();
        targetDisplayNode->RemoveFromTree();

        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }
    }
private:
    RSDisplayModeTestCase() = default;
    void RenderContextInit()
    {
#ifdef ACE_ENABLE_GPU
        cout << "ACE_ENABLE_GPU is true. \n";
        cout << "Init RenderContext start. \n";
            rc_ = RenderContextFactory::GetInstance().CreateEngine();
            if (rc_) {
                cout << "Init RenderContext success.\n";
                rc_->InitializeEglContext();
            } else {
                cout << "Init RenderContext failed, RenderContext is nullptr.\n";
            }
        cout << "Init RenderContext start.\n";
#endif
    }
    int screenWidth_ = 0;
    int screenHeight_ = 0;
}; // class RSDisplayModeTestCase
} // namespace OHOS::Rosen

int main()
{
    RSDisplayModeTestCase::GetInstance().InitTestCase();
    RSDisplayModeTestCase::GetInstance().TestCaseDefault();
    return 0;
}
