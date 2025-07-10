/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef FUNCTION_CANVAS_TEST_H
#define FUNCTION_CANVAS_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

/*
 * OH_Drawing_CanvasDrawLine接口绘制在画布外
 * OH_Drawing_CanvasDrawLine接口绘制在一半画布内一半画布外
 */
class FunctionCanvasDrawLine : public TestBase {
public:
    FunctionCanvasDrawLine()
    {
        fileName_ = "FunctionCanvasDrawLine";
    }
    ~FunctionCanvasDrawLine() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

/*
 * OH_Drawing_CanvasDrawRegion绘制在画布外
 * OH_Drawing_CanvasDrawRegion一半绘制在画布内一半绘制在画布外
 */
class FunctionCanvasDrawRegion : public TestBase {
public:
    FunctionCanvasDrawRegion()
    {
        fileName_ = "FunctionCanvasDrawRegion";
    }
    ~FunctionCanvasDrawRegion() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_CanvasDrawPoint点绘制在画布外
class FunctionCanvasDrawPoint : public TestBase {
public:
    FunctionCanvasDrawPoint()
    {
        fileName_ = "FunctionCanvasDrawPoint";
    }
    ~FunctionCanvasDrawPoint() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

/*
 * OH_Drawing_CanvasDrawPoints点绘制在画布外
 * OH_Drawing_CanvasDrawPoints点个数一半绘制在画布内一半绘制在画布外
 */
class FunctionCanvasDrawPoints : public TestBase {
public:
    FunctionCanvasDrawPoints()
    {
        fileName_ = "FunctionCanvasDrawPoints";
    }
    ~FunctionCanvasDrawPoints() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

/*
 * OH_Drawing_CanvasDrawOval绘制在画布外
 * OH_Drawing_CanvasDrawOval一半绘制在画布内一半绘制在画布外
 */
class FunctionCanvasDrawOval : public TestBase {
public:
    FunctionCanvasDrawOval()
    {
        fileName_ = "FunctionCanvasDrawOval";
    }
    ~FunctionCanvasDrawOval() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_CanvasClipRoundRect接口clipop传入INTERSECT
class FunctionCanvasClipRoundRect : public TestBase {
public:
    FunctionCanvasClipRoundRect()
    {
        fileName_ = "FunctionCanvasClipRoundRect";
    }
    ~FunctionCanvasClipRoundRect() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_CanvasClipRoundRect接口doAntialias遍历验证
class FunctionCanvasClipRoundRect2 : public TestBase {
public:
    FunctionCanvasClipRoundRect2()
    {
        fileName_ = "FunctionCanvasClipRoundRect2";
    }
    ~FunctionCanvasClipRoundRect2() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

/*
 * OH_Drawing_CanvasSkew参数sx设置为负数
 * OH_Drawing_CanvasSkew参数sy设置为正数
 * OH_Drawing_CanvasSkew参数sy设置为负数
 */
class FunctionCanvasSkew : public TestBase {
public:
    FunctionCanvasSkew()
    {
        fileName_ = "FunctionCanvasSkew";
    }
    ~FunctionCanvasSkew() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

/*
 * OH_Drawing_CanvasDrawImageRectWithSrc参数src设置在画布外
 * OH_Drawing_CanvasDrawImageRectWithSrc参数src设置为一半画布内一半画布外
 * OH_Drawing_CanvasDrawImageRectWithSrc参数dst设置在画布外
 * OH_Drawing_CanvasDrawImageRectWithSrc参数dst设置为一半画布内一半画布外
 */
class FunctionCanvasDrawImageRectWithSrc : public TestBase {
public:
    FunctionCanvasDrawImageRectWithSrc()
    {
        fileName_ = "FunctionCanvasDrawImageRectWithSrc";
        int32_t imageWidth = 600; //600 for test
        int32_t imageHeight = 600; //600 for test
        OH_Drawing_Image_Info imageInfo { imageWidth, imageHeight, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
        // 4 rgba
        srcPixels_ = new uint32_t[imageWidth * imageHeight];
        if (srcPixels_ == nullptr) {
            return;
        }
        for (int i = 0; i < imageWidth * imageHeight; i++) {
            // 3 for test
            if (i < imageWidth * imageHeight / 3) {
                srcPixels_[i] = 0xFF5500FF;
                // 3, 2 for test
            } else if (i < imageWidth * imageHeight * 2 / 3) {
                srcPixels_[i] = 0xFF0055FF;
            } else {
                srcPixels_[i] = 0xFFFF5500;
            }
        }
        bitmap1_ = OH_Drawing_BitmapCreateFromPixels(&imageInfo, srcPixels_, imageWidth * 4); // 4 for test
    }
    ~FunctionCanvasDrawImageRectWithSrc() override
    {
        delete[] srcPixels_;
        OH_Drawing_BitmapDestroy(bitmap1_);
    };

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
private:
    OH_Drawing_Bitmap* bitmap1_ = nullptr;
    uint32_t* srcPixels_ = nullptr;
};

/*
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SRC
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DST
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SRC_OVER
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DST_OVER
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SRC_IN
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DST_IN
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SRC_OUT
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DST_OUT
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SRC_ATOP
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DST_ATOP
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_XOR
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_PLUS
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_MODULATE
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SCREEN
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_OVERLAY
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DARKEN
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_LIGHTEN
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_COLOR_DODGE
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_COLOR_BURN
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_HARD_LIGHT
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SOFT_LIGHT
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_DIFFERENCE
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_EXCLUSION
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_MULTIPLY
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_HUE
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_SATURATION
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_COLOR
 * OH_Drawing_CanvasDrawVertices参数blendMode传BLEND_MODE_LUMINOSITY
 */
class FunctionCanvasDrawVertices : public TestBase {
public:
    explicit FunctionCanvasDrawVertices(OH_Drawing_BlendMode mode)
    {
        fileName_ = "FunctionCanvasDrawVertices";
        mode_ = mode;
    }
    ~FunctionCanvasDrawVertices() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    OH_Drawing_BlendMode mode_;
};

// OH_Drawing_CanvasIsClipEmpty参数isClipEmpty获取为true
class FunctionCanvasIsClipEmpty : public TestBase {
public:
    FunctionCanvasIsClipEmpty()
    {
        fileName_ = "FunctionCanvasIsClipEmpty";
    }
    ~FunctionCanvasIsClipEmpty() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_CanvasClipRegion参数clipop传入DIFFERENCE
class FunctionCanvasClipRegion : public TestBase {
public:
    FunctionCanvasClipRegion()
    {
        fileName_ = "FunctionCanvasClipRegion";
    }
    ~FunctionCanvasClipRegion() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

class FunctionCanvasQuickRejectPath : public TestBase {
public:
    FunctionCanvasQuickRejectPath()
    {
        fileName_ = "FunctionCanvasQuickRejectPath";
    };

    ~FunctionCanvasQuickRejectPath() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

class FunctionCanvasQuickRejectRect : public TestBase {
public:
    FunctionCanvasQuickRejectRect()
    {
        fileName_ = "FunctionCanvasQuickRejectRect";
    };

    ~FunctionCanvasQuickRejectRect() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

class FunctionCanvasDrawArcWithCenter : public TestBase {
public:
    FunctionCanvasDrawArcWithCenter()
    {
        fileName_ = "FunctionCanvasDrawArcWithCenter";
    };
    ~FunctionCanvasDrawArcWithCenter() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

class FunctionCanvasDrawNestedRoundRect : public TestBase {
public:
    FunctionCanvasDrawNestedRoundRect()
    {
        fileName_ = "FunctionCanvasDrawNestedRoundRect";
    };
    ~FunctionCanvasDrawNestedRoundRect() override{};

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};

class FunctionCanvasDrawPixelMapNine : public TestBase {
public:
    FunctionCanvasDrawPixelMapNine()
    {
        fileName_ = "FunctionCanvasDrawPixelMapNine";
    };
    ~FunctionCanvasDrawPixelMapNine() override{};

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // FUNCTION_CANVAS_TEST_H
