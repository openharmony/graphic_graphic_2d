/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_SAMPLES_TEXGINE_SKIA_FRAMEWORK_H
#define ROSEN_SAMPLES_TEXGINE_SKIA_FRAMEWORK_H

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <include/core/SkMatrix.h>
#include <include/core/SkPoint3.h>
#include <include/core/SkTypeface.h>

#include <ui/rs_surface_node.h>

#include "drawing.h"

class SkBitmap;
class SkCanvas;
class SkFont;
class SkPaint;
class PointerFilter;

class SkiaFramework {
public:
    using DrawFunc = std::function<void(RSCanvas &canvas)>;
    using ResizeFunc = std::function<void(int width, int height)>;

    SkiaFramework();
    virtual ~SkiaFramework();

    // Draw function is nullptr by default.
    void SetDrawFunc(const DrawFunc &onDraw);

    // Resize function is nullptr by default.
    void SetResizeFunc(const ResizeFunc &onResize);

    // Allow GPU by default.
    void SetGPUAllowance(bool allow);

    // Window width is 800 by default.
    void SetWindowWidth(int width);

    // Window height is 640 by default.
    void SetWindowHeight(int height);

    // Window scale is 1 by default.
    void SetWindowScale(double scale);

    // Window title is "skia framework" by default.
    void SetWindowTitle(const std::string &title);

    int GetWindowWidth() const;
    int GetWindowHeight() const;
    double GetWindowScale() const;
    void Run();

    static void DrawString(RSCanvas &canvas, const std::string &str, double x, double y);
    virtual void DrawBefore(RSCanvas &canvas);
    virtual void DrawAfter(RSCanvas &canvas);
    void DrawColorPicker(RSCanvas &canvas, RSDrawing::Bitmap &bitmap);

private:
    friend class PointerFilter;
    void UIThreadMain();
    void UpdateInvertMatrix();
    void ProcessBitmap(RSDrawing::Bitmap &bitmap, const OHOS::sptr<OHOS::SurfaceBuffer> buffer);
    void PrepareVsyncFunc();
    void DrawPathAndString(RSCanvas &canvas, RSFont &font, RSDrawing::Pen &paint1, RSDrawing::Pen &paint2);

    DrawFunc onDraw_ = nullptr;
    ResizeFunc onResize_ = nullptr;
    bool allowGPU_ = true;
    int windowWidth_ = 800;
    int windowHeight_ = 640;
    double windowScale_ = 1;
    std::string windowTitle_ = "skia framework";

    void *data_ = nullptr;

    std::mutex propsMutex_;
    std::atomic<int> x_ = 0;
    std::atomic<int> y_ = 0;
    std::atomic<bool> left_ = false;
    std::atomic<bool> right_ = false;
    std::atomic<bool> dirty_ = true;
    std::atomic<int> downRX_ = 0;
    std::atomic<int> downRY_ = 0;
    std::atomic<int> diffx_ = 0;
    std::atomic<int> diffy_ = 0;
    std::atomic<int> clickx_ = 0;
    std::atomic<int> clicky_ = 0;
    std::atomic<int> scalex_ = 0;
    std::atomic<int> scaley_ = 0;
    std::atomic<double> scalediff_ = 0;
    RSDrawing::Matrix scaleMat_;
    RSDrawing::Matrix mat_;
    RSDrawing::Matrix invmat_;
};

#endif // ROSEN_SAMPLES_TEXGINE_SKIA_FRAMEWORK_H
