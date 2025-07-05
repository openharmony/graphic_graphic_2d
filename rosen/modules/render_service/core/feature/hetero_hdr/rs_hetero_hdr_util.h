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

#ifndef RS_HETERO_HDR_UTIL_H
#define RS_HETERO_HDR_UTIL_H

#include <buffer_handle.h>
#include <condition_variable>
#include <cstdint>
#include <dlfcn.h>
#include <set>
#include <unordered_map>
#include <vector>

#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "rs_trace.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

namespace OHOS::Rosen {
#define DVFS_LEVEL_MIDDLE 3
#define DVFS_LEVEL_HIGH 4

// for scene id capbility bit0 ~ bit55
constexpr uint64_t MDC_CAP_HEBCE = 1;
constexpr uint64_t MDC_CAP_UVUP = 1 << 1;
constexpr uint64_t MDC_CAP_SCALE = 1 << 2;
constexpr uint64_t MDC_CAP_ROT = 1 << 3;
constexpr uint64_t MDC_CAP_HDR = 1 << 4;

constexpr uint64_t EFFECT_RESOURCE_HDR_BIT = 5;

/* Rectangle */
using MdcRectT = struct MdcRect {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
};

using MdcContentsT = struct MdcCopybitContents {
    /*
     * Request Direct Mdc Channel
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
     * Request Direct Mdc Channel
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
    uint32_t secure;

    /*
     * Transformation to apply to the buffer during copybit.
     * Reference system/core/include/system/window.h defined
     * Set default value: 0
     *        90 degrees: NATIVE_WINDOW_TRANSFORM_ROT_90
     */
    uint32_t transform;

    /*
     * This is the information of the source buffer to copybit. This handle
     * is guaranteed to have been allocated from gralloc by caller.
     * The width, height and format have been set.
     */
    union {
        BufferHandle *srcBufferHandle;

        BufferHandle *srcHandle;

        uint64_t srcHandlePadding;
    };

    /*
     * This is the information of the destination buffer to copybit. This handle
     * is guaranteed to have been allocated from gralloc by caller.
     * The width, height and format have been set.
     */
    union {
        BufferHandle *dstBufferHandle;

        BufferHandle *dstHandle;

        uint64_t dstHandlePadding;
    };

    /*
     * Area of the source to consider, the origin is the top-left corner of
     * the buffer.
     */
    MdcRectT srcRect;

    /* where to copybit the source rect onto the display. The sourceCrop
     * is scaled using linear filtering to the displayFrame. The origin is the
     */
    MdcRectT dstRect;

    /*
     * Sync fence object that will be signaled when the buffer's
     * contents are available. May be -1 if the contents are already
     * available. but the MDC must wait for the source buffer
     * to be signaled before reading from them.
     */
    int32_t acquireFenceFd;

    /*
     * Sync fence object that will be signaled when the buffer's
     * contents are available. May be -1 if the contents are already
     * available. but the caller must wait for the destination buffer
     * to be signaled before reading from them. The destination buffer will
     * be signaled when MDC copybit operation had been finished.
     */
    int32_t releaseFenceFd;

    /*
     * Request MDC effect resources
     *
     * EFFECT_ARSR = 1ULL << 0,
     * AIHDR_LUT = 1ULL << 1,
     * ROUNDED_CORNER = 1ULL << 2,
     * AIHDR_ENHANCE_MODE = 1ULL << 3,
     * AIHDR_HIGHLIGHT_MODE = 1ULL << 4,
     */
    uint64_t effectResourceRequest = 0;

    /*
    * MDC support async & sync ffts task
    */
    bool isAsyncTask = false;

    /*
    * taskId For async mode to destroy cmdlist
    */
    uint32_t *taskId;

    /*
    * taskPtr(cmdlistHeader) For async mode to submit task
    */
    void **taskPtr;

    int32_t expectRunTime = -1;

    /*
    * accept nit from rs
    */
    float displaySdrNit;
    float displayHdrNit;
};

/*
 * Support set more prelayers
 * for aihdr lut layer
 * for cld top bottom layers.
 */
struct mdcLayerInfoT {
    uint32_t transform = 0;
    uint64_t effectResourceRequest = 0;
    MdcRectT srcRect;
    MdcRectT dstRect;
    BufferHandle *handle;
};

/*
 * Every device data structure must begin with hw_device_t
 * followed by module specific public methods and attributes.
 */
struct MdcDeviceT {
    int (* copybit)(struct MdcDeviceT *dev, int channel, MdcContentsT *hwLayers);
    int (* requestPowerMode) (struct MdcDeviceT *dev, int powerMode);
    int (* requestVoltaLev) (struct MdcDeviceT *dev, int voltaLev);
    int (* requestCoreClkLev) (struct MdcDeviceT *dev, int coreClkLev);
    int (* requestChannel) (struct MdcDeviceT *dev);
    int (* releaseChannel) (struct MdcDeviceT *dev, int channel);
    int (* requestChannelByCap) (struct MdcDeviceT *dev, uint64_t needCaps);
    void (* dump) (struct MdcDeviceT *dev);
    int (* setMultiPreLayers) (std::vector<mdcLayerInfoT> &preLayers);
    void (* destroyTask) (uint32_t taskId);
};

struct hapeTaskInfoT {
    void** taskPtr;
    uint32_t* taskId;
    MdcRectT srcRect;
    MdcRectT dstRect;
    int transform = 0;
    BufferHandle* srcHandle;
    BufferHandle* dstHandle;
    int32_t acquireFenceFd = -1;
    int32_t releaseFenceFd = -1;
    float displaySDRNits = 500.0f;
    float displayHDRNits = 500.0f;
};

class RSHeteroHdrUtil  {
public:
    static RSHeteroHdrUtil  &GetInstance();
    int32_t BuildHpaeHdrTask(hapeTaskInfoT& taskinfo);

    int32_t RequestHpaeChannel();
    void DestroyHpaeHdrTask(uint32_t taskId);
    void ReleaseHpaeHdrChannel();

private:
    RSHeteroHdrUtil();
    ~RSHeteroHdrUtil();
    RSHeteroHdrUtil(const RSHeteroHdrUtil  &);
    RSHeteroHdrUtil(const RSHeteroHdrUtil  &&);
    RSHeteroHdrUtil  &operator = (const RSHeteroHdrUtil  &);
    RSHeteroHdrUtil  &operator = (const RSHeteroHdrUtil  &&);

    void* mdcHandle = nullptr;
    const char* mdcLib = "/vendor/lib64/libmediacomm.z.so";
    MdcDeviceT*(*getMdcDevice)() = nullptr;
    MdcDeviceT* mdcDev = nullptr;
    MdcContentsT mdcContent;
    std::mutex mdcMtx_;
    std::atomic<bool> mdcStatus_{false};
    std::atomic<bool> mdcExistedStatus_{false};
};
}
#endif // RS_HETERO_HDR_UTIL_H
