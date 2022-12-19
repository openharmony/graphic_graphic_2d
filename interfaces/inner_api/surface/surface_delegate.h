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

#ifndef INTERFACES_INNERKITS_SURFACE_SURFACE_DELEGATE_H
#define INTERFACES_INNERKITS_SURFACE_SURFACE_DELEGATE_H

#include <refbase.h>

#include "surface.h"

struct NativeSurfaceCallback;

namespace OHOS {

enum class SurfaceDelegateError : int32_t {
    SURFACE_DELEGATE_ERROR_DEFAULT = -1,
    SURFACE_DELEGATE_OK = 0,
    SURFACE_DELEGATE_DO_NOTHING = 1,
    SURFACE_DELEGATE_ERROR_SAMGR = 100,
    SURFACE_DELEGATE_ERROR_IPC_FAILED = 101,
    SURFACE_DELEGATE_ERROR_NO_MEM = 110,
    SURFACE_DELEGATE_ERROR_NULLPTR = 120,
    SURFACE_DELEGATE_ERROR_INVALID_PARAM = 130,
    SURFACE_DELEGATE_ERROR_DESTROYED_OBJECT = 140,
    SURFACE_DELEGATE_ERROR_DEATH_RECIPIENT = 150,
    SURFACE_DELEGATE_ERROR_INVALID_WINDOW = 160,
    SURFACE_DELEGATE_ERROR_INVALID_OPERATION = 170,
    SURFACE_DELEGATE_ERROR_INVALID_TYPE = 180,
    SURFACE_DELEGATE_ERROR_INVALID_PERMISSION = 190,
    SURFACE_DELEGATE_ERROR_UNKNOWN,
};

class SurfaceDelegate : public RefBase {
public:
    /*
     * Define callback for the surface lifecycle.
     */
    class ISurfaceCallback : virtual public RefBase {
        public:
            /*
             * Callback when the surface was created.
             *
             * @param surface the surface.
             */
            virtual void OnSurfaceCreated(const sptr<Surface>& surface) = 0;

            /*
             * Callback when the surface was changed.
             *
             * @param surface the surface.
             * @param width the surface width.
             * @param height the surface height.
             */
            virtual void OnSurfaceChanged(const sptr<Surface>& surface, int32_t width, int32_t height) = 0;

            /*
             * Callback when the surface was destroyed.
             */
            virtual void OnSurfaceDestroyed() = 0;
    };

    /*
     * Add the surface callback.
     *
     * @param callback the ISurfaceCallback.
     */
    void AddSurfaceCallback(const sptr<ISurfaceCallback>& callback);

    /*
     * Remove the surface callback.
     *
     * @param callback the ISurfaceCallback.
     */
    void RemoveSurfaceCallback(const sptr<ISurfaceCallback>& callback);

    /*
     * Constructor of SurfaceDelegate.
     *
     * @param windowId the window id the surface will be on.
     */
    SurfaceDelegate(int windowId);

    /*
     * Destructor of SurfaceDelegate.
     */
    ~SurfaceDelegate() = default;
    
    /*
     * Create the surface.
     *
     * @param isWindowSurface if the surface is a window type surface.
     * @return the SurfaceDelegateError code.
     */
    SurfaceDelegateError CreateSurface(bool isWindowSurface = false);

    /*
     * Set the boundaries of the area where the surface is located.
     *
     * @param left the left of the boundaries.
     * @param right the right of the boundaries.
     * @param width the width of the boundaries.
     * @param height the height of the boundaries.
     * @return the SurfaceDelegateError code.
     */
    SurfaceDelegateError SetBounds(int32_t left, int32_t right, int32_t width, int32_t height);

    /*
     * Release the surface.
     *
     * @return the SurfaceDelegateError code.
     */
    SurfaceDelegateError ReleaseSurface();

    /*
     * Set the surface size.
     *
     * @param width the surface width.
     * @param height the surface height.
     * @return the SurfaceDelegateError code.
     */
    SurfaceDelegateError SetSurfaceSize(uint32_t width, uint32_t height);

    /*
     * Get the surface.
     *
     * @return the surface.
     */
    sptr<Surface> GetSurface();

    /*
     * Get the native window.
     *
     * @return the void pointer of the native window.
     */
    void* GetNativeWindow();

private:
    NativeSurfaceCallback CreateNativeSurfaceCallback();

    sptr<NativeSurface> GetNativeSurface();

    void OnSurfaceCreated(uint32_t windowId, uint64_t surfaceId);
    void OnSurfaceChanged(uint32_t windowId, uint64_t surfaceId, int32_t width, int32_t height);
    void OnSurfaceDestroyed(uint32_t windowId, uint64_t surfaceId);

    void OnNativeSurfaceCreated(void* nativeWindow);
    void OnNativeSurfaceDestroyed();

    sptr<Surface> surface_ = nullptr;

    std::vector<sptr<ISurfaceCallback>> surfaceCallbacks_;
    std::recursive_mutex mutex_;

    int windowId_ = 0;
};
} // namespace OHOS
#endif // INTERFACES_INNERKITS_SURFACE_SURFACE_DELEGATE_H