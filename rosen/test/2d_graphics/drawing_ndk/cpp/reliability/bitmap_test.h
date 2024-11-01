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

#ifndef RELIABILITY_BITMAP_TEST_H
#define RELIABILITY_BITMAP_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

// 销毁Bitmap后调用draw接口
class ReliabilityDeleteBitmap : public TestBase {
public:
    explicit ReliabilityDeleteBitmap(int type) : TestBase(type)
    {
        fileName_ = "ReliabilityDeleteBitmap";
    };
    ~ReliabilityDeleteBitmap() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// 销毁pixels后调用draw接口
class ReliabilityDeletePixel : public TestBase {
public:
    explicit ReliabilityDeletePixel(int type) : TestBase(type)
    {
        fileName_ = "ReliabilityDeletePixel";
    };
    ~ReliabilityDeletePixel() override {};

protected:
    int rectWidth_ = 300;
    int rectHeight_ = 300;
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// draw过程中，销毁Bitmap
class ReliabilityDeleteBitmapJoinThread : public TestBase {
public:
    explicit ReliabilityDeleteBitmapJoinThread(int type) : TestBase(type)
    {
        fileName_ = "ReliabilityDeleteBitmapJoinThread";
    };
    ~ReliabilityDeleteBitmapJoinThread() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// draw过程中，销毁pixels
class ReliabilityDeletePixelJoinThread : public TestBase {
public:
    explicit ReliabilityDeletePixelJoinThread(int type) : TestBase(type)
    {
        fileName_ = "ReliabilityDeletePixelJoinThread";
    };
    ~ReliabilityDeletePixelJoinThread() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

#endif // RELIABILITY_BITMAP_TEST_H