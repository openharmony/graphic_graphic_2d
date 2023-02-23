/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "transaction/rs_marshalling_helper.h"

#include <memory>
#include <message_parcel.h>
#include <sys/mman.h>
#include <unistd.h>

#include "ashmem.h"
#include "include/core/SkDrawable.h"
#include "include/core/SkImage.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPicture.h"
#include "include/core/SkSerialProcs.h"
#include "include/core/SkStream.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkVertices.h"
#include "pixel_map.h"
#include "securec.h"
#include "src/core/SkAutoMalloc.h"
#include "src/core/SkPaintPriv.h"
#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"
#include "src/image/SkImage_Base.h"

#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "animation/rs_render_transition.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "modifier/rs_render_modifier.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "platform/common/rs_log.h"
#include "render/rs_blur_filter.h"
#include "render/rs_filter.h"
#include "render/rs_image.h"
#include "render/rs_material_filter.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "transaction/rs_ashmem_helper.h"

namespace OHOS {
namespace Rosen {

#define MARSHALLING_AND_UNMARSHALLING(TYPE, TYPENAME)                      \
    bool RSMarshallingHelper::Marshalling(Parcel& parcel, const TYPE& val) \
    {                                                                      \
        return parcel.Write##TYPENAME(val);                                \
    }                                                                      \
    bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, TYPE& val)     \
    {                                                                      \
        return parcel.Read##TYPENAME(val);                                 \
    }

// basic types
MARSHALLING_AND_UNMARSHALLING(bool, Bool)
MARSHALLING_AND_UNMARSHALLING(int8_t, Int8)
MARSHALLING_AND_UNMARSHALLING(uint8_t, Uint8)
MARSHALLING_AND_UNMARSHALLING(int16_t, Int16)
MARSHALLING_AND_UNMARSHALLING(uint16_t, Uint16)
MARSHALLING_AND_UNMARSHALLING(int32_t, Int32)
MARSHALLING_AND_UNMARSHALLING(uint32_t, Uint32)
MARSHALLING_AND_UNMARSHALLING(int64_t, Int64)
MARSHALLING_AND_UNMARSHALLING(uint64_t, Uint64)
MARSHALLING_AND_UNMARSHALLING(float, Float)
MARSHALLING_AND_UNMARSHALLING(double, Double)

#undef MARSHALLING_AND_UNMARSHALLING

namespace {
template<typename T, typename P>
static inline sk_sp<T> sk_reinterpret_cast(sk_sp<P> ptr)
{
    return sk_sp<T>(static_cast<T*>(SkSafeRef(ptr.get())));
}
} // namespace

// SkData
bool RSMarshallingHelper::Marshalling(Parcel& parcel, sk_sp<SkData> val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }

    bool ret = parcel.WriteInt32(val->size());
    if (val->size() == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Marshalling SkData size is 0");
        return ret;
    }

    ret = ret && RSMarshallingHelper::WriteToParcel(parcel, val->data(), val->size());
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling SkData");
    }
    return ret;
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkData>& val)
{
    int32_t size = parcel.ReadInt32();
    if (size == -1) {
        val = nullptr;
        return true;
    }
    if (size == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Unmarshalling SkData size is 0");
        val = SkData::MakeEmpty();
        return true;
    }

    const void* data = RSMarshallingHelper::ReadFromParcel(parcel, size);
    if (data == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkData");
        return false;
    }

    if (static_cast<uint32_t>(size) < MIN_DATA_SIZE) {
        val = SkData::MakeWithoutCopy(data, size);
    } else {
        val = SkData::MakeFromMalloc(data, size);
    }
    return val != nullptr;
}
bool RSMarshallingHelper::SkipSkData(Parcel& parcel)
{
    int32_t size = parcel.ReadInt32();
    if (size <= 0) {
        return true;
    }
    return SkipFromParcel(parcel, size);
}

bool RSMarshallingHelper::UnmarshallingWithCopy(Parcel& parcel, sk_sp<SkData>& val)
{
    bool success = Unmarshalling(parcel, val);
    if (success) {
        if (val && val->size() < MIN_DATA_SIZE) {
            val = SkData::MakeWithCopy(val->data(), val->size());
        }
    }
    return success;
}

// SkTypeface serial proc
sk_sp<SkData> RSMarshallingHelper::SerializeTypeface(SkTypeface* tf, void* ctx)
{
    if (tf == nullptr) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::SerializeTypeface SkTypeface is nullptr");
        return nullptr;
    }
    return tf->serialize();
}

// SkTypeface deserial proc
sk_sp<SkTypeface> RSMarshallingHelper::DeserializeTypeface(const void* data, size_t length, void* ctx)
{
    SkMemoryStream stream(data, length);
    return SkTypeface::MakeDeserialize(&stream);
}

// SkTextBlob
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkTextBlob>& val)
{
    sk_sp<SkData> data;
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling SkTextBlob is nullptr");
        return Marshalling(parcel, data);
    }
    SkSerialProcs serialProcs;
    serialProcs.fTypefaceProc = &RSMarshallingHelper::SerializeTypeface;
    data = val->serialize(serialProcs);
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkTextBlob>& val)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkTextBlob");
        return false;
    }
    if (data == nullptr) {
        val = nullptr;
        return true;
    }
    SkDeserialProcs deserialProcs;
    deserialProcs.fTypefaceProc = &RSMarshallingHelper::DeserializeTypeface;
    val = SkTextBlob::Deserialize(data->data(), data->size(), deserialProcs);
    return val != nullptr;
}

// SkPaint
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const SkPaint& val)
{
    SkBinaryWriteBuffer writer;
    writer.writePaint(val);
    size_t length = writer.bytesWritten();
    sk_sp<SkData> data = SkData::MakeUninitialized(length);
    writer.writeToMemory(data->writable_data());
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, SkPaint& val)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkPaint");
        return false;
    }
    SkReadBuffer reader(data->data(), data->size());
    reader.readPaint(&val, nullptr);
    return true;
}

// SkImage
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkImage>& val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }
    int32_t type = val->isLazyGenerated();
    parcel.WriteInt32(type);
    if (type == 1) {
        ROSEN_LOGD("RSMarshallingHelper::Marshalling SkImage isLazyGenerated");
        SkBinaryWriteBuffer writer;
        writer.writeImage(val.get());
        size_t length = writer.bytesWritten();
        sk_sp<SkData> data = SkData::MakeUninitialized(length);
        writer.writeToMemory(data->writable_data());
        return Marshalling(parcel, data);
    } else {
        SkBitmap bitmap;
        if (!as_IB(val.get())->getROPixels(&bitmap)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling SkImage getROPixels failed");
            return false;
        }
        SkPixmap pixmap;
        if (!bitmap.peekPixels(&pixmap)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling SkImage peekPixels failed");
            return false;
        }
        size_t rb = pixmap.rowBytes();
        int width = pixmap.width();
        int height = pixmap.height();
        const void* addr = pixmap.addr();
        size_t size = rb * static_cast<size_t>(height);

        parcel.WriteUint32(size);
        if (!WriteToParcel(parcel, addr, size)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling SkImage WriteToParcel failed");
            return false;
        }

        parcel.WriteUint32(rb);
        parcel.WriteInt32(width);
        parcel.WriteInt32(height);

        parcel.WriteUint32(pixmap.colorType());
        parcel.WriteUint32(pixmap.alphaType());

        if (pixmap.colorSpace() == nullptr) {
            parcel.WriteUint32(0);
            return true;
        } else {
            auto data = pixmap.colorSpace()->serialize();
            parcel.WriteUint32(data->size());
            if (!WriteToParcel(parcel, data->data(), data->size())) {
                ROSEN_LOGE("RSMarshallingHelper::Marshalling SkImage WriteToParcel colorSpace failed");
                return false;
            }
        }
        return true;
    }
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkImage>& val)
{
    int32_t type = parcel.ReadInt32();
    if (type == -1) {
        val = nullptr;
        return true;
    }
    if (type == 1) {
        sk_sp<SkData> data;
        ROSEN_LOGD("RSMarshallingHelper::Unmarshalling lazy");
        if (!Unmarshalling(parcel, data)) {
            ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling SkImage");
            return false;
        }
        SkReadBuffer reader(data->data(), data->size());
        val = reader.readImage();
        return val != nullptr;
    } else {
        size_t pixmapSize = parcel.ReadUint32();
        const void* addr = RSMarshallingHelper::ReadFromParcel(parcel, pixmapSize);
        if (addr == nullptr) {
            ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling SkData addr");
            return false;
        }

        size_t rb = parcel.ReadUint32();
        int width = parcel.ReadInt32();
        int height = parcel.ReadInt32();

        SkColorType colorType = static_cast<SkColorType>(parcel.ReadUint32());
        SkAlphaType alphaType = static_cast<SkAlphaType>(parcel.ReadUint32());
        sk_sp<SkColorSpace> colorSpace;

        size_t size = parcel.ReadUint32();
        if (size == 0) {
            colorSpace = nullptr;
        } else {
            const void* data = RSMarshallingHelper::ReadFromParcel(parcel, size);
            if (data == nullptr) {
                ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling SkData data");
                return false;
            }
            colorSpace = SkColorSpace::Deserialize(data, size);
            if (size >= MIN_DATA_SIZE) {
                free(const_cast<void*>(data));
            }
        }

        SkImageInfo imageInfo = SkImageInfo::Make(width, height, colorType, alphaType, colorSpace);
        auto skData = pixmapSize < MIN_DATA_SIZE ? SkData::MakeWithCopy(addr, pixmapSize)
                                                 : SkData::MakeFromMalloc(addr, pixmapSize);
        val = SkImage::MakeRasterData(imageInfo, skData, rb);
        return val != nullptr;
    }
}

bool RSMarshallingHelper::SkipSkImage(Parcel& parcel)
{
    int32_t type = parcel.ReadInt32();
    if (type == -1) {
        return true;
    }
    sk_sp<SkData> data;
    if (type == 1) {
        ROSEN_LOGD("RSMarshallingHelper::SkipSkImage lazy");
        return SkipSkData(parcel);
    } else {
        size_t pixmapSize = parcel.ReadUint32();
        if (!SkipFromParcel(parcel, pixmapSize)) {
            ROSEN_LOGE("failed RSMarshallingHelper::SkipSkImage SkData addr");
            return false;
        }

        parcel.ReadUint32();
        parcel.ReadInt32();
        parcel.ReadInt32();
        parcel.ReadUint32();
        parcel.ReadUint32();
        size_t size = parcel.ReadUint32();
        return size == 0 ? true : SkipFromParcel(parcel, size);
    }
}

// SkPicture
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkPicture>& val)
{
    sk_sp<SkData> data;
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling SkPicture is nullptr");
        return Marshalling(parcel, data);
    }
    data = val->serialize();
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkPicture>& val)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkPicture");
        return false;
    }
    if (data == nullptr) {
        val = nullptr;
        return true;
    }
    val = SkPicture::MakeFromData(data->data(), data->size());
    return val != nullptr;
}

// SkVertices
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkVertices>& val)
{
    sk_sp<SkData> data;
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling SkVertices is nullptr");
        return Marshalling(parcel, data);
    }
    data = val->encode();
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkVertices>& val)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkVertices");
        return false;
    }
    if (data == nullptr) {
        val = nullptr;
        return true;
    }
    val = SkVertices::Decode(data->data(), data->size());
    return val != nullptr;
}

// SkRect
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const SkRect& rect)
{
    SkBinaryWriteBuffer writer;
    writer.writeRect(rect);
    size_t length = writer.bytesWritten();
    sk_sp<SkData> data = SkData::MakeUninitialized(length);
    writer.writeToMemory(data->writable_data());
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, SkRect& rect)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkRect");
        return false;
    }
    SkReadBuffer reader(data->data(), data->size());
    reader.readRect(&rect);
    return true;
}

// SkRegion
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const SkRegion& region)
{
    SkBinaryWriteBuffer writer;
    writer.writeRegion(region);
    size_t length = writer.bytesWritten();
    sk_sp<SkData> data = SkData::MakeUninitialized(length);
    writer.writeToMemory(data->writable_data());
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, SkRegion& region)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkRegion");
        return false;
    }
    SkReadBuffer reader(data->data(), data->size());
    reader.readRegion(&region);
    return true;
}

// SKPath
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const SkPath& val)
{
    SkBinaryWriteBuffer writer;
    writer.writePath(val);
    size_t length = writer.bytesWritten();
    sk_sp<SkData> data = SkData::MakeUninitialized(length);
    writer.writeToMemory(data->writable_data());
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, SkPath& val)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SKPath");
        return false;
    }
    SkReadBuffer reader(data->data(), data->size());
    reader.readPath(&val);
    return true;
}

// SkFlattenable
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkFlattenable>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling SkFlattenable is nullptr");
        return parcel.WriteInt32(-1);
    }
    sk_sp<SkData> data = val->serialize();
    return parcel.WriteInt32(val->getFlattenableType()) && Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkFlattenable>& val)
{
    int32_t type = parcel.ReadInt32();
    if (type == -1) {
        val = nullptr;
        return true;
    }
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkFlattenable");
        return false;
    }
    val = SkFlattenable::Deserialize(static_cast<SkFlattenable::Type>(type), data->data(), data->size());
    return val != nullptr;
}

// SkDrawable
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkDrawable>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling SkDrawable is nullptr");
    }
    return Marshalling(parcel, sk_sp<SkFlattenable>(val));
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkDrawable>& val)
{
    sk_sp<SkFlattenable> flattenablePtr;
    if (!Unmarshalling(parcel, flattenablePtr)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkDrawable");
        return false;
    }
    val = sk_reinterpret_cast<SkDrawable>(flattenablePtr);
    return true;
}

// SkImageFilter
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkImageFilter>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling SkImageFilter is nullptr");
    }
    return Marshalling(parcel, sk_sp<SkFlattenable>(val));
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkImageFilter>& val)
{
    sk_sp<SkFlattenable> flattenablePtr;
    if (!Unmarshalling(parcel, flattenablePtr)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkImageFilter");
        return false;
    }
    val = sk_reinterpret_cast<SkImageFilter>(flattenablePtr);
    return true;
}

// RSShader
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSShader>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSShader is nullptr");
        return parcel.WriteInt32(-1);
    }
    return parcel.WriteInt32(1) && Marshalling(parcel, sk_sp<SkFlattenable>(val->GetSkShader()));
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSShader>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    sk_sp<SkFlattenable> flattenablePtr;
    if (!Unmarshalling(parcel, flattenablePtr)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling RSShader");
        return false;
    }
    auto shaderPtr = sk_reinterpret_cast<SkShader>(flattenablePtr);
    val = RSShader::CreateRSShader(shaderPtr);
    return val != nullptr;
}

// RSPath
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSPath>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSPath is nullptr");
        return parcel.WriteInt32(-1);
    }
    return parcel.WriteInt32(1) && Marshalling(parcel, val->GetSkiaPath());
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSPath>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    SkPath path;
    if (!Unmarshalling(parcel, path)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling RSPath");
        return false;
    }
    val = RSPath::CreateRSPath(path);
    return val != nullptr;
}

// RSMask
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSMask>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSMask is nullptr");
        return parcel.WriteInt32(-1);
    }
    return parcel.WriteInt32(1) && val->Marshalling(parcel);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSMask>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(RSMask::Unmarshalling(parcel));
    return val != nullptr;
}

// RSFilter
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSFilter>& val)
{
    if (!val) {
        return parcel.WriteInt32(RSFilter::NONE);
    }
    bool success = parcel.WriteInt32(static_cast<int>(val->GetFilterType()));
    switch (val->GetFilterType()) {
        case RSFilter::BLUR: {
            auto blur = std::static_pointer_cast<RSBlurFilter>(val);
            success = success && parcel.WriteFloat(blur->GetBlurRadiusX()) && parcel.WriteFloat(blur->GetBlurRadiusY());
            break;
        }
        case RSFilter::MATERIAL: {
            auto material = std::static_pointer_cast<RSMaterialFilter>(val);
            success = success && parcel.WriteInt32(material->style_) && parcel.WriteFloat(material->dipScale_) &&
                      parcel.WriteInt32(material->colorMode_);
            break;
        }
        default:
            break;
    }
    return success;
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSFilter>& val)
{
    int type = 0;
    bool success = parcel.ReadInt32(type);
    switch (static_cast<RSFilter::FilterType>(type)) {
        case RSFilter::BLUR: {
            float blurRadiusX;
            float blurRadiusY;
            success = success && parcel.ReadFloat(blurRadiusX) && parcel.ReadFloat(blurRadiusY);
            if (success) {
                val = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
            }
            break;
        }
        case RSFilter::MATERIAL: {
            int style;
            float dipScale;
            int colorMode;
            success = success && parcel.ReadInt32(style) && parcel.ReadFloat(dipScale) && parcel.ReadInt32(colorMode);
            if (success) {
                val = RSFilter::CreateMaterialFilter(style, dipScale, static_cast<BLUR_COLOR_MODE>(colorMode));
            }
            break;
        }
        default: {
            val = nullptr;
            break;
        }
    }
    return success;
}

// RSImage
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSImage>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSImage is nullptr");
        return parcel.WriteInt32(-1);
    }
    return parcel.WriteInt32(1) && val->Marshalling(parcel);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSImage>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(RSImage::Unmarshalling(parcel));
    return val != nullptr;
}

// Media::PixelMap
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<Media::PixelMap>& val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }
    if (!(parcel.WriteInt32(1) && val->Marshalling(parcel))) {
        ROSEN_LOGE("failed RSMarshallingHelper::Marshalling Media::PixelMap");
        return false;
    }
    return true;
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Media::PixelMap>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(Media::PixelMap::Unmarshalling(parcel));
    if (val == nullptr) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling Media::PixelMap");
        return false;
    }
    return true;
}

#define MARSHALLING_AND_UNMARSHALLING(TYPE)                                                 \
    bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TYPE>& val) \
    {                                                                                       \
        return parcel.WriteParcelable(val.get());                                           \
    }                                                                                       \
    bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TYPE>& val)     \
    {                                                                                       \
        val.reset(parcel.ReadParcelable<TYPE>());                                           \
        return val != nullptr;                                                              \
    }
MARSHALLING_AND_UNMARSHALLING(RSRenderTransition)
MARSHALLING_AND_UNMARSHALLING(RSRenderTransitionEffect)
MARSHALLING_AND_UNMARSHALLING(DrawCmdList)
#undef MARSHALLING_AND_UNMARSHALLING

#define MARSHALLING_AND_UNMARSHALLING(TEMPLATE)                                                 \
    bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE>& val) \
    {                                                                                           \
        return parcel.WriteParcelable(val.get());                                               \
    }                                                                                           \
    bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE>& val)     \
    {                                                                                           \
        val.reset(parcel.ReadParcelable<TEMPLATE>());                                           \
        return val != nullptr;                                                                  \
    }

MARSHALLING_AND_UNMARSHALLING(RSRenderCurveAnimation)
MARSHALLING_AND_UNMARSHALLING(RSRenderKeyframeAnimation)
MARSHALLING_AND_UNMARSHALLING(RSRenderSpringAnimation)
MARSHALLING_AND_UNMARSHALLING(RSRenderPathAnimation)
#undef MARSHALLING_AND_UNMARSHALLING

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSRenderModifier>& val)
{
    return val != nullptr && val->Marshalling(parcel);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSRenderModifier>& val)
{
    val.reset(RSRenderModifier::Unmarshalling(parcel));
    return val != nullptr;
}

#define MARSHALLING_AND_UNMARSHALLING(TEMPLATE)                                                    \
    template<typename T>                                                                           \
    bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<T>>& val) \
    {                                                                                              \
        return parcel.WriteUint64(val->GetId()) && Marshalling(parcel, val->Get());                \
    }                                                                                              \
    template<typename T>                                                                           \
    bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<T>>& val)     \
    {                                                                                              \
        PropertyId id = 0;                                                                         \
        if (!parcel.ReadUint64(id)) {                                                              \
            return false;                                                                          \
        }                                                                                          \
        T value;                                                                                   \
        if (!Unmarshalling(parcel, value)) {                                                       \
            return false;                                                                          \
        }                                                                                          \
        val.reset(new TEMPLATE<T>(value, id));                                                     \
        return val != nullptr;                                                                     \
    }

MARSHALLING_AND_UNMARSHALLING(RSRenderProperty)
MARSHALLING_AND_UNMARSHALLING(RSRenderAnimatableProperty)
#undef MARSHALLING_AND_UNMARSHALLING

#define EXPLICIT_INSTANTIATION(TEMPLATE, TYPE)                                                                  \
    template bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<TYPE>>& val); \
    template bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<TYPE>>& val);

#define BATCH_EXPLICIT_INSTANTIATION(TEMPLATE)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, bool)                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, float)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, int)                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, Color)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Gravity)                   \
    EXPLICIT_INSTANTIATION(TEMPLATE, Matrix3f)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Quaternion)                \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSFilter>) \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSImage>)  \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSMask>)   \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSPath>)   \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSShader>) \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector2f)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<uint32_t>)         \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<Color>)            \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<DrawCmdList>)

BATCH_EXPLICIT_INSTANTIATION(RSRenderProperty)

#undef EXPLICIT_INSTANTIATION
#undef BATCH_EXPLICIT_INSTANTIATION

#define EXPLICIT_INSTANTIATION(TEMPLATE, TYPE)                                                                  \
    template bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<TYPE>>& val); \
    template bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<TYPE>>& val);

#define BATCH_EXPLICIT_INSTANTIATION(TEMPLATE)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, float)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, int)                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, Color)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Matrix3f)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Quaternion)                \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSFilter>) \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector2f)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<Color>)            \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f)

BATCH_EXPLICIT_INSTANTIATION(RSRenderAnimatableProperty)

#undef EXPLICIT_INSTANTIATION
#undef BATCH_EXPLICIT_INSTANTIATION

bool RSMarshallingHelper::WriteToParcel(Parcel& parcel, const void* data, size_t size)
{
    if (data == nullptr) {
        ROSEN_LOGE("RSMarshallingHelper::WriteToParcel data is nullptr");
        return false;
    }
    if (size > MAX_DATA_SIZE) {
        ROSEN_LOGD("RSMarshallingHelper::WriteToParcel data exceed MAX_DATA_SIZE, size:%zu", size);
    }

    if (!parcel.WriteUint32(size)) {
        return false;
    }
    if (size < MIN_DATA_SIZE) {
        return parcel.WriteUnpadBuffer(data, size);
    }

    // write to ashmem
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(size, PROT_READ | PROT_WRITE);
    if (!ashmemAllocator) {
        ROSEN_LOGE("RSMarshallingHelper::WriteToParcel CreateAshmemAllocator fail");
        return false;
    }
    int fd = ashmemAllocator->GetFd();
    if (!(static_cast<MessageParcel*>(&parcel)->WriteFileDescriptor(fd))) {
        ROSEN_LOGE("RSMarshallingHelper::WriteToParcel WriteFileDescriptor error");
        return false;
    }
    if (!ashmemAllocator->WriteToAshmem(data, size)) {
        ROSEN_LOGE("RSMarshallingHelper::WriteToParcel memcpy_s failed");
        return false;
    }
    return true;
}

const void* RSMarshallingHelper::ReadFromParcel(Parcel& parcel, size_t size)
{
    uint32_t bufferSize = parcel.ReadUint32();
    if (static_cast<unsigned int>(bufferSize) != size) {
        ROSEN_LOGE("RSMarshallingHelper::ReadFromParcel size mismatch");
        return nullptr;
    }

    if (static_cast<unsigned int>(bufferSize) < MIN_DATA_SIZE) {
        return parcel.ReadUnpadBuffer(size);
    }
    // read from ashmem
    int fd = static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocatorWithFd(fd, size, PROT_READ);
    if (!ashmemAllocator) {
        ROSEN_LOGE("RSMarshallingHelper::ReadFromParcel CreateAshmemAllocator fail");
        return nullptr;
    }
    return ashmemAllocator->CopyFromAshmem(size);
}

bool RSMarshallingHelper::SkipFromParcel(Parcel& parcel, size_t size)
{
    int32_t bufferSize = parcel.ReadInt32();
    if (static_cast<unsigned int>(bufferSize) != size) {
        ROSEN_LOGE("RSMarshallingHelper::SkipFromParcel size mismatch");
        return false;
    }

    if (static_cast<unsigned int>(bufferSize) < MIN_DATA_SIZE) {
        parcel.SkipBytes(size);
        return true;
    }
    // read from ashmem
    int fd = static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocatorWithFd(fd, size, PROT_READ);
    return ashmemAllocator != nullptr;
}
} // namespace Rosen
} // namespace OHOS
