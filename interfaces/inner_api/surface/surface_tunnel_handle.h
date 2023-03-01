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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_SURFACE_TUNNEL_HANDLE_H
#define FRAMEWORKS_SURFACE_INCLUDE_SURFACE_TUNNEL_HANDLE_H
#include <mutex>
#include <refbase.h>
#include <graphic_common.h>
#include "surface_type.h"

namespace OHOS {
GraphicExtDataHandle *AllocExtDataHandle(uint32_t reserveInts);
void FreeExtDataHandle(GraphicExtDataHandle *handle);
class SurfaceTunnelHandle : public RefBase {
public:
    SurfaceTunnelHandle();
    virtual ~SurfaceTunnelHandle();
    GSError SetHandle(const GraphicExtDataHandle *handle);
    GraphicExtDataHandle *GetHandle();
    bool Different(const sptr<SurfaceTunnelHandle> &handle);

protected:
    SurfaceTunnelHandle(const SurfaceTunnelHandle&) = delete;
    SurfaceTunnelHandle& operator=(const SurfaceTunnelHandle&) = delete;

private:
    GraphicExtDataHandle *tunnelHandle_ = nullptr;
    mutable std::mutex mutex_;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_SURFACE_TUNNEL_HANDLE_H
