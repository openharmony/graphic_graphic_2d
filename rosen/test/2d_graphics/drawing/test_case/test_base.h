/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef DRAWING_DEMO_TEST_BASE_H
#define DRAWING_DEMO_TEST_BASE_H
#include "pixel_map.h"
#include "recording/recording_canvas.h"

namespace OHOS {
namespace Rosen {
#define DEFAULT_WIDTH 720
#define DEFAULT_HEIGHT 720
#define DEFAULT_BACKGROUND_COLOR 0xFFFFFFFF
#define DEFAULT_TEST_COUNT 1

using TestDisplayCanvas = Drawing::RecordingCanvas;

class TestBase {
public:
    TestBase(){};
    virtual ~TestBase() = default;
    void SetCanvas(TestDisplayCanvas* canvas);
    void SetFileName(std::string fileName);
    void SetTestCount(uint32_t count);
    void TestFunctionCpu();
    void TestPerformanceCpu();
    void TestFunctionGpuUpScreen();
    void TestPerformanceGpuUpScreen();
    virtual void OnTestFunction(Drawing::Canvas* canvas) {};
    virtual void OnTestPerformance(Drawing::Canvas* canvas) {};
    virtual void OnTestFunctionCpu(Drawing::Canvas* canvas)
    {
        return OnTestFunction(canvas);
    };
    virtual void OnTestPerformanceCpu(Drawing::Canvas* canvas)
    {
        return OnTestPerformance(canvas);
    };
    virtual void OnTestFunctionGpuUpScreen(Drawing::Canvas* canvas)
    {
        return OnTestFunction(canvas);
    };
    virtual void OnTestPerformanceGpuUpScreen(Drawing::Canvas* canvas)
    {
        return OnTestPerformance(canvas);
    };

protected:
    int CreateBitmapCanvas();
    int DiasplayToScreen();
    void LogStart();
    void LogEnd();

    TestDisplayCanvas* playbackCanvas_ = nullptr;

    int width_ = DEFAULT_WIDTH;
    int height_ = DEFAULT_HEIGHT;
    uint32_t background_ = DEFAULT_BACKGROUND_COLOR;
    std::string fileName_ = "default";
    uint32_t testCount_ = DEFAULT_TEST_COUNT;

    Drawing::Bitmap bitmap_;
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap_;
    std::shared_ptr<Drawing::Canvas> bitmapCanvas_;
    std::chrono::high_resolution_clock::time_point timeStart_;
};
} // namespace Rosen
} // namespace OHOS
#endif // DRAWING_DEMO_TEST_BASE_H