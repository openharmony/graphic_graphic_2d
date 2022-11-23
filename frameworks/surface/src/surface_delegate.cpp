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

#include "surface_delegate.h"

namespace OHOS {

void SurfaceDelegate::AddSurfaceCallback(const sptr<ISurfaceCallback>& callback) {}

void SurfaceDelegate::RemoveSurfaceCallback(const sptr<ISurfaceCallback>& callback) {}

SurfaceDelegate::SurfaceDelegate(int windowId)
{
    windowId_ = windowId;
}
    
SurfaceDelegateError SurfaceDelegate::CreateSurface(bool isWindowSurface)
{
    return SurfaceDelegateError::SURFACE_DELEGATE_ERROR_DEFAULT;
}

SurfaceDelegateError SurfaceDelegate::SetBounds(int32_t left, int32_t right, int32_t width, int32_t height)
{
    return SurfaceDelegateError::SURFACE_DELEGATE_ERROR_DEFAULT;
}

SurfaceDelegateError SurfaceDelegate::ReleaseSurface()
{
    return SurfaceDelegateError::SURFACE_DELEGATE_ERROR_DEFAULT;
}

SurfaceDelegateError SurfaceDelegate::SetSurfaceSize(uint32_t width, uint32_t height)
{
    return SurfaceDelegateError::SURFACE_DELEGATE_ERROR_DEFAULT;
}

sptr<Surface> SurfaceDelegate::GetSurface()
{
    return nullptr;
}

void* SurfaceDelegate::GetNativeWindow()
{
    return nullptr;
}
}