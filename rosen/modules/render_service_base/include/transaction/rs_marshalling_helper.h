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

#ifndef RENDER_SERVICE_BASE_TRANSACTION_RS_MARSHALLING_HELPER_H
#define RENDER_SERVICE_BASE_TRANSACTION_RS_MARSHALLING_HELPER_H

#include <memory>
#include "common/rs_macros.h"

#include <parcel.h>

#include "common/rs_common_def.h"

template<typename T>
class sk_sp;
class SkData;
class SkDrawable;
class SkFlattenable;
class SkImage;
class SkImageFilter;
class SkPaint;
class SkPath;
class SkPicture;
struct SkRect;
class SkRegion;
class SkTextBlob;
class SkVertices;
class SkTypeface;

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
class DrawCmdList;
class RSFilter;
class RSImage;
class RSMask;
class RSPath;
class RSShader;
class RSRenderCurveAnimation;
class RSRenderKeyframeAnimation;
class RSRenderSpringAnimation;
class RSRenderPathAnimation;
class RSRenderTransition;
class RSRenderTransitionEffect;
class RSRenderModifier;
template<typename T>
class RSRenderProperty;
template<typename T>
class RSRenderAnimatableProperty;

class RSB_EXPORT RSMarshallingHelper {
public:
    // default marshalling and unmarshalling method for POD types
    // [PLANNING]: implement marshalling & unmarshalling methods for other types (e.g. RSImage, drawCMDList)
    template<typename T>
    static bool Marshalling(Parcel& parcel, const T& val)
    {
        return parcel.WriteUnpadBuffer(&val, sizeof(T));
    }
    template<typename T>
    static bool Unmarshalling(Parcel& parcel, T& val)
    {
        if (const uint8_t* buff = parcel.ReadUnpadBuffer(sizeof(T))) {
            val = *(reinterpret_cast<const T*>(buff));
            return true;
        }
        return false;
    }

    template<typename T>
    static bool MarshallingArray(Parcel& parcel, const T* val, int count)
    {
        if (count <= 0) {
            return true;
        }
        return parcel.WriteUnpadBuffer(val, count * sizeof(T));
    }
    template<typename T>
    static bool UnmarshallingArray(Parcel& parcel, T*& val, int count)
    {
        if (count <= 0) {
            return true;
        }
        if (const uint8_t* buff = parcel.ReadUnpadBuffer(count * sizeof(T))) {
            val = reinterpret_cast<const T*>(buff);
            return true;
        }
        return false;
    }

    // reloaded marshalling & unmarshalling function for types
#define DECLARE_FUNCTION_OVERLOAD(TYPE)                       \
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const TYPE& val); \
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, TYPE& val);

    // basic types
    DECLARE_FUNCTION_OVERLOAD(bool)
    DECLARE_FUNCTION_OVERLOAD(int8_t)
    DECLARE_FUNCTION_OVERLOAD(uint8_t)
    DECLARE_FUNCTION_OVERLOAD(int16_t)
    DECLARE_FUNCTION_OVERLOAD(uint16_t)
    DECLARE_FUNCTION_OVERLOAD(int32_t)
    DECLARE_FUNCTION_OVERLOAD(uint32_t)
    DECLARE_FUNCTION_OVERLOAD(int64_t)
    DECLARE_FUNCTION_OVERLOAD(uint64_t)
    DECLARE_FUNCTION_OVERLOAD(float)
    DECLARE_FUNCTION_OVERLOAD(double)
    // skia types
    DECLARE_FUNCTION_OVERLOAD(SkPath)
    DECLARE_FUNCTION_OVERLOAD(SkPaint)
    DECLARE_FUNCTION_OVERLOAD(SkRect)
    DECLARE_FUNCTION_OVERLOAD(SkRegion)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkFlattenable>)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkTextBlob>)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkPicture>)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkDrawable>)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkImageFilter>)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkImage>)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkVertices>)
    static bool SkipSkData(Parcel& parcel);
    static bool SkipSkImage(Parcel& parcel);
    // RS types
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSShader>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSPath>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSFilter>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSMask>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSImage>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<DrawCmdList>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<Media::PixelMap>)
    // animation
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSRenderTransition>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSRenderTransitionEffect>)

    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSRenderModifier>)
#undef DECLARE_FUNCTION_OVERLOAD

    // reloaded marshalling & unmarshalling function for animation
#define DECLARE_ANIMATION_OVERLOAD(TEMPLATE)                                       \
    static bool Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE>& val); \
    static bool Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE>& val);

    DECLARE_ANIMATION_OVERLOAD(RSRenderCurveAnimation)
    DECLARE_ANIMATION_OVERLOAD(RSRenderKeyframeAnimation)
    DECLARE_ANIMATION_OVERLOAD(RSRenderSpringAnimation)
    DECLARE_ANIMATION_OVERLOAD(RSRenderPathAnimation)
#undef DECLARE_ANIMATION_OVERLOAD

#define DECLARE_TEMPLATE_OVERLOAD(TEMPLATE)                                           \
    template<typename T>                                                              \
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<T>>& val); \
    template<typename T>                                                              \
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<T>>& val);

    DECLARE_TEMPLATE_OVERLOAD(RSRenderProperty)
    DECLARE_TEMPLATE_OVERLOAD(RSRenderAnimatableProperty)
#undef DECLARE_TEMPLATE_OVERLOAD

    // reloaded marshalling & unmarshalling function for std::vector
    template<typename T>
    static bool Marshalling(Parcel& parcel, const std::vector<T>& val)
    {
        bool success = parcel.WriteUint32(val.size());
        for (const auto& item : val) {
            success = success && Marshalling(parcel, item);
        }
        return success;
    }
    template<typename T>
    static bool Unmarshalling(Parcel& parcel, std::vector<T>& val)
    {
        uint32_t size = 0;
        if (!Unmarshalling(parcel, size)) {
            return false;
        }
        val.resize(size);
        for (uint32_t i = 0; i < size; ++i) {
            // in-place unmarshalling
            if (!Unmarshalling(parcel, val[i])) {
                return false;
            }
        }
        return true;
    }

    template<typename T, typename... Args>
    static bool Marshalling(Parcel& parcel, const T& first, const Args&... args)
    {
        return Marshalling(parcel, first) && Marshalling(parcel, args...);
    }
    template<typename T, typename... Args>
    static bool Unmarshalling(Parcel& parcel, T& first, Args&... args)
    {
        return Unmarshalling(parcel, first) && Unmarshalling(parcel, args...);
    }

    static bool Marshalling(Parcel& parcel, sk_sp<SkData> val);
    static bool Unmarshalling(Parcel& parcel, sk_sp<SkData>& val);
    static bool UnmarshallingWithCopy(Parcel& parcel, sk_sp<SkData>& val);

private:
    static bool WriteToParcel(Parcel& parcel, const void* data, size_t size);
    static const void* ReadFromParcel(Parcel& parcel, size_t size);
    static bool SkipFromParcel(Parcel& parcel, size_t size);
    static sk_sp<SkData> SerializeTypeface(SkTypeface* tf, void* ctx);
    static sk_sp<SkTypeface> DeserializeTypeface(const void* data, size_t length, void* ctx);

    static constexpr size_t MAX_DATA_SIZE = 128 * 1024 * 1024; // 128M
    static constexpr size_t MIN_DATA_SIZE = 8 * 1024;          // 8k
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_TRANSACTION_RS_MARSHALLING_HELPER_H
