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
#include "src/core/SkAutoMalloc.h"
#include "src/core/SkPaintPriv.h"
#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"
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
static inline sk_sp<T> sk_reinterprat_cast(sk_sp<P> ptr)
{
    return sk_sp<T>(static_cast<T*>(ptr.get()));
}
} // namespace

// SkData
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkData>& val)
{
    return parcel.WriteUint32(val->size()) && parcel.WriteUnpadBuffer(val->data(), val->size());
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkData>& val)
{
    auto size = parcel.ReadUint32();
    const void* data = parcel.ReadUnpadBuffer(size);
    if (data != nullptr) {
        val = SkData::MakeWithoutCopy(data, size);
        return true;
    }
    return false;
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

// SKPath
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const SkPath& val)
{
    SkBinaryWriteBuffer writer;
    writer.writePath(val);
    SkAutoMalloc buf(writer.bytesWritten());
    writer.writeToMemory(buf.get());
    auto skData = SkData::MakeFromMalloc(buf.get(), writer.bytesWritten());
    return Marshalling(parcel, skData);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, SkPath& val)
{
    sk_sp<SkData> data;
    Unmarshalling(parcel, data);
    SkReadBuffer reader(data->data(), data->size());
    reader.readPath(&val);
    return true;
}

// RSShader
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const RSShader& val)
{
    return Marshalling(parcel, sk_reinterprat_cast<SkFlattenable>(val.GetSkShader()));
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, RSShader& val)
{
    sk_sp<SkFlattenable> flattenablePtr;
    if (!Unmarshalling(parcel, flattenablePtr)) {
        return false;
    }
    auto shaderPtr = sk_reinterprat_cast<SkShader>(flattenablePtr);
    val.SetSkShader(shaderPtr);
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

} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS
