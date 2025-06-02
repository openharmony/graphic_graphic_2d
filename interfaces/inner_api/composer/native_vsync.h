/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

/**
 * @addtogroup NativeVsync
 * @{
 *
 * @brief Provides the native vsync capability.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeVsync
 * @since 9
 * @version 1.0
 */

/**
 * @file native_vsync.h
 *
 * @brief Defines the functions for obtaining and using a native vsync.
 *
 * @library libnative_vsync.so
 * @since 9
 * @version 1.0
 */

#ifndef NDK_INCLUDE_NATIVE_VSYNC_H_
#define NDK_INCLUDE_NATIVE_VSYNC_H_

#ifdef __cplusplus
extern "C" {
#endif

struct OH_NativeVSync;
typedef struct OH_NativeVSync OH_NativeVSync;
typedef void (*OH_NativeVSync_FrameCallback)(long long timestamp, void *data);

/**
 * @brief Defines the expected frame rate range struct.
 *
 * @since 20
 * @version 1.0
 */
 typedef struct {
    /**The minimum frame rate of dynamical callback rate range */
    int32_t min;
    /**The maximum frame rate of dynamical callback rate range */
    int32_t max;
    /**The expected frame rate of dynamical callback rate range */
    int32_t expected;
} OH_NativeVSync_ExpectedRateRange;


/**
 * @brief Creates a <b>NativeVsync</b> instance.\n
 * A new <b>NativeVsync</b> instance is created each time this function is called.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeVsync
 * @param name Indicates the vsync connection name.
 * @param length Indicates the name's length.
 * @return Returns the pointer to the <b>NativeVsync</b> instance created.
 * @since 9
 * @version 1.0
 */
OH_NativeVSync* OH_NativeVSync_Create(const char* name, unsigned int length);

/**
 * @brief Creates a associated with Xcomponentid <b>NativeVsync</b> instance.\n
 * A new associated with Xcomponentid<b>NativeVsync</b> instance is created each time this function is called.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeVsync
 * @param windowID Indicates the Xcomponentid of the associated window.
 * @param name Indicates the vsync connection name.
 * @param length Indicates the name's length.
 * @return Returns the pointer to the <b>NativeVsync</b> instance created.
 * @since 14
 * @version 1.0
 */
OH_NativeVSync* OH_NativeVSync_Create_ForAssociatedWindow(uint64_t windowID, const char* name, unsigned int length);

/**
 * @brief Delete the NativeVsync instance.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeVsync
 * @param nativeVsync Indicates the pointer to a <b>NativeVsync</b> instance.
 * @since 9
 * @version 1.0
 */
void OH_NativeVSync_Destroy(OH_NativeVSync* nativeVsync);

/**
 * @brief Request next vsync with callback.
 * If you call this Interface multi times in one frame, it will only call the last callback.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeVsync
 * @param nativeVsync Indicates the pointer to a NativeVsync.
 * @param callback Indicates the OH_NativeVSync_FrameCallback which will be called when next vsync coming.
 * @param data Indicates data which will be used in callback.
 * @return {@link NATIVE_ERROR_OK} 0 - Success.
 *     {@link NATIVE_ERROR_INVALID_ARGUMENTS} 40001000 - the parameter nativeVsync is NULL or callback is NULL.
 *     {@link NATIVE_ERROR_BINDER_ERROR} 50401000 - ipc send failed.
 * @since 9
 * @version 1.0
 */
int OH_NativeVSync_RequestFrame(OH_NativeVSync* nativeVsync, OH_NativeVSync_FrameCallback callback, void* data);

/**
 * @brief Request next vsync with callback.
 * If this function is called multiple times in one vsync period, all these callbacks and datas be callbacked.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeVsync
 * @param nativeVsync Indicates the pointer to a NativeVsync.
 * @param callback Indicates the OH_NativeVSync_FrameCallback which will be called when next vsync coming.
 * @param data Indicates data which will be used in callback.
 * @return {@link NATIVE_ERROR_OK} 0 - Success.
 *     {@link NATIVE_ERROR_INVALID_ARGUMENTS} 40001000 - the parameter nativeVsync is NULL or callback is NULL.
 *     {@link NATIVE_ERROR_BINDER_ERROR} 50401000 - ipc send failed.
 * @since 12
 * @version 1.0
 */
int OH_NativeVSync_RequestFrameWithMultiCallback(
    OH_NativeVSync* nativeVsync, OH_NativeVSync_FrameCallback callback, void* data);

/**
 * @brief Get vsync period.
 * This interface uses a prerequisite that the OH_NativeVSync object in the input parameter
 * has called OH_NativeVSync_RequestFrame() and received a signal callback.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeVsync
 * @param nativeVsync Indicates the pointer to a NativeVsync.
 * @param period Indicates the vsync period.
 * @return Returns int32_t, return value == 0, success, otherwise, failed.
 * @since 10
 * @version 1.0
 */
int OH_NativeVSync_GetPeriod(OH_NativeVSync* nativeVsync, long long* period);

/**
 * @brief Enables DVSync to improve the smoothness of self-drawing animations.
 * DVSync, short for Decoupled VSync, is a frame timing management policy that is decoupled from the hardware's VSync.
 * DVSync drives the early rendering of upcoming animation frames by sending VSync signals with future timestamps.
 * These frames are stored in a frame buffer queue. This helps DVSync reduce potential frame drop and therefore
 * enhances the smoothness of animations.
 * DVSync requires free self-drawing frame buffers to store these pre-rendered animation frames.
 * Therefore, you must ensure that at least one free frame buffer is available. Otherwise, do not enable DVSync.
 * After DVSync is enabled, you must correctly respond to the early VSync signals and request the subsequent VSync
 * after the animation frame associated with the previous VSync is complete. In addition, the self-drawing frames must
 * carry timestamps that align with VSync.
 * After the animation ends, disable DVSync.
 * Only phones and tablets support DVSync.
 * On a platform that does not support DVSync or if another application has enabled DVSync, the attempt to enable it
 * will not take effect, and the application still receives normal VSync signals.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeVsync
 * @param nativeVsync Indicates the pointer to a NativeVsync.
 * @param enable Whether to enable DVSync.The value true means to enable DVSync, and false means the opposite.
 * @return {@link NATIVE_ERROR_OK} 0 - Success.
 *     {@link NATIVE_ERROR_INVALID_ARGUMENTS} 40001000 - the parameter nativeVsync is NULL.
 *     {@link NATIVE_ERROR_BINDER_ERROR} 50401000 - ipc send failed.
 * @since 14
 * @version 1.0
 */
int OH_NativeVSync_DVSyncSwitch(OH_NativeVSync* nativeVsync, bool enable);

/**
 * @brief Set Vsync expected frame rate range.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeVsync
 * @param nativeVsync Indicates the pointer to a NativeVsync
 * @param range Indicates the pointer to an expected rate range.
 *              Valid range is 0 <= min <= expected <= max <= 144.
 *              expected == 0 indicates cancle vote.
 *              This pointer needs to be released by developer.
 * @return {@link NATIVE_ERROR_OK} 0 - Success.
 *      {@link NATIVE_ERROR_INVALID_ARGUMENTS} 40001000 - the parameter is NULL or range is invalid.
 *      {@link NATIVE_ERROR_NOT_SUPPORT} 50102000 -the object nativeVSync does not support this interface
 *                                                 Consider using other methods to create nativeVSync instead.
 * @since 20
 * @version 1.0
 */
int OH_NativeVSync_SetExpectedFrameRateRange(OH_NativeVSync* nativeVsync, OH_NativeVSync_ExpectedRateRange *range);
#ifdef __cplusplus
}
#endif

/** @} */
#endif