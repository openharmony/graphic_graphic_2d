/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "rs_render_service_util.h"

#include <unordered_set>

#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "render/rs_blur_filter.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace Detail {
// [PLANNING]: Use GPU to do the gamut convertion instead of these following works.
using PixelTransformFunc = std::function<float(float)>;

inline constexpr float PassThrough(float v)
{
    return v;
}

template<typename T>
static constexpr T Saturate(T v) noexcept
{
    return T(std::min(static_cast<T>(1), std::max(static_cast<T>(0), v)));
}

inline Vector3f ApplyTransForm(const Vector3f& val, const PixelTransformFunc& func)
{
    return Vector3f {func(val.x_), func(val.y_), func(val.z_)};
}

inline float SafePow(float x, float e)
{
    return powf(x < 0.0f ? 0.0f : x, e);
}

inline PixelTransformFunc GenOETF(float gamma)
{
    if (gamma == 1.0f || gamma == 0.0f) {
        return PassThrough;
    }

    return std::bind(SafePow, std::placeholders::_1, 1.0f / gamma);
}

inline PixelTransformFunc GenEOTF(float gamma)
{
    if (gamma == 1.0f) {
        return PassThrough;
    }

    return std::bind(SafePow, std::placeholders::_1, gamma);
}

struct TransferParameters {
    float g = 0.0f;
    float a = 0.0f;
    float b = 0.0f;
    float c = 0.0f;
    float d = 0.0f;
    float e = 0.0f;
    float f = 0.0f;
};

static constexpr float RcpResponse(float x, const TransferParameters& p)
{
    return x >= p.d * p.c ? (std::pow(x, 1.0f / p.g) - p.b) / p.a : x / p.c;
}

inline constexpr float Response(float x, const TransferParameters& p)
{
    return x >= p.d ? std::pow(p.a * x + p.b, p.g) : p.c * x;
}

inline constexpr float RcpFullResponse(float x, const TransferParameters& p)
{
    return x >= p.d * p.c ? (std::pow(x - p.e, 1.0f / p.g) - p.b) / p.a : (x - p.f) / p.c;
}

inline constexpr float FullResponse(float x, const TransferParameters& p)
{
    return x >= p.d ? std::pow(p.a * x + p.b, p.g) + p.e : p.c * x + p.f;
}

inline PixelTransformFunc GenOETF(const TransferParameters& params)
{
    if (params.e == 0.0f && params.f == 0.0f) {
        return std::bind(RcpResponse, std::placeholders::_1, params);
    }

    return std::bind(RcpFullResponse, std::placeholders::_1, params);
}

inline PixelTransformFunc GenEOTF(const TransferParameters& params)
{
    if (params.e == 0.0f && params.f == 0.0f) {
        return std::bind(Response, std::placeholders::_1, params);
    }

    return std::bind(FullResponse, std::placeholders::_1, params);
}

Matrix3f GenRGBToXYZMatrix(const std::array<Vector2f, 3>& basePoints, const Vector2f& whitePoint)
{
    const Vector2f& R = basePoints[0];
    const Vector2f& G = basePoints[1];
    const Vector2f& B = basePoints[2];

    float RxRy = R.x_ / R.y_;
    float GxGy = G.x_ / G.y_;
    float BxBy = B.x_ / B.y_;
    float WxWy = whitePoint.x_ / whitePoint.y_;

    float oneRxRy = (1 - R.x_) / R.y_;
    float oneGxGy = (1 - G.x_) / G.y_;
    float oneBxBy = (1 - B.x_) / B.y_;
    float oneWxWy = (1 - whitePoint.x_) / whitePoint.y_;

    float BY =
            ((oneWxWy - oneRxRy) * (GxGy - RxRy) - (WxWy - RxRy) * (oneGxGy - oneRxRy)) /
            ((oneBxBy - oneRxRy) * (GxGy - RxRy) - (BxBy - RxRy) * (oneGxGy - oneRxRy));
    float GY = (WxWy - RxRy - BY * (BxBy - RxRy)) / (GxGy - RxRy);
    float RY = 1 - GY - BY;

    float RYRy = RY / R.y_;
    float GYGy = GY / G.y_;
    float BYBy = BY / B.y_;

    return Matrix3f {
        RYRy * R.x_, RY, RYRy * (1 - R.x_ - R.y_),
        GYGy * G.x_, GY, GYGy * (1 - G.x_ - G.y_),
        BYBy * B.x_, BY, BYBy * (1 - B.x_ - B.y_)
    };
}

class SimpleColorSpace {
public:
    // 3 RGB basePoints and 1 whitePoint.
    SimpleColorSpace(
        const std::array<Vector2f, 3>& basePoints,
        const Vector2f& whitePoint,
        float gamma,
        PixelTransformFunc clamper = Saturate<float>
    ) noexcept
        : rgbToXyz_(GenRGBToXYZMatrix(basePoints, whitePoint)),
          xyzToRgb_(rgbToXyz_.Inverse()),
          transEOTF_(GenEOTF(gamma)),
          transOETF_(GenOETF(gamma)),
          clamper_(std::move(clamper)),
          transferParams_({ gamma, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f })
    {
    }

    SimpleColorSpace(
        const std::array<Vector2f, 3>& basePoints,
        const Vector2f& whitePoint,
        const TransferParameters& parameters,
        PixelTransformFunc clamper = Saturate<float>
    ) noexcept
        : rgbToXyz_(GenRGBToXYZMatrix(basePoints, whitePoint)),
          xyzToRgb_(rgbToXyz_.Inverse()),
          transEOTF_(GenEOTF(parameters)),
          transOETF_(GenOETF(parameters)),
          clamper_(std::move(clamper)),
          transferParams_(parameters)
    {
    }

    ~SimpleColorSpace() noexcept = default;

    Vector3f ToLinear(const Vector3f& val) const
    {
        return ApplyTransForm(val, transEOTF_);
    }

    Vector3f FromLinear(const Vector3f& val) const
    {
        return ApplyTransForm(val, transOETF_);
    }

    Vector3f RGBToXYZ(const Vector3f& rgb) const
    {
        return rgbToXyz_ * ToLinear(rgb);
    }

    Vector3f XYZToRGB(const Vector3f& xyz) const
    {
        return ApplyTransForm(FromLinear(xyzToRgb_ * xyz), clamper_);
    }

private:
    Matrix3f rgbToXyz_;
    Matrix3f xyzToRgb_;
    PixelTransformFunc transEOTF_;
    PixelTransformFunc transOETF_;
    PixelTransformFunc clamper_;
    TransferParameters transferParams_;
};

SimpleColorSpace &GetSRGBColorSpace()
{
    static SimpleColorSpace sRGB {
        {{Vector2f{0.640f, 0.330f}, {0.300f, 0.600f}, {0.150f, 0.060f}}}, // rgb base points.
        {0.3127f, 0.3290f}, // white points.
        {2.4f, 1 / 1.055f, 0.055f / 1.055f, 1 / 12.92f, 0.04045f, 0.0f, 0.0f}}; // TransferParameters
    return sRGB;
}

SimpleColorSpace &GetAdobeRGBColorSpace()
{
    static SimpleColorSpace adobeRGB {
        {{Vector2f{0.64f, 0.33f}, {0.21f, 0.71f}, {0.15f, 0.06f}}}, // rgb base points.
        {0.3127f, 0.3290f}, // white points.
        2.2f}; // gamma 2.2
    return adobeRGB;
}

SimpleColorSpace &GetDisplayP3ColorSpace()
{
    static SimpleColorSpace displayP3 {
        {{Vector2f{0.680f, 0.320f}, {0.265f, 0.690f}, {0.150f, 0.060f}}}, // rgb base points.
        {0.3127f, 0.3290f}, // white points.
        {2.4f, 1 / 1.055f, 0.055f / 1.055f, 1 / 12.92f, 0.039f, 0.0f, 0.0f}}; // TransferParameters
    return displayP3;
}

SimpleColorSpace &GetDCIP3ColorSpace()
{
    static SimpleColorSpace dciP3 {
        {{Vector2f{0.680f, 0.320f}, {0.265f, 0.690f}, {0.150f, 0.060f}}}, // rgb base points.
        {0.314f, 0.351f}, // white points.
        2.6f}; // gamma 2.6
    return dciP3;
}

bool IsSupportedFormatForGamutConvertion(int32_t pixelFormat)
{
    static std::unordered_set<PixelFormat> supportedFormats = {
        PixelFormat::PIXEL_FMT_RGBX_8888,
        PixelFormat::PIXEL_FMT_RGBA_8888,
        PixelFormat::PIXEL_FMT_RGB_888,
        PixelFormat::PIXEL_FMT_BGRX_8888,
        PixelFormat::PIXEL_FMT_BGRA_8888
    };
    return supportedFormats.count(static_cast<PixelFormat>(pixelFormat)) > 0;
}

bool IsSupportedColorGamut(ColorGamut colorGamut)
{
    static std::unordered_set<ColorGamut> supportedColorGamuts = {
        ColorGamut::COLOR_GAMUT_SRGB,
        ColorGamut::COLOR_GAMUT_ADOBE_RGB,
        ColorGamut::COLOR_GAMUT_DISPLAY_P3,
        ColorGamut::COLOR_GAMUT_DCI_P3
    };
    return supportedColorGamuts.count(colorGamut) > 0;
}

SimpleColorSpace& GetColorSpaceOfCertainGamut(ColorGamut colorGamut)
{
    switch (colorGamut) {
        case ColorGamut::COLOR_GAMUT_SRGB: {
            return GetSRGBColorSpace();
        }
        case ColorGamut::COLOR_GAMUT_ADOBE_RGB: {
            return GetAdobeRGBColorSpace();
        }
        case ColorGamut::COLOR_GAMUT_DISPLAY_P3: {
            return GetDisplayP3ColorSpace();
        }
        case ColorGamut::COLOR_GAMUT_DCI_P3: {
            return GetDCIP3ColorSpace();
        }
        default: {
            return GetSRGBColorSpace();
        }
    }
}

float RGBUint8ToFloat(uint8_t val)
{
    return val * 1.0f / 255.0f; // 255.0f is the max value.
}

uint8_t RGBFloatToUint8(float val)
{
    return static_cast<uint8_t>(Saturate(val) * 255 + 0.5f); // 255.0 is the max value, + 0.5f to avoid negetive.
}

uint8_t ConvertColorGamut(uint8_t *dst, uint8_t* src, int32_t pixelFormat, ColorGamut srcGamut, ColorGamut dstGamut)
{
    uint8_t len = 0;
    Vector3f srcColor;
    std::array<uint8_t *, 3> colorDst; // color dst, 3 bytes (R G B).
    switch (static_cast<PixelFormat>(pixelFormat)) {
        case PixelFormat::PIXEL_FMT_RGBX_8888:
        case PixelFormat::PIXEL_FMT_RGBA_8888: {
            // R: src[0], G: src[1], B: src[2]
            srcColor = {RGBUint8ToFloat(src[0]), RGBUint8ToFloat(src[1]), RGBUint8ToFloat(src[2])};
            // R: dst + 0, G: dst + 1, B: dst + 2
            colorDst = {dst + 0, dst + 1, dst + 2};
            len = 4; // 4 bytes per pixel.
            break;
        }
        case PixelFormat::PIXEL_FMT_RGB_888: {
            // R: src[0], G: src[1], B: src[2]
            srcColor = {RGBUint8ToFloat(src[0]), RGBUint8ToFloat(src[1]), RGBUint8ToFloat(src[2])};
            // R: dst + 0, G: dst + 1, B: dst + 2
            colorDst = {dst + 0, dst + 1, dst + 2};
            len = 3; // 3 bytes per pixel.
            break;
        }
        case PixelFormat::PIXEL_FMT_BGRX_8888:
        case PixelFormat::PIXEL_FMT_BGRA_8888: {
            // R: src[2], G: src[1], B: src[0]
            srcColor = {RGBUint8ToFloat(src[2]), RGBUint8ToFloat(src[1]), RGBUint8ToFloat(src[0])};
            // R: dst + 2, G: dst + 1, B: dst + 0
            colorDst = {dst + 2, dst + 1, dst + 0};
            len = 4; // 4 bytes per pixel.
            break;
        }
        default: {
            ROSEN_LOGE("ConvertColorGamut: unexpected pixelFormat(%d).", pixelFormat);
            return 0;
        }
    }

    auto& srcColorSpace = GetColorSpaceOfCertainGamut(srcGamut);
    auto& dstColorSpace = GetColorSpaceOfCertainGamut(dstGamut);
    Vector3f outColor = dstColorSpace.XYZToRGB(srcColorSpace.RGBToXYZ(srcColor));
    *(colorDst[0]) = RGBFloatToUint8(outColor[0]); // outColor 0 to colorDst[0]
    *(colorDst[1]) = RGBFloatToUint8(outColor[1]); // outColor 1 to colorDst[1]
    *(colorDst[2]) = RGBFloatToUint8(outColor[2]); // outColor 2 to colorDst[2]

    return len;
}

bool ConvertBufferColorGamut(std::vector<uint8_t>& dstBuf, const sptr<OHOS::SurfaceBuffer>& srcBuf,
    ColorGamut srcGamut, ColorGamut dstGamut)
{
    RS_TRACE_NAME("ConvertBufferColorGamut");

    int32_t pixelFormat = srcBuf->GetFormat();
    if (!IsSupportedFormatForGamutConvertion(pixelFormat)) {
        ROSEN_LOGE("ConvertBufferColorGamut: the buffer's format is not supported.");
        return false;
    }
    if (!IsSupportedColorGamut(srcGamut) || !IsSupportedColorGamut(dstGamut)) {
        return false;
    }

    uint32_t bufferSize = srcBuf->GetSize();
    dstBuf.resize(bufferSize);

    auto bufferAddr = srcBuf->GetVirAddr();
    uint8_t* srcStart = static_cast<uint8_t*>(bufferAddr);

    uint32_t offset = 0;
    while (offset < bufferSize) {
        uint8_t* dst = &dstBuf[offset];
        uint8_t* src = srcStart + offset;
        uint8_t len = ConvertColorGamut(dst, src, pixelFormat, srcGamut, dstGamut);
        offset += len;
    }

    return true;
}

SkImageInfo GenerateSkImageInfo(const sptr<OHOS::SurfaceBuffer>& buffer)
{
    SkColorType colorType = (buffer->GetFormat() == PIXEL_FMT_BGRA_8888) ?
            kBGRA_8888_SkColorType : kRGBA_8888_SkColorType;
    return SkImageInfo::Make(buffer->GetWidth(), buffer->GetHeight(),
        colorType, kPremul_SkAlphaType);
}

void FillDrawParameters(BufferDrawParameters& params, const sptr<OHOS::SurfaceBuffer>& buffer,
    const RSSurfaceRenderNode& node)
{
    params.bitmap = SkBitmap();
    params.antiAlias = true;
    const RSProperties& property = node.GetRenderProperties();
    params.alpha = node.GetAlpha() * property.GetAlpha();
    params.dstRect = SkRect::MakeXYWH(0, 0, buffer->GetWidth(), buffer->GetHeight());
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (geoPtr) {
        params.transform = geoPtr->GetAbsMatrix();
        params.widthScale = static_cast<double>(geoPtr->GetAbsRect().width_ * 1.0 / buffer->GetWidth());
        params.heightScale = static_cast<double>(geoPtr->GetAbsRect().height_ * 1.0 / buffer->GetHeight());
    }
}
} // namespace Detail

void RsRenderServiceUtil::ComposeSurface(std::shared_ptr<HdiLayerInfo> layer, sptr<Surface> consumerSurface,
    std::vector<LayerInfoPtr>& layers,  ComposeInfo info, RSSurfaceRenderNode* node)
{
    layer->SetSurface(consumerSurface);
    layer->SetBuffer(info.buffer, info.fence, info.preBuffer, info.preFence);
    layer->SetZorder(info.zOrder);
    layer->SetAlpha(info.alpha);
    layer->SetLayerSize(info.dstRect);
    layer->SetLayerAdditionalInfo(node);
    layer->SetCompositionType(IsNeedClient(node) ?
        CompositionType::COMPOSITION_CLIENT : CompositionType::COMPOSITION_DEVICE);
    layer->SetVisibleRegion(1, info.visibleRect);
    layer->SetDirtyRegion(info.srcRect);
    layer->SetBlendType(info.blendType);
    layer->SetCropRect(info.srcRect);
    layers.emplace_back(layer);
}

bool RsRenderServiceUtil::IsNeedClient(RSSurfaceRenderNode* node)
{
    if (node == nullptr) {
        ROSEN_LOGE("RsRenderServiceUtil::ComposeSurface node is empty");
        return false;
    }
    auto filter = std::static_pointer_cast<RSBlurFilter>(node->GetRenderProperties().GetBackgroundFilter());
    if (filter != nullptr && filter->GetBlurRadiusX() > 0 && filter->GetBlurRadiusY() > 0) {
        return true;
    }
    auto transitionProperties = node->GetAnimationManager().GetTransitionProperties();
    if (!transitionProperties) {
        return false;
    }
    SkMatrix matrix = transitionProperties->GetRotate();
    float value[9];
    matrix.get9(value);
    float rAngle = -round(atan2(value[SkMatrix::kMSkewX], value[SkMatrix::kMScaleX]) * (180 / PI));
    return rAngle > 0;
}

// inner interface
void RsRenderServiceUtil::DealAnimation(SkCanvas& canvas, SkPaint& paint, RSSurfaceRenderNode& node)
{
    auto transitionProperties = node.GetAnimationManager().GetTransitionProperties();
    if (transitionProperties == nullptr) {
        ROSEN_LOGD("RsRenderServiceUtil::dealAnimation: node's transitionProperties is nullptr.");
        return;
    }

    const RSProperties& property = node.GetRenderProperties();

    paint.setAlphaf(paint.getAlphaf() * transitionProperties->GetAlpha());
    auto translate = transitionProperties->GetTranslate();
    canvas.translate(translate.x_, translate.y_);

    // scale and rotate about the center of node, currently scaleZ is not used
    auto center = property.GetBoundsSize() * 0.5f;
    auto scale = transitionProperties->GetScale();
    canvas.translate(center.x_, center.y_);
    canvas.scale(scale.x_, scale.y_);
    canvas.concat(transitionProperties->GetRotate());
    canvas.translate(-center.x_, -center.y_);
}

// inner interface
void RsRenderServiceUtil::Draw(SkCanvas& canvas, BufferDrawParameters& params, RSSurfaceRenderNode& node)
{
    SkPaint paint;
    paint.setAntiAlias(params.antiAlias);
    paint.setAlphaf(params.alpha);
    SkBitmap& bitmap = params.bitmap;
    const SkPixmap& pixmap = params.pixmap;
    if (bitmap.installPixels(pixmap)) {
        canvas.save();
        if (params.onDisplay) {
            canvas.setMatrix(params.transform);
            canvas.scale(params.widthScale, params.heightScale);
            DealAnimation(canvas, paint, node);
            const RSProperties& property = node.GetRenderProperties();
            auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
            if (filter != nullptr) {
                auto skRectPtr = std::make_unique<SkRect>();
                skRectPtr->setXYWH(0, 0, params.dstRect.width(), params.dstRect.height());
                RSPropertiesPainter::SaveLayerForFilter(property, canvas, filter, skRectPtr);
                RSPropertiesPainter::RestoreForFilter(canvas);
            }
        }
        canvas.drawBitmapRect(bitmap, params.dstRect, &paint);
        canvas.restore();
    }
}

void RsRenderServiceUtil::DrawBuffer(SkCanvas* canvas, sptr<OHOS::SurfaceBuffer> buffer,
    RSSurfaceRenderNode& node, bool isDrawnOnDisplay)
{
    if (!canvas) {
        ROSEN_LOGE("RsRenderServiceUtil::DrawBuffer canvas is nullptr");
        return;
    }
    if (!buffer) {
        ROSEN_LOGE("RsRenderServiceUtil::DrawBuffer buffer is nullptr");
        return;
    }
    auto addr = buffer->GetVirAddr();
    if (addr == nullptr) {
        ROSEN_LOGE("RsRenderServiceUtil::DrawBuffer this buffer have no vir addr");
        return;
    }
    if (buffer->GetWidth() <= 0 || buffer->GetHeight() <= 0) {
        ROSEN_LOGE("RsRenderServiceUtil::DrawBuffer this buffer width or height is negative [%d %d]",
            buffer->GetWidth(), buffer->GetHeight());
        return;
    }

    BufferDrawParameters params;
    SkImageInfo imageInfo = Detail::GenerateSkImageInfo(buffer);
    params.pixmap = SkPixmap(imageInfo, buffer->GetVirAddr(), buffer->GetStride());
    Detail::FillDrawParameters(params, buffer, node);
    params.onDisplay = isDrawnOnDisplay;

    Draw(*canvas, params, node);
}

void RsRenderServiceUtil::DrawBuffer(SkCanvas& canvas, const sptr<OHOS::SurfaceBuffer>& buffer,
    RSSurfaceRenderNode& node, ColorGamut dstGamut, bool isDrawnOnDisplay)
{
    if (buffer == nullptr || buffer->GetHeight() < 0 || buffer->GetWidth() < 0 ||
        buffer->GetStride() < 0 || buffer->GetSize() == 0 || buffer->GetVirAddr() == nullptr) {
        ROSEN_LOGE("RsRenderServiceUtil::DrawBuffer: buffer is not valid!");
        return;
    }

    ColorGamut srcGamut = static_cast<ColorGamut>(buffer->GetSurfaceBufferColorGamut());
    if (srcGamut != dstGamut) {
        std::vector<uint8_t> newBuffer;
        if (Detail::ConvertBufferColorGamut(newBuffer, buffer, srcGamut, dstGamut)) {
            // use newBuffer to draw.
            BufferDrawParameters params;
            SkImageInfo imageInfo = Detail::GenerateSkImageInfo(buffer);
            params.pixmap = SkPixmap(imageInfo, newBuffer.data(), buffer->GetStride());
            Detail::FillDrawParameters(params, buffer, node);
            params.onDisplay = isDrawnOnDisplay;
            Draw(canvas, params, node);
            return;
        }
    }

    DrawBuffer(&canvas, buffer, node, isDrawnOnDisplay);
}
} // namespace Rosen
} // namespace OHOS
