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

#include "rsrenderimage_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "render/rs_blur_filter.h"
#include "render/rs_border.h"
#include "render/rs_image.h"
#include "render/rs_image_cache.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "render/rs_shadow.h"

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

bool RSBlurFilterFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float blurRadiusX1 = GetData<float>();
    float blurRadiusY1 = GetData<float>();
    RSBlurFilter blurFilter(blurRadiusX1, blurRadiusY1);
    float blurRadiusX2 = GetData<float>();
    float blurRadiusY2 = GetData<float>();
    std::shared_ptr<RSFilter> rhs = RSFilter::CreateBlurFilter(blurRadiusX2, blurRadiusY2);
    float fRhs = GetData<float>();

    blurFilter.Add(rhs);
    blurFilter.Sub(rhs);
    blurFilter.Multiply(fRhs);

    return true;
}

bool RSBorderFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int16_t red1 = GetData<int16_t>();
    int16_t green1 = GetData<int16_t>();
    int16_t blue1 = GetData<int16_t>();
    Color color1(red1, green1, blue1);
    float width = GetData<float>();
    BorderStyle style = GetData<BorderStyle>();
    int idx = GetData<int>();
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
    Vector4<Color> vectorColor(color1, color2, color3, color4);
    float x = GetData<float>();
    float y = GetData<float>();
    float z = GetData<float>();
    float w = GetData<float>();
    Vector4f vectorWidth(x, y, z, w);
    uint32_t dataX = GetData<uint32_t>();
    uint32_t dataY = GetData<uint32_t>();
    uint32_t dataZ = GetData<uint32_t>();
    uint32_t dataW = GetData<uint32_t>();
    Vector4<uint32_t> vectorStyle(dataX, dataY, dataZ, dataW);

    RSBorder border;
    border.SetColor(color1);
    border.SetWidth(width);
    border.SetStyle(style);
    border.GetColor(idx);
    border.GetWidth(idx);
    border.GetStyle(idx);
    border.SetColorFour(vectorColor);
    border.SetWidthFour(width);
    border.SetStyleFour(vectorStyle);

    return true;
}

bool RSImageCacheFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint64_t uniqueId = GetData<uint64_t>();
    std::shared_ptr<Drawing::Image> img;
    RSImageCache::Instance().CacheDrawingImage(uniqueId, img);
    RSImageCache::Instance().GetDrawingImageCache(uniqueId);
    RSImageCache::Instance().ReleaseDrawingImageCache(uniqueId);

    return true;
}

bool RSImageFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSImage other;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    float fLeft = GetData<float>();
    float fTop = GetData<float>();
    float fRight = GetData<float>();
    float fBottom = GetData<float>();
    Drawing::Rect rect { fLeft, fTop, fRight, fBottom };
    Drawing::Brush brush;
    bool isBackground = GetData<bool>();
    std::shared_ptr<Drawing::Image> image;
    float left = GetData<float>();
    float top = GetData<float>();
    float right = GetData<float>();
    float bottom = GetData<float>();
    RectF dstRect(left, top, right, bottom);
    int fitNum = GetData<int>();
    int repeatNum = GetData<int>();
    float fX1 = GetData<float>();
    float fY1 = GetData<float>();
    float fX2 = GetData<float>();
    float fY2 = GetData<float>();
    float fX3 = GetData<float>();
    float fY3 = GetData<float>();
    float fX4 = GetData<float>();
    float fY4 = GetData<float>();
    Drawing::Point vector1 { fX1, fY1 };
    Drawing::Point vector2 { fX2, fY2 };
    Drawing::Point vector3 { fX3, fY3 };
    Drawing::Point vector4 { fX4, fY4 };
    std::vector<Drawing::Point> radius = { vector1, vector2, vector3, vector4 };
    double scale = GetData<double>();
    std::shared_ptr<Drawing::Data> drawingData;
    int width = GetData<int>();
    int height = GetData<int>();
    int id = GetData<int>();

    RSImage rsImage;
    rsImage.IsEqual(other);
    canvas.AttachBrush(brush);
    rsImage.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), isBackground);
    canvas.DetachBrush();
    rsImage.SetImage(image);
    rsImage.SetDstRect(dstRect);
    rsImage.SetImageFit(fitNum);
    rsImage.SetImageRepeat(repeatNum);
    rsImage.SetRadius(radius);
    rsImage.SetScale(scale);
    rsImage.SetCompressData(drawingData, id, width, height);
    return true;
}

bool RSMaskFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    double x = GetData<double>();
    double y = GetData<double>();
    double scaleX = GetData<double>();
    double scaleY = GetData<double>();
    Drawing::Path path;
    Drawing::Brush brush;
    sk_sp<SkSVGDOM> svgDom;
    MaskType type = GetData<MaskType>();

    RSMask rsMask;
    rsMask.SetSvgX(x);
    rsMask.SetSvgY(y);
    rsMask.SetScaleX(scaleX);
    rsMask.SetScaleY(scaleY);
    rsMask.SetMaskPath(path);
    rsMask.SetMaskBrush(brush);
    rsMask.SetSvgDom(svgDom);
    rsMask.SetMaskType(type);

    return true;
}

bool RSPathFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Drawing::Path drPath = Drawing::Path();
    // std::string path = GetStringFromData(STR_LEN);
    float distance = GetData<float>();
    float x = GetData<float>();
    float y = GetData<float>();
    Vector2f pos = Vector2f(x, y);
    float degrees = GetData<float>();
    std::shared_ptr<RSPath> rsPath1 = RSPath::CreateRSPath();

    rsPath1->GetPosTan(distance, pos, degrees);
    rsPath1->SetDrawingPath(drPath);

    return true;
}

bool RSShaderFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSShader> shaderPtr = RSShader::CreateRSShader();
    std::shared_ptr<Drawing::ShaderEffect> shader;
    shaderPtr->SetDrawingShader(shader);

    return true;
}

bool RSShadowFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t rgba = GetData<uint32_t>();
    Color color = Color(rgba);
    float offsetX = GetData<float>();
    float offsetY = GetData<float>();
    float alpha = GetData<float>();
    float elevation = GetData<float>();
    float radius = GetData<float>();
    std::shared_ptr<RSPath> path = std::make_shared<RSPath>();

    RSShadow shadow = RSShadow();
    shadow.SetColor(color);
    shadow.SetOffsetX(offsetX);
    shadow.SetOffsetY(offsetY);
    shadow.SetAlpha(alpha);
    shadow.SetElevation(elevation);
    shadow.SetRadius(radius);
    shadow.SetPath(path);

    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSBlurFilterFuzzTest(data, size);
    OHOS::Rosen::RSBorderFuzzTest(data, size);
    OHOS::Rosen::RSImageCacheFuzzTest(data, size);
    OHOS::Rosen::RSImageFuzzTest(data, size);
    OHOS::Rosen::RSMaskFuzzTest(data, size);
    OHOS::Rosen::RSPathFuzzTest(data, size);
    OHOS::Rosen::RSShaderFuzzTest(data, size);
    OHOS::Rosen::RSShadowFuzzTest(data, size);
    return 0;
}
