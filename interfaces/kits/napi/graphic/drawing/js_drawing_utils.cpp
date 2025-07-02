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

#include "font_napi/js_typeface.h"
#include "js_drawing_utils.h"

#ifdef ROSEN_OHOS
#include <parameters.h>
#endif

#include "draw/color.h"
#include "effect/color_space.h"
#include "image/image_info.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "rosen_text/font_collection.h"
#include "txt/platform.h"

namespace OHOS::Rosen {

#ifdef ROSEN_OHOS
bool JsDrawingTestUtils::closeDrawingTest_ =
    std::atoi((OHOS::system::GetParameter("persist.sys.graphic.drawing.test", "0").c_str())) != 1;
#else
bool JsDrawingTestUtils::closeDrawingTest_ = true;
#endif

namespace Drawing {
const char* const JSCOLOR[4] = {"alpha", "red", "green", "blue"};

void BindNativeFunction(napi_env env, napi_value object, const char* name, const char* moduleName, napi_callback func)
{
    std::string fullName;
    if (moduleName) {
        fullName = moduleName;
        fullName += '.';
    }
    fullName += name;
    napi_value funcValue = nullptr;
    napi_create_function(env, fullName.c_str(), fullName.size(), func, nullptr, &funcValue);
    napi_set_named_property(env, object, fullName.c_str(), funcValue);
}

napi_value CreateJsError(napi_env env, int32_t errCode, const std::string& message)
{
    napi_value result = nullptr;
    napi_create_error(env, CreateJsValue(env, errCode), CreateJsValue(env, message), &result);
    return result;
}

bool ConvertFromJsTextEncoding(napi_env env, TextEncoding& textEncoding, napi_value nativeType)
{
    napi_value type = nativeType;
    if (type == nullptr) {
        return false;
    }
    uint32_t resultValue = 0;
    napi_get_value_uint32(env, type, &resultValue);

    switch (resultValue) {
        case 0: // 0: TextEncoding::UTF8
            textEncoding = TextEncoding::UTF8;
            break;
        case 1: // 1: TextEncoding::UTF16
            textEncoding = TextEncoding::UTF16;
            break;
        case 2: // 2: TextEncoding::UTF32
            textEncoding = TextEncoding::UTF32;
            break;
        case 3: // 3: TextEncoding::GLYPH_ID
            textEncoding = TextEncoding::GLYPH_ID;
            break;
        default: // default: TextEncoding::UTF8
            textEncoding = TextEncoding::UTF8;
            break;
    }
    return true;
}

napi_value NapiThrowError(napi_env env, DrawingErrorCode err, const std::string& message)
{
    napi_throw(env, CreateJsError(env, static_cast<int32_t>(err), message));
    return nullptr;
}

static const char* g_argbString[4] = {"alpha", "red", "green", "blue"};
static const char* g_ltrbString[4] = {"left", "top", "right", "bottom"};
static const char* g_pointString[2] = {"x", "y"};

bool ConvertFromJsColor(napi_env env, napi_value jsValue, int32_t* argb, size_t size)
{
    napi_value tempValue = nullptr;
    for (size_t idx = 0; idx < size; idx++) {
        int32_t* curChannel = argb + idx;
        napi_get_named_property(env, jsValue, g_argbString[idx], &tempValue);
        if (napi_get_value_int32(env, tempValue, curChannel) != napi_ok ||
            *curChannel < 0 || *curChannel > Color::RGB_MAX) {
            return false;
        }
    }
    return true;
}

bool ConvertFromJsColor4F(napi_env env, napi_value jsValue, double* argbF, size_t size)
{
    napi_value tempValue = nullptr;
    for (size_t idx = 0; idx < size; idx++) {
        double* curChannel = argbF + idx;
        napi_get_named_property(env, jsValue, g_argbString[idx], &tempValue);
        if (napi_get_value_double(env, tempValue, curChannel) != napi_ok) {
            return false;
        }
    }
    return true;
}

bool ConvertFromAdaptHexJsColor(napi_env env, napi_value jsValue, Drawing::ColorQuad& jsColor)
{
    bool isJsColor = false;
    napi_has_named_property(env, jsValue, JSCOLOR[0], &isJsColor);
    if (isJsColor) {
        int32_t argb[ARGC_FOUR] = {0};
        if (!ConvertFromJsColor(env, jsValue, argb, ARGC_FOUR)) {
            return false;
        }
        jsColor = Color::ColorQuadSetARGB(argb[ARGC_ZERO], argb[ARGC_ONE], argb[ARGC_TWO], argb[ARGC_THREE]);
    } else {
        if (napi_get_value_uint32(env, jsValue, &jsColor) != napi_ok) {
            return false;
        }
    }
    return true;
}

bool ConvertFromAdaptJsColor4F(napi_env env, napi_value jsValue, Drawing::Color4f& jsColor4F)
{
    bool isJsColor4F = false;
    napi_has_named_property(env, jsValue, JSCOLOR[0], &isJsColor4F);
    if (!isJsColor4F) {
        return false;
    }

    double argbF[ARGC_FOUR] = {0};
    if (!ConvertFromJsColor4F(env, jsValue, argbF, ARGC_FOUR)) {
        return false;
    }
    jsColor4F.alphaF_ = static_cast<float>(argbF[ARGC_ZERO]);
    jsColor4F.redF_ = static_cast<float>(argbF[ARGC_ONE]);
    jsColor4F.greenF_ = static_cast<float>(argbF[ARGC_TWO]);
    jsColor4F.blueF_ = static_cast<float>(argbF[ARGC_THREE]);

    return true;
}

bool ConvertFromJsRect(napi_env env, napi_value jsValue, double* ltrb, size_t size)
{
    napi_value tempValue = nullptr;
    for (size_t idx = 0; idx < size; idx++) {
        double* curEdge = ltrb + idx;
        napi_get_named_property(env, jsValue, g_ltrbString[idx], &tempValue);
        if (napi_get_value_double(env, tempValue, curEdge) != napi_ok) {
            return false;
        }
    }
    return true;
}

bool ConvertFromJsIRect(napi_env env, napi_value jsValue, int32_t* ltrb, size_t size)
{
    napi_value tempValue = nullptr;
    for (size_t idx = 0; idx < size; idx++) {
        int32_t* curEdge = ltrb + idx;
        napi_get_named_property(env, jsValue, g_ltrbString[idx], &tempValue);
        if (napi_get_value_int32(env, tempValue, curEdge) != napi_ok) {
            return false;
        }
    }
    return true;
}

bool ConvertFromJsShadowFlag(napi_env env, napi_value src, ShadowFlags& shadowFlag, ShadowFlags defaultFlag)
{
    if (src == nullptr) {
        return false;
    }
    uint32_t value = 0;
    if (!ConvertFromJsValue(env, src, value)) {
        return false;
    }
    shadowFlag = defaultFlag;
    if (value >= static_cast<uint32_t>(ShadowFlags::NONE) && value <= static_cast<uint32_t>(ShadowFlags::ALL)) {
        shadowFlag = static_cast<ShadowFlags>(value);
    }
    return true;
}

bool ConvertFromJsPoint3d(napi_env env, napi_value src, Point3& point3d)
{
    if (src == nullptr) {
        return false;
    }
    napi_value tempValue = nullptr;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    napi_get_named_property(env, src, "x", &tempValue);
    bool isXOk = ConvertFromJsValue(env, tempValue, x);
    napi_get_named_property(env, src, "y", &tempValue);
    bool isYOk = ConvertFromJsValue(env, tempValue, y);
    napi_get_named_property(env, src, "z", &tempValue);
    bool isZOk = ConvertFromJsValue(env, tempValue, z);
    if (!(isXOk && isYOk && isZOk)) {
        return false;
    }
    point3d = Point3(x, y, z);
    return true;
}

bool ConvertFromJsPoint(napi_env env, napi_value jsValue, double* point, size_t size)
{
    if (point == nullptr) {
        return false;
    }
    napi_value tempValue = nullptr;
    for (size_t idx = 0; idx < size; idx++) {
        double* curEdge = point + idx;
        napi_get_named_property(env, jsValue, g_pointString[idx], &tempValue);
        if (napi_get_value_double(env, tempValue, curEdge) != napi_ok) {
            return false;
        }
    }
    return true;
}

bool ConvertFromJsPointsArray(napi_env env, napi_value array, Drawing::Point* points, uint32_t count)
{
    if (points == nullptr) {
        return false;
    }
    for (uint32_t i = 0; i < count; i++)  {
        napi_value tempPoint = nullptr;
        if (napi_get_element(env, array, i, &tempPoint) != napi_ok) {
            return false;
        }
        if (!GetDrawingPointFromJsValue(env, tempPoint, points[i])) {
            return false;
        }
    }
    return true;
}

bool ConvertFromJsPointsArrayOffset(napi_env env, napi_value array, Drawing::Point* points,
    uint32_t count, uint32_t offset)
{
    if (points == nullptr) {
        return false;
    }
    for (uint32_t i = offset; i < offset + count; i++)  {
        napi_value tempPoint = nullptr;
        if (napi_get_element(env, array, i, &tempPoint) != napi_ok) {
            return false;
        }
        if (!GetDrawingPointFromJsValue(env, tempPoint, points[i - offset])) {
            return false;
        }
    }
    return true;
}

napi_value GetFontMetricsAndConvertToJsValue(napi_env env, FontMetrics* metrics)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (metrics != nullptr && objValue != nullptr) {
        napi_set_named_property(env, objValue, "top", CreateJsNumber(env, metrics->fTop));
        napi_set_named_property(env, objValue, "ascent", CreateJsNumber(env, metrics->fAscent));
        napi_set_named_property(env, objValue, "descent", CreateJsNumber(env, metrics->fDescent));
        napi_set_named_property(env, objValue, "bottom", CreateJsNumber(env, metrics->fBottom));
        napi_set_named_property(env, objValue, "leading", CreateJsNumber(env, metrics->fLeading));
        napi_set_named_property(env, objValue, "flags", CreateJsNumber(env, metrics->fFlags));
        napi_set_named_property(env, objValue, "avgCharWidth", CreateJsNumber(env, metrics->fAvgCharWidth));
        napi_set_named_property(env, objValue, "maxCharWidth", CreateJsNumber(env, metrics->fMaxCharWidth));
        napi_set_named_property(env, objValue, "xMin", CreateJsNumber(env, metrics->fXMin));
        napi_set_named_property(env, objValue, "xMax", CreateJsNumber(env, metrics->fXMax));
        napi_set_named_property(env, objValue, "xHeight", CreateJsNumber(env, metrics->fXHeight));
        napi_set_named_property(env, objValue, "capHeight", CreateJsNumber(env, metrics->fCapHeight));
        napi_set_named_property(env, objValue, "underlineThickness", CreateJsNumber(env,
            metrics->fUnderlineThickness));
        napi_set_named_property(env, objValue, "underlinePosition", CreateJsNumber(env,
            metrics->fUnderlinePosition));
        napi_set_named_property(env, objValue, "strikethroughThickness", CreateJsNumber(env,
            metrics->fStrikeoutThickness));
        napi_set_named_property(env, objValue, "strikethroughPosition", CreateJsNumber(env,
            metrics->fStrikeoutPosition));
    }
    return objValue;
}

#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
std::shared_ptr<Drawing::ColorSpace> ColorSpaceToDrawingColorSpace(Media::ColorSpace colorSpace)
{
    switch (colorSpace) {
        case Media::ColorSpace::DISPLAY_P3:
            return Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
        case Media::ColorSpace::LINEAR_SRGB:
            return Drawing::ColorSpace::CreateSRGBLinear();
        case Media::ColorSpace::SRGB:
            return Drawing::ColorSpace::CreateSRGB();
        default:
            return Drawing::ColorSpace::CreateSRGB();
    }
}

Drawing::ColorType PixelFormatToDrawingColorType(Media::PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case Media::PixelFormat::RGB_565:
            return Drawing::ColorType::COLORTYPE_RGB_565;
        case Media::PixelFormat::RGBA_8888:
            return Drawing::ColorType::COLORTYPE_RGBA_8888;
        case Media::PixelFormat::BGRA_8888:
            return Drawing::ColorType::COLORTYPE_BGRA_8888;
        case Media::PixelFormat::ALPHA_8:
            return Drawing::ColorType::COLORTYPE_ALPHA_8;
        case Media::PixelFormat::RGBA_F16:
            return Drawing::ColorType::COLORTYPE_RGBA_F16;
        case Media::PixelFormat::UNKNOWN:
        case Media::PixelFormat::ARGB_8888:
        case Media::PixelFormat::RGB_888:
        case Media::PixelFormat::NV21:
        case Media::PixelFormat::NV12:
        case Media::PixelFormat::CMYK:
        default:
            return Drawing::ColorType::COLORTYPE_UNKNOWN;
    }
}

Drawing::AlphaType AlphaTypeToDrawingAlphaType(Media::AlphaType alphaType)
{
    switch (alphaType) {
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return Drawing::AlphaType::ALPHATYPE_OPAQUE;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return Drawing::AlphaType::ALPHATYPE_PREMUL;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return Drawing::AlphaType::ALPHATYPE_UNPREMUL;
        default:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
    }
}

bool ExtracetDrawingBitmap(std::shared_ptr<Media::PixelMap> pixelMap, Drawing::Bitmap& bitmap)
{
    if (!pixelMap) {
        ROSEN_LOGE("Drawing_napi ::pixelMap fail");
        return false;
    }
    Media::ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    Drawing::ImageInfo drawingImageInfo { imageInfo.size.width, imageInfo.size.height,
        PixelFormatToDrawingColorType(imageInfo.pixelFormat), AlphaTypeToDrawingAlphaType(imageInfo.alphaType),
        ColorSpaceToDrawingColorSpace(imageInfo.colorSpace) };
    bitmap.Build(drawingImageInfo, pixelMap->GetRowStride());
    bitmap.SetPixels(const_cast<void*>(reinterpret_cast<const void*>(pixelMap->GetPixels())));
    return true;
}

struct PixelMapReleaseContext {
    explicit PixelMapReleaseContext(std::shared_ptr<Media::PixelMap> pixelMap) : pixelMap_(pixelMap) {}

    ~PixelMapReleaseContext()
    {
        pixelMap_ = nullptr;
    }

private:
    std::shared_ptr<Media::PixelMap> pixelMap_;
};

static void PixelMapReleaseProc(const void* /* pixels */, void* context)
{
    PixelMapReleaseContext* ctx = static_cast<PixelMapReleaseContext*>(context);
    if (ctx) {
        delete ctx;
        ctx = nullptr;
    }
}

std::shared_ptr<Drawing::Image> ExtractDrawingImage(std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (!pixelMap) {
        ROSEN_LOGE("Drawing_napi::pixelMap fail");
        return nullptr;
    }
    Media::ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    Drawing::ImageInfo drawingImageInfo { imageInfo.size.width, imageInfo.size.height,
        PixelFormatToDrawingColorType(imageInfo.pixelFormat), AlphaTypeToDrawingAlphaType(imageInfo.alphaType),
        ColorSpaceToDrawingColorSpace(imageInfo.colorSpace) };
    Drawing::Pixmap imagePixmap(
        drawingImageInfo, reinterpret_cast<const void*>(pixelMap->GetPixels()), pixelMap->GetRowStride());
    PixelMapReleaseContext* releaseContext = new PixelMapReleaseContext(pixelMap);
    auto image = Drawing::Image::MakeFromRaster(imagePixmap, PixelMapReleaseProc, releaseContext);
    if (!image) {
        ROSEN_LOGE("Drawing_napi :RSPixelMapUtil::ExtractDrawingImage fail");
        delete releaseContext;
        releaseContext = nullptr;
    }
    return image;
}
#endif

std::shared_ptr<Font> GetThemeFont(std::shared_ptr<Font> font)
{
    std::shared_ptr<FontMgr> fontMgr = GetFontMgr(font);
    if (fontMgr == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Typeface> themeTypeface =
        std::shared_ptr<Typeface>(fontMgr->MatchFamilyStyle(SPText::OHOS_THEME_FONT, FontStyle()));
    if (themeTypeface == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Font> themeFont = std::make_shared<Font>(*font);
    themeFont->SetTypeface(themeTypeface);
    return themeFont;
}

std::shared_ptr<Font> MatchThemeFont(std::shared_ptr<Font> font, int32_t unicode)
{
    std::shared_ptr<FontMgr> fontMgr = GetFontMgr(font);
    if (fontMgr == nullptr) {
        return nullptr;
    }
    auto themeFamilies = SPText::DefaultFamilyNameMgr::GetInstance().GetThemeFontFamilies();
    std::shared_ptr<Drawing::Font> themeFont = std::make_shared<Drawing::Font>(*font);
    for (const auto& family : themeFamilies) {
        std::shared_ptr<Drawing::Typeface> themeTypeface =
            std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(family.c_str(), FontStyle()));
        themeFont->SetTypeface(themeTypeface);
        if (themeFont->UnicharToGlyph(unicode)) {
            return themeFont;
        }
    }
    return nullptr;
}

std::shared_ptr<FontMgr> GetFontMgr(std::shared_ptr<Font> font)
{
    if (!font->IsThemeFontFollowed() || font->GetTypeface() != JsTypeface::GetZhCnTypeface()) {
        return nullptr;
    }

    std::shared_ptr<FontCollection> fontCollection = FontCollection::Create();
    if (fontCollection == nullptr) {
        return nullptr;
    }
    std::shared_ptr<FontMgr> fontMgr = fontCollection->GetFontMgr();
    if (fontMgr == nullptr) {
        return nullptr;
    }
    return fontMgr;
}

void MakeFontFeaturesFromJsArray(napi_env env, std::shared_ptr<DrawingFontFeatures> features,
    uint32_t size, napi_value& array)
{
    features->clear();

    for (uint32_t i = 0; i < size; ++i) {
        napi_value tempNumber = nullptr;
        napi_status status = napi_get_element(env, array, i, &tempNumber);
        if (status != napi_ok || tempNumber == nullptr) {
            continue;
        }
        std::string name;
        napi_value tempValue = nullptr;
        status = napi_get_named_property(env, tempNumber, "name", &tempValue);
        if (status != napi_ok || tempValue == nullptr) {
            continue;
        }
        if (!ConvertFromJsValue(env, tempValue, name)) {
            continue;
        }
        double value = 0.0;
        status = napi_get_named_property(env, tempNumber, "value", &tempValue);
        if (status != napi_ok || tempValue == nullptr) {
            continue;
        }
        if (!ConvertFromJsValue(env, tempValue, value)) {
            continue;
        }
        features->push_back({{name, value}});
    }
}
} // namespace Drawing
} // namespace OHOS::Rosen
