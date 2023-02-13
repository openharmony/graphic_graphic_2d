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

/**
 * @file flat_obj.h
 *
 * @brief Define types and structures for reading and writing object in parcel.
 *
 * Types are platform-sensitive. Structures is only used for checking the
 * validity of data in parcel, not for saving and using of data.
 */
#ifndef UTILS_BASE_FLAT_OBJ_H
#define UTILS_BASE_FLAT_OBJ_H

#if defined(__APPLE__) || defined(_WIN32)
#include <cstdint>
#include <sys/types.h>

#ifdef BINDER_IPC_32BIT
    typedef uint32_t binder_size_t;
    typedef uint32_t binder_uintptr_t;
#else
    typedef uint64_t binder_size_t;
    typedef uint64_t binder_uintptr_t;
#endif

struct parcel_binder_object_header {
    uint32_t type;
};
struct parcel_flat_binder_object {
    struct parcel_binder_object_header hdr;
    uint32_t flags;
    union {
        binder_uintptr_t binder;
        uint32_t handle;
    };
    binder_uintptr_t cookie;
};
#else // !_WIN32
#include <sys/types.h>
#include <linux/types.h>

#ifdef BINDER_IPC_32BIT
    typedef __u32 binder_size_t;
    typedef __u32 binder_uintptr_t;
#else
    typedef __u64 binder_size_t;
    typedef __u64 binder_uintptr_t;
#endif

struct parcel_binder_object_header {
    __u32 type;
};
struct parcel_flat_binder_object {
    struct parcel_binder_object_header hdr;
    __u32 flags;
    union {
        binder_uintptr_t binder;
        __u32 handle;
    };
    binder_uintptr_t cookie;
};
#endif // _WIN32

#endif // UTILS_BASE_FLAT_OBJ_H
