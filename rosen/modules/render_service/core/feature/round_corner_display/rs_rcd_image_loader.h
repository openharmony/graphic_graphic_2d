/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RCD_RENDER_RS_RCD_IMG_LOADER_H
#define RENDER_SERVICE_CORE_PIPELINE_RCD_RENDER_RS_RCD_IMG_LOADER_H

#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include "pipeline/rs_paint_filter_canvas.h"
#include "screen_manager/screen_types.h"
#include "feature/round_corner_display/rs_round_corner_config.h"

namespace OHOS {
namespace Rosen {

enum ShowTopResourceType {
    // choose type and then choose resource for harden or RS
    TOP_PORTRAIT = 0,
    TOP_LADS_ORIT,
    TOP_HIDDEN
};
class RCDImageLoader {
public:
    RCDImageLoader(bool supportHardware, bool supportTopSurface, bool supportBottomSurface)
        : supportHardware_{supportHardware}, supportTopSurface_{supportTopSurface},
        supportBottomSurface_{supportBottomSurface} {};
    virtual ~RCDImageLoader() {};

    virtual void RcdChooseHardwareResource(int type, rs_rcd::ROGSetting* rog,
        rs_rcd::RoundCornerHardware& hardInfo) = 0;

    static std::shared_ptr<RCDImageLoader> Create(rs_rcd::ImageLoadType type,
        bool supportHardware, bool supportTopSurface, bool supportBottomSurface);
protected:
    bool supportHardware_ = false;
    bool supportTopSurface_ = false;
    bool supportBottomSurface_ = false;
};

class RCDImageRogFullyLoader : public RCDImageLoader {
public:
    RCDImageRogFullyLoader(bool supportHardware, bool supportTopSurface, bool supportBottomSurface)
        : RCDImageLoader(supportHardware, supportTopSurface, supportBottomSurface) {}
    ~RCDImageRogFullyLoader() override {};
    void RcdChooseHardwareResource(int type, rs_rcd::ROGSetting* rog,
        rs_rcd::RoundCornerHardware& hardInfo) override;
    bool GetTopSurfaceSource(rs_rcd::ROGSetting* rog);
    bool GetBottomSurfaceSource(rs_rcd::ROGSetting* rog);
private:
    // notch resources for harden
    std::shared_ptr<Drawing::Bitmap> bitmapTopPortrait_;
    std::shared_ptr<Drawing::Bitmap> bitmapTopLadsOrit_;
    std::shared_ptr<Drawing::Bitmap> bitmapTopHidden_;
    std::shared_ptr<Drawing::Bitmap> bitmapBottomPortrait_;

    rs_rcd::ROGSetting* rog_ = nullptr;
};

class RCDImageRogPartiallyLoader : public RCDImageLoader {
public:
    RCDImageRogPartiallyLoader(bool supportHardware, bool supportTopSurface, bool supportBottomSurface)
        : RCDImageLoader(supportHardware, supportTopSurface, supportBottomSurface) {}
    ~RCDImageRogPartiallyLoader() override {};
    void RcdChooseHardwareResource(int type, rs_rcd::ROGSetting* rog,
        rs_rcd::RoundCornerHardware& hardInfo) override;
private:
    void UpdateTopLayer(rs_rcd::RoundCornerHardware& hardInfo,
        std::shared_ptr<rs_rcd::RoundCornerLayer> layer);
    void UpdateBottomLayer(rs_rcd::RoundCornerHardware& hardInfo,
        std::shared_ptr<rs_rcd::RoundCornerLayer> layer);
    // the resource to be drawn
    std::shared_ptr<Drawing::Bitmap> bitmapTop_ = nullptr;
    std::shared_ptr<Drawing::Bitmap> bitmapBottom_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif