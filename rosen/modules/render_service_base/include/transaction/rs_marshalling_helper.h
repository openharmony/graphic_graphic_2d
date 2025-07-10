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

#include <map>
#include <memory>
#include <optional>
#include <parcel.h>
#include <thread>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"

#include "image/image.h"

#define RSPARCELVER_ALWAYS 0x100
#define RSPARCELVER_ADD_ANIMTOKEN 0

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
class RSExtendImageObject;
class RSExtendImageBaseObj;
class RSExtendImageNineObject;
class RSExtendImageLatticeObject;
namespace Drawing {
class DrawCmdList;
class RecordCmd;
class MaskCmdList;
class Data;
class Image;
class Bitmap;
class Typeface;
}
namespace ModifierNG {
class RSRenderModifier;
}
class RSFilter;
class RSRenderFilter;
class RSNGRenderFilterBase;
class RSNGRenderMaskBase;
class RSNGRenderShaderBase;
class RSImage;
class RSImageBase;
class RSMask;
class RSPath;
class RSLinearGradientBlurPara;
class MotionBlurParam;
class RSMagnifierParams;
class EmitterUpdater;
class ParticleNoiseField;
class ParticleNoiseFields;
template<typename T>
class RenderParticleParaType;
class AnnulusRegion;
class Shape;
class EmitterConfig;
class ParticleVelocity;
class RenderParticleColorParaType;
class ParticleRenderParams;
class RSRenderCurveAnimation;
class RSRenderParticleAnimation;
class RSRenderInterpolatingSpringAnimation;
class RSRenderKeyframeAnimation;
class RSRenderPathAnimation;
class RSRenderSpringAnimation;
class RSRenderTransition;
class RSRenderTransitionEffect;
class RSRenderModifier;
class RSRenderPropertyBase;
template<typename T>
class RSRenderProperty;
template<typename T>
class RSRenderAnimatableProperty;
class RSShader;
template<typename T>
class RectT;
template<typename T>
class RRectT;
struct PixelMapInfo;

class RSB_EXPORT RSMarshallingHelper {
public:
    static constexpr size_t UNMARSHALLING_MAX_VECTOR_SIZE = 65535;
    // default marshalling and unmarshalling method for POD types
    template<typename T>
    struct is_shared_ptr : std::false_type {};

    template<typename T>
    struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

    template<typename T>
    using is_not_pointer_or_shared_ptr = std::enable_if_t<!std::is_pointer<T>::value && !is_shared_ptr<T>::value, bool>;

    template<typename T, typename = is_not_pointer_or_shared_ptr<T>>
    static bool Marshalling(Parcel& parcel, const T& val)
    {
        return parcel.WriteUnpadBuffer(&val, sizeof(T));
    }
    template<typename T, typename = is_not_pointer_or_shared_ptr<T>>
    static bool Unmarshalling(Parcel& parcel, T& val)
    {
        if (const uint8_t* buff = parcel.ReadUnpadBuffer(sizeof(T))) {
            if (buff == nullptr) {
                return false;
            }
            val = *(reinterpret_cast<const T*>(buff));
            return true;
        }
        return false;
    }

    static bool Marshalling(Parcel& parcel)
    {
        return true;
    }
    static bool Unmarshalling(Parcel& parcel)
    {
        return true;
    }
    static bool Marshalling(Parcel& parcel, const std::string& val)
    {
        if (!parcel.WriteString(val)) {
            return false;
        }
        return true;
    }

    static bool Unmarshalling(Parcel& parcel, std::string& val)
    {
        if (!parcel.ReadString(val)) {
            return false;
        }
        return true;
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
            return false;
        }
        if (const uint8_t* buff = parcel.ReadUnpadBuffer(count * sizeof(T))) {
            if (buff == nullptr) {
                return false;
            }
            val = reinterpret_cast<const T*>(buff);
            return true;
        }
        return false;
    }

    static RSB_EXPORT bool Marshalling(Parcel& parcel, const std::shared_ptr<Drawing::DrawCmdList>& val,
        int32_t recordCmdDepth = 0);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::DrawCmdList>& val);
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const std::shared_ptr<Drawing::RecordCmd>& val,
        int32_t recordCmdDepth = 0);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::RecordCmd>& val,
        uint32_t* opItemCount = nullptr, uint32_t* recordCmdCount = nullptr, int32_t recordCmdDepth = 0);
    static RSB_EXPORT bool Marshalling(Parcel& parcel, std::shared_ptr<Drawing::Typeface>& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Typeface>& val);
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const std::shared_ptr<Drawing::Image>& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Image>& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Image>& val, void*& imagepixelAddr);
    static RSB_EXPORT bool UnmarshallingNoLazyGeneratedImage(Parcel& parcel,
    std::shared_ptr<Drawing::Image>& val, void*& imagepixelAddr);
    static RSB_EXPORT bool ReadColorSpaceFromParcel(Parcel& parcel, std::shared_ptr<Drawing::ColorSpace>& colorSpace);

    static RSB_EXPORT bool UnmarshallingPidPlusId(Parcel& parcel, uint64_t& val);
    static RSB_EXPORT bool UnmarshallingPidPlusIdNoChangeIfZero(Parcel& parcel, uint64_t& val);

    // reloaded marshalling & unmarshalling function for types
#define DECLARE_FUNCTION_OVERLOAD(TYPE)                                                                   \
    static RSB_EXPORT bool CompatibleMarshalling(Parcel& parcel, const TYPE& val, uint16_t paramVersion); \
    static RSB_EXPORT bool CompatibleUnmarshalling(Parcel& parcel, TYPE& val, TYPE defaultValue, uint16_t paramVersion);

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
#undef DECLARE_FUNCTION_OVERLOAD

    static RSB_EXPORT void CompatibleUnmarshallingObsolete(Parcel& parcel, size_t typeSize, uint16_t paramVersion);

// reloaded marshalling & unmarshalling function for types
#define DECLARE_FUNCTION_OVERLOAD(TYPE)                                  \
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
    DECLARE_FUNCTION_OVERLOAD(Drawing::Matrix)
    DECLARE_FUNCTION_OVERLOAD(Drawing::Bitmap)
    // RS types
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSShader>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSPath>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSLinearGradientBlurPara>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<MotionBlurParam>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSMagnifierParams>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<EmitterUpdater>)
    DECLARE_FUNCTION_OVERLOAD(std::vector<std::shared_ptr<EmitterUpdater>>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<ParticleNoiseField>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<ParticleNoiseFields>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSRenderFilter>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSNGRenderFilterBase>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSNGRenderMaskBase>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSNGRenderShaderBase>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSMask>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSImage>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSImageBase>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<AnnulusRegion>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<Shape>)
    DECLARE_FUNCTION_OVERLOAD(EmitterConfig)
    DECLARE_FUNCTION_OVERLOAD(ParticleVelocity)
    DECLARE_FUNCTION_OVERLOAD(RenderParticleParaType<float>)
    DECLARE_FUNCTION_OVERLOAD(RenderParticleColorParaType)
    DECLARE_FUNCTION_OVERLOAD(PixelMapInfo)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<ParticleRenderParams>)
    DECLARE_FUNCTION_OVERLOAD(std::vector<std::shared_ptr<ParticleRenderParams>>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSExtendImageObject>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSExtendImageBaseObj>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSExtendImageNineObject>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSExtendImageLatticeObject>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<Drawing::MaskCmdList>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<Media::PixelMap>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RectT<float>>)
    DECLARE_FUNCTION_OVERLOAD(RRectT<float>)
    // animation
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSRenderTransition>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSRenderTransitionEffect>)

    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSRenderModifier>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<ModifierNG::RSRenderModifier>)
#undef DECLARE_FUNCTION_OVERLOAD

    // reloaded marshalling & unmarshalling function for animation
#define DECLARE_ANIMATION_OVERLOAD(TEMPLATE)                                       \
    static bool Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE>& val); \
    static bool Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE>& val);

    DECLARE_ANIMATION_OVERLOAD(RSRenderCurveAnimation)
    DECLARE_ANIMATION_OVERLOAD(RSRenderParticleAnimation)
    DECLARE_ANIMATION_OVERLOAD(RSRenderInterpolatingSpringAnimation)
    DECLARE_ANIMATION_OVERLOAD(RSRenderKeyframeAnimation)
    DECLARE_ANIMATION_OVERLOAD(RSRenderSpringAnimation)
    DECLARE_ANIMATION_OVERLOAD(RSRenderPathAnimation)
#undef DECLARE_ANIMATION_OVERLOAD

#define DECLARE_TEMPLATE_OVERLOAD(TEMPLATE)                                                      \
    template<typename T>                                                                         \
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<T>>& val); \
    template<typename T>                                                                         \
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<T>>& val);

    DECLARE_TEMPLATE_OVERLOAD(RSRenderProperty)
    DECLARE_TEMPLATE_OVERLOAD(RSRenderAnimatableProperty)
#undef DECLARE_TEMPLATE_OVERLOAD

    // reloaded marshalling & unmarshalling function for std::map
    template<typename T, typename P>
    static bool Marshalling(Parcel& parcel, const std::map<T, P>& val)
    {
        if (!parcel.WriteUint32(val.size())) {
            return false;
        }
        for (const auto& [key, value] : val) {
            if (!Marshalling(parcel, key) || !Marshalling(parcel, value)) {
                return false;
            }
        }
        return true;
    }

    template<typename T, typename P>
    static bool Unmarshalling(Parcel& parcel, std::map<T, P>& val)
    {
        uint32_t size = 0;
        if (!Unmarshalling(parcel, size)) {
            return false;
        }
        val.clear();
        for (uint32_t i = 0; i < size; ++i) {
            T key;
            P value;
            if (!Unmarshalling(parcel, key) || !Unmarshalling(parcel, value)) {
                return false;
            }
            val.emplace(key, value);
        }
        return true;
    }

    static bool Marshalling(Parcel& parcel, const std::shared_ptr<RSRenderPropertyBase>& val);
    static bool Unmarshalling(Parcel& parcel, std::shared_ptr<RSRenderPropertyBase>& val);

    // reloaded marshalling & unmarshalling function for std::vector
    template<typename T>
    static bool Marshalling(Parcel& parcel, const std::vector<T>& val, size_t maxSize = UNMARSHALLING_MAX_VECTOR_SIZE)
    {
        if (val.size() > maxSize) {
            return false;
        }
        if (!Marshalling(parcel, val.size())) {
            return false;
        }
        for (const auto& item : val) {
            if (!Marshalling(parcel, item)) {
                return false;
            }
        }
        return true;
    }
    template<typename T>
    static bool Unmarshalling(Parcel& parcel, std::vector<T>& val, size_t maxSize = UNMARSHALLING_MAX_VECTOR_SIZE)
    {
        size_t size = 0;
        if (!Unmarshalling(parcel, size)) {
            return false;
        }
        if (size > maxSize) {
            return false;
        }
        val.clear();
        val.reserve(size);
        for (uint32_t i = 0; i < size; ++i) {
            // in-place unmarshalling
            T tmp;
            if (!Unmarshalling(parcel, tmp)) {
                return false;
            }
            val.push_back(std::move(tmp));
        }
        return true;
    }

    // reloaded marshalling & unmarshalling function for std::optional
    template<typename T>
    static bool Marshalling(Parcel& parcel, const std::optional<T>& val)
    {
        if (!val.has_value()) {
            if (!parcel.WriteBool(false)) {
                return false;
            }
            return true;
        }
        if (!parcel.WriteBool(true)) {
            return false;
        }
        return Marshalling(parcel, val.value());
    }

    template<typename T>
    static bool Unmarshalling(Parcel& parcel, std::optional<T>& val)
    {
        if (!parcel.ReadBool()) {
            val.reset();
            return true;
        }
        T value;
        if (!Unmarshalling(parcel, value)) {
            return false;
        }
        val = value;
        return true;
    }

    template<class T1, class T2>
    static bool Marshalling(Parcel& parcel, const std::pair<T1, T2>& val)
    {
        return Marshalling(parcel, val.first) && Marshalling(parcel, val.second);
    }

    template<class T1, class T2>
    static bool Unmarshalling(Parcel& parcel, std::pair<T1, T2>& val)
    {
        return Unmarshalling(parcel, val.first) && Unmarshalling(parcel, val.second);
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

    static bool Marshalling(Parcel& parcel, std::shared_ptr<Drawing::Data> val);
    static bool Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Data>& val);
    static bool UnmarshallingWithCopy(Parcel& parcel, std::shared_ptr<Drawing::Data>& val);

    static void BeginNoSharedMem(std::thread::id tid);
    static void EndNoSharedMem();
    static bool GetUseSharedMem(std::thread::id tid);
    static bool CheckReadPosition(Parcel& parcel);

    static void SetCallingPid(pid_t callingPid);

    static bool MarshallingTransactionVer(Parcel& parcel);
    static bool UnmarshallingTransactionVer(Parcel& parcel);
    static bool TransactionVersionCheck(Parcel& parcel, uint8_t supportedFlag);

    static bool SkipData(Parcel& parcel);
    static bool SkipImage(Parcel& parcel);
    static bool SkipPixelMap(Parcel& parcel);

private:
    static bool WriteToParcel(Parcel& parcel, const void* data, size_t size);
    static const void* ReadFromParcel(Parcel& parcel, size_t size, bool& isMalloc);
    static bool SkipFromParcel(Parcel& parcel, size_t size);
    static const void* ReadFromAshmem(Parcel& parcel, size_t size, bool& isMalloc);
    static bool SafeUnmarshallingDrawCmdList(Parcel& parcel, std::shared_ptr<Drawing::DrawCmdList>& val,
        uint32_t* opItemCount, uint32_t* recordCmdCount, int32_t recordCmdDepth);

    static constexpr size_t MAX_DATA_SIZE = 128 * 1024 * 1024; // 128M
    static constexpr size_t MIN_DATA_SIZE = 8 * 1024;          // 8k

#ifdef RS_PROFILER_ENABLED
    friend class RSProfiler;
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_TRANSACTION_RS_MARSHALLING_HELPER_H
