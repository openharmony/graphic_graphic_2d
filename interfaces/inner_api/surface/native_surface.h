/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNERKITS_SURFACE_NATIVE_SURFACE_H
#define INTERFACES_INNERKITS_SURFACE_NATIVE_SURFACE_H

#include <future>
#include <refbase.h>

namespace OHOS {
class NativeSurface : public RefBase {
    friend class SurfaceDelegate;

public:
    /*
     * Constructor of native surface.
     */
    NativeSurface();

    /*
     * Destructor of native surface.
     */
    ~NativeSurface();

    /*
     * Get compat native window.
     *
     * @return native window.
     */
    void* GetNativeWindow();

    /*
     * Get compat native window(for compatbility).
     *
     * @return native window.
     */
    void* Get();

private:
    /*
     * Callback when the compat native window is created.
     *
     * @param nativeWindow the native window.
     */
    void OnNativeWindowCreated(void* nativeWindow);

    /*
     * Callback when the compat native window is destroyed.
     */
    void OnNativeWindowDestroyed();

    /*
     * Release native window.
     */
    void ReleaseNativeWindow();

    void* nativeWindow_ = nullptr;

    std::condition_variable condVar_;

    std::mutex mutex_;
};
} // namespace OHOS
#endif // INTERFACES_INNERKITS_SURFACE_NATIVE_SURFACE_H