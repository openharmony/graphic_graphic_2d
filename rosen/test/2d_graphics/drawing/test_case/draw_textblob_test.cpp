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
#include "draw_textblob_test.h"

namespace OHOS {
namespace Rosen {
using namespace Drawing;
void DrawTextBlobTest::OnTestFunction(Drawing::Canvas* canvas)
{
    Font font = Font();
    font.SetSize(60); // text size 60
    std::shared_ptr<TextBlob> textblob = TextBlob::MakeFromString("Hello",
        font, TextEncoding::UTF8);
    // 1. test pen + BlurDrawLooper
    Pen pen;
    pen.SetWidth(2.f);
    pen.SetColor(0xFFFF0000); // color:red
    canvas->AttachPen(pen);
    canvas->DrawTextBlob(textblob.get(), 100, 100); // Screen coordinates(100, 100)
    canvas->DetachPen();

    auto blurDrawLooper = BlurDrawLooper::CreateBlurDrawLooper(3.f,
        -3.f, 3.f, Color{0xFF00FF00}); // radius:3.f, offset(-3.f, 3.f), color:green
    pen.SetLooper(blurDrawLooper);
    canvas->AttachPen(pen);
    canvas->DrawTextBlob(textblob.get(), 100, 200); // Screen coordinates(100, 200)
    canvas->DetachPen();

    // 2. test brush + BlurDrawLooper
    Brush brush;
    brush.SetColor(0xFFFF0000); // color:red
    canvas->AttachBrush(brush);
    canvas->DrawTextBlob(textblob.get(), 300, 100); // Screen coordinates(300, 100)
    canvas->DetachBrush();

    brush.SetLooper(blurDrawLooper);
    canvas->AttachBrush(brush);
    canvas->DrawTextBlob(textblob.get(), 300, 200); // Screen coordinates(300, 200)
    canvas->DetachBrush();

    // 3. test pen + brush + BlurDrawLooper
    pen.SetLooper(nullptr);
    brush.SetLooper(nullptr);
    canvas->AttachPen(pen);
    canvas->AttachBrush(brush);
    canvas->DrawTextBlob(textblob.get(), 500, 100); // Screen coordinates(500, 100)
    canvas->DetachBrush();
    canvas->DetachPen();

    pen.SetLooper(blurDrawLooper);
    brush.SetLooper(blurDrawLooper);
    canvas->AttachPen(pen);
    canvas->AttachBrush(brush);
    canvas->DrawTextBlob(textblob.get(), 500, 200); // Screen coordinates(500, 200)
    canvas->DetachBrush();
    canvas->DetachPen();

    // 4. test pen + BlurDrawLooper, brush + BlurDrawLooper2, can not combine paint
    auto blurDrawLooper2 = BlurDrawLooper::CreateBlurDrawLooper(3.f,
        -3.f, 3.f, Color{0xFF00FF00}); // radius:3.f, offset(-3.f, 3.f), color:green
    pen.SetLooper(blurDrawLooper);
    brush.SetLooper(blurDrawLooper2);
    canvas->AttachPen(pen);
    canvas->AttachBrush(brush);
    canvas->DrawTextBlob(textblob.get(), 500, 300); // Screen coordinates(500, 300)
    canvas->DetachBrush();
    canvas->DetachPen();
}

void DrawTextBlobTest::OnTestPerformance(Drawing::Canvas* canvas)
{
    Font font = Font();
    font.SetSize(60); // text size 60
    std::shared_ptr<TextBlob> textblob = TextBlob::MakeFromString("Hello",
        font, TextEncoding::UTF8);
    // test drawtextblob performance with BlurDrawLooper
    Brush brush;
    brush.SetColor(0xFFFF0000); // color:red
    auto blurDrawLooper = BlurDrawLooper::CreateBlurDrawLooper(3.f,
        3.f, 3.f, Color{0xFF00FF00}); // radius:3.f, offset(3.f, 3.f), color:green
    brush.SetLooper(blurDrawLooper);
    canvas->AttachBrush(brush);
    for (int i = 0; i < testCount_; i++) {
        canvas->DrawTextBlob(textblob.get(), 300, 200); // Screen coordinates(300, 200)
    }
    canvas->DetachBrush();
}
} // namespace Rosen
} // namespace OHOS