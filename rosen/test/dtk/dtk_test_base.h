/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef DDGR_TEST_DTK_DTK_TEST_BASE_H
#define DDGR_TEST_DTK_DTK_TEST_BASE_H

#include "recording/recording_canvas.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

namespace OHOS {
namespace Rosen {
using TestPlaybackCanvas = Drawing::Canvas;

class TestBase {
public:
    TestBase() {};
    virtual ~TestBase() = default;
    void Recording();
    void SetTestCount(int testCount);
    void SetCanvas(Drawing::Canvas* canvas);
    void SetSurface(Drawing::Surface* surface);

    void AddTestBrush(bool isAA = true);
    void AddTestPen(bool isAA = true);
    void AddTestPen(bool isAA, float width);
    void AddTestPen(bool isAA, Drawing::Pen::CapStyle capStyle);
    void AddTestPen(bool isAA, Drawing::Pen::JoinStyle joinStyle);
    void AddTestPen(bool isAA, Drawing::Pen::CapStyle capStyle, Drawing::Pen::JoinStyle joinStyle);
    void AddTestBrushAndPen(bool isAA = true);

    void ClearTestBrush();
    void ClearTestPen();
    void ClearTestBrushAndPen();

    void ClipPath(bool doAntiAlias);
    void ClipRect(bool doAntiAlias);
    void ClipRoundRectG2(bool doAntiAlias);
    void ClipRoundRectG2capsule(bool doAntiAlias);
    void ClipRoundRectnotG2(bool doAntiAlias);

    std::shared_ptr<Drawing::Image> GetEffectTestImage(const std::string& pathName);
    std::shared_ptr<Drawing::Surface> GetNewColorSpaceSurface();
    std::shared_ptr<Drawing::RuntimeEffect> GetTestRuntimeEffectForShader(const char* glsl);
    std::shared_ptr<Drawing::RuntimeEffect> GetTestRuntimeEffectForBlender(const char* glsl);

protected:
    virtual void OnRecording() = 0;

    std::shared_ptr<Drawing::Image> OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap);
    void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height);
    using MakeImageFunc = std::function<void(TestPlaybackCanvas* canvas, int w, int h)>;
    std::shared_ptr<Drawing::Image> MakeImage(int w, int h, MakeImageFunc func);

    TestPlaybackCanvas* playbackCanvas_ = nullptr;
    Drawing::Surface* mSurface = nullptr;
    int testCount_ = 50;
    int width_ = 1000;
    int height_ = 1000;

    std::shared_ptr<RSNode> rootNode_ = nullptr;
    std::shared_ptr<RSCanvasNode> canvasNode_ = nullptr;

private:
    std::unordered_map<std::string, std::shared_ptr<Drawing::Image>> effectTestImageMap_;
    std::unordered_map<size_t, std::shared_ptr<Drawing::RuntimeEffect>> runtimeEffectMap_;
    std::shared_ptr<Drawing::Surface> csSurface_;
};
} // namespace Rosen
} // namespace OHOS
#endif // DDGR_TEST_DTK_DTK_TEST_BASE_H