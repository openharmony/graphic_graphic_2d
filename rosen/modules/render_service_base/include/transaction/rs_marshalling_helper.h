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

#ifdef USE_ROSEN_DRAWING
#include "image/image.h"
#include "text/hm_symbol.h"
#endif

#ifndef USE_ROSEN_DRAWING
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
class SkBitmap;
struct SkPoint;
class HMSymbol;
class HMSymbolData;
struct GroupInfo;
struct RenderGroup;
struct SymbolLayers;
struct SColor;
struct SymbolLayersGroups;
struct GroupSetting;
struct AnimationSetting;

#ifdef NEW_SKIA
struct SkSamplingOptions;
#endif
#endif

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
class DrawCmdList;
class OpItem;
#else
class RSExtendImageObject;
class RSExtendImageBaseObj;
namespace Drawing {
class DrawCmdList;
class MaskCmdList;
class Data;
class Image;
class Bitmap;
}
#endif
class RSFilter;
class RSImage;
class RSImageBase;
class RSMask;
class RSPath;
class RSLinearGradientBlurPara;
template<typename T>
class RenderParticleParaType;
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
            if (buff == nullptr) {
                return false;
            }
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

    template<typename T>
    static bool MarshallingVec(Parcel& parcel, const std::vector<T>& val)
    {
        int size = val.size();
        Marshalling(parcel, size);
        for (int i = 0; i < size; i++) {
            if (!Marshalling(parcel, val[i])) {
                return false;
            }
        }
        return true;
    }

    template<typename T>
    static bool UnmarshallingVec(Parcel& parcel, std::vector<T>& val)
    {
        int size = 0;
        Unmarshalling(parcel, size);
        if (size < 0) {
            return false;
        }
        val.clear();
        for (int i = 0; i < size; i++) {
            T tmp;
            if (!Unmarshalling(parcel, tmp)) {
                return false;
            }
            val.push_back(tmp);
        }
        return true;
    }

    template<typename T>
    static bool MarshallingVec2(Parcel& parcel, const std::vector<std::vector<T>>& val)
    {
        int size = val.size();
        Marshalling(parcel, size);
        for (int i = 0; i < size; i++) {
            if (!MarshallingVec(parcel, val[i])) {
                return false;
            }
        }
        return true;
    }

    template<typename T>
    static bool UnmarshallingVec2(Parcel& parcel, std::vector<std::vector<T>>& val)
    {
        int size = 0;
        Unmarshalling(parcel, size);
        if (size < 0) {
            return false;
        }
        val.clear();
        for (int i = 0; i < size; i++) {
            std::vector<T> tmp;
            if (!UnmarshallingVec(parcel, tmp)) {
                return false;
            }
            val.push_back(tmp);
        }
        return true;
    }

#ifndef USE_ROSEN_DRAWING
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const sk_sp<SkImage>& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, sk_sp<SkImage>& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, sk_sp<SkImage>& val, void*& imagepixelAddr);
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const GroupInfo& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, GroupInfo& val);
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const RenderGroup& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, RenderGroup& val);
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const SymbolLayers& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, SymbolLayers& val);
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const SymbolLayersGroups& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, SymbolLayersGroups& val);
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const HMSymbolData& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, HMSymbolData& val);
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const SkPoint& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, SkPoint& val);
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const SColor& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, SColor& val);
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const GroupSetting& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, GroupSetting& val);
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const AnimationSetting& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, AnimationSetting& val);
#else
    static RSB_EXPORT bool Marshalling(Parcel& parcel, const std::shared_ptr<Drawing::Image>& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Image>& val);
    static RSB_EXPORT bool Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Image>& val, void*& imagepixelAddr);
    static RSB_EXPORT bool UnmarshallingNoLazyGeneratedImage(Parcel& parcel,
    std::shared_ptr<Drawing::Image>& val, void*& imagepixelAddr);
    static RSB_EXPORT bool ReadColorSpaceFromParcel(Parcel& parcel, std::shared_ptr<Drawing::ColorSpace>& colorSpace);
#endif

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
#ifndef USE_ROSEN_DRAWING
    DECLARE_FUNCTION_OVERLOAD(SkPath)
    DECLARE_FUNCTION_OVERLOAD(SkPaint)
    DECLARE_FUNCTION_OVERLOAD(SkRect)
    DECLARE_FUNCTION_OVERLOAD(SkRegion)
    DECLARE_FUNCTION_OVERLOAD(SkBitmap)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkFlattenable>)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkTextBlob>)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkPicture>)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkDrawable>)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkImageFilter>)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkVertices>)
    static bool SkipSkData(Parcel& parcel);
    static bool SkipSkImage(Parcel& parcel);
#else
    DECLARE_FUNCTION_OVERLOAD(Drawing::Matrix)
    DECLARE_FUNCTION_OVERLOAD(Drawing::Bitmap)
    static bool SkipData(Parcel& parcel);
    static bool SkipImage(Parcel& parcel);
#endif
    // RS types
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSShader>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSPath>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSLinearGradientBlurPara>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSFilter>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSMask>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSImage>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSImageBase>)
    DECLARE_FUNCTION_OVERLOAD(EmitterConfig)
    DECLARE_FUNCTION_OVERLOAD(ParticleVelocity)
    DECLARE_FUNCTION_OVERLOAD(RenderParticleParaType<float>)
    DECLARE_FUNCTION_OVERLOAD(RenderParticleColorParaType)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<ParticleRenderParams>)
    DECLARE_FUNCTION_OVERLOAD(std::vector<std::shared_ptr<ParticleRenderParams>>)
#ifndef USE_ROSEN_DRAWING
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<DrawCmdList>)
#else
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<Drawing::DrawCmdList>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSExtendImageObject>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSExtendImageBaseObj>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<Drawing::MaskCmdList>)
#endif
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<Media::PixelMap>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RectT<float>>)
    DECLARE_FUNCTION_OVERLOAD(RRectT<float>)
    static bool SkipPixelMap(Parcel& parcel);
    // animation
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSRenderTransition>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSRenderTransitionEffect>)

    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSRenderModifier>)
#ifndef USE_ROSEN_DRAWING
    DECLARE_FUNCTION_OVERLOAD(std::unique_ptr<OpItem>)
#ifdef NEW_SKIA
    DECLARE_FUNCTION_OVERLOAD(SkSamplingOptions)
#endif
#endif
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

    // reloaded marshalling & unmarshalling function for std::optional
    template<typename T>
    static bool Marshalling(Parcel& parcel, const std::optional<T>& val)
    {
        if (!val.has_value()) {
            parcel.WriteBool(false);
            return true;
        }
        parcel.WriteBool(true);
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

#ifndef USE_ROSEN_DRAWING
    static bool Marshalling(Parcel& parcel, sk_sp<SkData> val);
    static bool Unmarshalling(Parcel& parcel, sk_sp<SkData>& val);
    static bool UnmarshallingWithCopy(Parcel& parcel, sk_sp<SkData>& val);
#else
    static bool Marshalling(Parcel& parcel, std::shared_ptr<Drawing::Data> val);
    static bool Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Data>& val);
    static bool UnmarshallingWithCopy(Parcel& parcel, std::shared_ptr<Drawing::Data>& val);
#endif

    static void BeginNoSharedMem(std::thread::id tid);
    static void EndNoSharedMem();
    static bool GetUseSharedMem(std::thread::id tid);
private:
    static bool WriteToParcel(Parcel& parcel, const void* data, size_t size);
    static const void* ReadFromParcel(Parcel& parcel, size_t size, bool& isMalloc);
    static bool SkipFromParcel(Parcel& parcel, size_t size);
#ifndef USE_ROSEN_DRAWING
    static sk_sp<SkData> SerializeTypeface(SkTypeface* tf, void* ctx);
    static sk_sp<SkTypeface> DeserializeTypeface(const void* data, size_t length, void* ctx);
#endif

    static constexpr size_t MAX_DATA_SIZE = 128 * 1024 * 1024; // 128M
    static constexpr size_t MIN_DATA_SIZE = 8 * 1024;          // 8k
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_TRANSACTION_RS_MARSHALLING_HELPER_H
