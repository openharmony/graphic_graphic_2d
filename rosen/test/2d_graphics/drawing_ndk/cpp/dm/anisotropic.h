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
#ifndef ANISOTROPIC_H
#define ANISOTROPIC_H
#include "../test_base.h"
#include "test_common.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_pen.h>

class Anisotropic : public TestBase {
    uint32_t kImageSize = 256;
    uint32_t kSpacer = 10;
    uint32_t kNumVertImages = 5;
    int kNumLines = 100;
    float kAngleStep = 360.0f / kNumLines;
    int kInnerOffset = 10;
    int xOff;
    int yOff;
    float angle = 0.0f;

public:
    Anisotropic()
    {
        bitmapWidth_ = 2 * kImageSize + 3 * kSpacer;                                  // 2,3宽度
        bitmapHeight_ = kNumVertImages * kImageSize + (kNumVertImages + 1) * kSpacer; // 1高度
        fileName_ = "anisotropic_hq";
    }
    ~Anisotropic() override;

protected:
    OH_Drawing_Image *image = OH_Drawing_ImageCreate();
    OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_Canvas *bimap_canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_Rect *rect;
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void BeforeDraw(OH_Drawing_Canvas *canvas);
    void Draw(OH_Drawing_Canvas *canvas, int x, int y, int xSize, int ySize);
};

#endif // ANISOTROPIC_H
