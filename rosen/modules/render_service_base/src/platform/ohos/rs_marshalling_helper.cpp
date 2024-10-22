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
#include "image_type.h"
#include "rs_profiler.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <message_parcel.h>
#include <sys/mman.h>
#include <unistd.h>

#include "memory/rs_memory_track.h"
#include "pixel_map.h"

#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_interpolating_spring_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_particle.h"
#include "animation/rs_particle_noise_field.h"
#include "animation/rs_render_particle_animation.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "animation/rs_render_transition.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "modifier/rs_render_modifier.h"
#include "pipeline/rs_draw_cmd.h"
#include "platform/common/rs_log.h"
#include "recording/record_cmd.h"
#include "render/rs_blur_filter.h"
#include "render/rs_filter.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_image.h"
#include "render/rs_image_base.h"
#include "render/rs_light_up_effect_filter.h"
#include "render/rs_magnifier_para.h"
#include "render/rs_mask.h"
#include "render/rs_material_filter.h"
#include "render/rs_motion_blur_filter.h"
#include "render/rs_path.h"
#include "render/rs_pixel_map_shader.h"
#include "render/rs_shader.h"
#include "transaction/rs_ashmem_helper.h"
#include "rs_trace.h"

#ifdef ROSEN_OHOS
#include "buffer_utils.h"
#endif
#include "recording/mask_cmd_list.h"
#include "property/rs_properties_def.h"

#ifdef RS_ENABLE_RECORDING
#include "benchmarks/rs_recording_thread.h"
#endif

namespace OHOS {
namespace Rosen {

namespace {
    bool g_useSharedMem = true;
    std::thread::id g_tid = std::thread::id();
    constexpr size_t PIXELMAP_UNMARSHALLING_DEBUG_OFFSET = 12;
}

 
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
static bool MarshallingRecordCmdFromDrawCmdList(Parcel& parcel, const std::shared_ptr<Drawing::DrawCmdList>& val)
{
    if (!val) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::MarshallingRecordCmdFromDrawCmdList failed with null CmdList");
        return false;
    }
    std::vector<std::shared_ptr<Drawing::RecordCmd>> recordCmdVec;
    uint32_t recordCmdSize = val->GetAllRecordCmd(recordCmdVec);
    if (!parcel.WriteUint32(recordCmdSize)) {
        return false;
    }
    if (recordCmdSize == 0) {
        return true;
    }
    if (recordCmdSize > USHRT_MAX) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::MarshallingRecordCmdFromDrawCmdList failed with max limit");
        return false;
    }
    for (const auto& recordCmd : recordCmdVec) {
        if (!RSMarshallingHelper::Marshalling(parcel, recordCmd)) {
            return false;
        }
    }
    return true;
}

static bool UnmarshallingRecordCmdToDrawCmdList(Parcel& parcel, std::shared_ptr<Drawing::DrawCmdList>& val)
{
    if (!val) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::UnmarshallingRecordCmdToDrawCmdList failed with null CmdList");
        return false;
    }
    uint32_t recordCmdSize = parcel.ReadUint32();
    if (recordCmdSize == 0) {
        return true;
    }
    if (recordCmdSize > USHRT_MAX) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::UnmarshallingRecordCmdToDrawCmdList failed with max limit");
        return false;
    }
    std::vector<std::shared_ptr<Drawing::RecordCmd>> recordCmdVec;
    for (uint32_t i = 0; i < recordCmdSize; i++) {
        std::shared_ptr<Drawing::RecordCmd> recordCmd = nullptr;
        if (!RSMarshallingHelper::Unmarshalling(parcel, recordCmd)) {
            return false;
        }
        recordCmdVec.emplace_back(recordCmd);
    }
    return val->SetupRecordCmd(recordCmdVec);
}

bool MarshallingExtendObjectFromDrawCmdList(Parcel& parcel, const std::shared_ptr<Drawing::DrawCmdList>& val)
{
    if (!val) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::MarshallingExtendObjectFromDrawCmdList failed with null CmdList");
        return false;
    }
    std::vector<std::shared_ptr<Drawing::ExtendObject>> objectVec;
    uint32_t objectSize = val->GetAllExtendObject(objectVec);
    if (!parcel.WriteUint32(objectSize)) {
        return false;
    }
    if (objectSize == 0) {
        return true;
    }
    if (objectSize > USHRT_MAX) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::MarshallingExtendObjectFromDrawCmdList failed with max limit");
        return false;
    }
    for (const auto& object : objectVec) {
        if (!object->Marshalling(parcel)) {
            return false;
        }
    }
    return true;
}

bool UnmarshallingExtendObjectToDrawCmdList(Parcel& parcel, std::shared_ptr<Drawing::DrawCmdList>& val)
{
    if (!val) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::UnmarshallingExtendObjectToDrawCmdList failed with null CmdList");
        return false;
    }
    uint32_t objectSize = parcel.ReadUint32();
    if (objectSize == 0) {
        return true;
    }
    if (objectSize > USHRT_MAX) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::UnmarshallingExtendObjectToDrawCmdList failed with max limit");
        return false;
    }
    std::vector<std::shared_ptr<Drawing::ExtendObject>> objectVec;
    for (uint32_t i = 0; i < objectSize; i++) {
        std::shared_ptr<RSPixelMapShader> object = std::make_shared<RSPixelMapShader>();
        if (!object->Unmarshalling(parcel)) {
            return false;
        }
        objectVec.emplace_back(object);
    }
    return val->SetupExtendObject(objectVec);
}
} // namespace


// Drawing::Data
bool RSMarshallingHelper::Marshalling(Parcel& parcel, std::shared_ptr<Drawing::Data> val)
{
    if (!val) {
        return parcel.WriteUint32(UINT32_MAX);
    }

    uint32_t size = val->GetSize();
    if (size == UINT32_MAX) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::Marshalling Data failed with max limit");
        return false;
    }

    if (size == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Marshalling Data size is 0");
        return parcel.WriteUint32(0);
    }

    const void* data = val->GetData();
    if (!data) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::Marshalling Data failed with max nullptr");
        return false;
    }

    if (!parcel.WriteUint32(size) || !RSMarshallingHelper::WriteToParcel(parcel, data, size)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Data");
        return false;
    }
    return true;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Data>& val)
{
    uint32_t size = parcel.ReadUint32();
    if (size == UINT32_MAX) {
        val = nullptr;
        return true;
    }
    val = std::make_shared<Drawing::Data>();
    if (size == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Unmarshalling Data size is 0");
        val->BuildUninitialized(0);
        return true;
    }

    bool isMalloc = false;
    const void* data = RSMarshallingHelper::ReadFromParcel(parcel, size, isMalloc);
    if (data == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Data failed with read data");
        return false;
    }

    bool ret = false;
    if (!isMalloc) {
        ret = val->BuildWithoutCopy(data, size);
    } else {
        ret = val->BuildFromMalloc(data, size);
    }
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Data failed with Build Data");
    }
    return ret;
}

bool RSMarshallingHelper::SkipData(Parcel& parcel)
{
    int32_t size = parcel.ReadInt32();
    if (size <= 0) {
        return true;
    }
    return SkipFromParcel(parcel, size);
}

bool RSMarshallingHelper::UnmarshallingWithCopy(Parcel& parcel, std::shared_ptr<Drawing::Data>& val)
{
    bool success = Unmarshalling(parcel, val);
    if (success && val && val->GetSize() < MIN_DATA_SIZE) {
        val->BuildWithCopy(val->GetData(), val->GetSize());
    }
    return success;
}


bool RSMarshallingHelper::Marshalling(Parcel& parcel, const Drawing::Bitmap& val)
{
    Drawing::BitmapFormat bitmapFormat = val.GetFormat();
    Marshalling(parcel, bitmapFormat);

    std::shared_ptr<Drawing::Data> data = val.Serialize();
    if (!data) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling Bitmap is nullptr");
        return false;
    }
    Marshalling(parcel, data);
    return true;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, Drawing::Bitmap& val)
{
    Drawing::BitmapFormat bitmapFormat;
    if (!Unmarshalling(parcel, bitmapFormat)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling read BitmapFormat in Bitmap failed");
        return false;
    }

    val.SetFormat(bitmapFormat);

    std::shared_ptr<Drawing::Data> data;
    if (!Unmarshalling(parcel, data) || !data) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling Drawing::Bitmap");
        return false;
    }
    
    if (!val.Deserialize(data)) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling Drawing::Bitmap Deserialize");
        return false;
    }

    return true;
}

static void SkFreeReleaseproc(const void* ptr, void*)
{
    MemoryTrack::Instance().RemovePictureRecord(ptr);
    free(const_cast<void*>(ptr));
    ptr = nullptr;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, std::shared_ptr<Drawing::Typeface>& typeface)
{
    if (!typeface) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling Typeface is nullptr");
        return false;
    }
    std::shared_ptr<Drawing::Data> data = typeface->Serialize();
    if (!data) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling Typeface serialize failed");
        return false;
    }
    Marshalling(parcel, data);
    return true;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Typeface>& typeface)
{
    std::shared_ptr<Drawing::Data> data;
    if (!Unmarshalling(parcel, data) || !data) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling Drawing::Typeface data");
        return false;
    }
    typeface = Drawing::Typeface::Deserialize(data->GetData(), data->GetSize());
    if (typeface == nullptr) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling Drawing::Typeface Deserialize");
        return false;
    }
    return true;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<Drawing::Image>& val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }
    int32_t type = val->IsLazyGenerated();
    parcel.WriteInt32(type);
    if (type == 1) {
        auto data = val->Serialize();
        return Marshalling(parcel, data);
    } else {
        Drawing::Bitmap bitmap;
        if (!val->GetROPixels(bitmap)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling get bitmap failed");
            return false;
        }
        Drawing::Pixmap pixmap;
        if (!bitmap.PeekPixels(pixmap)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling peek pixels failed");
            return false;
        }
        size_t rb = pixmap.GetRowBytes();
        int width = pixmap.GetWidth();
        int height = pixmap.GetHeight();
        const void* addr = pixmap.GetAddr();
        size_t size = bitmap.ComputeByteSize();

        parcel.WriteUint32(size);
        if (!WriteToParcel(parcel, addr, size)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling Image write parcel failed");
            return false;
        }

        parcel.WriteUint32(rb);
        parcel.WriteInt32(width);
        parcel.WriteInt32(height);
        parcel.WriteUint32(pixmap.GetColorType());
        parcel.WriteUint32(pixmap.GetAlphaType());

        if (pixmap.GetColorSpace() == nullptr) {
            parcel.WriteUint32(0);
            return true;
        } else {
            auto data = pixmap.GetColorSpace()->Serialize();
            if (data == nullptr || data->GetSize() == 0) {
                parcel.WriteUint32(0);
                return true;
            }
            parcel.WriteUint32(data->GetSize());
            if (!WriteToParcel(parcel, data->GetData(), data->GetSize())) {
                ROSEN_LOGE("RSMarshallingHelper::Marshalling data write parcel failed");
                return false;
            }
        }
        return true;
    }
}

bool RSMarshallingHelper::ReadColorSpaceFromParcel(Parcel& parcel, std::shared_ptr<Drawing::ColorSpace>& colorSpace)
{
    size_t size = parcel.ReadUint32();
    if (size == 0) {
        colorSpace = nullptr;
    } else {
        bool isMal = false;
        auto data = std::make_shared<Drawing::Data>();
        const void* dataPtr = RSMarshallingHelper::ReadFromParcel(parcel, size, isMal);
        if (dataPtr == nullptr) {
            ROSEN_LOGE("dataPtr is nullptr");
            return false;
        }
        if (data->BuildWithoutCopy(dataPtr, size) == false) {
            ROSEN_LOGE("data build without copy failed");
            return false;
        }
        if (colorSpace->Deserialize(data) == false) {
            ROSEN_LOGE("colorSpace deserialize failed");
            return false;
        }
        if (isMal) {
            free(const_cast<void*>(dataPtr));
            dataPtr = nullptr;
        }
    }
    return true;
}

bool RSMarshallingHelper::UnmarshallingNoLazyGeneratedImage(Parcel& parcel,
    std::shared_ptr<Drawing::Image>& val, void*& imagepixelAddr)
{
    size_t pixmapSize = parcel.ReadUint32();
    bool isMalloc = false;
    const void* addr = RSMarshallingHelper::ReadFromParcel(parcel, pixmapSize, isMalloc);
    if (addr == nullptr) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling Drawing::Image read from parcel");
        return false;
    }

    size_t rb = parcel.ReadUint32();
    int width = parcel.ReadInt32();
    int height = parcel.ReadInt32();

    Drawing::ColorType colorType = static_cast<Drawing::ColorType>(parcel.ReadUint32());
    Drawing::AlphaType alphaType = static_cast<Drawing::AlphaType>(parcel.ReadUint32());
    auto colorSpace = std::make_shared<Drawing::ColorSpace>(Drawing::ColorSpace::ColorSpaceType::NO_TYPE);

    if (!ReadColorSpaceFromParcel(parcel, colorSpace)) {
        return false;
    }

    // if pixelmapSize >= MIN_DATA_SIZE(copyFromAshMem). record this memory size
    // use this proc to follow release step
    Drawing::ImageInfo imageInfo = Drawing::ImageInfo(width, height, colorType, alphaType, colorSpace);
    auto skData = std::make_shared<Drawing::Data>();
    if (pixmapSize < MIN_DATA_SIZE || (!g_useSharedMem && g_tid == std::this_thread::get_id())) {
        skData->BuildWithCopy(addr, pixmapSize);
    } else {
        skData->BuildWithProc(addr, pixmapSize, SkFreeReleaseproc, nullptr);
    }
    val = Drawing::Image::MakeRasterData(imageInfo, skData, rb);
    // add to MemoryTrack for memoryManager
    if (isMalloc) {
        MemoryInfo info = {pixmapSize, 0, 0, MEMORY_TYPE::MEM_SKIMAGE};
        MemoryTrack::Instance().AddPictureRecord(addr, info);
        imagepixelAddr = const_cast<void*>(addr);
    }
    return val != nullptr;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Image>& val)
{
    void* addr = nullptr;
    return Unmarshalling(parcel, val, addr);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Image>& val, void*& imagepixelAddr)
{
    (void)imagepixelAddr;
    int32_t type = parcel.ReadInt32();
    if (type == -1) {
        val = nullptr;
        return true;
    }
    if (type == 1) {
        std::shared_ptr<Drawing::Data> data;
        if (!Unmarshalling(parcel, data) || !data) {
            ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling Drawing::Image");
            return false;
        }
        val = std::make_shared<Drawing::Image>();
        if (!val->Deserialize(data)) {
            ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling Drawing::Image deserialize");
            val = nullptr;
            return false;
        }
        return true;
    }
    return UnmarshallingNoLazyGeneratedImage(parcel, val, imagepixelAddr);
}

bool RSMarshallingHelper::SkipImage(Parcel& parcel)
{
    int32_t type = parcel.ReadInt32();
    if (type == -1) {
        return true;
    }
    std::shared_ptr<Drawing::Data> data;
    if (type == 1) {
        ROSEN_LOGD("RSMarshallingHelper::SkipImage lazy");
        return SkipData(parcel);
    } else {
        size_t pixmapSize = parcel.ReadUint32();
        if (!SkipFromParcel(parcel, pixmapSize)) {
            ROSEN_LOGE("failed RSMarshallingHelper::SkipImage Data addr");
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


// RSShader
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSShader>& val)
{
    if (!val || !val->GetDrawingShader()) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSShader is nullptr");
        return parcel.WriteInt32(-1);
    }
    auto& shaderEffect = val->GetDrawingShader();
    int32_t type = static_cast<int32_t>(shaderEffect->GetType());
    std::shared_ptr<Drawing::Data> data = shaderEffect->Serialize();
    if (!data) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::Marshalling RSShader, data is nullptr");
        return parcel.WriteInt32(-1);
    }
    return parcel.WriteInt32(type) && Marshalling(parcel, data);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSShader>& val)
{
    int32_t type = parcel.ReadInt32();
    if (type == -1) {
        val = nullptr;
        return true;
    }
    std::shared_ptr<Drawing::Data> data;
    if (!Unmarshalling(parcel, data) || !data) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::Unmarshalling RSShader, data is nullptr");
        return false;
    }
    auto shaderEffect = std::make_shared<Drawing::ShaderEffect>
        (static_cast<Drawing::ShaderEffect::ShaderEffectType>(type));
    if (!shaderEffect->Deserialize(data)) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::Unmarshalling RSShader, Deserialize failed");
        return false;
    }
    val = RSShader::CreateRSShader(shaderEffect);
    return val != nullptr;
}

// Drawing::Matrix
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const Drawing::Matrix& val)
{
    Drawing::Matrix::Buffer buffer;
    val.GetAll(buffer);
    uint32_t size = buffer.size() * sizeof(Drawing::scalar);
    bool ret = parcel.WriteUint32(size);
    ret &= RSMarshallingHelper::WriteToParcel(parcel, buffer.data(), size);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::Matrix");
    }
    return ret;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, Drawing::Matrix& val)
{
    uint32_t size = parcel.ReadUint32();
    if (size < sizeof(Drawing::scalar) * Drawing::Matrix::MATRIX_SIZE) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling Drawing::Matrix failed size %{public}d", size);
        return false;
    }
    bool isMalloc = false;
    auto data = static_cast<const Drawing::scalar*>(RSMarshallingHelper::ReadFromParcel(parcel, size, isMalloc));
    if (data == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Drawing::Matrix");
        return false;
    }

    val.SetMatrix(data[Drawing::Matrix::SCALE_X], data[Drawing::Matrix::SKEW_X], data[Drawing::Matrix::TRANS_X],
        data[Drawing::Matrix::SKEW_Y], data[Drawing::Matrix::SCALE_Y], data[Drawing::Matrix::TRANS_Y],
        data[Drawing::Matrix::PERSP_0], data[Drawing::Matrix::PERSP_1], data[Drawing::Matrix::PERSP_2]);
    if (isMalloc) {
        free(static_cast<void*>(const_cast<Drawing::scalar*>(data)));
        data = nullptr;
    }
    return true;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSLinearGradientBlurPara>& val)
{
    if (!val) {
        ROSEN_LOGD("RSMarshallingHelper::Marshalling RSLinearGradientBlurPara is nullptr");
        return parcel.WriteInt32(-1);
    }
    bool success = parcel.WriteInt32(1) && Marshalling(parcel, val->blurRadius_);
    success = success && parcel.WriteUint32(static_cast<uint32_t>(val->fractionStops_.size()));
    for (size_t i = 0; i < val->fractionStops_.size(); i++) {
        success = success && Marshalling(parcel, val->fractionStops_[i].first);
        success = success && Marshalling(parcel, val->fractionStops_[i].second);
    }
    success = success && Marshalling(parcel, val->direction_);
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSLinearGradientBlurPara>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    float blurRadius;
    std::vector<std::pair<float, float>> fractionStops;
    GradientDirection direction = GradientDirection::NONE;
    bool success = Unmarshalling(parcel, blurRadius);
    uint32_t fractionStopsSize = parcel.ReadUint32();
    if (fractionStopsSize > SIZE_UPPER_LIMIT) {
        return false;
    }
    for (size_t i = 0; i < fractionStopsSize; i++) {
        std::pair<float, float> fractionStop;
        float first = 0.0;
        float second = 0.0;
        success = success && Unmarshalling(parcel, first);
        if (!success) {
            return false;
        }
        fractionStop.first = first;
        success = success && Unmarshalling(parcel, second);
        if (!success) {
            return false;
        }
        fractionStop.second = second;
        fractionStops.push_back(fractionStop);
    }
    success = success && Unmarshalling(parcel, direction);
    if (success) {
        val = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    }
    return success;
}

// MotionBlurPara
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<MotionBlurParam>& val)
{
    bool success = Marshalling(parcel, val->radius);
    success = success && Marshalling(parcel, val->scaleAnchor[0]);
    success = success && Marshalling(parcel, val->scaleAnchor[1]);
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<MotionBlurParam>& val)
{
    float radius;
    float anchorX = 0.f;
    float anchorY = 0.f;

    bool success = Unmarshalling(parcel, radius);
    success = success && Unmarshalling(parcel, anchorX);
    success = success && Unmarshalling(parcel, anchorY);
    Vector2f anchor(anchorX, anchorY);

    if (success) {
        val = std::make_shared<MotionBlurParam>(radius, anchor);
    }
    return success;
}

// MagnifierPara
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSMagnifierParams>& val)
{
    bool success = Marshalling(parcel, val->factor_);
    success = success && Marshalling(parcel, val->width_);
    success = success && Marshalling(parcel, val->height_);
    success = success && Marshalling(parcel, val->cornerRadius_);
    success = success && Marshalling(parcel, val->borderWidth_);
    success = success && Marshalling(parcel, val->offsetX_);
    success = success && Marshalling(parcel, val->offsetY_);

    success = success && Marshalling(parcel, val->shadowOffsetX_);
    success = success && Marshalling(parcel, val->shadowOffsetY_);
    success = success && Marshalling(parcel, val->shadowSize_);
    success = success && Marshalling(parcel, val->shadowStrength_);
 
    success = success && Marshalling(parcel, val->gradientMaskColor1_);
    success = success && Marshalling(parcel, val->gradientMaskColor2_);
    success = success && Marshalling(parcel, val->outerContourColor1_);
    success = success && Marshalling(parcel, val->outerContourColor2_);

    return success;
}
 
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSMagnifierParams>& val)
{
    float factor = 0.f;
    float width = 0.f;
    float height = 0.f;
    float cornerRadius = 0.f;
    float borderWidth = 0.f;
    float offsetX = 0.f;
    float offsetY = 0.f;
    float shadowOffsetX = 0.f;
    float shadowOffsetY = 0.f;
    float shadowSize = 0.f;
    float shadowStrength = 0.f;
    uint32_t gradientMaskColor1 = 0x00000000;
    uint32_t gradientMaskColor2 = 0x00000000;
    uint32_t outerContourColor1 = 0x00000000;
    uint32_t outerContourColor2 = 0x00000000;

    val = std::make_shared<RSMagnifierParams>();
    if (val == nullptr) { return false; }

    bool success = Unmarshalling(parcel, factor);
    if (success) { val->factor_ = factor; }
    success = success && Unmarshalling(parcel, width);
    if (success) { val->width_ = width; }
    success = success && Unmarshalling(parcel, height);
    if (success) { val->height_ = height; }
    success = success && Unmarshalling(parcel, cornerRadius);
    if (success) { val->cornerRadius_ = cornerRadius; }
    success = success && Unmarshalling(parcel, borderWidth);
    if (success) { val->borderWidth_ = borderWidth; }
    success = success && Unmarshalling(parcel, offsetX);
    if (success) { val->offsetX_ = offsetX; }
    success = success && Unmarshalling(parcel, offsetY);
    if (success) { val->offsetY_ = offsetY; }

    success = success && Unmarshalling(parcel, shadowOffsetX);
    if (success) { val->shadowOffsetX_ = shadowOffsetX; }
    success = success && Unmarshalling(parcel, shadowOffsetY);
    if (success) { val->shadowOffsetY_ = shadowOffsetY; }
    success = success && Unmarshalling(parcel, shadowSize);
    if (success) { val->shadowSize_ = shadowSize; }
    success = success && Unmarshalling(parcel, shadowStrength);
    if (success) { val->shadowStrength_ = shadowStrength; }

    success = success && Unmarshalling(parcel, gradientMaskColor1);
    if (success) { val->gradientMaskColor1_ = gradientMaskColor1; }
    success = success && Unmarshalling(parcel, gradientMaskColor2);
    if (success) { val->gradientMaskColor2_ = gradientMaskColor2; }
    success = success && Unmarshalling(parcel, outerContourColor1);
    if (success) { val->outerContourColor1_ = outerContourColor1; }
    success = success && Unmarshalling(parcel, outerContourColor2);
    if (success) { val->outerContourColor2_ = outerContourColor2; }

    return success;
}

// Particle
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<EmitterUpdater>& val)
{
    if (!val) {
        ROSEN_LOGD("RSMarshallingHelper::Marshalling EmitterUpdater is nullptr");
        return parcel.WriteInt32(-1);
    }
    bool success = parcel.WriteInt32(1) && Marshalling(parcel, val->emitterIndex_);
    success = success && Marshalling(parcel, val->position_);
    success = success && Marshalling(parcel, val->emitSize_) ;
    success = success && Marshalling(parcel, val->emitRate_);
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<EmitterUpdater>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    int emitterIndex = 0;
    std::optional<Vector2f> position = std::nullopt;
    std::optional<Vector2f> emitSize = std::nullopt;
    std::optional<int> emitRate = std::nullopt;

    bool success = Unmarshalling(parcel, emitterIndex);
    success = success && Unmarshalling(parcel, position);
    success = success && Unmarshalling(parcel, emitSize);
    success = success && Unmarshalling(parcel, emitRate);
    if (success) {
        val = std::make_shared<EmitterUpdater>(emitterIndex, position, emitSize, emitRate);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::vector<std::shared_ptr<EmitterUpdater>>& val)
{
    bool success = parcel.WriteUint32(static_cast<uint32_t>(val.size()));
    for (size_t i = 0; i < val.size(); i++) {
        success = success && Marshalling(parcel, val[i]);
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::vector<std::shared_ptr<EmitterUpdater>>& val)
{
    uint32_t size = parcel.ReadUint32();
    bool success = true;
    std::vector<std::shared_ptr<EmitterUpdater>> emitterUpdaters;
    if (size > PARTICLE_UPPER_LIMIT) {
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        std::shared_ptr<EmitterUpdater> emitterUpdater;
        success = success && Unmarshalling(parcel, emitterUpdater);
        emitterUpdaters.push_back(emitterUpdater);
    }
    if (success) {
        val = emitterUpdaters;
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<ParticleNoiseField>& val)
{
    if (!val) {
        ROSEN_LOGD("RSMarshallingHelper::Marshalling ParticleNoiseField is nullptr");
        return parcel.WriteInt32(-1);
    }
    bool success = parcel.WriteInt32(1) && Marshalling(parcel, val->fieldStrength_);
    success = success && Marshalling(parcel, val->fieldShape_);
    success = success && Marshalling(parcel, val->fieldSize_.x_) && Marshalling(parcel, val->fieldSize_.y_);
    success = success && Marshalling(parcel, val->fieldCenter_.x_) && Marshalling(parcel, val->fieldCenter_.y_);
    success = success && Marshalling(parcel, val->fieldFeather_) &&  Marshalling(parcel, val->noiseScale_);
    success = success && Marshalling(parcel, val->noiseFrequency_) &&  Marshalling(parcel, val->noiseAmplitude_);
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<ParticleNoiseField>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    int fieldStrength = 0;
    ShapeType fieldShape = ShapeType::RECT;
    float fieldSizeX = 0.f;
    float fieldSizeY = 0.f;
    float fieldCenterX = 0.f;
    float fieldCenterY = 0.f;
    uint16_t fieldFeather = 0;
    float noiseScale = 0.f;
    float noiseFrequency = 0.f;
    float noiseAmplitude = 0.f;

    bool success = Unmarshalling(parcel, fieldStrength);
    success = success && Unmarshalling(parcel, fieldShape);
    success = success && Unmarshalling(parcel, fieldSizeX) && Unmarshalling(parcel, fieldSizeY);
    Vector2f fieldSize(fieldSizeX, fieldSizeY);
    success = success && Unmarshalling(parcel, fieldCenterX) && Unmarshalling(parcel, fieldCenterY);
    Vector2f fieldCenter(fieldCenterX, fieldCenterY);
    success = success && Unmarshalling(parcel, fieldFeather);
    success = success && Unmarshalling(parcel, noiseScale);
    success = success && Unmarshalling(parcel, noiseFrequency);
    success = success && Unmarshalling(parcel, noiseAmplitude);
    if (success) {
        val = std::make_shared<ParticleNoiseField>(fieldStrength, fieldShape, fieldSize, fieldCenter, fieldFeather,
            noiseScale, noiseFrequency, noiseAmplitude);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<ParticleNoiseFields>& val)
{
    if (!val) {
        ROSEN_LOGD("RSMarshallingHelper::Marshalling ParticleNoiseFields is nullptr");
        return parcel.WriteInt32(-1);
    }
    bool success = parcel.WriteInt32(1) && parcel.WriteUint32(static_cast<uint32_t>(val->fields_.size()));
    for (size_t i = 0; i < val->fields_.size(); i++) {
        success = success && Marshalling(parcel, val->fields_[i]);
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<ParticleNoiseFields>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    uint32_t size = parcel.ReadUint32();
    bool success = true;
    if (size > PARTICLE_UPPER_LIMIT) {
        return false;
    }
    std::shared_ptr<ParticleNoiseFields> noiseFields = std::make_shared<ParticleNoiseFields>();
    for (size_t i = 0; i < size; i++) {
        std::shared_ptr<ParticleNoiseField> ParticleNoiseField;
        success = success && Unmarshalling(parcel, ParticleNoiseField);
        noiseFields->AddField(ParticleNoiseField);
    }
    if (success) {
        val = noiseFields;
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const EmitterConfig& val)
{
    bool success = Marshalling(parcel, val.emitRate_);
    success = success && Marshalling(parcel, val.emitShape_);
    success = success && Marshalling(parcel, val.position_.x_);
    success = success && Marshalling(parcel, val.position_.y_);
    success = success && Marshalling(parcel, val.emitSize_.x_);
    success = success && Marshalling(parcel, val.emitSize_.y_);
    success = success && Marshalling(parcel, val.particleCount_);
    success = success && Marshalling(parcel, val.lifeTime_.start_);
    success = success && Marshalling(parcel, val.lifeTime_.end_);
    success = success && Marshalling(parcel, val.type_);
    success = success && Marshalling(parcel, val.radius_);
    success = success && Marshalling(parcel, val.image_);
    success = success && Marshalling(parcel, val.imageSize_.x_);
    success = success && Marshalling(parcel, val.imageSize_.y_);

    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, EmitterConfig& val)
{
    int emitRate = 0;
    ShapeType emitShape = ShapeType::RECT;
    float positionX = 0.f;
    float positionY = 0.f;
    float emitSizeWidth = 0.f;
    float emitSizeHeight = 0.f;
    int particleCount = 0;
    int64_t lifeTimeStart = 0;
    int64_t lifeTimeEnd = 0;
    ParticleType particleType = ParticleType::POINTS;
    float radius = 0.f;
    std::shared_ptr<RSImage> image = nullptr;
    float imageWidth = 0.f;
    float imageHeight = 0.f;

    bool success = Unmarshalling(parcel, emitRate);
    success = success && Unmarshalling(parcel, emitShape);
    success = success && Unmarshalling(parcel, positionX);
    success = success && Unmarshalling(parcel, positionY);
    Vector2f position(positionX, positionY);
    success = success && Unmarshalling(parcel, emitSizeWidth);
    success = success && Unmarshalling(parcel, emitSizeHeight);
    Vector2f emitSize(emitSizeWidth, emitSizeHeight);
    success = success && Unmarshalling(parcel, particleCount);
    success = success && Unmarshalling(parcel, lifeTimeStart);
    success = success && Unmarshalling(parcel, lifeTimeEnd);
    success = success && Unmarshalling(parcel, particleType);
    success = success && Unmarshalling(parcel, radius);
    Unmarshalling(parcel, image);
    success = success && Unmarshalling(parcel, imageWidth);
    success = success && Unmarshalling(parcel, imageHeight);
    Vector2f imageSize(imageWidth, imageHeight);
    if (success) {
        Range<int64_t> lifeTime(lifeTimeStart, lifeTimeEnd);
        val = EmitterConfig(
            emitRate, emitShape, position, emitSize, particleCount, lifeTime, particleType, radius, image, imageSize);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const ParticleVelocity& val)
{
    return Marshalling(parcel, val.velocityValue_.start_) && Marshalling(parcel, val.velocityValue_.end_) &&
           Marshalling(parcel, val.velocityAngle_.start_) && Marshalling(parcel, val.velocityAngle_.end_);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, ParticleVelocity& val)
{
    float valueStart = 0.f;
    float valueEnd = 0.f;
    float angleStart = 0.f;
    float angleEnd = 0.f;
    bool success = Unmarshalling(parcel, valueStart) && Unmarshalling(parcel, valueEnd) &&
        Unmarshalling(parcel, angleStart) && Unmarshalling(parcel, angleEnd);
    if (success) {
        Range<float> velocityValue(valueStart, valueEnd);
        Range<float> velocityAngle(angleStart, angleEnd);
        val = ParticleVelocity(velocityValue, velocityAngle);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const RenderParticleParaType<float>& val)
{
    bool success = Marshalling(parcel, val.val_.start_) && Marshalling(parcel, val.val_.end_) &&
                   Marshalling(parcel, val.updator_);
    if (val.updator_ == ParticleUpdator::RANDOM) {
        success = success && Marshalling(parcel, val.random_.start_) && Marshalling(parcel, val.random_.end_);
    } else if (val.updator_ == ParticleUpdator::CURVE) {
        success = success && parcel.WriteUint32(static_cast<uint32_t>(val.valChangeOverLife_.size()));
        for (size_t i = 0; i < val.valChangeOverLife_.size(); i++) {
            if (val.valChangeOverLife_[i] == nullptr || val.valChangeOverLife_[i]->interpolator_ == nullptr) {
                ROSEN_LOGE("RSMarshallingHelper::Unmarshalling RenderParticleParaType fail cause nullptr");
                return false;
            }
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->fromValue_);
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->toValue_);
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->startMillis_);
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->endMillis_);
            success = success && val.valChangeOverLife_[i]->interpolator_->Marshalling(parcel);
        }
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, RenderParticleParaType<float>& val)
{
    float valueStart = 0.f;
    float valueEnd = 0.f;
    ParticleUpdator updator = ParticleUpdator::NONE;
    float randomStart = 0.f;
    float randomEnd = 0.f;
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> valChangeOverLife;
    bool success =
        Unmarshalling(parcel, valueStart) && Unmarshalling(parcel, valueEnd) && Unmarshalling(parcel, updator);
    if (updator == ParticleUpdator::RANDOM) {
        success = success && Unmarshalling(parcel, randomStart) && Unmarshalling(parcel, randomEnd);
    } else if (updator == ParticleUpdator::CURVE) {
        uint32_t valChangeOverLifeSize = parcel.ReadUint32();
        if (valChangeOverLifeSize > SIZE_UPPER_LIMIT) {
            return false;
        }
        for (size_t i = 0; i < valChangeOverLifeSize; i++) {
            float fromValue = 0.f;
            float toValue = 0.f;
            int startMillis = 0;
            int endMillis = 0;
            success = success && Unmarshalling(parcel, fromValue);
            success = success && Unmarshalling(parcel, toValue);
            success = success && Unmarshalling(parcel, startMillis);
            success = success && Unmarshalling(parcel, endMillis);
            std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
            auto change = std::make_shared<ChangeInOverLife<float>>(
                fromValue, toValue, startMillis, endMillis, interpolator);
            valChangeOverLife.push_back(change);
        }
    }
    if (success) {
        Range<float> value(valueStart, valueEnd);
        Range<float> random(randomStart, randomEnd);
        val = RenderParticleParaType<float>(value, updator, random, std::move(valChangeOverLife));
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const RenderParticleColorParaType& val)
{
    bool success = Marshalling(parcel, val.colorVal_.start_) && Marshalling(parcel, val.colorVal_.end_) &&
                   Marshalling(parcel, val.distribution_) && Marshalling(parcel, val.updator_);
    if (val.updator_ == ParticleUpdator::RANDOM) {
        success = success && Marshalling(parcel, val.redRandom_.start_) && Marshalling(parcel, val.redRandom_.end_);
        success =
            success && Marshalling(parcel, val.greenRandom_.start_) && Marshalling(parcel, val.greenRandom_.end_);
        success =
            success && Marshalling(parcel, val.blueRandom_.start_) && Marshalling(parcel, val.blueRandom_.end_);
        success =
            success && Marshalling(parcel, val.alphaRandom_.start_) && Marshalling(parcel, val.alphaRandom_.end_);
    } else if (val.updator_ == ParticleUpdator::CURVE) {
        success = success && parcel.WriteUint32(static_cast<uint32_t>(val.valChangeOverLife_.size()));
        for (size_t i = 0; i < val.valChangeOverLife_.size(); i++) {
            if (val.valChangeOverLife_[i] == nullptr || val.valChangeOverLife_[i]->interpolator_ == nullptr) {
                ROSEN_LOGE("RSMarshallingHelper::Unmarshalling RenderParticleColorParaType fail cause nullptr");
                return false;
            }
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->fromValue_);
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->toValue_);
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->startMillis_);
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->endMillis_);
            success = success && val.valChangeOverLife_[i]->interpolator_->Marshalling(parcel);
        }
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, RenderParticleColorParaType& val)
{
    Color colorValStart = RSColor(0, 0, 0);
    Color colorValEnd = RSColor(0, 0, 0);
    DistributionType distribution = DistributionType::UNIFORM;
    ParticleUpdator updator = ParticleUpdator::NONE;
    float redRandomStart = 0.f;
    float redRandomEnd = 0.f;
    float greenRandomStart = 0.f;
    float greenRandomEnd = 0.f;
    float blueRandomStart = 0.f;
    float blueRandomEnd = 0.f;
    float alphaRandomStart = 0.f;
    float alphaRandomEnd = 0.f;
    std::vector<std::shared_ptr<ChangeInOverLife<Color>>> valChangeOverLife;
    bool success = Unmarshalling(parcel, colorValStart) && Unmarshalling(parcel, colorValEnd) &&
                   Unmarshalling(parcel, distribution) && Unmarshalling(parcel, updator);
    if (updator == ParticleUpdator::RANDOM) {
        success = success && Unmarshalling(parcel, redRandomStart) && Unmarshalling(parcel, redRandomEnd) &&
                  Unmarshalling(parcel, greenRandomStart) && Unmarshalling(parcel, greenRandomEnd) &&
                  Unmarshalling(parcel, blueRandomStart) && Unmarshalling(parcel, blueRandomEnd) &&
                  Unmarshalling(parcel, alphaRandomStart) && Unmarshalling(parcel, alphaRandomEnd);
    } else if (updator == ParticleUpdator::CURVE) {
        uint32_t valChangeOverLifeSize = parcel.ReadUint32();
        if (valChangeOverLifeSize > SIZE_UPPER_LIMIT) {
            return false;
        }
        for (size_t i = 0; i < valChangeOverLifeSize; i++) {
            Color fromValue = RSColor(0, 0, 0);
            Color toValue = RSColor(0, 0, 0);
            int startMillis = 0;
            int endMillis = 0;
            success = success && Unmarshalling(parcel, fromValue) && Unmarshalling(parcel, toValue) &&
                      Unmarshalling(parcel, startMillis) && Unmarshalling(parcel, endMillis);
            std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
            auto change = std::make_shared<ChangeInOverLife<Color>>(
                fromValue, toValue, startMillis, endMillis, std::move(interpolator));
            valChangeOverLife.push_back(change);
        }
    }
    if (success) {
        Range<Color> colorVal(colorValStart, colorValEnd);
        Range<float> redRandom(redRandomStart, redRandomEnd);
        Range<float> greenRandom(greenRandomStart, greenRandomEnd);
        Range<float> blueRandom(blueRandomStart, blueRandomEnd);
        Range<float> alphaRandom(alphaRandomStart, alphaRandomEnd);
        val = RenderParticleColorParaType(colorVal, distribution, updator, redRandom, greenRandom, blueRandom,
            alphaRandom, std::move(valChangeOverLife));
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<ParticleRenderParams>& val)
{
    if (!val) {
        ROSEN_LOGD("RSMarshallingHelper::Marshalling ParticleRenderParams is nullptr");
        return parcel.WriteInt32(-1);
    }
    bool success = parcel.WriteInt32(1) && Marshalling(parcel, val->emitterConfig_);
    success = success && Marshalling(parcel, val->velocity_);
    success = success && Marshalling(parcel, val->acceleration_.accelerationValue_);
    success = success && Marshalling(parcel, val->acceleration_.accelerationAngle_);
    success = success && Marshalling(parcel, val->color_);
    success = success && Marshalling(parcel, val->opacity_);
    success = success && Marshalling(parcel, val->scale_);
    success = success && Marshalling(parcel, val->spin_);
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<ParticleRenderParams>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    EmitterConfig emitterConfig;
    ParticleVelocity velocity;
    RenderParticleParaType<float> accelerationValue;
    RenderParticleParaType<float> accelerationAngle;
    RenderParticleColorParaType color;
    RenderParticleParaType<float> opacity;
    RenderParticleParaType<float> scale;
    RenderParticleParaType<float> spin;
    bool success = Unmarshalling(parcel, emitterConfig);
    success = success && Unmarshalling(parcel, velocity);
    success = success && Unmarshalling(parcel, accelerationValue);
    success = success && Unmarshalling(parcel, accelerationAngle);
    RenderParticleAcceleration acceleration = RenderParticleAcceleration(accelerationValue, accelerationAngle);
    success = success && Unmarshalling(parcel, color);
    success = success && Unmarshalling(parcel, opacity);
    success = success && Unmarshalling(parcel, scale);
    success = success && Unmarshalling(parcel, spin);
    if (success) {
        val =
            std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::vector<std::shared_ptr<ParticleRenderParams>>& val)
{
    bool success = parcel.WriteUint32(static_cast<uint32_t>(val.size()));
    for (size_t i = 0; i < val.size(); i++) {
        success = success && Marshalling(parcel, val[i]);
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::vector<std::shared_ptr<ParticleRenderParams>>& val)
{
    uint32_t size = parcel.ReadUint32();
    bool success = true;
    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams;
    if (size > PARTICLE_UPPER_LIMIT) {
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        std::shared_ptr<ParticleRenderParams> particleRenderParams;
        success = success && Unmarshalling(parcel, particleRenderParams);
        particlesRenderParams.push_back(particleRenderParams);
    }
    if (success) {
        val = particlesRenderParams;
    }
    return success;
}

// RSPath
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSPath>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSPath is nullptr");
        return parcel.WriteInt32(-1);
    }
    
    std::shared_ptr<Drawing::Data> data = val->GetDrawingPath().Serialize();
    if (!data) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling Path is nullptr");
        return false;
    }

    return parcel.WriteInt32(1) && Marshalling(parcel, data);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSPath>& val)
{
    int32_t size = parcel.ReadInt32();
    if (size == -1) {
        val = nullptr;
        return true;
    }

    std::shared_ptr<Drawing::Data> data;
    Drawing::Path path;
    if (!Unmarshalling(parcel, data) || !data) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Path");
        return false;
    }

    if (!path.Deserialize(data)) {
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
            success = success && parcel.WriteFloat(material->radius_) && parcel.WriteFloat(material->saturation_) &&
                      parcel.WriteFloat(material->brightness_) &&
                      RSMarshallingHelper::Marshalling(parcel, material->maskColor_) &&
                      parcel.WriteInt32(material->colorMode_);
            break;
        }
        case RSFilter::LIGHT_UP_EFFECT: {
            auto lightUp = std::static_pointer_cast<RSLightUpEffectFilter>(val);
            success = success && parcel.WriteFloat(lightUp->lightUpDegree_);
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
            MaterialParam materialParam;
            int colorMode;
            success = success && parcel.ReadFloat(materialParam.radius) && parcel.ReadFloat(materialParam.saturation) &&
                      parcel.ReadFloat(materialParam.brightness) &&
                      RSMarshallingHelper::Unmarshalling(parcel, materialParam.maskColor) &&
                      parcel.ReadInt32(colorMode);
            if (success) {
                val = std::make_shared<RSMaterialFilter>(materialParam, static_cast<BLUR_COLOR_MODE>(colorMode));
            }
            break;
        }
        case RSFilter::LIGHT_UP_EFFECT: {
            float lightUpDegree;
            success = success && parcel.ReadFloat(lightUpDegree);
            if (success) {
                val = RSFilter::CreateLightUpEffectFilter(lightUpDegree);
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

// RSImageBase
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSImageBase>& val)
{
    if (!val) {
        ROSEN_LOGD("RSMarshallingHelper::Marshalling RSImageBase is nullptr");
        return parcel.WriteInt32(-1);
    }
    return parcel.WriteInt32(1) && val->Marshalling(parcel);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSImageBase>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(RSImageBase::Unmarshalling(parcel));
    return val != nullptr;
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
    auto position = parcel.GetWritePosition();
    if (!(parcel.WriteInt32(1) && RS_PROFILER_MARSHAL_PIXELMAP(parcel, val))) {
        ROSEN_LOGE("failed RSMarshallingHelper::Marshalling Media::PixelMap");
        return false;
    }
    // correct pixelmap size recorded in Parcel
    *reinterpret_cast<int32_t*>(parcel.GetData() + position) =
        static_cast<int32_t>(parcel.GetWritePosition() - position - sizeof(int32_t));
    return true;
}

static void CustomFreePixelMap(void* addr, void* context, uint32_t size)
{
#ifdef ROSEN_OHOS
    MemoryTrack::Instance().RemovePictureRecord(context);
#else
    MemoryTrack::Instance().RemovePictureRecord(addr);
#endif
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Media::PixelMap>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    auto readPosition = parcel.GetReadPosition();
    val.reset(RS_PROFILER_UNMARSHAL_PIXELMAP(parcel));
    if (val == nullptr) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling Media::PixelMap");
        if (readPosition > PIXELMAP_UNMARSHALLING_DEBUG_OFFSET &&
            parcel.RewindRead(readPosition - PIXELMAP_UNMARSHALLING_DEBUG_OFFSET)) {
            ROSEN_LOGE("RSMarshallingHelper::Unmarshalling PixelMap before "
                       " [%{public}d, %{public}d, %{public}d] [w, h, format]: [%{public}d, %{public}d, %{public}d]",
                parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadInt32(),
                parcel.ReadInt32());
        } else {
            ROSEN_LOGE("RSMarshallingHelper::Unmarshalling RewindRead failed");
        }
        
        return false;
    }
    MemoryInfo info = {
        val->GetByteCount(), 0, 0, val->GetUniqueId(), MEMORY_TYPE::MEM_PIXELMAP, val->GetAllocatorType(), val
    };

#ifdef ROSEN_OHOS
    if (RSSystemProperties::GetRsMemoryOptimizeEnabled() &&
        (val->GetAllocatorType() == Media::AllocatorType::SHARE_MEM_ALLOC) &&
        !val->IsEditable() &&
        !val->IsAstc() &&
        !val->IsHdr()) {
        val->UnMap();
    }
    MemoryTrack::Instance().AddPictureRecord(val->GetFd(), info);
#else
    MemoryTrack::Instance().AddPictureRecord(val->GetPixels(), info);
#endif
    val->SetFreePixelMapProc(CustomFreePixelMap);
    return true;
}

bool RSMarshallingHelper::SkipPixelMap(Parcel& parcel)
{
    if (RS_PROFILER_SKIP_PIXELMAP(parcel)) {
        return true;
    }
    auto size = parcel.ReadInt32();
    if (size != -1) {
        parcel.SkipBytes(size);
    }
    return true;
}

// RectF
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RectT<float>>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RectF is nullptr");
        return parcel.WriteInt32(-1);
    }
    return parcel.WriteInt32(1) && val->Marshalling(parcel);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RectT<float>>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(RectT<float>::Unmarshalling(parcel));
    return val != nullptr;
}

// RRect
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const RRectT<float>& val)
{
    return Marshalling(parcel, val.rect_) && Marshalling(parcel, val.radius_[0]) &&
           Marshalling(parcel, val.radius_[1]) && Marshalling(parcel, val.radius_[2]) &&
           Marshalling(parcel, val.radius_[3]);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, RRectT<float>& val)
{
    return Unmarshalling(parcel, val.rect_) && Unmarshalling(parcel, val.radius_[0]) &&
           Unmarshalling(parcel, val.radius_[1]) && Unmarshalling(parcel, val.radius_[2]) &&
           Unmarshalling(parcel, val.radius_[3]);
}


// Drawing::DrawCmdList
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<Drawing::DrawCmdList>& val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }
    auto cmdListData = val->GetData();
    bool ret = parcel.WriteInt32(cmdListData.second);
    parcel.WriteInt32(val->GetWidth());
    parcel.WriteInt32(val->GetHeight());

    parcel.WriteBool(val->GetIsCache());
    parcel.WriteBool(val->GetCachedHighContrast());
    auto replacedOpList = val->GetReplacedOpList();
    parcel.WriteUint32(replacedOpList.size());
    for (size_t i = 0; i < replacedOpList.size(); ++i) {
        parcel.WriteUint32(replacedOpList[i].first);
        parcel.WriteUint32(replacedOpList[i].second);
    }

    if (cmdListData.second == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Marshalling Drawing::DrawCmdList, size is 0");
        return ret;
    }
    ret &= RSMarshallingHelper::WriteToParcel(parcel, cmdListData.first, cmdListData.second);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList");
        return ret;
    }

    auto imageData = val->GetAllImageData();
    ret &= parcel.WriteInt32(imageData.second);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList image size");
        return ret;
    }
    if (imageData.second > 0) {
        ret &= RSMarshallingHelper::WriteToParcel(parcel, imageData.first, imageData.second);
        if (!ret) {
            ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList image");
            return ret;
        }
    }

    auto bitmapData = val->GetAllBitmapData();
    ret &= parcel.WriteInt32(bitmapData.second);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList bitmap size");
        return ret;
    }
    if (bitmapData.second > 0) {
        ret &= RSMarshallingHelper::WriteToParcel(parcel, bitmapData.first, bitmapData.second);
        if (!ret) {
            ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList bitmap");
            return ret;
        }
    }

    std::vector<std::shared_ptr<Drawing::ExtendImageObject>> objectVec;
    uint32_t objectSize = val->GetAllObject(objectVec);
    ret &= parcel.WriteUint32(objectSize);
    if (objectSize > 0) {
        for (const auto& object : objectVec) {
            auto rsObject = std::static_pointer_cast<RSExtendImageObject>(object);
            ret &= RSMarshallingHelper::Marshalling(parcel, rsObject);
            if (!ret) {
                ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList imageObject");
                RS_TRACE_NAME("RSMarshallingHelper pixelmap marshalling failed");
                return ret;
            }
        }
    }

    std::vector<std::shared_ptr<Drawing::ExtendImageBaseObj>> objectBaseVec;
    uint32_t objectBaseSize = val->GetAllBaseObj(objectBaseVec);
    ret &= parcel.WriteUint32(objectBaseSize);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList imageBase");
        return ret;
    }
    if (objectBaseSize > 0) {
        for (const auto& objectBase : objectBaseVec) {
            auto rsBaseObject = std::static_pointer_cast<RSExtendImageBaseObj>(objectBase);
            ret &= RSMarshallingHelper::Marshalling(parcel, rsBaseObject);
            if (!ret) {
                ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList imageBase");
                return ret;
            }
        }
    }

    ret &= MarshallingExtendObjectFromDrawCmdList(parcel, val);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList ExtendObject");
        return ret;
    }
    ret &= MarshallingRecordCmdFromDrawCmdList(parcel, val);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList RecordCmd");
        return ret;
    }
#ifdef ROSEN_OHOS
    std::vector<std::shared_ptr<Drawing::SurfaceBufferEntry>> surfaceBufferEntryVec;
    uint32_t surfaceBufferSize = val->GetAllSurfaceBufferEntry(surfaceBufferEntryVec);
    ret = parcel.WriteUint32(surfaceBufferSize);
    if (surfaceBufferSize > 0) {
        for (const auto& object : surfaceBufferEntryVec) {
            if (!object) {
                ROSEN_LOGE("RSMarshallingHelper::Marshalling DrawCmdList surfaceBufferVec has null object");
                return false;
            }
            auto surfaceBuffer = object->surfaceBuffer_;
            MessageParcel* parcelSurfaceBuffer =  static_cast<MessageParcel*>(&parcel);
            WriteSurfaceBufferImpl(
                *parcelSurfaceBuffer, surfaceBuffer->GetSeqNum(), surfaceBuffer);
            auto acquireFence = object->acquireFence_;
            if (acquireFence) {
                parcel.WriteBool(true);
                acquireFence->WriteToMessageParcel(*parcelSurfaceBuffer);
            } else {
                parcel.WriteBool(false);
            }
        }
    }
#endif
    return ret;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::DrawCmdList>& val)
{
    int32_t size = parcel.ReadInt32();
    if (size == -1) {
        val = nullptr;
        return true;
    }
    int32_t width = parcel.ReadInt32();
    int32_t height = parcel.ReadInt32();
    if (size == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Unmarshalling Drawing::DrawCmdList size is 0");
        val = std::make_shared<Drawing::DrawCmdList>(width, height);
        return true;
    }

    bool isCache = parcel.ReadBool();
    bool cachedHighContrast = parcel.ReadBool();

    uint32_t replacedOpListSize = parcel.ReadUint32();
    uint32_t readableSize = parcel.GetReadableBytes() / (sizeof(uint32_t) * 2);    // 增加IPC异常保护，读取2个uint_32_t
    if (replacedOpListSize > readableSize) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling Drawing readableSize %{public}d less than size %{public}d",
            readableSize, replacedOpListSize);
        val = nullptr;
        return false;
    }
    std::vector<std::pair<uint32_t, uint32_t>> replacedOpList;
    for (uint32_t i = 0; i < replacedOpListSize; ++i) {
        auto regionPos = parcel.ReadUint32();
        auto replacePos = parcel.ReadUint32();
        replacedOpList.emplace_back(regionPos, replacePos);
    }

    bool isMalloc = false;
    const void* data = RSMarshallingHelper::ReadFromParcel(parcel, size, isMalloc);
    if (data == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Drawing::DrawCmdList");
        return false;
    }

    val = Drawing::DrawCmdList::CreateFromData({ data, size }, true);
    if (isMalloc) {
        free(const_cast<void*>(data));
        data = nullptr;
    }
    if (val == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Drawing::DrawCmdList is nullptr");
        return false;
    }
    val->SetWidth(width);
    val->SetHeight(height);

    val->SetIsCache(isCache);
    val->SetCachedHighContrast(cachedHighContrast);
    val->SetReplacedOpList(replacedOpList);

    int32_t imageSize = parcel.ReadInt32();
    if (imageSize > 0) {
        bool isMal = false;
        const void* imageData = RSMarshallingHelper::ReadFromParcel(parcel, imageSize, isMal);
        if (imageData == nullptr) {
            ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Drawing::DrawCmdList image is nullptr");
            return false;
        }
        val->SetUpImageData(imageData, imageSize);
        if (isMal) {
            free(const_cast<void*>(imageData));
            imageData = nullptr;
        }
    }

    int32_t bitmapSize = parcel.ReadInt32();
    if (bitmapSize > 0) {
        bool isMal = false;
        const void* bitmapData = RSMarshallingHelper::ReadFromParcel(parcel, bitmapSize, isMal);
        if (bitmapData == nullptr) {
            ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Drawing::DrawCmdList bitmap is nullptr");
            return false;
        }
        val->SetUpBitmapData(bitmapData, bitmapSize);
        if (isMal) {
            free(const_cast<void*>(bitmapData));
            bitmapData = nullptr;
        }
    }

    bool ret = true;
    uint32_t objectSize = parcel.ReadUint32();
    if (objectSize > 0) {
        if (objectSize > PARTICLE_UPPER_LIMIT) {
            return false;
        }
        std::vector<std::shared_ptr<Drawing::ExtendImageObject>> imageObjectVec;
        for (uint32_t i = 0; i < objectSize; i++) {
            std::shared_ptr<RSExtendImageObject> object;
            ret &= RSMarshallingHelper::Unmarshalling(parcel, object);
            if (!ret) {
                ROSEN_LOGE(
                    "unirender: failed RSMarshallingHelper::Unmarshalling DrawCmdList imageObject: %{public}d", i);
                return ret;
            }
            imageObjectVec.emplace_back(object);
        }
        val->SetupObject(imageObjectVec);
    }

    uint32_t objectBaseSize = parcel.ReadUint32();
    if (objectBaseSize > 0) {
        if (objectBaseSize > PARTICLE_UPPER_LIMIT) {
            return false;
        }
        std::vector<std::shared_ptr<Drawing::ExtendImageBaseObj>> ObjectBaseVec;
        for (uint32_t i = 0; i < objectBaseSize; i++) {
            std::shared_ptr<RSExtendImageBaseObj> objectBase;
            ret &= RSMarshallingHelper::Unmarshalling(parcel, objectBase);
            if (!ret) {
                ROSEN_LOGE(
                    "unirender: failed RSMarshallingHelper::Unmarshalling DrawCmdList objectBase: %{public}d", i);
                return ret;
            }
            ObjectBaseVec.emplace_back(objectBase);
        }
        val->SetupBaseObj(ObjectBaseVec);
    }

    ret &= UnmarshallingExtendObjectToDrawCmdList(parcel, val);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList ExtendObject");
        return ret;
    }

    ret &= UnmarshallingRecordCmdToDrawCmdList(parcel, val);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList RecordCmd");
        return ret;
    }
#ifdef ROSEN_OHOS
    uint32_t surfaceBufferEntrySize = parcel.ReadUint32();
    if (surfaceBufferEntrySize > 0) {
        if (surfaceBufferEntrySize > PARTICLE_UPPER_LIMIT) {
            return false;
        }
        std::vector<std::shared_ptr<Drawing::SurfaceBufferEntry>> surfaceBufferEntryVec;
        for (uint32_t i = 0; i < surfaceBufferEntrySize; ++i) {
            sptr<SurfaceBuffer> surfaceBuffer;
            MessageParcel* parcelSurfaceBuffer = static_cast<MessageParcel*>(&parcel);
            uint32_t sequence = 0U;
            GSError retCode = ReadSurfaceBufferImpl(*parcelSurfaceBuffer, sequence, surfaceBuffer);
            if (retCode != GSERROR_OK) {
                ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList failed read surfaceBuffer: %{public}d %{public}d", i, retCode);
                return false;
            }
            sptr<SyncFence> acquireFence = nullptr;
            bool hasAcquireFence = parcel.ReadBool();
            if (hasAcquireFence) {
                acquireFence = SyncFence::ReadFromMessageParcel(*parcelSurfaceBuffer);
            }
            std::shared_ptr<Drawing::SurfaceBufferEntry> surfaceBufferEntry =
                std::make_shared<Drawing::SurfaceBufferEntry>(surfaceBuffer, acquireFence);
            surfaceBufferEntryVec.emplace_back(surfaceBufferEntry);
        }
        val->SetupSurfaceBufferEntry(surfaceBufferEntryVec);
    }
#endif

    RS_PROFILER_PATCH_TYPEFACE_ID(parcel, val);
    val->UnmarshallingDrawOps();
    return ret;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<Drawing::RecordCmd>& val)
{
    if (!val) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling failed, RecordCmd is nullptr");
        return false;
    }
    const auto& rect = val->GetCullRect();
    return parcel.WriteFloat(rect.GetLeft()) && parcel.WriteFloat(rect.GetTop()) &&
        parcel.WriteFloat(rect.GetRight()) && parcel.WriteFloat(rect.GetBottom()) &&
        RSMarshallingHelper::Marshalling(parcel, val->GetDrawCmdList());
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::RecordCmd>& val)
{
    val = nullptr;
    float left = 0;
    float top = 0;
    float right = 0;
    float bottom = 0;
    bool success = parcel.ReadFloat(left) && parcel.ReadFloat(top) &&
        parcel.ReadFloat(right) && parcel.ReadFloat(bottom);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling RecordCmd, read float failed.");
        return false;
    }
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr;
    success = RSMarshallingHelper::Unmarshalling(parcel, drawCmdList);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling RecordCmd, drawCmdList unmarshalling failed.");
        return false;
    }
    Drawing::Rect rect(left, top, right, bottom);
    val = std::make_shared<Drawing::RecordCmd>(drawCmdList, rect);
    return true;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSExtendImageObject>& val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }
    if (!(parcel.WriteInt32(1) && val->Marshalling(parcel))) {
        ROSEN_LOGE("failed RSMarshallingHelper::Marshalling imageObject");
        return false;
    }

    return true;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSExtendImageObject>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(RSExtendImageObject::Unmarshalling(parcel));
    if (val == nullptr) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling imageObject");
        return false;
    }

    return true;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSExtendImageBaseObj>& val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }
    if (!(parcel.WriteInt32(1) && val->Marshalling(parcel))) {
        ROSEN_LOGE("failed RSMarshallingHelper::Marshalling ImageBaseObj");
        return false;
    }

    return true;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSExtendImageBaseObj>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(RSExtendImageBaseObj::Unmarshalling(parcel));
    if (val == nullptr) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling ImageBaseObj");
        return false;
    }

    return true;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<Drawing::MaskCmdList>& val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }
    auto cmdListData = val->GetData();
    bool ret = parcel.WriteInt32(cmdListData.second);

    if (cmdListData.second == 0 || !ret) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Marshalling Drawing::MaskCmdList, size is 0");
        return ret;
    }

    ret = RSMarshallingHelper::WriteToParcel(parcel, cmdListData.first, cmdListData.second);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::MaskCmdList");
        return ret;
    }

    auto imageData = val->GetAllImageData();
    ret = parcel.WriteInt32(imageData.second);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::MaskCmdList image size");
        return ret;
    }
    if (imageData.second > 0) {
        ret = RSMarshallingHelper::WriteToParcel(parcel, imageData.first, imageData.second);
        if (!ret) {
            ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::MaskCmdList image");
            return ret;
        }
    }

    auto bitmapData = val->GetAllBitmapData();
    ret &= parcel.WriteInt32(bitmapData.second);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::MaskCmdList bitmap size");
        return ret;
    }
    if (bitmapData.second > 0) {
        ret &= RSMarshallingHelper::WriteToParcel(parcel, bitmapData.first, bitmapData.second);
        if (!ret) {
            ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::MaskCmdList bitmap");
            return ret;
        }
    }

    return ret;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::MaskCmdList>& val)
{
    int32_t size = parcel.ReadInt32();
    if (size == -1) {
        return true;
    }
    if (size == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Unmarshalling Drawing::MaskCmdList size is 0");
        return true;
    }
    bool isMalloc = false;
    const void* data = RSMarshallingHelper::ReadFromParcel(parcel, size, isMalloc);
    if (data == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Drawing::MaskCmdList");
        return false;
    }
    val = Drawing::MaskCmdList::CreateFromData({ data, size }, true);
    if (isMalloc) {
        free(const_cast<void*>(data));
        data = nullptr;
    }
    if (val == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Drawing::MaskCmdList is nullptr");
        return false;
    }
    int32_t imageSize = parcel.ReadInt32();
    if (imageSize > 0) {
        bool isMal = false;
        const void* imageData = RSMarshallingHelper::ReadFromParcel(parcel, imageSize, isMal);
        if (imageData == nullptr) {
            ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Drawing::MaskCmdList image is nullptr");
            return false;
        }
        val->SetUpImageData(imageData, imageSize);
        if (isMal) {
            free(const_cast<void*>(imageData));
            imageData = nullptr;
        }
    }
    int32_t bitmapSize = parcel.ReadInt32();
    if (bitmapSize > 0) {
        bool isMal = false;
        const void* bitmapData = RSMarshallingHelper::ReadFromParcel(parcel, bitmapSize, isMal);
        if (bitmapData == nullptr) {
            ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Drawing::MaskCmdList bitmap is nullptr");
            return false;
        }
        val->SetUpBitmapData(bitmapData, bitmapSize);
        if (isMal) {
            free(const_cast<void*>(bitmapData));
            bitmapData = nullptr;
        }
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
#undef MARSHALLING_AND_UNMARSHALLING

#define MARSHALLING_AND_UNMARSHALLING(TEMPLATE)                                                 \
    bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE>& val) \
    {                                                                                           \
        if (val == nullptr) {                                                                   \
            RS_LOGW("MarshallingHelper::Marshalling nullptr");                                  \
        }                                                                                       \
        return parcel.WriteParcelable(val.get());                                               \
    }                                                                                           \
    bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE>& val)     \
    {                                                                                           \
        val.reset(parcel.ReadParcelable<TEMPLATE>());                                           \
        return val != nullptr;                                                                  \
    }

MARSHALLING_AND_UNMARSHALLING(RSRenderCurveAnimation)
MARSHALLING_AND_UNMARSHALLING(RSRenderParticleAnimation)
MARSHALLING_AND_UNMARSHALLING(RSRenderInterpolatingSpringAnimation)
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

#define MARSHALLING_AND_UNMARSHALLING(TEMPLATE)                                                                       \
    template<typename T>                                                                                              \
    bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<T>>& val)                    \
    {                                                                                                                 \
        return parcel.WriteInt16(static_cast<int16_t>(val->GetPropertyType())) && parcel.WriteUint64(val->GetId()) && \
               Marshalling(parcel, val->Get());                                                                       \
    }                                                                                                                 \
    template<typename T>                                                                                              \
    bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<T>>& val)                        \
    {                                                                                                                 \
        PropertyId id = 0;                                                                                            \
        int16_t typeId = 0;                                                                                           \
        if (!parcel.ReadInt16(typeId)) {                                                                              \
            return false;                                                                                             \
        }                                                                                                             \
        RSRenderPropertyType type = static_cast<RSRenderPropertyType>(typeId);                                        \
        if (!parcel.ReadUint64(id)) {                                                                                 \
            return false;                                                                                             \
        }                                                                                                             \
        T value;                                                                                                      \
        if (!Unmarshalling(parcel, value)) {                                                                          \
            return false;                                                                                             \
        }                                                                                                             \
        RS_PROFILER_PATCH_NODE_ID(parcel, id);                                                                        \
        val.reset(new TEMPLATE<T>(value, id, type));                                                                  \
        return val != nullptr;                                                                                        \
    }

MARSHALLING_AND_UNMARSHALLING(RSRenderProperty)
MARSHALLING_AND_UNMARSHALLING(RSRenderAnimatableProperty)
#undef MARSHALLING_AND_UNMARSHALLING

#define EXPLICIT_INSTANTIATION(TEMPLATE, TYPE)                                                                  \
    template bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<TYPE>>& val); \
    template bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<TYPE>>& val);

#define BATCH_EXPLICIT_INSTANTIATION(TEMPLATE)                                           \
    EXPLICIT_INSTANTIATION(TEMPLATE, bool)                                               \
    EXPLICIT_INSTANTIATION(TEMPLATE, float)                                              \
    EXPLICIT_INSTANTIATION(TEMPLATE, int)                                                \
    EXPLICIT_INSTANTIATION(TEMPLATE, Color)                                              \
    EXPLICIT_INSTANTIATION(TEMPLATE, RSDynamicBrightnessPara)                            \
    EXPLICIT_INSTANTIATION(TEMPLATE, RSWaterRipplePara)                                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, RSFlyOutPara)                                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, Gravity)                                            \
    EXPLICIT_INSTANTIATION(TEMPLATE, GradientDirection)                                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, ForegroundColorStrategyType)                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, Matrix3f)                                           \
    EXPLICIT_INSTANTIATION(TEMPLATE, Quaternion)                                         \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSFilter>)                          \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSImage>)                           \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSMask>)                            \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSPath>)                            \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSShader>)                          \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSLinearGradientBlurPara>)          \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<MotionBlurParam>)                   \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSMagnifierParams>)                 \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<EmitterUpdater>)                    \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::vector<std::shared_ptr<EmitterUpdater>>)       \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<ParticleNoiseField>)                \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<ParticleNoiseFields>)               \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::vector<std::shared_ptr<ParticleRenderParams>>) \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<ParticleRenderParams>)              \
    EXPLICIT_INSTANTIATION(TEMPLATE, RSRenderParticleVector)                             \
    EXPLICIT_INSTANTIATION(TEMPLATE, RenderParticleColorParaType)                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, RenderParticleParaType<float>)                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, ParticleVelocity)                                   \
    EXPLICIT_INSTANTIATION(TEMPLATE, EmitterConfig)                                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector2f)                                           \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<uint32_t>)                                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<Color>)                                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f)                                           \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<Drawing::DrawCmdList>)              \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<Drawing::RecordCmd>)                \
    EXPLICIT_INSTANTIATION(TEMPLATE, Drawing::Matrix)

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
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, RRectT<float>)

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
        ROSEN_LOGD("RSMarshallingHelper::WriteToParcel data exceed MAX_DATA_SIZE, size:%{public}zu", size);
    }

    if (!parcel.WriteUint32(size)) {
        return false;
    }
    if (size < MIN_DATA_SIZE || (!g_useSharedMem && g_tid == std::this_thread::get_id())) {
        return parcel.WriteUnpadBuffer(data, size);
    }

    // write to ashmem
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(size, PROT_READ | PROT_WRITE);
    if (!ashmemAllocator) {
        ROSEN_LOGE("RSMarshallingHelper::WriteToParcel CreateAshmemAllocator fail");
        return false;
    }
    RS_PROFILER_WRITE_PARCEL_DATA(parcel);
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

const void* RSMarshallingHelper::ReadFromParcel(Parcel& parcel, size_t size, bool& isMalloc)
{
    uint32_t bufferSize = parcel.ReadUint32();
    if (static_cast<unsigned int>(bufferSize) != size) {
        ROSEN_LOGE("RSMarshallingHelper::ReadFromParcel size mismatch");
        return nullptr;
    }
    if (static_cast<unsigned int>(bufferSize) < MIN_DATA_SIZE  ||
        (!g_useSharedMem && g_tid == std::this_thread::get_id())) {
        isMalloc = false;
        return parcel.ReadUnpadBuffer(size);
    }
    // read from ashmem
    return RS_PROFILER_READ_PARCEL_DATA(parcel, size, isMalloc);
}

bool RSMarshallingHelper::SkipFromParcel(Parcel& parcel, size_t size)
{
    int32_t bufferSize = parcel.ReadInt32();
    if (static_cast<unsigned int>(bufferSize) != size) {
        ROSEN_LOGE("RSMarshallingHelper::SkipFromParcel size mismatch");
        return false;
    }
    if (static_cast<unsigned int>(bufferSize) < MIN_DATA_SIZE ||
        (!g_useSharedMem && g_tid == std::this_thread::get_id())) {
        parcel.SkipBytes(size);
        return true;
    }
    if (RS_PROFILER_SKIP_PARCEL_DATA(parcel, size)) {
        return true;
    }
    // read from ashmem
    int fd = static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocatorWithFd(fd, size, PROT_READ);
    return ashmemAllocator != nullptr;
}

const void* RSMarshallingHelper::ReadFromAshmem(Parcel& parcel, size_t size, bool& isMalloc)
{
    isMalloc = false;
    int fd = static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocatorWithFd(fd, size, PROT_READ);
    if (!ashmemAllocator) {
        ROSEN_LOGE("RSMarshallingHelper::ReadFromAshmem CreateAshmemAllocator fail");
        return nullptr;
    }
    isMalloc = true;
    return ashmemAllocator->CopyFromAshmem(size);
}

void RSMarshallingHelper::BeginNoSharedMem(std::thread::id tid)
{
    g_useSharedMem = false;
    g_tid = tid;
}
void RSMarshallingHelper::EndNoSharedMem()
{
    g_useSharedMem = true;
    g_tid.__reset();
}

bool RSMarshallingHelper::GetUseSharedMem(std::thread::id tid)
{
    if (tid == g_tid) {
        return g_useSharedMem;
    }
    return true;
}

bool RSMarshallingHelper::CheckReadPosition(Parcel& parcel)
{
    auto curPosition = parcel.GetReadPosition();
    auto positionRead = parcel.ReadUint32();
    if (positionRead != static_cast<uint32_t>(curPosition)) {
        RS_LOGE("RSMarshallingHelper::CheckReadPosition failed, curPosition:%{public}zu, positionRead:%{public}u",
            curPosition, positionRead);
        return false;
    }
    return true;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSRenderPropertyBase>& val)
{
    return RSRenderPropertyBase::Marshalling(parcel, val);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSRenderPropertyBase>& val)
{
    return RSRenderPropertyBase::Unmarshalling(parcel, val);
}
} // namespace Rosen
} // namespace OHOS
