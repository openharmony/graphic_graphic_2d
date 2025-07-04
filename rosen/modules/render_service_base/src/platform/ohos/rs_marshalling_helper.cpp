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

#include <cstddef>
#include <cstdint>
#include <memory>
#include <message_parcel.h>
#include <sys/mman.h>
#include <unistd.h>

#include "feature/capture/rs_ui_capture.h"
#include "image_type.h"
#include "pixel_map.h"
#include "recording/record_cmd.h"
#include "rs_profiler.h"
#include "rs_trace.h"

#include "animation/rs_particle_noise_field.h"
#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_interpolating_spring_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_particle.h"
#include "animation/rs_render_particle_animation.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "animation/rs_render_transition.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_mask_base.h"
#include "effect/rs_render_shader_base.h"
#include "memory/rs_memory_flow_control.h"
#include "memory/rs_memory_track.h"
#include "modifier/rs_render_modifier.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "pipeline/rs_draw_cmd.h"
#include "platform/common/rs_log.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_image.h"
#include "render/rs_image_base.h"
#include "render/rs_magnifier_para.h"
#include "render/rs_mask.h"
#include "render/rs_motion_blur_filter.h"
#include "render/rs_path.h"
#include "render/rs_pixel_map_shader.h"
#include "render/rs_render_filter.h"
#include "render/rs_shader.h"
#include "transaction/rs_ashmem_helper.h"

#ifdef ROSEN_OHOS
#include "buffer_utils.h"
#endif
#include "recording/mask_cmd_list.h"

#include "property/rs_properties_def.h"

namespace OHOS {
namespace Rosen {

namespace {
bool g_useSharedMem = true;
std::thread::id g_tid = std::thread::id();
std::mutex g_writeMutex;
constexpr size_t PIXELMAP_UNMARSHALLING_DEBUG_OFFSET = 12;
thread_local pid_t g_callingPid = 0;
constexpr size_t NUM_ITEMS_IN_VERSION = 4;
}

static std::vector<uint8_t> supportedParcelVerFlags = { RSPARCELVER_ADD_ANIMTOKEN };

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

#define MARSHALLING_AND_UNMARSHALLING(TYPE, TYPENAME)                                                       \
    bool RSMarshallingHelper::CompatibleMarshalling(Parcel& parcel, const TYPE& val, uint16_t paramVersion) \
    {                                                                                                       \
        return parcel.Write##TYPENAME(val);                                                                 \
    }                                                                                                       \
    bool RSMarshallingHelper::CompatibleUnmarshalling(                                                      \
        Parcel& parcel, TYPE& val, TYPE defaultValue, uint16_t paramVersion)                                \
    {                                                                                                       \
        if (paramVersion == RSPARCELVER_ALWAYS ||                                                           \
            RSMarshallingHelper::TransactionVersionCheck(parcel, paramVersion)) {                           \
            return parcel.Read##TYPENAME(val);                                                              \
        }                                                                                                   \
        val = defaultValue;                                                                                 \
        return true;                                                                                        \
    }

void RSMarshallingHelper::CompatibleUnmarshallingObsolete(Parcel& parcel, size_t typeSize, uint16_t paramVersion)
{
    if (paramVersion != RSPARCELVER_ALWAYS && !RSMarshallingHelper::TransactionVersionCheck(parcel, paramVersion)) {
        parcel.SkipBytes(typeSize);
    }
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
static bool MarshallingRecordCmdFromDrawCmdList(Parcel& parcel, const std::shared_ptr<Drawing::DrawCmdList>& val,
    int32_t recordCmdDepth)
{
    if (!val) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::MarshallingRecordCmdFromDrawCmdList failed with null CmdList");
        return false;
    }
    std::vector<std::shared_ptr<Drawing::RecordCmd>> recordCmdVec;
    uint32_t recordCmdSize = val->GetAllRecordCmd(recordCmdVec);
    if (!parcel.WriteUint32(recordCmdSize)) {
        ROSEN_LOGE("MarshallingRecordCmdFromDrawCmdList WriteUint32 failed");
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
        if (!RSMarshallingHelper::Marshalling(parcel, recordCmd, recordCmdDepth)) {
            return false;
        }
    }
    return true;
}

static bool UnmarshallingRecordCmdToDrawCmdList(Parcel& parcel, std::shared_ptr<Drawing::DrawCmdList>& val,
    uint32_t* opItemCount, uint32_t* recordCmdCount, int32_t recordCmdDepth)
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
        if (recordCmdCount && ++(*recordCmdCount) > Drawing::RECORD_CMD_MAX_SIZE) {
            ROSEN_LOGE("unirender: RSMarshallingHelper::UnmarshallingRecordCmdToDrawCmdList failed, "
                "exceed recordCmd count limit, cur[%{public}u]", *recordCmdCount);
            return false;
        }
        std::shared_ptr<Drawing::RecordCmd> recordCmd = nullptr;
        if (!RSMarshallingHelper::Unmarshalling(parcel, recordCmd, opItemCount, recordCmdCount, recordCmdDepth)) {
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
        ROSEN_LOGE("MarshallingExtendObjectFromDrawCmdList WriteUint32 failed");
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
        bool flag = parcel.WriteUint32(UINT32_MAX);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteUint32 failed");
        }
        return flag;
    }

    uint32_t size = val->GetSize();
    if (size == UINT32_MAX) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::Marshalling Data failed with max limit");
        return false;
    }

    if (size == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Marshalling Data size is 0");
        bool flag = parcel.WriteUint32(0);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteUint32 failed");
        }
        return flag;
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
        if (isMalloc) {
            free(const_cast<void*>(data));
            data = nullptr;
        }
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
    MemoryInfo info = { 0 };
    bool result = MemoryTrack::Instance().GetPictureRecordMemInfo(ptr, info);
    if (result && info.initialPid && (info.type == MEMORY_TYPE::MEM_SKIMAGE)) {
        MemorySnapshot::Instance().RemoveCpuMemory(info.initialPid, info.size);
    }
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
    bool flag;
    if (!val) {
        flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteInt32 -1 failed");
        }
        return flag;
    }
    int32_t type = val->IsLazyGenerated();
    if (!parcel.WriteInt32(type)) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteInt32 type failed");
    }
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

        if (!parcel.WriteInt32(size)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteInt32 size failed");
            return false;
        }
        if (!WriteToParcel(parcel, addr, size)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling Image write parcel failed");
            return false;
        }
        if (!parcel.WriteInt32(rb)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteInt32 rb failed");
            return false;
        }
        if (!parcel.WriteInt32(width)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteInt32 width failed");
            return false;
        }
        if (!parcel.WriteInt32(height)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteInt32 height failed");
            return false;
        }
        if (!parcel.WriteInt32(pixmap.GetColorType())) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteInt32 color type failed");
            return false;
        }
        if (!parcel.WriteInt32(pixmap.GetAlphaType())) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteInt32 alpha type failed");
            return false;
        }

        if (pixmap.GetColorSpace() == nullptr) {
            flag = parcel.WriteUint32(0);
            if (!flag) {
                ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteUint32 0 failed");
            }
            return flag;
        } else {
            auto data = pixmap.GetColorSpace()->Serialize();
            if (data == nullptr || data->GetSize() == 0) {
                flag = parcel.WriteUint32(0);
                if (!flag) {
                    ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteUint32 0 with get size 0 failed");
                }
                return flag;
            }
            if (!parcel.WriteUint32(data->GetSize())) {
                ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteUint32 get size failed");
                return false;
            }
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
            if (isMal) {
                free(const_cast<void*>(dataPtr));
                dataPtr = nullptr;
            }
            ROSEN_LOGE("data build without copy failed");
            return false;
        }
        if (colorSpace->Deserialize(data) == false) {
            if (isMal) {
                free(const_cast<void*>(dataPtr));
                dataPtr = nullptr;
            }
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
    uint32_t pixmapSize{0};
    if (!parcel.ReadUint32(pixmapSize)) {
        ROSEN_LOGE("RSMarshallingHelper::UnmarshallingNoLazyGeneratedImage Read pixmapSize failed");
        return false;
    }
    bool isMalloc = false;
    const void* addr = RSMarshallingHelper::ReadFromParcel(parcel, pixmapSize, isMalloc);
    if (addr == nullptr) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling Drawing::Image read from parcel");
        return false;
    }

    uint32_t rb{0};
    int width{0};
    int height{0};
    if (!parcel.ReadUint32(rb) || !parcel.ReadInt32(width) || !parcel.ReadInt32(height)) {
        ROSEN_LOGE("RSMarshallingHelper::UnmarshallingNoLazyGeneratedImage Read ImageInfo failed");
        return false;
    }
    uint32_t ct{0};
    if (!parcel.ReadUint32(ct)) {
        ROSEN_LOGE("RSMarshallingHelper::UnmarshallingNoLazyGeneratedImage Read ct failed");
        return false;
    }
    Drawing::ColorType colorType = Drawing::ColorType::COLORTYPE_UNKNOWN;
    if (ct >= Drawing::ColorType::COLORTYPE_ALPHA_8 && ct <= Drawing::ColorType::COLORTYPE_RGB_888X) {
        colorType = static_cast<Drawing::ColorType>(ct);
    }
    uint32_t at{0};
    if (!parcel.ReadUint32(at)) {
        ROSEN_LOGE("RSMarshallingHelper::UnmarshallingNoLazyGeneratedImage Read at failed");
        return false;
    }
    Drawing::AlphaType alphaType = Drawing::AlphaType::ALPHATYPE_UNKNOWN;
    if (at >= Drawing::AlphaType::ALPHATYPE_OPAQUE && at <= Drawing::AlphaType::ALPHATYPE_UNPREMUL) {
        alphaType = static_cast<Drawing::AlphaType>(at);
    }
    auto colorSpace = std::make_shared<Drawing::ColorSpace>(Drawing::ColorSpace::ColorSpaceType::NO_TYPE);

    if (!ReadColorSpaceFromParcel(parcel, colorSpace)) {
        if (isMalloc) {
            free(const_cast<void*>(addr));
            addr = nullptr;
        }
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
        MemoryInfo info = {pixmapSize, g_callingPid, 0, 0, MEMORY_TYPE::MEM_SKIMAGE, g_callingPid,
            OHOS::Media::AllocatorType::DEFAULT};
        MemoryTrack::Instance().AddPictureRecord(addr, info);
        if (g_callingPid) {
            MemorySnapshot::Instance().AddCpuMemory(g_callingPid, pixmapSize);
        }
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
        uint32_t pixmapSize{0};
        if (!parcel.ReadUint32(pixmapSize)) {
            ROSEN_LOGE("RSMarshallingHelper::SkipImage Read pixmapSize failed");
            return false;
        }
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
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSShader is nullptr");
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteInt32 -1 failed");
        }
        return flag;
    } else {
        bool flag = parcel.WriteInt32(0);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteInt32 0 failed");
            return false;
        }
        return parcel.WriteInt32(-1);
    }
    bool success = parcel.WriteInt32(static_cast<int>(val->GetShaderType()));
    success &= val->Marshalling(parcel);
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSShader>& val)
{
    int32_t type = parcel.ReadInt32();
    if (type == -1) {
        val = nullptr;
        return true;
    }

    val = RSShader::CreateRSShader(static_cast<RSShader::ShaderType>(type));
    if (!val) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::Unmarshalling RSShader, create RSShader failed");
        return false;
    }

    bool needReset = false;
    bool success = val->Unmarshalling(parcel, needReset);
    if (needReset) {
        val = nullptr;
    }
    return success;
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
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteInt32 -1 failed");
        }
        return flag;
    }
    bool success = parcel.WriteInt32(1) && Marshalling(parcel, val->blurRadius_);
    success &= Marshalling(parcel, val->isRadiusGradient_);
    success &= parcel.WriteUint32(static_cast<uint32_t>(val->fractionStops_.size()));
    for (size_t i = 0; i < val->fractionStops_.size(); i++) {
        success &= Marshalling(parcel, val->fractionStops_[i].first);
        success &= Marshalling(parcel, val->fractionStops_[i].second);
    }
    success &= Marshalling(parcel, val->direction_);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RSLinearGradientBlurPara failed");
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSLinearGradientBlurPara>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    float blurRadius;
    bool isRadiusGradient = false;
    std::vector<std::pair<float, float>> fractionStops;
    GradientDirection direction = GradientDirection::NONE;
    bool success = Unmarshalling(parcel, blurRadius);
    success &= Unmarshalling(parcel, isRadiusGradient);
    uint32_t fractionStopsSize{0};
    if (!parcel.ReadUint32(fractionStopsSize)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling RSLinearGradientBlurPara Read fractionStopsSize failed");
        return false;
    }
    if (fractionStopsSize > SIZE_UPPER_LIMIT) {
        return false;
    }
    for (size_t i = 0; i < fractionStopsSize; i++) {
        std::pair<float, float> fractionStop;
        float first = 0.0;
        float second = 0.0;
        success &= Unmarshalling(parcel, first);
        if (!success) {
            return false;
        }
        fractionStop.first = first;
        success &= Unmarshalling(parcel, second);
        if (!success) {
            return false;
        }
        fractionStop.second = second;
        fractionStops.push_back(fractionStop);
    }
    success &= Unmarshalling(parcel, direction);
    if (success) {
        val = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
        val->isRadiusGradient_ = isRadiusGradient;
    }
    return success;
}

// MotionBlurPara
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<MotionBlurParam>& val)
{
    bool success = Marshalling(parcel, val->radius);
    success &= Marshalling(parcel, val->scaleAnchor[0]);
    success &= Marshalling(parcel, val->scaleAnchor[1]);
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<MotionBlurParam>& val)
{
    float radius;
    float anchorX = 0.f;
    float anchorY = 0.f;

    bool success = Unmarshalling(parcel, radius);
    success &= Unmarshalling(parcel, anchorX);
    success &= Unmarshalling(parcel, anchorY);
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
    success &= Marshalling(parcel, val->width_);
    success &= Marshalling(parcel, val->height_);
    success &= Marshalling(parcel, val->cornerRadius_);
    success &= Marshalling(parcel, val->borderWidth_);
    success &= Marshalling(parcel, val->offsetX_);
    success &= Marshalling(parcel, val->offsetY_);

    success &= Marshalling(parcel, val->shadowOffsetX_);
    success &= Marshalling(parcel, val->shadowOffsetY_);
    success &= Marshalling(parcel, val->shadowSize_);
    success &= Marshalling(parcel, val->shadowStrength_);
 
    success &= Marshalling(parcel, val->gradientMaskColor1_);
    success &= Marshalling(parcel, val->gradientMaskColor2_);
    success &= Marshalling(parcel, val->outerContourColor1_);
    success &= Marshalling(parcel, val->outerContourColor2_);

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
    success &= Unmarshalling(parcel, width);
    if (success) { val->width_ = width; }
    success &= Unmarshalling(parcel, height);
    if (success) { val->height_ = height; }
    success &= Unmarshalling(parcel, cornerRadius);
    if (success) { val->cornerRadius_ = cornerRadius; }
    success &= Unmarshalling(parcel, borderWidth);
    if (success) { val->borderWidth_ = borderWidth; }
    success &= Unmarshalling(parcel, offsetX);
    if (success) { val->offsetX_ = offsetX; }
    success &= Unmarshalling(parcel, offsetY);
    if (success) { val->offsetY_ = offsetY; }

    success &= Unmarshalling(parcel, shadowOffsetX);
    if (success) { val->shadowOffsetX_ = shadowOffsetX; }
    success &= Unmarshalling(parcel, shadowOffsetY);
    if (success) { val->shadowOffsetY_ = shadowOffsetY; }
    success &= Unmarshalling(parcel, shadowSize);
    if (success) { val->shadowSize_ = shadowSize; }
    success &= Unmarshalling(parcel, shadowStrength);
    if (success) { val->shadowStrength_ = shadowStrength; }

    success &= Unmarshalling(parcel, gradientMaskColor1);
    if (success) { val->gradientMaskColor1_ = gradientMaskColor1; }
    success &= Unmarshalling(parcel, gradientMaskColor2);
    if (success) { val->gradientMaskColor2_ = gradientMaskColor2; }
    success &= Unmarshalling(parcel, outerContourColor1);
    if (success) { val->outerContourColor1_ = outerContourColor1; }
    success &= Unmarshalling(parcel, outerContourColor2);
    if (success) { val->outerContourColor2_ = outerContourColor2; }

    return success;
}

// Particle
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<AnnulusRegion>& val)
{
    if (!val) {
        return false;
    }
    return Marshalling(parcel, val->center_.x_) && Marshalling(parcel, val->center_.y_) &&
        Marshalling(parcel, val->innerRadius_) && Marshalling(parcel, val->outerRadius_) &&
        Marshalling(parcel, val->startAngle_) && Marshalling(parcel, val->endAngle_);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<AnnulusRegion>& val)
{
    float centerX = 0.f;
    float centerY = 0.f;
    float innerRadius = 0.f;
    float outerRadius = 0.f;
    float startAngle = 0.f;
    float endAngle = 0.f;
    bool success = Unmarshalling(parcel, centerX);
    success &= Unmarshalling(parcel, centerY);
    success &= Unmarshalling(parcel, innerRadius);
    success &= Unmarshalling(parcel, outerRadius);
    success &= Unmarshalling(parcel, startAngle);
    success &= Unmarshalling(parcel, endAngle);
    if (success) {
        Vector2f center(centerX, centerY);
        val = std::make_shared<AnnulusRegion>(center, innerRadius, outerRadius,
            startAngle, endAngle);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<Shape>& shape)
{
    if (!shape) {
        return parcel.WriteInt32(-1);
    }
    ShapeType type = shape->GetShapeType();
    bool success = parcel.WriteInt32(1) && Marshalling(parcel, type);
    if (type == ShapeType::ANNULUS) {
        auto annulusRegion = std::static_pointer_cast<AnnulusRegion>(shape);
        Marshalling(parcel, annulusRegion);
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Shape>& shape)
{
    if (parcel.ReadInt32() == -1) {
        shape = nullptr;
        return true;
    }
    ShapeType type = ShapeType::RECT;
    bool success = Unmarshalling(parcel, type);
    if (type == ShapeType::ANNULUS) {
        std::shared_ptr<AnnulusRegion> annulusRegion = nullptr;
        Unmarshalling(parcel, annulusRegion);
        shape = std::move(annulusRegion);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<EmitterUpdater>& val)
{
    if (!val) {
        ROSEN_LOGD("RSMarshallingHelper::Marshalling EmitterUpdater is nullptr");
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteInt32 -1 failed");
        }
        return flag;
    }
    bool success = parcel.WriteInt32(1) && Marshalling(parcel, val->emitterIndex_);
    success &= Marshalling(parcel, val->position_);
    success &= Marshalling(parcel, val->emitSize_) ;
    success &= Marshalling(parcel, val->emitRate_);
    success &= Marshalling(parcel, val->shape_);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling EmitterUpdater failed");
    }
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
    std::shared_ptr<Shape> shape = nullptr;

    bool success = Unmarshalling(parcel, emitterIndex);
    success &= Unmarshalling(parcel, position);
    success &= Unmarshalling(parcel, emitSize);
    success &= Unmarshalling(parcel, emitRate);
    success &= Unmarshalling(parcel, shape);
    if (success) {
        val = std::make_shared<EmitterUpdater>(emitterIndex, position, emitSize, emitRate);
        val->SetShape(shape);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::vector<std::shared_ptr<EmitterUpdater>>& val)
{
    bool success = parcel.WriteUint32(static_cast<uint32_t>(val.size()));
    for (size_t i = 0; i < val.size() && success; i++) {
        success &= Marshalling(parcel, val[i]);
    }
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling vector EmitterUpdater failed");
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::vector<std::shared_ptr<EmitterUpdater>>& val)
{
    uint32_t size{0};
    if (!parcel.ReadUint32(size)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling EmitterUpdater Read size failed");
        return false;
    }
    bool success = true;
    std::vector<std::shared_ptr<EmitterUpdater>> emitterUpdaters;
    if (size > PARTICLE_EMMITER_UPPER_LIMIT) {
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        std::shared_ptr<EmitterUpdater> emitterUpdater;
        success &= Unmarshalling(parcel, emitterUpdater);
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
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling ParticleNoiseField WriteInt32 failed");
        }
        return flag;
    }
    bool success = parcel.WriteInt32(1) && Marshalling(parcel, val->fieldStrength_);
    success &= Marshalling(parcel, val->fieldShape_);
    success &= Marshalling(parcel, val->fieldSize_.x_) && Marshalling(parcel, val->fieldSize_.y_);
    success &= Marshalling(parcel, val->fieldCenter_.x_) && Marshalling(parcel, val->fieldCenter_.y_);
    success &= Marshalling(parcel, val->fieldFeather_) &&  Marshalling(parcel, val->noiseScale_);
    success &= Marshalling(parcel, val->noiseFrequency_) &&  Marshalling(parcel, val->noiseAmplitude_);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling  ParticleNoiseField failed");
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<ParticleNoiseField>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling ParticleNoiseField ReadInt32 failed");
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
    success &= Unmarshalling(parcel, fieldShape);
    success &= Unmarshalling(parcel, fieldSizeX) && Unmarshalling(parcel, fieldSizeY);
    Vector2f fieldSize(fieldSizeX, fieldSizeY);
    success &= Unmarshalling(parcel, fieldCenterX) && Unmarshalling(parcel, fieldCenterY);
    Vector2f fieldCenter(fieldCenterX, fieldCenterY);
    success &= Unmarshalling(parcel, fieldFeather);
    success &= Unmarshalling(parcel, noiseScale);
    success &= Unmarshalling(parcel, noiseFrequency);
    success &= Unmarshalling(parcel, noiseAmplitude);
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
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling ParticleNoiseFields WriteInt32 failed");
        }
        return flag;
    }
    bool success = parcel.WriteInt32(1) && parcel.WriteUint32(static_cast<uint32_t>(val->fields_.size()));
    for (size_t i = 0; i < val->fields_.size() && success; i++) {
        success &= Marshalling(parcel, val->fields_[i]);
    }
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling ParticleNoiseFields failed");
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
    if (size > PARTICLE_EMMITER_UPPER_LIMIT) {
        return false;
    }
    std::shared_ptr<ParticleNoiseFields> noiseFields = std::make_shared<ParticleNoiseFields>();
    for (size_t i = 0; i < size; i++) {
        std::shared_ptr<ParticleNoiseField> ParticleNoiseField;
        success &= Unmarshalling(parcel, ParticleNoiseField);
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
    success &= Marshalling(parcel, val.emitShape_);
    success &= Marshalling(parcel, val.position_.x_);
    success &= Marshalling(parcel, val.position_.y_);
    success &= Marshalling(parcel, val.emitSize_.x_);
    success &= Marshalling(parcel, val.emitSize_.y_);
    success &= Marshalling(parcel, val.particleCount_);
    success &= Marshalling(parcel, val.lifeTime_.start_);
    success &= Marshalling(parcel, val.lifeTime_.end_);
    success &= Marshalling(parcel, val.type_);
    success &= Marshalling(parcel, val.radius_);
    success &= Marshalling(parcel, val.image_);
    success &= Marshalling(parcel, val.imageSize_.x_);
    success &= Marshalling(parcel, val.imageSize_.y_);
    success &= Marshalling(parcel, val.shape_);

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
    success &= Unmarshalling(parcel, emitShape);
    success &= Unmarshalling(parcel, positionX);
    success &= Unmarshalling(parcel, positionY);
    Vector2f position(positionX, positionY);
    success &= Unmarshalling(parcel, emitSizeWidth);
    success &= Unmarshalling(parcel, emitSizeHeight);
    Vector2f emitSize(emitSizeWidth, emitSizeHeight);
    success &= Unmarshalling(parcel, particleCount);
    success &= Unmarshalling(parcel, lifeTimeStart);
    success &= Unmarshalling(parcel, lifeTimeEnd);
    success &= Unmarshalling(parcel, particleType);
    success &= Unmarshalling(parcel, radius);
    Unmarshalling(parcel, image);
    success &= Unmarshalling(parcel, imageWidth);
    success &= Unmarshalling(parcel, imageHeight);
    Vector2f imageSize(imageWidth, imageHeight);
    std::shared_ptr<Shape> shape = nullptr;
    success &= Unmarshalling(parcel, shape);
    if (success) {
        Range<int64_t> lifeTime(lifeTimeStart, lifeTimeEnd);
        val = EmitterConfig(
            emitRate, emitShape, position, emitSize, particleCount, lifeTime, particleType, radius, image, imageSize);
        val.SetConfigShape(shape);
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
        success &= Marshalling(parcel, val.random_.start_) && Marshalling(parcel, val.random_.end_);
    } else if (val.updator_ == ParticleUpdator::CURVE) {
        success &= parcel.WriteUint32(static_cast<uint32_t>(val.valChangeOverLife_.size()));
        for (size_t i = 0; i < val.valChangeOverLife_.size(); i++) {
            if (val.valChangeOverLife_[i] == nullptr || val.valChangeOverLife_[i]->interpolator_ == nullptr) {
                ROSEN_LOGE("RSMarshallingHelper::Unmarshalling RenderParticleParaType fail cause nullptr");
                return false;
            }
            success &= Marshalling(parcel, val.valChangeOverLife_[i]->fromValue_);
            success &= Marshalling(parcel, val.valChangeOverLife_[i]->toValue_);
            success &= Marshalling(parcel, val.valChangeOverLife_[i]->startMillis_);
            success &= Marshalling(parcel, val.valChangeOverLife_[i]->endMillis_);
            success &= val.valChangeOverLife_[i]->interpolator_->Marshalling(parcel);
        }
    }
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RenderParticleParaType failed");
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
        success &= Unmarshalling(parcel, randomStart) && Unmarshalling(parcel, randomEnd);
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
            success &= Unmarshalling(parcel, fromValue);
            success &= Unmarshalling(parcel, toValue);
            success &= Unmarshalling(parcel, startMillis);
            success &= Unmarshalling(parcel, endMillis);
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
        success &= Marshalling(parcel, val.redRandom_.start_) && Marshalling(parcel, val.redRandom_.end_);
        success &= Marshalling(parcel, val.greenRandom_.start_) && Marshalling(parcel, val.greenRandom_.end_);
        success &= Marshalling(parcel, val.blueRandom_.start_) && Marshalling(parcel, val.blueRandom_.end_);
        success &= Marshalling(parcel, val.alphaRandom_.start_) && Marshalling(parcel, val.alphaRandom_.end_);
    } else if (val.updator_ == ParticleUpdator::CURVE) {
        success &= parcel.WriteUint32(static_cast<uint32_t>(val.valChangeOverLife_.size()));
        for (size_t i = 0; i < val.valChangeOverLife_.size(); i++) {
            if (val.valChangeOverLife_[i] == nullptr || val.valChangeOverLife_[i]->interpolator_ == nullptr) {
                ROSEN_LOGE("RSMarshallingHelper::Unmarshalling RenderParticleColorParaType fail cause nullptr");
                return false;
            }
            success &= Marshalling(parcel, val.valChangeOverLife_[i]->fromValue_);
            success &= Marshalling(parcel, val.valChangeOverLife_[i]->toValue_);
            success &= Marshalling(parcel, val.valChangeOverLife_[i]->startMillis_);
            success &= Marshalling(parcel, val.valChangeOverLife_[i]->endMillis_);
            success &= val.valChangeOverLife_[i]->interpolator_->Marshalling(parcel);
        }
    }
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RenderParticleColorParaType failed");
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
        success &= Unmarshalling(parcel, redRandomStart) && Unmarshalling(parcel, redRandomEnd) &&
                  Unmarshalling(parcel, greenRandomStart) && Unmarshalling(parcel, greenRandomEnd) &&
                  Unmarshalling(parcel, blueRandomStart) && Unmarshalling(parcel, blueRandomEnd) &&
                  Unmarshalling(parcel, alphaRandomStart) && Unmarshalling(parcel, alphaRandomEnd);
    } else if (updator == ParticleUpdator::CURVE) {
        uint32_t valChangeOverLifeSize{0};
        if (!parcel.ReadUint32(valChangeOverLifeSize)) {
            ROSEN_LOGE(
                "RSMarshallingHelper::Unmarshalling RenderParticleColorParaType Read valChangeOverLifeSize failed");
            return false;
        }
        if (valChangeOverLifeSize > SIZE_UPPER_LIMIT) {
            return false;
        }
        for (size_t i = 0; i < valChangeOverLifeSize; i++) {
            Color fromValue = RSColor(0, 0, 0);
            Color toValue = RSColor(0, 0, 0);
            int startMillis = 0;
            int endMillis = 0;
            success &= Unmarshalling(parcel, fromValue) && Unmarshalling(parcel, toValue) &&
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
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling ParticleRenderParams WriteInt32 failed");
        }
        return flag;
    }
    bool success = parcel.WriteInt32(1) && Marshalling(parcel, val->emitterConfig_);
    success &= Marshalling(parcel, val->velocity_);
    success &= Marshalling(parcel, val->acceleration_.accelerationValue_);
    success &= Marshalling(parcel, val->acceleration_.accelerationAngle_);
    success &= Marshalling(parcel, val->color_);
    success &= Marshalling(parcel, val->opacity_);
    success &= Marshalling(parcel, val->scale_);
    success &= Marshalling(parcel, val->spin_);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling ParticleRenderParams failed");
    }
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
    success &= Unmarshalling(parcel, velocity);
    success &= Unmarshalling(parcel, accelerationValue);
    success &= Unmarshalling(parcel, accelerationAngle);
    RenderParticleAcceleration acceleration = RenderParticleAcceleration(accelerationValue, accelerationAngle);
    success &= Unmarshalling(parcel, color);
    success &= Unmarshalling(parcel, opacity);
    success &= Unmarshalling(parcel, scale);
    success &= Unmarshalling(parcel, spin);
    if (success) {
        val =
            std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::vector<std::shared_ptr<ParticleRenderParams>>& val)
{
    bool success = parcel.WriteUint32(static_cast<uint32_t>(val.size()));
    for (size_t i = 0; i < val.size() && success; i++) {
        success &= Marshalling(parcel, val[i]);
    }
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling vector ParticleRenderParams failed");
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::vector<std::shared_ptr<ParticleRenderParams>>& val)
{
    uint32_t size{0};
    if (!parcel.ReadUint32(size)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling ParticleRenderParams Read size failed");
        return false;
    }
    bool success = true;
    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams;
    if (size > PARTICLE_EMMITER_UPPER_LIMIT) {
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        std::shared_ptr<ParticleRenderParams> particleRenderParams;
        success &= Unmarshalling(parcel, particleRenderParams);
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
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling RSPath WriteInt32 failed");
        }
        return flag;
    }
    
    std::shared_ptr<Drawing::Data> data = val->GetDrawingPath().Serialize();
    if (!data) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling Path is nullptr");
        return false;
    }
    bool success = parcel.WriteInt32(1) && Marshalling(parcel, data);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RSPath failed");
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSPath>& val)
{
    int32_t size{0};
    if (!parcel.ReadInt32(size)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling RSPath Read size failed");
        return false;
    }
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
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling RSMask WriteInt32 failed");
        }
        return flag;
    }
    bool flag = parcel.WriteInt32(1) && val->Marshalling(parcel);
    if (!flag) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RSMask failed");
    }
    return flag;
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

// RSRenderFilter
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSRenderFilter>& val)
{
    if (!val) {
        ROSEN_LOGD("RSMarshallingHelper::Marshalling RSRenderFilter is nullptr");
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling RSRenderFilter WriteInt32 failed");
        }
        return flag;
    }
    bool success = parcel.WriteInt32(1) && val->WriteToParcel(parcel);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RSRenderFilter failed");
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSRenderFilter>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    if (!val) {
        ROSEN_LOGW("RSMarshallingHelper::Unmarshalling val is nullptr and create it");
        val = std::make_shared<RSRenderFilter>();
    }
    return val->ReadFromParcel(parcel);
}

// RSNGRenderFilterBase
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSNGRenderFilterBase>& val)
{
    if (val == nullptr) {
        ROSEN_LOGW("RSMarshallingHelper::Marshalling RSNGRenderFilterBase is nullptr");
        if (!RSMarshallingHelper::Marshalling(parcel, END_OF_CHAIN)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling RSNGRenderFilterBase write end failed");
            return false;
        }
        return true;
    }
    bool success = val->Marshalling(parcel);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RSNGRenderFilterBase failed");
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderFilterBase>& val)
{
    bool success = RSNGRenderFilterBase::Unmarshalling(parcel, val);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling RSNGRenderFilterBase failed");
    }
    return success;
}

// RSNGRenderMaskBase
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSNGRenderMaskBase>& val)
{
    if (val == nullptr) {
        ROSEN_LOGW("RSMarshallingHelper::Marshalling RSNGRenderFilterBase is nullptr");
        if (!RSMarshallingHelper::Marshalling(parcel, END_OF_CHAIN)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling RSNGRenderFilterBase write end failed");
            return false;
        }
        return true;
    }
    bool success = val->Marshalling(parcel);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RSNGRenderMaskBase failed");
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderMaskBase>& val)
{
    bool success = RSNGRenderMaskBase::Unmarshalling(parcel, val);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling RSNGRenderMaskBase failed");
    }
    return success;
}

// RSNGRenderShaderBase
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSNGRenderShaderBase>& val)
{
    if (val == nullptr) {
        ROSEN_LOGW("RSMarshallingHelper::Marshalling RSNGRenderShaderBase is nullptr");
        if (!RSMarshallingHelper::Marshalling(parcel, END_OF_CHAIN)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling RSNGRenderShaderBase write end failed");
            return false;
        }
        return true;
    }
    bool success = val->Marshalling(parcel);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RSNGRenderShaderBase failed");
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSNGRenderShaderBase>& val)
{
    bool success = RSNGRenderShaderBase::Unmarshalling(parcel, val);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling RSNGRenderShaderBase failed");
    }
    return success;
}

// RSImageBase
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSImageBase>& val)
{
    if (!val) {
        ROSEN_LOGD("RSMarshallingHelper::Marshalling RSImageBase is nullptr");
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling RSImageBase WriteInt32 failed");
        }
        return flag;
    }
    bool success = parcel.WriteInt32(1) && val->Marshalling(parcel);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RSImageBase failed");
    }
    return success;
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
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling RSImage WriteInt32 failed");
        }
        return flag;
    }
    bool success = parcel.WriteInt32(1) && val->Marshalling(parcel);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RSImage failed");
    }
    return success;
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
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling PixelMap WriteInt32 failed");
        }
        return flag;
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
    void* pIndex = nullptr;
#ifdef ROSEN_OHOS
    if (RSSystemProperties::GetClosePixelMapFdEnabled()) {
        pIndex = addr;
    } else {
        pIndex = context;
    }
#else
    pIndex = addr;
#endif
    MemoryInfo info = { 0 };
    bool result = MemoryTrack::Instance().GetPictureRecordMemInfo(pIndex, info);
    if (result && info.initialPid && info.allocType != Media::AllocatorType::DMA_ALLOC) {
        auto realSize = info.allocType == Media::AllocatorType::SHARE_MEM_ALLOC
            ? info.size / 2 // rs only counts half of the SHARE_MEM_ALLOC memory
            : info.size;
        MemorySnapshot::Instance().RemoveCpuMemory(info.initialPid, realSize);
    }
    MemoryTrack::Instance().RemovePictureRecord(pIndex);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Media::PixelMap>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    auto readPosition = parcel.GetReadPosition();
    auto readSafeFdFunc = [](Parcel& parcel, std::function<int(Parcel&)> readFdDefaultFunc) -> int {
        return AshmemFdContainer::Instance().ReadSafeFd(parcel, readFdDefaultFunc);
    };
    val.reset(RS_PROFILER_UNMARSHAL_PIXELMAP(parcel, readSafeFdFunc));
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
    if (RSSystemProperties::GetClosePixelMapFdEnabled()) {
        val->CloseFd();
    }
    OHOS::Media::ImageInfo imageInfo;
    val->GetImageInfo(imageInfo);
    auto allocType = val->GetAllocatorType();
    MemoryInfo info = { val->GetByteCount(), g_callingPid, 0, val->GetUniqueId(),
        MEMORY_TYPE::MEM_PIXELMAP, g_callingPid, allocType, imageInfo.pixelFormat
    };

#ifdef ROSEN_OHOS
    if (RSSystemProperties::GetClosePixelMapFdEnabled()) {
        MemoryTrack::Instance().AddPictureRecord(val->GetPixels(), info);
    } else {
        MemoryTrack::Instance().AddPictureRecord(val->GetFd(), info);
    }
#else
    MemoryTrack::Instance().AddPictureRecord(val->GetPixels(), info);
#endif
    if (g_callingPid && allocType != Media::AllocatorType::DMA_ALLOC) {
        auto realSize = allocType == Media::AllocatorType::SHARE_MEM_ALLOC
            ? val->GetByteCount() / 2 // rs only counts half of the SHARE_MEM_ALLOC memory
            : val->GetByteCount();
        MemorySnapshot::Instance().AddCpuMemory(g_callingPid, realSize);
    }
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
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling RectT WriteInt32 failed");
        }
        return flag;
    }
    bool success = parcel.WriteInt32(1) && val->Marshalling(parcel);
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RectT failed");
    }
    return success;
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
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<Drawing::DrawCmdList>& val,
    int32_t recordCmdDepth)
{
    if (!val) {
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling DrawCmdList WriteInt32 failed");
        }
        return flag;
    }
    auto opItemSize = val->GetOpItemSize();
    if (opItemSize > Drawing::MAX_OPITEMSIZE) {
        ROSEN_LOGE("OpItemSize is too large, OpItemSize is %{public}zu", opItemSize);
        return false;
    }
    auto cmdListData = val->GetData();
    bool ret = parcel.WriteInt32(cmdListData.second);

    parcel.WriteInt32(val->GetWidth());
    parcel.WriteInt32(val->GetHeight());

    parcel.WriteBool(val->GetIsCache());
    parcel.WriteBool(val->GetCachedHighContrast());
    parcel.WriteBool(val->GetNoNeedUICaptured());
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

    std::vector<std::shared_ptr<Drawing::ExtendImageNineObject>> objectNineVec;
    uint32_t objectNineSize = val->GetAllImageNineObject(objectNineVec);
    ret &= parcel.WriteUint32(objectNineSize);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList imageNine");
        return ret;
    }
    if (objectNineSize > 0) {
        for (const auto& objectNine : objectNineVec) {
            auto rsNineObject = std::static_pointer_cast<RSExtendImageNineObject>(objectNine);
            ret &= RSMarshallingHelper::Marshalling(parcel, rsNineObject);
            if (!ret) {
                ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList imageNine");
                return ret;
            }
        }
    }

    std::vector<std::shared_ptr<Drawing::ExtendImageLatticeObject>> objectLatticeVec;
    uint32_t objectLatticeSize = val->GetAllImageLatticeObject(objectLatticeVec);
    ret &= parcel.WriteUint32(objectLatticeSize);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList imageLattice");
        return ret;
    }
    if (objectLatticeSize > 0) {
        for (const auto& objectLattice : objectLatticeVec) {
            auto rsLatticeObject = std::static_pointer_cast<RSExtendImageLatticeObject>(objectLattice);
            ret &= RSMarshallingHelper::Marshalling(parcel, rsLatticeObject);
            if (!ret) {
                ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList imageLattice");
                return ret;
            }
        }
    }

    ret &= MarshallingExtendObjectFromDrawCmdList(parcel, val);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList ExtendObject");
        return ret;
    }

#ifdef ROSEN_OHOS
    std::vector<std::shared_ptr<Drawing::SurfaceBufferEntry>> surfaceBufferEntryVec;
    uint32_t surfaceBufferSize = val->GetAllSurfaceBufferEntry(surfaceBufferEntryVec);
    ret = parcel.WriteUint32(surfaceBufferSize);
    if (surfaceBufferSize > 0) {
        uint32_t posEntrySizeInBytes = parcel.GetWritePosition();
        ret &= parcel.WriteUint32(0); // will put total size in bytes of the entry here later
        for (const auto& object : surfaceBufferEntryVec) {
            if (!object) {
                ROSEN_LOGE("RSMarshallingHelper::Marshalling DrawCmdList surfaceBufferVec has null object");
                return false;
            }
            auto surfaceBuffer = object->surfaceBuffer_;
            MessageParcel* parcelSurfaceBuffer =  static_cast<MessageParcel*>(&parcel);
            if (surfaceBuffer) {
                parcel.WriteBool(true);
                WriteSurfaceBufferImpl(
                    *parcelSurfaceBuffer, surfaceBuffer->GetSeqNum(), surfaceBuffer);
            } else {
                parcel.WriteBool(false);
            }
            auto acquireFence = object->acquireFence_;
            if (acquireFence) {
                parcel.WriteBool(true);
                acquireFence->WriteToMessageParcel(*parcelSurfaceBuffer);
            } else {
                parcel.WriteBool(false);
            }
        }
        if (ret) {
            *reinterpret_cast<uint32_t*>(parcel.GetData() + posEntrySizeInBytes) =
                parcel.GetWritePosition() - posEntrySizeInBytes - sizeof(uint32_t);
        }
    }
#endif

    ret &= MarshallingRecordCmdFromDrawCmdList(parcel, val, recordCmdDepth);
    if (!ret) {
        return ret;
    }
    return ret;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::DrawCmdList>& val)
{
    uint32_t opItemCount = 0;
    uint32_t recordCmdCount = 0;
    int32_t recordCmdDepth = 0;
    return SafeUnmarshallingDrawCmdList(parcel, val, &opItemCount, &recordCmdCount, recordCmdDepth);
}

bool RSMarshallingHelper::SafeUnmarshallingDrawCmdList(Parcel& parcel, std::shared_ptr<Drawing::DrawCmdList>& val,
    uint32_t* opItemCount, uint32_t* recordCmdCount, int32_t recordCmdDepth)
{
    int32_t size = parcel.ReadInt32();
    if (size == -1) {
        val = nullptr;
        return true;
    } else if (size < 0) {
        ROSEN_LOGE("unirender: RSMarshallingHelper::Unmarshalling Drawing::DrawCmdList size is invalid!");
        val = nullptr;
        return false;
    }
    int32_t width{0};
    int32_t height{0};
    if (!parcel.ReadInt32(width) || !parcel.ReadInt32(height)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read val failed");
        return false;
    }
    if (size == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Unmarshalling Drawing::DrawCmdList size is 0");
        val = std::make_shared<Drawing::DrawCmdList>(width, height);
        return true;
    }

    bool isCache{false};
    bool cachedHighContrast{false};
    bool noNeedUICaptured{false};
    if (!parcel.ReadBool(isCache) || !parcel.ReadBool(cachedHighContrast) || !parcel.ReadBool(noNeedUICaptured)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read parcel failed");
        return false;
    }

    uint32_t replacedOpListSize{0};
    if (!parcel.ReadUint32(replacedOpListSize)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read replacedOpListSize failed");
        return false;
    }
    uint32_t readableSize = parcel.GetReadableBytes() / (sizeof(uint32_t) * 2);    // 增加IPC异常保护，读取2个uint_32_t
    if (replacedOpListSize > readableSize) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling Drawing readableSize %{public}d less than size %{public}d",
            readableSize, replacedOpListSize);
        val = nullptr;
        return false;
    }
    if (replacedOpListSize > Drawing::MAX_OPITEMSIZE) {
        val = nullptr;
        return false;
    }
    std::vector<std::pair<size_t, size_t>> replacedOpList;
    for (uint32_t i = 0; i < replacedOpListSize; ++i) {
        uint32_t regionPos{0};
        uint32_t replacePos{0};
        if (!parcel.ReadUint32(regionPos) || !parcel.ReadUint32(replacePos)) {
            ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read Pos failed");
            return false;
        }
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
    val->SetNoNeedUICaptured(noNeedUICaptured);
    val->SetReplacedOpList(replacedOpList);

    int32_t imageSize{0};
    if (!parcel.ReadInt32(imageSize)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read imageSize failed");
        return false;
    }
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

    int32_t bitmapSize{0};
    if (!parcel.ReadInt32(bitmapSize)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read bitmapSize failed");
        return false;
    }
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
    uint32_t objectSize{0};
    if (!parcel.ReadUint32(objectSize)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read objectSize failed");
        return false;
    }
    if (objectSize > 0) {
        if (objectSize > Drawing::MAX_OPITEMSIZE) {
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

    uint32_t objectBaseSize{0};
    if (!parcel.ReadUint32(objectBaseSize)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read objectBaseSize failed");
        return false;
    }
    if (objectBaseSize > 0) {
        if (objectBaseSize > Drawing::MAX_OPITEMSIZE) {
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

    uint32_t objectNineSize{0};
    if (!parcel.ReadUint32(objectNineSize)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read objectNineSize failed");
        return false;
    }
    if (objectNineSize > 0) {
        if (objectNineSize > Drawing::MAX_OPITEMSIZE) {
            return false;
        }
        std::vector<std::shared_ptr<Drawing::ExtendImageNineObject>> ObjectNineVec;
        for (uint32_t i = 0; i < objectNineSize; i++) {
            std::shared_ptr<RSExtendImageNineObject> objectNine;
            ret &= RSMarshallingHelper::Unmarshalling(parcel, objectNine);
            if (!ret) {
                ROSEN_LOGE(
                    "unirender: failed RSMarshallingHelper::Unmarshalling DrawCmdList objectNine: %{public}d", i);
                return ret;
            }
            ObjectNineVec.emplace_back(objectNine);
        }
        val->SetupImageNineObject(ObjectNineVec);
    }

    uint32_t objectLatticeSize{0};
    if (!parcel.ReadUint32(objectLatticeSize)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read objectLatticeSize failed");
        return false;
    }
    if (objectLatticeSize > 0) {
        if (objectLatticeSize > Drawing::MAX_OPITEMSIZE) {
            return false;
        }
        std::vector<std::shared_ptr<Drawing::ExtendImageLatticeObject>> ObjectLatticeVec;
        for (uint32_t i = 0; i < objectLatticeSize; i++) {
            std::shared_ptr<RSExtendImageLatticeObject> objectLattice;
            ret &= RSMarshallingHelper::Unmarshalling(parcel, objectLattice);
            if (!ret) {
                ROSEN_LOGE(
                    "unirender: failed RSMarshallingHelper::Unmarshalling DrawCmdList objectLattice: %{public}d", i);
                return ret;
            }
            ObjectLatticeVec.emplace_back(objectLattice);
        }
        val->SetupImageLatticeObject(ObjectLatticeVec);
    }

    ret &= UnmarshallingExtendObjectToDrawCmdList(parcel, val);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList ExtendObject");
        return ret;
    }

#ifdef ROSEN_OHOS
    uint32_t surfaceBufferEntrySize{0};
    if (!parcel.ReadUint32(surfaceBufferEntrySize)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read surfaceBufferEntrySize failed");
        return false;
    }
    if (surfaceBufferEntrySize > 0) {
        if (surfaceBufferEntrySize > Drawing::MAX_OPITEMSIZE) {
            return false;
        }
        uint32_t surfaceBufferEntrySizeInBytes = 0;
        if (!parcel.ReadUint32(surfaceBufferEntrySizeInBytes)) {
            ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read surfaceBufferEntrySize failed");
            return false;
        }
        if (RS_PROFILER_IF_NEED_TO_SKIP_DRAWCMD_SURFACE(parcel, surfaceBufferEntrySizeInBytes)) {
            surfaceBufferEntrySize = 0;
        }
    }

    if (surfaceBufferEntrySize > 0) {
        auto readSafeFdFunc = [](Parcel& parcel, std::function<int(Parcel&)> readFdDefaultFunc) -> int {
            return AshmemFdContainer::Instance().ReadSafeFd(parcel, readFdDefaultFunc);
        };
        std::vector<std::shared_ptr<Drawing::SurfaceBufferEntry>> surfaceBufferEntryVec;
        for (uint32_t i = 0; i < surfaceBufferEntrySize; ++i) {
            sptr<SurfaceBuffer> surfaceBuffer = nullptr;
            bool hasSurfaceBuffer{false};
            if (!parcel.ReadBool(hasSurfaceBuffer)) {
                ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read hasSurfaceBuffer failed");
                return false;
            }
            MessageParcel* parcelSurfaceBuffer = static_cast<MessageParcel*>(&parcel);
            if (hasSurfaceBuffer) {
                uint32_t sequence = 0U;
                GSError retCode = ReadSurfaceBufferImpl(*parcelSurfaceBuffer, sequence, surfaceBuffer, readSafeFdFunc);
                if (retCode != GSERROR_OK) {
                    ROSEN_LOGE("RSMarshallingHelper::Unmarshalling "
                        "DrawCmdList failed read surfaceBuffer: %{public}d %{public}d", i, retCode);
                    return false;
                }
            }
            sptr<SyncFence> acquireFence = nullptr;
            bool hasAcquireFence{false};
            if (!parcel.ReadBool(hasAcquireFence)) {
                ROSEN_LOGE("RSMarshallingHelper::Unmarshalling DrawCmdList Read hasAcquireFence failed");
                return false;
            }
            if (hasAcquireFence) {
                acquireFence = SyncFence::ReadFromMessageParcel(*parcelSurfaceBuffer, readSafeFdFunc);
            }
            std::shared_ptr<Drawing::SurfaceBufferEntry> surfaceBufferEntry =
                std::make_shared<Drawing::SurfaceBufferEntry>(surfaceBuffer, acquireFence);
            surfaceBufferEntryVec.emplace_back(surfaceBufferEntry);
        }
        val->SetupSurfaceBufferEntry(surfaceBufferEntryVec);
    }
#endif

    ret &= UnmarshallingRecordCmdToDrawCmdList(parcel, val, opItemCount, recordCmdCount, recordCmdDepth);
    if (!ret) { // Attention: don't add log here.
        return ret;
    }
    RS_PROFILER_PATCH_TYPEFACE_ID(parcel, val);
    val->SetIsReplayMode(RS_PROFILER_IS_PARCEL_MOCK(parcel));
    val->UnmarshallingDrawOps(opItemCount);
    if (opItemCount && (*opItemCount) > Drawing::MAX_OPITEMSIZE) {
        return false;
    }
    return true;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<Drawing::RecordCmd>& val,
    int32_t recordCmdDepth)
{
    if (++recordCmdDepth > Drawing::RECORD_CMD_MAX_DEPTH) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RecordCmd failed, "
            "exceed max depth, cur depth[%{public}d]", recordCmdDepth);
        return false;
    }
    if (!val) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling failed, RecordCmd is nullptr");
        return false;
    }
    const auto& rect = val->GetCullRect();
    bool success = parcel.WriteFloat(rect.GetLeft()) && parcel.WriteFloat(rect.GetTop()) &&
        parcel.WriteFloat(rect.GetRight()) && parcel.WriteFloat(rect.GetBottom());
    if (!success) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling RecordCmd failed");
        return success;
    }
    return RSMarshallingHelper::Marshalling(parcel, val->GetDrawCmdList(), recordCmdDepth);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::RecordCmd>& val,
    uint32_t* opItemCount, uint32_t* recordCmdCount, int32_t recordCmdDepth)
{
    if (++recordCmdDepth > Drawing::RECORD_CMD_MAX_DEPTH) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling RecordCmd failed, "
            "exceed max depth, cur depth[%{public}d]", recordCmdDepth);
        return false;
    }

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
    success = RSMarshallingHelper::SafeUnmarshallingDrawCmdList(parcel, drawCmdList,
        opItemCount, recordCmdCount, recordCmdDepth);
    if (!success) { // Attention: don't add log here.
        return false;
    }
    Drawing::Rect rect(left, top, right, bottom);
    val = std::make_shared<Drawing::RecordCmd>(drawCmdList, rect);
    return true;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSExtendImageObject>& val)
{
    if (!val) {
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling RSExtendImageObject WriteInt32 failed");
        }
        return flag;
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
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling RSExtendImageBaseObj WriteInt32 failed");
        }
        return flag;
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

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSExtendImageNineObject>& val)
{
    if (!val) {
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling RSExtendImageNineObject WriteInt32 failed");
        }
        return flag;
    }
    if (!(parcel.WriteInt32(1) && val->Marshalling(parcel))) {
        ROSEN_LOGE("failed RSMarshallingHelper::Marshalling ImageNineObject");
        return false;
    }

    return true;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSExtendImageNineObject>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(RSExtendImageNineObject::Unmarshalling(parcel));
    if (val == nullptr) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling ImageNineObject");
        return false;
    }

    return true;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSExtendImageLatticeObject>& val)
{
    if (!val) {
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling RSExtendImageLatticeObject WriteInt32 failed");
        }
        return flag;
    }
    if (!(parcel.WriteInt32(1) && val->Marshalling(parcel))) {
        ROSEN_LOGE("failed RSMarshallingHelper::Marshalling ImageLatticeObject");
        return false;
    }

    return true;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSExtendImageLatticeObject>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(RSExtendImageLatticeObject::Unmarshalling(parcel));
    if (val == nullptr) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling ImageLatticeObject");
        return false;
    }

    return true;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<Drawing::MaskCmdList>& val)
{
    if (!val) {
        bool flag = parcel.WriteInt32(-1);
        if (!flag) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling MaskCmdList WriteInt32 failed");
        }
        return flag;
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

// PixelMapInfo
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const PixelMapInfo& val)
{
    if (!Marshalling(parcel, val.pixelMap)) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling Marshalling pixelMap failed");
        return false;
    }

    bool marshallingSucc = true;
    marshallingSucc &= parcel.WriteInt32(val.location.x);
    marshallingSucc &= parcel.WriteInt32(val.location.y);
    marshallingSucc &= parcel.WriteInt32(val.location.width);
    marshallingSucc &= parcel.WriteInt32(val.location.height);
    marshallingSucc &= parcel.WriteInt32(val.location.z);

    if (!marshallingSucc) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteLocation failed");
        return false;
    }
    marshallingSucc &= parcel.WriteString(val.nodeName);
    if (!marshallingSucc) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling WriteString failed");
        return false;
    }
    return marshallingSucc;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, PixelMapInfo& val)
{
    if (!Unmarshalling(parcel, val.pixelMap)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling Unmarshalling pixelMap failed");
        return false;
    }

    bool unMarshallingSucc = true;
    unMarshallingSucc &= parcel.ReadInt32(val.location.x);
    unMarshallingSucc &= parcel.ReadInt32(val.location.y);
    unMarshallingSucc &= parcel.ReadInt32(val.location.width);
    unMarshallingSucc &= parcel.ReadInt32(val.location.height);
    unMarshallingSucc &= parcel.ReadInt32(val.location.z);

    if (!unMarshallingSucc) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling ReadLocation failed");
        return false;
    }
    unMarshallingSucc &= parcel.ReadString(val.nodeName);
    if (!unMarshallingSucc) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling ReadString failed");
        return false;
    }
    return unMarshallingSucc;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::MaskCmdList>& val)
{
    int32_t size{0};
    if (!parcel.ReadInt32(size)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling MaskCmdList Read size failed");
        return false;
    }
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
    int32_t imageSize{0};
    if (!parcel.ReadInt32(imageSize)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling MaskCmdList Read imageSize failed");
        return false;
    }
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
    int32_t bitmapSize{0};
    if (!parcel.ReadInt32(bitmapSize)) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling MaskCmdList Read bitmapSize failed");
        return false;
    }
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

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<ModifierNG::RSRenderModifier>& val)
{
    return val != nullptr && val->Marshalling(parcel);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<ModifierNG::RSRenderModifier>& val)
{
    val.reset(ModifierNG::RSRenderModifier::Unmarshalling(parcel));
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
        if (!RSMarshallingHelper::UnmarshallingPidPlusId(parcel, id)) {                            \
            ROSEN_LOGE("RSMarshallingHelper::Unmarshalling TEMPLATE<T> Read id failed");           \
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

#define BATCH_EXPLICIT_INSTANTIATION(TEMPLATE)                                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, bool)                                         \
    EXPLICIT_INSTANTIATION(TEMPLATE, float)                                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, int)                                          \
    EXPLICIT_INSTANTIATION(TEMPLATE, Color)                                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, RSWaterRipplePara)                            \
    EXPLICIT_INSTANTIATION(TEMPLATE, RSFlyOutPara)                                 \
    EXPLICIT_INSTANTIATION(TEMPLATE, Gravity)                                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, ForegroundColorStrategyType)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Matrix3f)                                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Quaternion)                                   \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSRenderFilter>)              \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSNGRenderFilterBase>)        \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSNGRenderMaskBase>)          \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSNGRenderShaderBase>)        \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSImage>)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSMask>)                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSPath>)                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSShader>)                    \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSLinearGradientBlurPara>)    \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<MotionBlurParam>)             \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSMagnifierParams>)           \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::vector<std::shared_ptr<EmitterUpdater>>) \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<ParticleNoiseFields>)         \
    EXPLICIT_INSTANTIATION(TEMPLATE, RSRenderParticleVector)                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector2f)                                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector3f)                                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<uint32_t>)                            \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<Color>)                               \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f)                                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::vector<float>)                           \
<<<<<<< HEAD
    EXPLICIT_INSTANTIATION(TEMPLATE, std::vector<Vector2f>)                        \
=======
    EXPLICIT_INSTANTIATION(TEMPLATE, std::vector<Vector2f>)                        \ 
>>>>>>> 24b779bec310a1323c3d53a22693c1b98a67bf79
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<Media::PixelMap>)             \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<Drawing::DrawCmdList>)        \
    EXPLICIT_INSTANTIATION(TEMPLATE, Drawing::Matrix)

BATCH_EXPLICIT_INSTANTIATION(RSRenderProperty)

#undef EXPLICIT_INSTANTIATION
#undef BATCH_EXPLICIT_INSTANTIATION

#define EXPLICIT_INSTANTIATION(TEMPLATE, TYPE)                                                                  \
    template bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<TYPE>>& val); \
    template bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<TYPE>>& val);

#define BATCH_EXPLICIT_INSTANTIATION(TEMPLATE)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, float)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Color)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Matrix3f)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Quaternion)                \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector2f)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector3f)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<Color>)            \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::vector<float>)        \
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
        ROSEN_LOGE("RSMarshallingHelper::WriteToParcel WriteUint32 failed");
        return false;
    }
    if (size < MIN_DATA_SIZE || (!g_useSharedMem && g_tid == std::this_thread::get_id())) {
        if (!parcel.WriteUnpadBuffer(data, size)) {
            return false;
        }
        return true;
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
    uint32_t bufferSize{0};
    if (!parcel.ReadUint32(bufferSize)) {
        ROSEN_LOGE("RSMarshallingHelper::ReadFromParcel Read bufferSize failed");
        return nullptr;
    }
    if (static_cast<unsigned int>(bufferSize) != size) {
        ROSEN_LOGE("RSMarshallingHelper::ReadFromParcel size mismatch");
        return nullptr;
    }
    if (static_cast<unsigned int>(bufferSize) < MIN_DATA_SIZE  ||
        (!g_useSharedMem && g_tid == std::this_thread::get_id())) {
        isMalloc = false;
        return parcel.ReadUnpadBuffer(size);
    }
    // read from ashmem flow control begins
    bool success = MemoryFlowControl::Instance().AddAshmemStatistic(g_callingPid, bufferSize);
    if (!success) {
        // discard this ashmem parcel since callingPid is submitting too many data to RS simultaneously
        isMalloc = false;
        RS_TRACE_NAME_FMT("RSMarshallingHelper::ReadFromParcel reject ashmem buffer size %" PRIu32
            " from pid %d", bufferSize, static_cast<int>(g_callingPid));
        ROSEN_LOGE("RSMarshallingHelper::ReadFromParcel reject ashmem buffer size %{public}" PRIu32
            " from pid %{public}d", bufferSize, static_cast<int>(g_callingPid));
        return nullptr;
    }
    // read from ashmem
    const void* data = RS_PROFILER_READ_PARCEL_DATA(parcel, size, isMalloc);
    // read from ashmem flow control ends
    MemoryFlowControl::Instance().RemoveAshmemStatistic(g_callingPid, bufferSize);
    return data;
}

bool RSMarshallingHelper::SkipFromParcel(Parcel& parcel, size_t size)
{
    int32_t bufferSize{0};
    if (!parcel.ReadInt32(bufferSize)) {
        ROSEN_LOGE("RSMarshallingHelper::SkipFromParcel Read bufferSize failed");
        return false;
    }
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
    auto readFdDefaultFunc = [](Parcel& parcel) -> int {
        return static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
    };
    int fd = AshmemFdContainer::Instance().ReadSafeFd(parcel, readFdDefaultFunc);
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocatorWithFd(fd, size, PROT_READ);
    return ashmemAllocator != nullptr;
}

const void* RSMarshallingHelper::ReadFromAshmem(Parcel& parcel, size_t size, bool& isMalloc)
{
    isMalloc = false;
    auto readFdDefaultFunc = [](Parcel& parcel) -> int {
        return static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
    };
    int fd = AshmemFdContainer::Instance().ReadSafeFd(parcel, readFdDefaultFunc);
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
    std::unique_lock<std::mutex> lock(g_writeMutex);
    g_useSharedMem = false;
    g_tid = tid;
}

void RSMarshallingHelper::EndNoSharedMem()
{
    std::unique_lock<std::mutex> lock(g_writeMutex);
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
    uint32_t positionRead{0};
    if (!parcel.ReadUint32(positionRead)) {
        ROSEN_LOGE("RSMarshallingHelper::CheckReadPosition Read positionRead failed");
        return false;
    }
    if (positionRead != static_cast<uint32_t>(curPosition)) {
        RS_LOGE("RSMarshallingHelper::CheckReadPosition failed, curPosition:%{public}zu, positionRead:%{public}u",
            curPosition, positionRead);
        return false;
    }
    return true;
}

void RSMarshallingHelper::SetCallingPid(pid_t callingPid)
{
    g_callingPid = callingPid;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSRenderPropertyBase>& val)
{
    if (val == nullptr) {
        return Marshalling(parcel, RSPropertyType::INVALID);
    }
    return val->Marshalling(parcel);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSRenderPropertyBase>& val)
{
    return RSRenderPropertyBase::Unmarshalling(parcel, val);
}

bool RSMarshallingHelper::UnmarshallingPidPlusId(Parcel& parcel, uint64_t& val)
{
    uint64_t retCode = 0;
    retCode = RSMarshallingHelper::Unmarshalling(parcel, val);
    if (retCode) {
        val = RS_PROFILER_PATCH_NODE_ID(parcel, val);
    }
    return retCode;
}

bool RSMarshallingHelper::UnmarshallingPidPlusIdNoChangeIfZero(Parcel& parcel, uint64_t& val)
{
    uint64_t retCode = 0;
    retCode = RSMarshallingHelper::Unmarshalling(parcel, val);
    if (retCode && val) {
        val = RS_PROFILER_PATCH_NODE_ID(parcel, val);
    }
    return retCode;
}

bool RSMarshallingHelper::MarshallingTransactionVer(Parcel& parcel)
{
    parcel.WriteInt64(-1);
    uint64_t flags[NUM_ITEMS_IN_VERSION] = { 0 };
    constexpr uint8_t bitsPerUint64 = 64;
    for (auto supportedFlag : supportedParcelVerFlags) {
        flags[supportedFlag / bitsPerUint64] |= static_cast<uint64_t>(1) << (supportedFlag % bitsPerUint64);
    }
    for (int i = 0; i < NUM_ITEMS_IN_VERSION; i++) {
        parcel.WriteUint64(flags[i]);
    }
    return true;
}

bool RSMarshallingHelper::UnmarshallingTransactionVer(Parcel& parcel)
{
    int64_t headerCode;
    size_t offset = parcel.GetReadPosition();
    headerCode = parcel.ReadInt64();
    if (headerCode == -1) {
        for (int i = 0; i < NUM_ITEMS_IN_VERSION; i++) {
            parcel.ReadUint64();
        }
    } else {
        parcel.RewindRead(offset);
    }
    return true;
}

bool RSMarshallingHelper::TransactionVersionCheck(Parcel& parcel, uint8_t supportedFlag)
{
    constexpr size_t startPositionOffset = 4;

    size_t offset = parcel.GetReadPosition();
    parcel.RewindRead(startPositionOffset);
    int64_t headerCode = parcel.ReadInt64();
    if (headerCode == -1) {
        uint64_t flags = 0;
        constexpr uint8_t bitsPerUint64 = 64;
        int numReads = supportedFlag / bitsPerUint64 + 1;
        for (int i = 0; i < numReads; i++) {
            flags = parcel.ReadUint64();
        }
        parcel.RewindRead(offset);
        return flags & (static_cast<uint64_t>(1) << (supportedFlag % bitsPerUint64));
    }
    parcel.RewindRead(offset);
    return false;
}
} // namespace Rosen
} // namespace OHOS
