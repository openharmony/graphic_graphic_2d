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

#ifndef NDK_INCLUDE_EXTERNAL_NATIVE_WINDOW_H_
#define NDK_INCLUDE_EXTERNAL_NATIVE_WINDOW_H_

/**
 * @addtogroup NativeWindow
 * @{
 *
 * @brief 提供NativeWindow功能，主要用来和egl对接
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @since 8
 * @version 1.0
 */

/**
 * @file external_window.h
 *
 * @brief 定义获取和使用NativeWindow的相关函数
 *
 * @since 8
 * @version 1.0
 */

#include "window.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 创建NativeWindow实例，每次调用都会产生一个新的NativeWindow实例
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param pSurface 参数是一个指向生产者ProduceSurface的指针，类型为Surface
 * @return 返回一个指针，指向NativeWindow的结构体实例
 * @since 8
 * @version 1.0
 */
struct NativeWindow* OH_NativeWindow_CreateNativeWindow(void* pSurface);

/**
 * @brief 将NativeWindow对象的引用计数减1，当引用计数为0的时候，该NativeWindow对象会被析构掉
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window 参数是一个NativeWindow的结构体实例的指针
 * @since 8
 * @version 1.0
 */
void OH_NativeWindow_DestroyNativeWindow(struct NativeWindow* window);

/**
 * @brief 创建NativeWindowBuffer实例，每次调用都会产生一个新的NativeWindow实例
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param pSurfaceBuffer 参数是一个指向生产者buffer的指针，类型为SurfaceBuffer
 * @return 返回一个指针，指向NativeWindowBuffer的结构体实例
 * @since 8
 * @version 1.0
 */
struct NativeWindowBuffer* OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(void* pSurfaceBuffer);

/**
 * @brief 将NativeWindowBuffer对象的引用计数减1，当引用计数为0的时候，该NativeWindowBuffer对象会被析构掉
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param buffer 参数是一个NativeWindowBuffer的结构体实例的指针
 * @since 8
 * @version 1.0
 */
void OH_NativeWindow_DestroyNativeWindowBuffer(struct NativeWindowBuffer* buffer);

/**
 * @brief 通过NativeWindow对象申请一块NativeWindowBuffer，用以内容生产
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window 参数是一个NativeWindow的结构体实例的指针
 * @param buffer 参数是一个NativeWindowBuffer的结构体实例的二级指针，作为出参传入
 * @param fenceFd 参数是一个文件描述符句柄，作为出参传入
 * @return GSError 返回值为错误码
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowRequestBuffer(struct NativeWindow *window,
    struct NativeWindowBuffer **buffer, int *fenceFd);

/**
 * @brief 通过NativeWindow将生产好内容的NativeWindowBuffer放回到Buffer队列中，用以内容消费
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window 参数是一个NativeWindow的结构体实例的指针
 * @param buffer 参数是一个NativeWindowBuffer的结构体实例的指针
 * @param fenceFd 参数是一个文件描述符句柄，用以同步时序
 * @param region 参数表示一块脏区域，该区域有内容更新
 * @return GSError 返回值为错误码
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowFlushBuffer(struct NativeWindow *window, struct NativeWindowBuffer *buffer,
    int fenceFd, Region region);

/**
 * @brief 通过NativeWindow将之前申请出来的NativeWindowBuffer返还到Buffer队列中，供下次再申请
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window 参数是一个NativeWindow的结构体实例的指针
 * @param buffer 参数是一个NativeWindowBuffer的结构体实例的指针
 * @return GSError 返回值为错误码
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowAbortBuffer(struct NativeWindow *window, struct NativeWindowBuffer *buffer);

/**
 * @brief 设置/获取NativeWindow的属性，包括设置/获取宽高、内容格式等
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param window 参数是一个NativeWindow的结构体实例的指针
 * @param code 操作码
 * @return GSError 返回值为错误码
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeWindowHandleOpt(struct NativeWindow *window, int code, ...);

/**
 * @brief 通过NativeWindowBuffer获取该buffer的BufferHandle指针
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param buffer 参数是一个NativeWindowBuffer的结构体实例的指针
 * @return BufferHandle 返回一个指针，指向BufferHandle的结构体实例
 * @since 8
 * @version 1.0
 */
BufferHandle *OH_NativeWindow_GetBufferHandleFromNative(struct NativeWindowBuffer *buffer);

/**
 * @brief 增加一个NativeObject的引用计数
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param obj 参数是一个NativeWindow or NativeWindowBuffer的结构体实例的指针
 * @return GSError 返回值为错误码
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeObjectReference(void *obj);

/**
 * @brief 减少一个NativeObject的引用计数，当引用计数减少为0时，该NativeObject将被析构掉
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param obj 参数是一个NativeWindow or NativeWindowBuffer的结构体实例的指针
 * @return GSError
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_NativeObjectUnreference(void *obj);

/**
 * @brief 获取NativeObject的MagicId
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeWindow
 * @param obj 参数是一个NativeWindow or NativeWindowBuffer的结构体实例的指针
 * @return MagicId 返回值为魔鬼数字，每个NativeObject唯一
 * @since 8
 * @version 1.0
 */
int32_t OH_NativeWindow_GetNativeObjectMagic(void *obj);

#ifdef __cplusplus
}
#endif

/** @} */
#endif