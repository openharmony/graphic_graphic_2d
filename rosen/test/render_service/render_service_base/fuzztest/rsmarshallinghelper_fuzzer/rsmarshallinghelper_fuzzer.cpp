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

#include "rsmarshallinghelper_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <hilog/log.h>
#include <parcel.h>
#include <securec.h>

#include "animation/rs_render_particle.h"
#include "draw/canvas.h"
#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/image_native/pixel_map.h"
#include "render/rs_shader.h"
#include "render/rs_filter.h"
#include "recording/mask_cmd_list.h"
#include "transaction/rs_marshalling_helper.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
using namespace Drawing;

namespace {
const uint8_t* DATA = nullptr;
constexpr uint32_t MAX_ARRAY_SIZE = 2500;
constexpr size_t FUNCTYPE_SIZE = 4;
constexpr size_t MATRIXTYPE_SIZE = 5;
constexpr size_t POINTNUMBER = 4;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoUnmarshalling(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    std::shared_ptr<RSLinearGradientBlurPara> val;
    std::shared_ptr<MotionBlurParam> motionBlurParam;
    std::shared_ptr<EmitterUpdater> emitterUpdater;
    std::vector<std::shared_ptr<EmitterUpdater>> emitterUpdaters;
    std::shared_ptr<ParticleNoiseField> particleNoiseField;
    std::shared_ptr<ParticleNoiseFields> noiseFields;
    std::shared_ptr<ParticleRenderParams> params;
    std::vector<std::shared_ptr<ParticleRenderParams>> particleRenderParams;
    std::shared_ptr<RSPath> path;
    std::shared_ptr<RSFilter> filter;
    std::shared_ptr<RSRenderModifier> modifier;
    std::shared_ptr<RSRenderPropertyBase> property;
    std::shared_ptr<RSImageBase> base = std::make_shared<RSImageBase>();
    std::shared_ptr<RSImage> image = std::make_shared<RSImage>();
    std::shared_ptr<Media::PixelMap> map = std::make_shared<Media::PixelMap>();
    std::shared_ptr<RectT<float>> rectt = std::make_shared<RectT<float>>();
    EmitterConfig emitterConfig;
    ParticleVelocity velocity;
    RenderParticleColorParaType color;
    RRectT<float> rrect;
    bool isMalloc = false;

    float start = GetData<float>();
    float end = GetData<float>();
    Range<float> value(start, end);
    Range<float> randomValue(start, end);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> valChangeOverLife;
    parcel.WriteInt16(1);
    parcel.WriteUint64(1);
    std::shared_ptr<RSInterpolator> interpolator = RSInterpolator::Unmarshalling(parcel);
    auto changeInOverLife = std::make_shared<ChangeInOverLife<float>>(0.f, 0.f, 0, 0, interpolator);
    valChangeOverLife.push_back(changeInOverLife);
    RenderParticleParaType<float> val2(value, ParticleUpdator::RANDOM, randomValue, std::move(valChangeOverLife));

    filter = RSFilter::CreateBlurFilter(start, end);
    filter->type_ = RSFilter::BLUR;

    RSTransactionData rsTransactionData;
    rsTransactionData.UnmarshallingCommand(parcel);
    RSMarshallingHelper::Unmarshalling(parcel, val);
    RSMarshallingHelper::Unmarshalling(parcel, motionBlurParam);
    RSMarshallingHelper::Unmarshalling(parcel, emitterUpdater);
    RSMarshallingHelper::Unmarshalling(parcel, emitterUpdaters);
    RSMarshallingHelper::Unmarshalling(parcel, particleNoiseField);
    RSMarshallingHelper::Unmarshalling(parcel, noiseFields);
    RSMarshallingHelper::Unmarshalling(parcel, emitterConfig);
    RSMarshallingHelper::Unmarshalling(parcel, velocity);
    RSMarshallingHelper::Unmarshalling(parcel, val2);
    RSMarshallingHelper::Unmarshalling(parcel, color);
    RSMarshallingHelper::Unmarshalling(parcel, params);
    RSMarshallingHelper::Unmarshalling(parcel, particleRenderParams);
    RSMarshallingHelper::Unmarshalling(parcel, path);
    RSMarshallingHelper::Unmarshalling(parcel, filter);
    RSMarshallingHelper::Unmarshalling(parcel, base);
    RSMarshallingHelper::Unmarshalling(parcel, image);
    RSMarshallingHelper::Unmarshalling(parcel, map);
    RSMarshallingHelper::Unmarshalling(parcel, rectt);
    RSMarshallingHelper::Unmarshalling(parcel, rrect);
    RSMarshallingHelper::Unmarshalling(parcel, modifier);
    RSMarshallingHelper::Unmarshalling(parcel, property);
    RSMarshallingHelper::ReadFromParcel(parcel, 1, isMalloc);
    RSMarshallingHelper::ReadFromAshmem(parcel, 1, isMalloc);
    return true;
}

bool DoMarshallingHelper001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    size_t length = GetData<size_t>() % MAX_ARRAY_SIZE;
    if (length == 0) {
        return false;
    }
    char* path = new char[length];
    if (path == nullptr) {
        return false;
    }
    for (size_t i = 0; i < length; i++) {
        path[i] = GetData<char>();
    }
    path[length - 1] = '\0';
    std::shared_ptr<Typeface> val = Typeface::MakeFromFile(path, length);
    RSMarshallingHelper::Marshalling(parcel, val);
    RSMarshallingHelper::Unmarshalling(parcel, val);
    if (path != nullptr) {
        delete [] path;
        path = nullptr;
    }
    return true;
}
bool DoMarshallingHelper002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    auto dataVal = std::make_shared<Data>();
    size_t length = GetData<size_t>() % MAX_ARRAY_SIZE;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetData<char>();
    }
    dataVal->BuildWithoutCopy(dataText, length);
    RSMarshallingHelper::Marshalling(parcel, dataVal);
    RSMarshallingHelper::Unmarshalling(parcel, dataVal);
    RSMarshallingHelper::UnmarshallingWithCopy(parcel, dataVal);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DoMarshallingHelper003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    size_t length = GetData<size_t>() % MAX_ARRAY_SIZE;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetData<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    bool isCopy = GetData<bool>();
    static std::shared_ptr<MaskCmdList> maskCmdList = MaskCmdList::CreateFromData(cmdListData, isCopy);
    RSMarshallingHelper::Marshalling(parcel, maskCmdList);
    RSMarshallingHelper::Unmarshalling(parcel, maskCmdList);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DoMarshallingHelper004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    MessageParcel parcel;
    Bitmap bitmap;
    int32_t width = GetData<int32_t>() % MAX_ARRAY_SIZE;
    int32_t height = GetData<int32_t>() % MAX_ARRAY_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool ret = bitmap.Build(width, height, bitmapFormat);
    if (!ret) {
        return false;
    }
    RSMarshallingHelper::Marshalling(parcel, bitmap);
    RSMarshallingHelper::Unmarshalling(parcel, bitmap);
    return true;
}

bool DoMarshallingHelper005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    uint32_t funcType = GetData<uint32_t>();
    uint32_t matrixType = GetData<uint32_t>();
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateRGB(
        static_cast<CMSTransferFuncType>(funcType % FUNCTYPE_SIZE),
        static_cast<CMSMatrixType>(matrixType % MATRIXTYPE_SIZE));
    RSMarshallingHelper::ReadColorSpaceFromParcel(parcel, colorSpace);
    return true;
}

bool DoMarshallingHelper006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    MessageParcel parcel;
    void* imagePixelPtr = nullptr;
    Bitmap bitmap;
    int32_t width = GetData<int32_t>() % MAX_ARRAY_SIZE;
    int32_t height = GetData<int32_t>() % MAX_ARRAY_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool ret = bitmap.Build(width, height, bitmapFormat);
    if (!ret) {
        return false;
    }
    auto image = std::make_shared<Image>();
    ret = image->BuildFromBitmap(bitmap);
    if (!ret) {
        return false;
    }
    RSMarshallingHelper::Marshalling(parcel, image);
    RSMarshallingHelper::UnmarshallingNoLazyGeneratedImage(parcel, image, imagePixelPtr);
    return true;
}

bool DoMarshallingHelper007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    MessageParcel parcel;
    Bitmap bitmap;
    int width = GetData<int>() % MAX_ARRAY_SIZE;
    int height = GetData<int>() % MAX_ARRAY_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool ret = bitmap.Build(width, height, bitmapFormat);
    if (!ret) {
        return false;
    }
    auto image = std::make_shared<Image>();
    ret = image->BuildFromBitmap(bitmap);
    if (!ret) {
        return false;
    }
    RSMarshallingHelper::Marshalling(parcel, image);
    RSMarshallingHelper::Unmarshalling(parcel, image);
    return true;
}

bool DoMarshallingHelper008(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    MessageParcel parcel;
    void* imagePixelPtr = nullptr;
    Bitmap bitmap;
    int32_t width = GetData<int32_t>() % MAX_ARRAY_SIZE;
    int32_t height = GetData<int32_t>() % MAX_ARRAY_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool ret = bitmap.Build(width, height, bitmapFormat);
    if (!ret) {
        return false;
    }
    auto image = std::make_shared<Image>();
    ret = image->BuildFromBitmap(bitmap);
    if (!ret) {
        return false;
    }
    RSMarshallingHelper::Marshalling(parcel, image);
    RSMarshallingHelper::Unmarshalling(parcel, image, imagePixelPtr);
    return true;
}

bool DoMarshallingHelper009(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    ColorQuad color = GetData<ColorQuad>();
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateColorShader(color);
    std::shared_ptr<RSShader> shader = RSShader::CreateRSShader(shaderEffect);
    RSMarshallingHelper::Marshalling(parcel, shader);
    RSMarshallingHelper::Unmarshalling(parcel, shader);
    return true;
}

bool DoMarshallingHelper010(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    Matrix matrix;
    scalar scaleX = GetData<scalar>();
    scalar skewX = GetData<scalar>();
    scalar transX = GetData<scalar>();
    scalar skewY = GetData<scalar>();
    scalar scaleY = GetData<scalar>();
    scalar transY = GetData<scalar>();
    scalar persp0 = GetData<scalar>();
    scalar persp1 = GetData<scalar>();
    scalar persp2 = GetData<scalar>();
    matrix.SetMatrix(scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
    RSMarshallingHelper::Marshalling(parcel, matrix);
    RSMarshallingHelper::Unmarshalling(parcel, matrix);
    return true;
}

bool DoMarshallingHelper011(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    DATA = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    size_t length = GetData<size_t>() % MAX_ARRAY_SIZE;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetData<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    bool isCopy = GetData<bool>();
    static std::shared_ptr<DrawCmdList> drawCmdList = DrawCmdList::CreateFromData(cmdListData, isCopy);
    RSMarshallingHelper::Marshalling(parcel, drawCmdList);
    RSMarshallingHelper::Unmarshalling(parcel, drawCmdList);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DoMarshallingHelper012(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    MessageParcel parcel;
    Bitmap bitmap;
    auto image = std::make_shared<Image>();
    int32_t width = GetData<int32_t>() % MAX_ARRAY_SIZE;
    int32_t height = GetData<int32_t>() % MAX_ARRAY_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool ret = bitmap.Build(width, height, bitmapFormat);
    if (!ret) {
        return false;
    }
    ret = image->BuildFromBitmap(bitmap);
    if (!ret) {
        return false;
    }

    auto dataVal = std::make_shared<Data>();
    size_t length = GetData<size_t>() % MAX_ARRAY_SIZE;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetData<char>();
    }
    dataVal->BuildWithoutCopy(dataText, length);

    scalar sx = GetData<scalar>();
    scalar sy = GetData<scalar>();
    scalar sz = GetData<scalar>();
    scalar dx = GetData<scalar>();
    scalar dy = GetData<scalar>();
    scalar dz = GetData<scalar>();
    scalar tx = GetData<scalar>();
    scalar ty = GetData<scalar>();
    scalar tz = GetData<scalar>();
    Drawing::Matrix matrix;
    matrix.SetMatrix(tx, ty, tz, sx, sy, sz, dx, dy, dz);
    AdaptiveImageInfo adaptiveImageInfo;
    adaptiveImageInfo.fitNum = GetData<int32_t>();
    adaptiveImageInfo.repeatNum = GetData<int32_t>();
    for (size_t i = 0; i < POINTNUMBER; i++) {
        Point point;
        int32_t x = GetData<int32_t>();
        int32_t y = GetData<int32_t>();
        point.Set(x, y);
        adaptiveImageInfo.radius[i] = point;
    }
    adaptiveImageInfo.scale = GetData<double>();
    adaptiveImageInfo.uniqueId = GetData<uint32_t>();
    adaptiveImageInfo.width = GetData<int32_t>();
    adaptiveImageInfo.height = GetData<int32_t>();
    adaptiveImageInfo.dynamicRangeMode = GetData<uint32_t>();
    adaptiveImageInfo.fitMatrix = matrix;
    auto extendImageObject = std::make_shared<RSExtendImageObject>(image, dataVal, adaptiveImageInfo);
    RSMarshallingHelper::Marshalling(parcel, extendImageObject);
    RSMarshallingHelper::Unmarshalling(parcel, extendImageObject);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DoMarshallingHelper013(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    int32_t left = GetData<int32_t>();
    int32_t top = GetData<int32_t>();
    int32_t right = GetData<int32_t>();
    int32_t bottom = GetData<int32_t>();
    Rect src = {
        left,
        top,
        right,
        bottom,
    };

    int32_t leftT = GetData<int32_t>();
    int32_t topT = GetData<int32_t>();
    int32_t rightT = GetData<int32_t>();
    int32_t bottomT = GetData<int32_t>();
    Rect dst = {
        leftT,
        topT,
        rightT,
        bottomT,
    };
    auto extendImageBaseObj = std::make_shared<RSExtendImageBaseObj>(nullptr, src, dst);
    RSMarshallingHelper::Marshalling(parcel, extendImageBaseObj);
    RSMarshallingHelper::Unmarshalling(parcel, extendImageBaseObj);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoUnmarshalling(data, size);
    OHOS::Rosen::DoMarshallingHelper001(data, size);
    OHOS::Rosen::DoMarshallingHelper002(data, size);
    OHOS::Rosen::DoMarshallingHelper003(data, size);
    OHOS::Rosen::DoMarshallingHelper004(data, size);
    OHOS::Rosen::DoMarshallingHelper005(data, size);
    OHOS::Rosen::DoMarshallingHelper006(data, size);
    OHOS::Rosen::DoMarshallingHelper007(data, size);
    OHOS::Rosen::DoMarshallingHelper008(data, size);
    OHOS::Rosen::DoMarshallingHelper009(data, size);
    OHOS::Rosen::DoMarshallingHelper010(data, size);
    OHOS::Rosen::DoMarshallingHelper011(data, size);
    OHOS::Rosen::DoMarshallingHelper012(data, size);
    OHOS::Rosen::DoMarshallingHelper013(data, size);
    return 0;
}