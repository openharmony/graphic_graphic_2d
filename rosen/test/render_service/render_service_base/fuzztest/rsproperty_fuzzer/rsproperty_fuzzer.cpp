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

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"
#include "property/rs_property_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

/*
 * get a string from g_data
 */
std::string GetStringFromData(int strlen)
{
    if (strlen <= 0) {
        return "fuzz";
    }
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        char tmp = GetData<char>();
        if (tmp == '\0') {
            tmp = '1';
        }
        cstr[i] = tmp;
    }
    std::string str(cstr);
    return str;
}

void RSPropertiesFuzzTestInner01(RSProperties& properties)
{
    float x1 = GetData<float>();
    float y1 = GetData<float>();
    float z1 = GetData<float>();
    float w1 = GetData<float>();
    Vector4f bounds(x1, y1, z1, w1);
    float x2 = GetData<float>();
    float y2 = GetData<float>();
    Vector2f size_vec(x2, y2);
    float width = GetData<float>();
    float height = GetData<float>();
    float x3 = GetData<float>();
    float y3 = GetData<float>();
    Vector2f position(x3, y3);
    float positionX = GetData<float>();
    float positionY = GetData<float>();
    float x4 = GetData<float>();
    float y4 = GetData<float>();
    float z4 = GetData<float>();
    float w4 = GetData<float>();
    Vector4f frame(x4, y4, z4, w4);
    float positionZ = GetData<float>();
    float x5 = GetData<float>();
    float y5 = GetData<float>();
    Vector2f pivot(x5, y5);
    float pivotX = GetData<float>();
    float pivotY = GetData<float>();
    float x6 = GetData<float>();
    float y6 = GetData<float>();
    float z6 = GetData<float>();
    float w6 = GetData<float>();
    Vector4f cornerRadius(x6, y6, z6, w6);
    properties.SetBounds(bounds);
    properties.SetBoundsSize(size_vec);
    properties.SetBoundsWidth(width);
    properties.SetBoundsHeight(height);
    properties.SetBoundsPosition(position);
    properties.SetBoundsPositionX(positionX);
    properties.SetBoundsPositionY(positionY);
    properties.SetFrame(frame);
    properties.SetFrameSize(size_vec);
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

void RSPropertiesFuzzTestInner02(RSProperties& properties)
{
    float x7 = GetData<float>();
    float y7 = GetData<float>();
    float z7 = GetData<float>();
    float w7 = GetData<float>();
    Quaternion quaternion(x7, y7, z7, w7);
    float degree = GetData<float>();
    float x8 = GetData<float>();
    float y8 = GetData<float>();
    Vector2f translateVector(x8, y8);
    float translate = GetData<float>();
    float x9 = GetData<float>();
    float y9 = GetData<float>();
    Vector2f scale(x9, y9);
    float x10 = GetData<float>();
    float y10 = GetData<float>();
    Vector2f skew(x10, y10);
    float x11 = GetData<float>();
    float y11 = GetData<float>();
    Vector2f persp(x11, y11);
    float alpha = GetData<float>();
    bool alphaOffscreen = GetData<bool>();
    int16_t red1 = GetData<int16_t>();
    int16_t green1 = GetData<int16_t>();
    int16_t blue1 = GetData<int16_t>();
    Color color1(red1, green1, blue1);
    std::shared_ptr<RSShader> shader = RSShader::CreateRSShader();
    int16_t red2 = GetData<int16_t>();
    int16_t green2 = GetData<int16_t>();
    int16_t blue2 = GetData<int16_t>();
    Color color2(red2, green2, blue2);
    int16_t red3 = GetData<int16_t>();
    int16_t green3 = GetData<int16_t>();
    int16_t blue3 = GetData<int16_t>();
    Color color3(red3, green3, blue3);
    int16_t red4 = GetData<int16_t>();
    int16_t green4 = GetData<int16_t>();
    int16_t blue4 = GetData<int16_t>();
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
    properties.SetSkew(skew);
    properties.SetPersp(persp);
    properties.SetAlpha(alpha);
    properties.SetAlphaOffscreen(alphaOffscreen);
    properties.SetForegroundColor(color1);
    properties.SetBackgroundColor(color2);
    properties.SetBackgroundShader(shader);
    properties.SetBorderColor(colorVector);
}

void RSPropertiesFuzzTestInner03(RSProperties& properties)
{
    float x10 = GetData<float>();
    float y10 = GetData<float>();
    float z10 = GetData<float>();
    float w10 = GetData<float>();
    Vector4f widthVector(x10, y10, z10, w10);
    uint32_t x11 = GetData<float>();
    uint32_t y11 = GetData<float>();
    uint32_t z11 = GetData<float>();
    uint32_t w11 = GetData<float>();
    Vector4<uint32_t> style(x11, y11, z11, w11);
    float blurRadiusX = GetData<float>();
    float blurRadiusY = GetData<float>();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    int styleInt = GetData<int>();
    float dipScale = GetData<float>();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateMaterialFilter(styleInt, dipScale);
    float offsetX = GetData<float>();
    float offsetY = GetData<float>();
    float radius = GetData<float>();
    std::shared_ptr<RSPath> shadowpath = std::make_shared<RSPath>();
    Gravity gravity = GetData<Gravity>();
    int intLeft = GetData<int>();
    int intTop = GetData<int>();
    int intWidth = GetData<int>();
    int intHeight = GetData<int>();
    std::shared_ptr<RectI> rect = std::make_shared<RectI>(intLeft, intTop, intWidth, intHeight);
    int16_t red1 = GetData<int16_t>();
    int16_t green1 = GetData<int16_t>();
    int16_t blue1 = GetData<int16_t>();
    Color color1(red1, green1, blue1);
    float alpha = GetData<float>();

    properties.SetBorderWidth(widthVector);
    properties.SetBorderStyle(style);
    properties.SetBackgroundFilter(backgroundFilter);
    properties.SetFilter(filter);
    properties.SetShadowColor(color1);
    properties.SetShadowOffsetX(offsetX);
    properties.SetShadowOffsetY(offsetY);
    properties.SetShadowAlpha(alpha);
    properties.SetShadowElevation(radius);
    properties.SetShadowRadius(radius);
    properties.SetShadowPath(shadowpath);
    properties.SetFrameGravity(gravity);
}

void RSPropertiesFuzzTestInner04(RSProperties& properties)
{
    float sx = GetData<float>();
    float sy = GetData<float>();
    std::shared_ptr<RSImage> image = std::make_shared<RSImage>();
    float width = GetData<float>();
    float height = GetData<float>();
    float positionX = GetData<float>();
    float positionY = GetData<float>();
    bool clipToBounds = GetData<bool>();
    bool clipToFrame = GetData<bool>();
    bool visible = GetData<bool>();

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
}

bool RSPropertiesFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSProperties properties;

    // test
    RSPropertiesFuzzTestInner01(properties);
    RSPropertiesFuzzTestInner02(properties);
    RSPropertiesFuzzTestInner03(properties);
    RSPropertiesFuzzTestInner04(properties);

    return true;
}

bool RSPropertiesPainterFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // getdata
    Drawing::Canvas tmpCanvas;
    float fLeft = GetData<float>();
    float fTop = GetData<float>();
    float fWidth = GetData<float>();
    float fHeight = GetData<float>();
    RectF rect(fLeft, fTop, fWidth, fHeight);
    RSProperties properties;
    int iLeft = GetData<int>();
    int iTop = GetData<int>();
    int iWidth = GetData<int>();
    int iHeight = GetData<int>();
    RectI dirtyShadow(iLeft, iTop, iWidth, iHeight);
    float skLeft = GetData<float>();
    float skTop = GetData<float>();
    float skRight = GetData<float>();
    float skBottom = GetData<float>();
    Drawing::Rect maskBounds { skLeft, skTop, skRight, skBottom };
    Drawing::Matrix mat;
    Gravity gravity = GetData<Gravity>();
    float fW = GetData<float>();
    float fH = GetData<float>();

    RSPropertiesPainter::Clip(tmpCanvas, rect);
    RSPropertiesPainter::DrawBorder(properties, tmpCanvas);
    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties);
    RSPropertiesPainter::DrawForegroundColor(properties, tmpCanvas);
    RSPropertiesPainter::DrawMask(properties, tmpCanvas);
    RSPropertiesPainter::DrawMask(properties, tmpCanvas, maskBounds);
    RSPropertiesPainter::GetGravityMatrix(gravity, rect, fW, fH, mat);
    RSPropertiesPainter::Rect2DrawingRect(rect);

    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSPropertiesFuzzTest(data, size);
    OHOS::Rosen::RSPropertiesPainterFuzzTest(data, size);
    return 0;
}
