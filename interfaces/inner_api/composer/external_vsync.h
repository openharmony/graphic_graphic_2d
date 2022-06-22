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

#ifndef NDK_INCLUDE_EXTERNAL_NATIVE_VSYNC_H_
#define NDK_INCLUDE_EXTERNAL_NATIVE_VSYNC_H_

/**
 * @addtogroup NativeVsync
 * @{
 *
 * @brief Provides the native vsync capability.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeVsync
 * @since 8
 * @version 1.0
 */

/**
 * @file external_vsync.h
 *
 * @brief Defines the functions for obtaining and using a native vsync.
 *
 * @since 8
 * @version 1.0
 */

#include <stdint.h>
#include "native_vsync.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a <b>NativeVsync</b> instance. A new <b>NativeVsync</b> instance is created each time this function is called.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeVsync
 * @param name Indicates the vsync connection name.
 * @param length Indicates the name's length.
 * @return Returns the pointer to the <b>NativeVsync</b> instance created.
 * @since 8
 * @version 1.0
 */
OHNativeVSync* OH_NativeVSync_CreateOHNativeVSync(const char* name, unsigned int length);

/**
 * @brief Delete the NativeVsync instance.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeVsync
 * @param window Indicates the pointer to a <b>NativeVsync</b> instance.
 * @return Returns int32_t, return value == 0, success, value < 0, failed.
 * @since 8
 * @version 1.0
 */
void OH_NativeVSync_DestroyOHNativeVSync(OHNativeVSync* nativeVsync);

/**
 * @brief Request next vsync with callback.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeVsync
 * @param nativeVsync Indicates the pointer to a NativeVsync.
 * @param callback Indicates the OHNativeVSyncFrameCallback which will be called when next vsync coming.
 * @param data Indicates data whick will be used in callback.
 * @return Returns int32_t, return value == 0, success, value < 0, failed.
 * @since 8
 * @version 1.0
 */
int OH_NativeVSync_OHNativeVSyncRequestFrame(OHNativeVSync* nativeVsync, OHNativeVSyncFrameCallback callback, void* data);
#ifdef __cplusplus
}
#endif

/** @} */
#endif