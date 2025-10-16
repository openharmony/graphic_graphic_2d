/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RS_HETERO_HDR_HPAE_H
#define RS_HETERO_HDR_HPAE_H

#include <condition_variable>
#include <cstdint>
#include <dlfcn.h>
#include <set>
#include <unordered_map>
#include <vector>

#include <buffer_handle.h>

#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "rs_trace.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
/* Rectangle */
struct MDCRectT {
    int32_t left = 0;
    int32_t top = 0;
    int32_t right = 0;
    int32_t bottom = 0;
};

/* Request MDC effect resources */
enum EffectResourceRequest {
    EFFECT_NONE,
    EFFECT_ARSR = 1ULL << 0,
    EFFECT_HDR = 1ULL << 1,
    ROUNDED_CORNER = 1ULL << 2,
    AIHDR_ENHANCE_MODE = 1ULL << 3,
    AIHDR_HIGHLIGHT_MODE = 1ULL << 4,
    AIHDR_ENHANCE_LUT = 1ULL << 5,
    AIHDR_HIGHLIGHT_LUT = 1ULL << 6,
    HDR_3D_LUT = 1ULL << 7,
};

enum GenericDataTag {
    GENERIC_DATA_TAG_NONE = 0,
    HDR_GMP_LUT_LOW = 1,
    HDR_GMP_LUT_HIGH = 2,
};

struct GenericData {
    void *buf;
    size_t size;
    int tag; // GenericDataTag
};

using MDCContentsT = struct MDCCopybitContents {
    /*
     * Request Direct MDC Channel
     *
     * MDC_POWER_UP = 0x1,
     * MDC_POWER_DOWN = 0x2,
     *
     */
    union {
        uint32_t powerMode;
        uint32_t mdcPowerMode;
    };

    /*
     * Request Direct MDC Channel
     *
     * MDC_VOLTAGE_LOW = 0x1,
     * MDC_VOLTAGE_NORMAL = 0x2,
     * MDC_VOLTAGE_HIGH = 0x3,
     */
    union {
        uint32_t mdcVoltaLev;
        uint32_t perfLev;
    };

    /*
     * MUST use copybit to compose,
     * input param: 1 is needed.
     * only for ISP and online play used.
     */
    union {
        uint32_t needHold;
        uint32_t hold;
    };
    /*
     * handle secure buffer(drm),
     * input param: 1 is secure.
     * default 0 is non-secure
     */
    uint32_t secure = 0;

    /*
     * Transformation to apply to the buffer during copybit.
     * Reference system/core/include/system/window.h defined
     * Set default value: 0
     * 90 degrees: NATIVE_WINDOW_TRANSFORM_ROT_90
     */
    uint32_t transform = 0;

    /*
     * This is the information of the source buffer to copybit. This handle
     * is guaranteed to have been allocated from gralloc by caller.
     * The width, height and format have been set.
     */
    union {
        BufferHandle* srcBufferHandle;

        BufferHandle* srcHandle;

        uint64_t srcHandlePadding;
    };

    /*
     * This is the information of the destination buffer to copybit. This handle
     * is guaranteed to have been allocated from gralloc by caller.
     * The width, height and format have been set.
     */
    union {
        BufferHandle* dstBufferHandle;

        BufferHandle* dstHandle;

        uint64_t dstHandlePadding;
    };

    /*
     * Area of the source to consider, the origin is the top-left corner of
     * the buffer.
     */
    MDCRectT srcRect = {0, 0, 0, 0 };

    /* where to copybit the source rect onto the display. */
    MDCRectT dstRect = {0, 0, 0, 0 };

    /*
     * Sync fence object that will be signaled when the buffer's
     * contents are available. May be -1 if the contents are already
     * available. but the MDC must wait for the source buffer
     * to be signaled before reading from them.
     */
    int32_t acquireFenceFd = -1;

    /*
     * Sync fence object that will be signaled when the buffer's
     * contents are available. May be -1 if the contents are already
     * available. but the caller must wait for the destination buffer
     * to be signaled before reading from them. The destination buffer will
     * be signaled when MDC copybit operation had been finished.
     */
    int32_t releaseFenceFd = -1;

    /* Refer to the definition of the EffectResourceRequest enumeration values. */
    uint64_t effectResourceRequest = 0;

    /*
    * MDC support async & sync ffts task
    */
    bool isAsyncTask = false;

    /*
    * taskId For async mode to destroy cmdlist
    */
    uint32_t* taskId = nullptr;

    /*
    * taskPtr(cmdlistHeader) For async mode to submit task
    */
    void** taskPtr = nullptr;

    int32_t expectRunTime = -1;

    /*
    * accept nit from rs
    */
    float displaySdrNit = 500.0f;
    float displayHdrNit = 500.0f;

    /*
    * stylize, user: Camera
    */
    uint32_t dataCnt = 0;
    struct GenericData *genericData = nullptr;
};

/*
 * Support set more prelayers
 * for AIHDR lut layer
 * for cld top bottom layers.
 */
struct MDCLayerInfoT {
    uint32_t transform = 0;
    uint64_t effectResourceRequest = 0;
    MDCRectT srcRect;
    MDCRectT dstRect;
    BufferHandle* handle;
};

/*
 * Every device data structure must begin with hw_device_t
 * followed by module specific public methods and attributes.
 */
struct MDCDeviceT {
    int (*copybit)(struct MDCDeviceT* dev, int channel, MDCContentsT* hwLayers);
    int (*requestPowerMode)(struct MDCDeviceT* dev, int powerMode);
    int (*requestVoltaLev)(struct MDCDeviceT* dev, int voltaLev);
    int (*requestCoreClkLev)(struct MDCDeviceT* dev, int coreClkLev);
    int (*requestChannel)(struct MDCDeviceT* dev);
    int (*releaseChannel)(struct MDCDeviceT* dev, int channel);
    int (*requestChannelByCap)(struct MDCDeviceT* dev, uint64_t needCaps);
    bool (*checkResourceConflict) (struct MDCDeviceT* dev, uint64_t needCaps);
    void (*dump)(struct MDCDeviceT* dev);
    int (*setMultiPreLayers)(std::vector<MDCLayerInfoT>& preLayers);
    void (*destroyTask)(uint32_t taskId);
};

struct HpaeTaskInfoT {
    void** taskPtr = nullptr;
    uint32_t* taskId = nullptr;
    MDCRectT srcRect = { 0, 0, 0, 0 };
    MDCRectT dstRect = { 0, 0, 0, 0 };
    uint32_t transform = 0;
    BufferHandle* srcHandle = nullptr;
    BufferHandle* dstHandle = nullptr;
    int32_t acquireFenceFd = -1;
    int32_t releaseFenceFd = -1;
    float displaySdrNit = 500.0f; // default SDR 500 nit
    float displayHdrNit = 500.0f; // default HDR 500 nit
    HdrStatus curHandleStatus = HdrStatus::NO_HDR;
};

class RSHeteroHDRHpae {
public:
    static RSHeteroHDRHpae& GetInstance();
    bool IsHpaeAvailable() const;
    int32_t BuildHpaeHDRTask(HpaeTaskInfoT& taskInfo);
    int32_t RequestHpaeChannel(HdrStatus curHandleHdrStatus);
    bool CheckHpaeAccessible(HdrStatus curHandleHdrStatus);
    void DestroyHpaeHDRTask(uint32_t taskId);
    void ReleaseHpaeHDRChannel();
private:
    RSHeteroHDRHpae();
    ~RSHeteroHDRHpae();
    RSHeteroHDRHpae(const RSHeteroHDRHpae&) = delete;
    RSHeteroHDRHpae(const RSHeteroHDRHpae&&) = delete;
    RSHeteroHDRHpae& operator=(const RSHeteroHDRHpae&) = delete;
    RSHeteroHDRHpae& operator=(const RSHeteroHDRHpae&&) = delete;
    void SetEffectResourceRequest(HdrStatus curHandleHdrStatus);
    uint64_t GetChannelCaps(HdrStatus curHandleHdrStatus);

    void* mdcHandle_ = nullptr;
    const char* mdcLib_ = "/vendor/lib64/libmediacomm.z.so";
    MDCDeviceT* (*getMDCDevice)() = nullptr;
    MDCDeviceT* mdcDev_ = nullptr;
    MDCContentsT mdcContent_;
    std::atomic<bool> mdcStatus_{ false };
    std::atomic<bool> mdcExistedStatus_{ false };
    std::atomic<HdrStatus> existedChannelStatus_{ HdrStatus::NO_HDR };
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_HETERO_HDR_HPAE_H