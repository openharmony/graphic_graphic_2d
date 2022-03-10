/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <sys/mman.h>
#include <unistd.h>
#include "ashmem.h"
#include "securec.h"

#include <message_parcel.h>
#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_transition.h"
#include "common/rs_color.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "include/core/SkPaint.h"
#include "render/rs_blur_filter.h"
#include "render/rs_filter.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "platform/common/rs_log.h"

#include "src/core/SkAutoMalloc.h"
#include "src/core/SkPaintPriv.h"
#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkImage.h"
#include "include/core/SkSerialProcs.h"
#include "include/core/SkPicture.h"
#include "include/core/SkDrawable.h"
#include "include/core/SkVertices.h"

#include <memory>

#ifdef ROSEN_OHOS
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
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkData>& val)
{
    bool ret = parcel.WriteInt32(val->size());
    if (val->size() == 0) {
        return ret;
    }
    ret &= RSMarshallingHelper::WriteToParcel(parcel, val->data(), val->size());
    ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteToParcel ret:%d", ret);
    return ret;
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkData>& val)
{
    size_t size = parcel.ReadInt32();
    if (size == 0) {
        val = SkData::MakeEmpty();
        return true;
    }
    const void* data = RSMarshallingHelper::ReadFromParcel(parcel, size);
    if (data == nullptr) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling ReadFromParcel failed");
        val = SkData::MakeEmpty();
        return false;
    }
    val = SkData::MakeWithoutCopy(data, size);
    return true;
}

// SkFlattenable
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkFlattenable>& val)
{
    SkBinaryWriteBuffer writer;
    writer.writeFlattenable(val.get());
    SkAutoMalloc buf(writer.bytesWritten());
    writer.writeToMemory(buf.get());
    auto skData = SkData::MakeFromMalloc(buf.get(), writer.bytesWritten());
    return parcel.WriteUint32(val->getFlattenableType()) && Marshalling(parcel, skData);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkFlattenable>& val)
{
    auto type = static_cast<SkFlattenable::Type>(parcel.ReadUint32());
    sk_sp<SkData> data;
    Unmarshalling(parcel, data);
    SkReadBuffer reader(data->data(), data->size());
    val = sk_sp<SkFlattenable>(reader.readFlattenable(type));
    return true;
}

// SkTextBlob
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkTextBlob>& val)
{
    sk_sp<SkData> data = val->serialize(SkSerialProcs());
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkTextBlob>& val)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        return false;
    }
    val = SkTextBlob::Deserialize(data->data(), data->size(), SkDeserialProcs());
    return true;
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
        return false;
    }
    SkReadBuffer reader(data->data(), data->size());
    reader.readPaint(&val, nullptr);
    return true;
}

// SkImage
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkImage>& val)
{
    if (val == nullptr) {
        return parcel.WriteInt32(0);
    }
    if (!parcel.WriteInt32(1)) {
        return false;
    }
    SkBinaryWriteBuffer writer;
    writer.writeImage(val.get());
    size_t length = writer.bytesWritten();
    sk_sp<SkData> data = SkData::MakeUninitialized(length);
    writer.writeToMemory(data->writable_data());
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkImage>& val)
{
    int32_t size = parcel.ReadInt32();
    if (size == 0) {
        val = nullptr;
        return true;
    }

    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        return false;
    }
    SkReadBuffer reader(data->data(), data->size());
    val = reader.readImage();
    return val != nullptr;
}

// SkPicture
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkPicture>& val)
{
    sk_sp<SkData> data = val->serialize();
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkPicture>& val)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        return false;
    }
    val = SkPicture::MakeFromData(data->data(), data->size());
    return true;
}

// SkVertices
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkVertices>& val)
{
    sk_sp<SkData> data = val->encode();
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkVertices>& val)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        return false;
    }
    val = SkVertices::Decode(data->data(), data->size());
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
        return false;
    }
    SkReadBuffer reader(data->data(), data->size());
    reader.readPath(&val);
    return true;
}

// SkDrawable
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkDrawable>& val)
{
    return Marshalling(parcel, sk_sp<SkFlattenable>(val));
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkDrawable>& val)
{
    sk_sp<SkFlattenable> flattenablePtr;
    if (!Unmarshalling(parcel, flattenablePtr)) {
        return false;
    }
    val = sk_reinterpret_cast<SkDrawable>(flattenablePtr);
    return true;
}

// SkImageFilter
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkImageFilter>& val)
{
    return Marshalling(parcel, sk_sp<SkFlattenable>(val));
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkImageFilter>& val)
{
    sk_sp<SkFlattenable> flattenablePtr;
    if (!Unmarshalling(parcel, flattenablePtr)) {
        return false;
    }
    val = sk_reinterpret_cast<SkImageFilter>(flattenablePtr);
    return true;
}

// RSShader
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const RSShader& val)
{
    return Marshalling(parcel, sk_sp<SkFlattenable>(val.GetSkShader()));
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, RSShader& val)
{
    sk_sp<SkFlattenable> flattenablePtr;
    if (!Unmarshalling(parcel, flattenablePtr)) {
        return false;
    }
    auto shaderPtr = sk_reinterpret_cast<SkShader>(flattenablePtr);
    val.SetSkShader(shaderPtr);
    return true;
}

// RSShader
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSShader>& val)
{
    return Marshalling(parcel, sk_sp<SkFlattenable>(val->GetSkShader()));
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSShader>& val)
{
    sk_sp<SkFlattenable> flattenablePtr;
    if (!Unmarshalling(parcel, flattenablePtr)) {
        return false;
    }
    auto shaderPtr = sk_reinterpret_cast<SkShader>(flattenablePtr);
    val->SetSkShader(shaderPtr);
    return true;
}

// RSPath
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const RSPath& val)
{
    SkBinaryWriteBuffer writer;
    writer.writePath(val.GetSkiaPath());
    SkAutoMalloc buf(writer.bytesWritten());
    writer.writeToMemory(buf.get());
    auto skData = SkData::MakeFromMalloc(buf.get(), writer.bytesWritten());
    return Marshalling(parcel, skData);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, RSPath& val)
{
    sk_sp<SkData> data;
    Unmarshalling(parcel, data);
    SkReadBuffer reader(data->data(), data->size());
    SkPath path;
    reader.readPath(&path);
    val.SetSkiaPath(path);
    return true;
}

// RSPath
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSPath>& val)
{
    SkBinaryWriteBuffer writer;
    writer.writePath(val->GetSkiaPath());
    SkAutoMalloc buf(writer.bytesWritten());
    writer.writeToMemory(buf.get());
    auto skData = SkData::MakeFromMalloc(buf.get(), writer.bytesWritten());
    return Marshalling(parcel, skData);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSPath>& val)
{
    sk_sp<SkData> data;
    Unmarshalling(parcel, data);
    SkReadBuffer reader(data->data(), data->size());
    SkPath path;
    reader.readPath(&path);
    val->SetSkiaPath(path);
    return true;
}

// RSMask
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSMask>& val)
{
    // TODO
    ROSEN_LOGE("unirender: RSMask Marshalling not define");
    return true;
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSMask>& val)
{
    // TODO
    ROSEN_LOGE("unirender: RSMask Unmarshalling not define");
    return true;
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
    if (val == nullptr) {
        return parcel.WriteInt32(0);
    }
    if (!parcel.WriteInt32(1)) {
        return false;
    }
    return val->Marshalling(parcel);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSImage>& val)
{
    int32_t size = parcel.ReadInt32();
    if (size == 0) {
        val = nullptr;
        return true;
    }

    val.reset(RSImage::Unmarshalling(parcel));
    return val != nullptr;
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
MARSHALLING_AND_UNMARSHALLING(RSRenderPathAnimation)
MARSHALLING_AND_UNMARSHALLING(RSRenderTransition)
MARSHALLING_AND_UNMARSHALLING(RSRenderTransitionEffect)
MARSHALLING_AND_UNMARSHALLING(DrawCmdList)
#undef MARSHALLING_AND_UNMARSHALLING

#define MARSHALLING_AND_UNMARSHALLING(TEMPLATE)                                                    \
    template<typename T>                                                                           \
    bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<T>>& val) \
    {                                                                                              \
        return parcel.WriteParcelable(val.get());                                                  \
    }                                                                                              \
    template<typename T>                                                                           \
    bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<T>>& val)     \
    {                                                                                              \
        val.reset(parcel.ReadParcelable<TEMPLATE<T>>());                                           \
        return val != nullptr;                                                                     \
    }

MARSHALLING_AND_UNMARSHALLING(RSRenderCurveAnimation)
MARSHALLING_AND_UNMARSHALLING(RSRenderKeyframeAnimation)
#undef MARSHALLING_AND_UNMARSHALLING

#define EXPLICIT_INSTANTIATION(TEMPLATE, TYPE)                                                                  \
    template bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<TYPE>>& val); \
    template bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<TYPE>>& val);

#define BATCH_EXPLICIT_INSTANTIATION(TEMPLATE) \
    EXPLICIT_INSTANTIATION(TEMPLATE, int)      \
    EXPLICIT_INSTANTIATION(TEMPLATE, float)    \
    EXPLICIT_INSTANTIATION(TEMPLATE, Color)    \
    EXPLICIT_INSTANTIATION(TEMPLATE, Matrix3f) \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector2f) \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f) \
    EXPLICIT_INSTANTIATION(TEMPLATE, Quaternion) \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSFilter>) \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<Color>)
    // [PLANNING]:complete the marshing and unmarshalling
// EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSFilter>)

BATCH_EXPLICIT_INSTANTIATION(RSRenderCurveAnimation)
BATCH_EXPLICIT_INSTANTIATION(RSRenderKeyframeAnimation)

#undef EXPLICIT_INSTANTIATION
#undef BATCH_EXPLICIT_INSTANTIATION

template<typename T>
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::vector<T>& val)
{
    bool success = parcel.WriteUint32(val.size());
    for (const auto& item : val) {
        success = success && Marshalling(parcel, item);
    }
    return success;
}

template<typename T>
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::vector<T>& val)
{
    uint32_t size = 0;
    if (!Unmarshalling(parcel, size)) {
        return false;
    }
    val.reserve(size);
    for (uint32_t i = 0; i < size; ++i) {
        T item;
        if (!Unmarshalling(parcel, item)) {
            return false;
        }
        val.push_back(std::move(item));
    }
    return true;
}

// explicit instantiation
template bool RSMarshallingHelper::Marshalling(
    Parcel& parcel, const std::vector<std::shared_ptr<RSRenderTransitionEffect>>& val);
template bool RSMarshallingHelper::Unmarshalling(
    Parcel& parcel, std::vector<std::shared_ptr<RSRenderTransitionEffect>>& val);

void RSMarshallingHelper::ReleaseMemory(void* data, int* fd, size_t size)
{
    if (data != nullptr) {
        ::munmap(data, size);
        data = nullptr;
    }
    if (fd != nullptr && (*fd) > 0) {
        ::close(*fd);
        *fd = -1;
    }
}

bool RSMarshallingHelper::WriteToParcel(Parcel &parcel, const void* data, size_t size)
{
    if (data == nullptr) {
        ROSEN_LOGE("RSShmemParcelable::WriteToParcel data is nullptr");
        return false;
    }
    if (data == nullptr || size > MAX_DATA_SIZE) {
        ROSEN_LOGE("RSShmemParcelable::WriteToParcel data exceed MAX_DATA_SIZE");
        return false;
    }

    if (!parcel.WriteInt32(size)) {
        return false;
    }
    if (size <= MIN_DATA_SIZE) {
        return parcel.WriteUnpadBuffer(data, size);
    }
    static pid_t pid_ = getpid();
    uint64_t id = ((uint64_t)pid_ << 32) | count++;
    std::string name = "Parcel RS" + std::to_string(id);
    int fd = AshmemCreate(name.c_str(), size);
    ROSEN_LOGE("RSShmemParcelable::WriteToParcel fd:%d", fd);
    if (fd < 0) {
        return false;
    }

    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        ROSEN_LOGE("RSShmemParcelable::WriteToParcel result:%d", result);
        return false;
    }
    void *ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        ROSEN_LOGE("RSShmemParcelable::WriteToParcel MAP_FAILED");
        return false;
    }

    if (!(static_cast<MessageParcel*>(&parcel)->WriteFileDescriptor(fd))) {
        ::munmap(ptr, size);
        ROSEN_LOGE("RSShmemParcelable::WriteToParcel WriteFileDescriptor error");
        return false;
    }
    if (memcpy_s(ptr, size, data, size) != EOK) {
        ::munmap(ptr, size);
        ROSEN_LOGE("RSShmemParcelable::WriteToParcel memcpy_s failed");
        return false;
    }
    ROSEN_LOGI("RSShmemParcelable::WriteToParcel success");
    return true;
}

const void* RSMarshallingHelper::ReadFromParcel(Parcel& parcel, size_t size)
{
    int32_t bufferSize = parcel.ReadInt32();
    if (static_cast<unsigned int>(bufferSize) != size) {
        ROSEN_LOGE("RSShmemParcelable::ReadFromParcel size mismatch");
        return nullptr;
    }

    if (static_cast<unsigned int>(bufferSize) <= MIN_DATA_SIZE) {
        return parcel.ReadUnpadBuffer(size);
    }

    int fd = static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
    if (fd < 0) {
        ROSEN_LOGE("RSShmemParcelable::ReadFromParcel fd < 0");
        return nullptr;
    }
    int ashmemSize = AshmemGetSize(fd);
    if (ashmemSize < 0 || size_t(ashmemSize) < size) {
        // do not close fd here. fd will be closed in FileDescriptor, ::close(fd)
        ROSEN_LOGE("RSShmemParcelable::ReadFromParcel ashmemSize < size");
        return nullptr;
    }

    void *ptr = ::mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        // do not close fd here. fd will be closed in FileDescriptor, ::close(fd)
        ROSEN_LOGE("RSShmemParcelable::ReadFromParcel MAP_FAILED");
        return nullptr;
    }
    uint8_t *base = static_cast<uint8_t *>(malloc(size));
    if (base == nullptr) {
        ROSEN_LOGE("RSShmemParcelable::ReadFromParcel malloc(size) failed");
        return nullptr;
    }
    if (memcpy_s(base, size, ptr, size) != 0) {
        free(base);
        base = nullptr;
        ROSEN_LOGE("RSShmemParcelable::ReadFromParcel memcpy_s failed");
        return nullptr;
    }
    ReleaseMemory(ptr, &fd, size);
    return base;
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS
