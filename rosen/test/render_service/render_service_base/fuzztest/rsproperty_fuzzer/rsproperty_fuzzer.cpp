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

#include "rsproperty_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"
#include "property/rs_property_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_INNER01 = 0;
const uint8_t DO_INNER02 = 1;
const uint8_t DO_INNER03 = 2;
const uint8_t DO_INNER04 = 3;
const uint8_t DO_PAINTER = 4;
const uint8_t TARGET_SIZE = 5;
}

void DoPropertiesInner01(FuzzedDataProvider& fdp)
{
    RSProperties properties;
    float x1 = fdp.ConsumeFloatingPoint<float>();
    float y1 = fdp.ConsumeFloatingPoint<float>();
    float z1 = fdp.ConsumeFloatingPoint<float>();
    float w1 = fdp.ConsumeFloatingPoint<float>();
    Vector4f bounds(x1, y1, z1, w1);
    float x2 = fdp.ConsumeFloatingPoint<float>();
    float y2 = fdp.ConsumeFloatingPoint<float>();
    Vector2f sizeVec(x2, y2);
    float width = fdp.ConsumeFloatingPoint<float>();
    float height = fdp.ConsumeFloatingPoint<float>();
    float x3 = fdp.ConsumeFloatingPoint<float>();
    float y3 = fdp.ConsumeFloatingPoint<float>();
    Vector2f position(x3, y3);
    float positionX = fdp.ConsumeFloatingPoint<float>();
    float positionY = fdp.ConsumeFloatingPoint<float>();
    float x4 = fdp.ConsumeFloatingPoint<float>();
    float y4 = fdp.ConsumeFloatingPoint<float>();
    float z4 = fdp.ConsumeFloatingPoint<float>();
    float w4 = fdp.ConsumeFloatingPoint<float>();
    Vector4f frame(x4, y4, z4, w4);
    float positionZ = fdp.ConsumeFloatingPoint<float>();
    float x5 = fdp.ConsumeFloatingPoint<float>();
    float y5 = fdp.ConsumeFloatingPoint<float>();
    Vector2f pivot(x5, y5);
    float pivotX = fdp.ConsumeFloatingPoint<float>();
    float pivotY = fdp.ConsumeFloatingPoint<float>();
    float x6 = fdp.ConsumeFloatingPoint<float>();
    float y6 = fdp.ConsumeFloatingPoint<float>();
    float z6 = fdp.ConsumeFloatingPoint<float>();
    float w6 = fdp.ConsumeFloatingPoint<float>();
    Vector4f cornerRadius(x6, y6, z6, w6);
    properties.SetBounds(bounds);
    properties.SetBoundsSize(sizeVec);
    properties.SetBoundsWidth(width);
    properties.SetBoundsHeight(height);
    properties.SetBoundsPosition(position);
    properties.SetBoundsPositionX(positionX);
    properties.SetBoundsPositionY(positionY);
    properties.SetFrame(frame);
    properties.SetFrameSize(sizeVec);
    properties.SetFrameWidth(width);
    properties.SetFrameHeight(height);
    properties.SetFramePosition(position);
    properties.SetFramePositionX(positionX);
    properties.SetFramePositionY(positionY);
    properties.SetPositionZ(positionZ);
    properties.SetPivot(pivot);
    properties.SetPivotX(pivotX);
    properties.SetPivotY(pivotY);
    properties.SetCornerRadius(cornerRadius);
}

void DoPropertiesInner02(FuzzedDataProvider& fdp)
{
    RSProperties properties;
    float x7 = fdp.ConsumeFloatingPoint<float>();
    float y7 = fdp.ConsumeFloatingPoint<float>();
    float z7 = fdp.ConsumeFloatingPoint<float>();
    float w7 = fdp.ConsumeFloatingPoint<float>();
    Quaternion quaternion(x7, y7, z7, w7);
    float degree = fdp.ConsumeFloatingPoint<float>();
    float x8 = fdp.ConsumeFloatingPoint<float>();
    float y8 = fdp.ConsumeFloatingPoint<float>();
    Vector2f translateVector(x8, y8);
    float translate = fdp.ConsumeFloatingPoint<float>();
    float x9 = fdp.ConsumeFloatingPoint<float>();
    float y9 = fdp.ConsumeFloatingPoint<float>();
    Vector2f scale(x9, y9);
    float x10 = fdp.ConsumeFloatingPoint<float>();
    float y10 = fdp.ConsumeFloatingPoint<float>();
    float z10 = fdp.ConsumeFloatingPoint<float>();
    float scaleZ = fdp.ConsumeFloatingPoint<float>();
    Vector3f skew(x10, y10, z10);
    float x11 = fdp.ConsumeFloatingPoint<float>();
    float y11 = fdp.ConsumeFloatingPoint<float>();
    float z11 = fdp.ConsumeFloatingPoint<float>();
    float w11 = fdp.ConsumeFloatingPoint<float>();
    Vector4f persp(x11, y11, z11, w11);
    float alpha = fdp.ConsumeFloatingPoint<float>();
    bool alphaOffscreen = fdp.ConsumeBool();
    int16_t red1 = fdp.ConsumeIntegral<int16_t>();
    int16_t green1 = fdp.ConsumeIntegral<int16_t>();
    int16_t blue1 = fdp.ConsumeIntegral<int16_t>();
    Color color1(red1, green1, blue1);
    std::shared_ptr<RSShader> shader = RSShader::CreateRSShader();
    int16_t red2 = fdp.ConsumeIntegral<int16_t>();
    int16_t green2 = fdp.ConsumeIntegral<int16_t>();
    int16_t blue2 = fdp.ConsumeIntegral<int16_t>();
    Color color2(red2, green2, blue2);
    int16_t red3 = fdp.ConsumeIntegral<int16_t>();
    int16_t green3 = fdp.ConsumeIntegral<int16_t>();
    int16_t blue3 = fdp.ConsumeIntegral<int16_t>();
    Color color3(red3, green3, blue3);
    int16_t red4 = fdp.ConsumeIntegral<int16_t>();
    int16_t green4 = fdp.ConsumeIntegral<int16_t>();
    int16_t blue4 = fdp.ConsumeIntegral<int16_t>();
    Color color4(red4, green4, blue4);
    Vector4<Color> colorVector(color1, color2, color3, color4);

    properties.SetQuaternion(quaternion);
    properties.SetRotation(degree);
    properties.SetRotationX(degree);
    properties.SetRotationY(degree);
    properties.SetTranslate(translateVector);
    properties.SetTranslateX(translate);
    properties.SetTranslateY(translate);
    properties.SetTranslateZ(translate);
    properties.SetScale(scale);
    properties.SetScaleZ(scaleZ);
    properties.SetSkew(skew);
    properties.SetPersp(persp);
    properties.SetAlpha(alpha);
    properties.SetAlphaOffscreen(alphaOffscreen);
    properties.SetForegroundColor(color1);
    properties.SetBackgroundColor(color2);
    properties.SetBackgroundShader(shader);
    properties.SetBorderColor(colorVector);
}

void DoPropertiesInner03(FuzzedDataProvider& fdp)
{
    RSProperties properties;
    float x10 = fdp.ConsumeFloatingPoint<float>();
    float y10 = fdp.ConsumeFloatingPoint<float>();
    float z10 = fdp.ConsumeFloatingPoint<float>();
    float w10 = fdp.ConsumeFloatingPoint<float>();
    Vector4f widthVector(x10, y10, z10, w10);
    uint32_t x11 = fdp.ConsumeIntegral<uint32_t>();
    uint32_t y11 = fdp.ConsumeIntegral<uint32_t>();
    uint32_t z11 = fdp.ConsumeIntegral<uint32_t>();
    uint32_t w11 = fdp.ConsumeIntegral<uint32_t>();
    Vector4<uint32_t> style(x11, y11, z11, w11);
    float blurRadiusX = fdp.ConsumeFloatingPoint<float>();
    float blurRadiusY = fdp.ConsumeFloatingPoint<float>();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    int styleInt = fdp.ConsumeIntegral<int>();
    float dipScale = fdp.ConsumeFloatingPoint<float>();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateMaterialFilter(styleInt, dipScale);
    float offsetX = fdp.ConsumeFloatingPoint<float>();
    float offsetY = fdp.ConsumeFloatingPoint<float>();
    float radius = fdp.ConsumeFloatingPoint<float>();
    std::shared_ptr<RSPath> shadowpath = std::make_shared<RSPath>();
    Gravity gravity = static_cast<Gravity>(fdp.ConsumeIntegral<int32_t>());
    int intLeft = fdp.ConsumeIntegral<int>();
    int intTop = fdp.ConsumeIntegral<int>();
    int intWidth = fdp.ConsumeIntegral<int>();
    int intHeight = fdp.ConsumeIntegral<int>();
    std::shared_ptr<RectI> rect = std::make_shared<RectI>(intLeft, intTop, intWidth, intHeight);
    int16_t red1 = fdp.ConsumeIntegral<int16_t>();
    int16_t green1 = fdp.ConsumeIntegral<int16_t>();
    int16_t blue1 = fdp.ConsumeIntegral<int16_t>();
    Color color1(red1, green1, blue1);

    properties.SetBorderWidth(widthVector);
    properties.SetBorderDashWidth(widthVector);
    properties.SetBorderDashGap(widthVector);
    properties.SetOutlineDashWidth(widthVector);
    properties.SetOutlineDashGap(widthVector);
    properties.SetBorderStyle(style);
    properties.SetShadowColor(color1);
    properties.SetShadowOffsetX(offsetX);
    properties.SetShadowOffsetY(offsetY);
    properties.SetShadowElevation(radius);
    properties.SetShadowRadius(radius);
    properties.SetShadowPath(shadowpath);
    properties.SetFrameGravity(gravity);
}

void DoPropertiesInner04(FuzzedDataProvider& fdp)
{
    RSProperties properties;
    float sx = fdp.ConsumeFloatingPoint<float>();
    float sy = fdp.ConsumeFloatingPoint<float>();
    std::shared_ptr<RSImage> image = std::make_shared<RSImage>();
    float width = fdp.ConsumeFloatingPoint<float>();
    float height = fdp.ConsumeFloatingPoint<float>();
    float positionX = fdp.ConsumeFloatingPoint<float>();
    float positionY = fdp.ConsumeFloatingPoint<float>();
    bool clipToBounds = fdp.ConsumeBool();
    bool clipToFrame = fdp.ConsumeBool();
    bool visible = fdp.ConsumeBool();
    float distortionK = fdp.ConsumeFloatingPoint<float>();
    bool distortionDirty = fdp.ConsumeBool();

    properties.SetScaleX(sx);
    properties.SetScaleY(sy);
    properties.SetBgImage(image);
    properties.SetBgImageWidth(width);
    properties.SetBgImageHeight(height);
    properties.SetBgImagePositionX(positionX);
    properties.SetBgImagePositionY(positionY);
    properties.SetClipToBounds(clipToBounds);
    properties.SetClipToFrame(clipToFrame);
    properties.SetVisible(visible);
    properties.SetDistortionK(distortionK);
    properties.SetDistortionDirty(distortionDirty);
}

void DoPropertiesPainter(FuzzedDataProvider& fdp)
{
    Drawing::Canvas tmpCanvas;
    float fLeft = fdp.ConsumeFloatingPoint<float>();
    float fTop = fdp.ConsumeFloatingPoint<float>();
    float fWidth = fdp.ConsumeFloatingPoint<float>();
    float fHeight = fdp.ConsumeFloatingPoint<float>();
    RectF rect(fLeft, fTop, fWidth, fHeight);
    RSProperties properties;
    int iLeft = fdp.ConsumeIntegral<int>();
    int iTop = fdp.ConsumeIntegral<int>();
    int iWidth = fdp.ConsumeIntegral<int>();
    int iHeight = fdp.ConsumeIntegral<int>();
    RectI dirtyShadow(iLeft, iTop, iWidth, iHeight);
    float skLeft = fdp.ConsumeFloatingPoint<float>();
    float skTop = fdp.ConsumeFloatingPoint<float>();
    float skRight = fdp.ConsumeFloatingPoint<float>();
    float skBottom = fdp.ConsumeFloatingPoint<float>();
    Drawing::Rect maskBounds { skLeft, skTop, skRight, skBottom };
    Drawing::Matrix mat;
    Gravity gravity = static_cast<Gravity>(fdp.ConsumeIntegral<int32_t>());
    float fW = fdp.ConsumeFloatingPoint<float>();
    float fH = fdp.ConsumeFloatingPoint<float>();

    RSPropertiesPainter::Clip(tmpCanvas, rect);
    RSPropertiesPainter::DrawBorder(properties, tmpCanvas);
    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties);
    RSPropertiesPainter::DrawMask(properties, tmpCanvas);
    RSPropertiesPainter::DrawMask(properties, tmpCanvas, maskBounds);
    RSPropertiesPainter::GetGravityMatrix(gravity, rect, fW, fH, mat);
    RSPropertiesPainter::Rect2DrawingRect(rect);
}

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_INNER01:
            OHOS::Rosen::DoPropertiesInner01(fdp);
            break;
        case OHOS::Rosen::DO_INNER02:
            OHOS::Rosen::DoPropertiesInner02(fdp);
            break;
        case OHOS::Rosen::DO_INNER03:
            OHOS::Rosen::DoPropertiesInner03(fdp);
            break;
        case OHOS::Rosen::DO_INNER04:
            OHOS::Rosen::DoPropertiesInner04(fdp);
            break;
        case OHOS::Rosen::DO_PAINTER:
            OHOS::Rosen::DoPropertiesPainter(fdp);
            break;
        default:
            break;
    }
    return 0;
}
