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

#ifndef NDK_INCLUDE_NATIVE_VSYNC_H_
#define NDK_INCLUDE_NATIVE_VSYNC_H_

#ifdef __cplusplus
extern "C" {
#endif

struct OHNativeVSync;
typedef struct OHNativeVSync OHNativeVSync;
typedef void (*OHNativeVSyncFrameCallback)(long long timestamp, void *data);

OHNativeVSync* CreateOHNativeVSync(const char* name, unsigned int length);
void DestroyOHNativeVSync(OHNativeVSync *nativeVSync);
int OHNativeVSyncRequestFrame(OHNativeVSync *nativeVSync, OHNativeVSyncFrameCallback callback, void* data);

#ifdef __cplusplus
}
#endif

#endif