/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_RCD_CONFIG_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_RCD_CONFIG_H

#pragma once
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "image/bitmap.h"
#include <cstring>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <unordered_map>

#include "securec.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace rs_rcd {

const char PATH_CONFIG_FILE[] = "/sys_prod/etc/display/RoundCornerDisplay/config.xml";
const char PATH_CONFIG_DIR[] = "/sys_prod/etc/display/RoundCornerDisplay";

const char NODE_ROUNDCORNERDISPLAY[] = "RoundCornerDisplay";
const char NODE_LCDMODEL[] = "LCDModel";
const char NODE_SURFACECONFIG[] = "SurfaceConfig";
const char NODE_TOPSURFACE[] = "TopSurface";
const char NODE_BOTTOMSURFACE[] = "BottomSurface";
const char NODE_SIDEREGIONCONFIG[] = "SideRegionConfig";
const char NODE_SIDEREGION[] = "SideRegion";
const char NODE_HARDWARECOMPOSERCONFIG[] = "HardwareComposerConfig";
const char NODE_HARDWARECOMPOSER[] = "HardwareComposer";
const char NODE_ROG[] = "ROG";
const char NODE_PORTRAIT[] = "Portrait";
const char NODE_LANDSCAPE[] = "Landscape";
const char NODE_PORTRAIT_SIDEREGION[] = "Portrait_sideRegion";
const char NODE_LANDSCAPE_SIDEREGION[] = "Landscape_sideRegion";
const char NODE_LAYERUP[] = "LayerUp";
const char NODE_LAYERDOWN[] = "LayerDown";
const char NODE_LAYERHIDE[] = "LayerHide";
const char NODE_CAMERAAUO[] = "CameraAuo";

const char ATTR_SUPPORT[] = "Support";
const char ATTR_DISPLAYMODE[] = "DisplayMode";
const char ATTR_REGIONMODE[] = "RegionMode";
const char ATTR_NAME[] = "Name";
const char ATTR_WIDTH[] = "Width";
const char ATTR_HEIGHT[] = "Height";
const char ATTR_FILENAME[] = "Filename";
const char ATTR_BINFILENAME[] = "BinFilename";
const char ATTR_OFFSET_X[] = "OffsetX";
const char ATTR_OFFSET_Y[] = "OffsetY";
const char ATTR_BUFFERSIZE[] = "BufferSize";
const char ATTR_CLDWIDTH[] = "CldWidth";
const char ATTR_CLDHEIGHT[] = "CldHeight";
const char ATTR_DEFAULT[] = "default";

using XMLReader = struct XMLReader {
    XMLReader() {}
    virtual ~XMLReader()
    {
        if (pdoc != nullptr) {
            xmlFreeDoc(pdoc);
        }
        xmlCleanupParser();
        xmlMemoryDump();
    }
    static xmlNodePtr FindNode(const xmlNodePtr& src, const std::string& index);
    static char* ReadAttrStr(const xmlNodePtr& src, std::string attr);
    static int ReadAttrInt(const xmlNodePtr& src, std::string attr);
    static float ReadAttrFloat(const xmlNodePtr& src, std::string attr);
    static bool ReadAttrBool(const xmlNodePtr& src, std::string attr);
    bool Init(std::string file);
    xmlNodePtr ReadNode(std::vector<std::string> attribute);
    xmlChar* Read(std::vector<std::string> attribute);
private:
    xmlDocPtr pdoc = nullptr;
    xmlNodePtr proot = nullptr;
};

using SupportConfig = struct SupportConfig {
    bool support = false;
    int mode = 0;
    bool ReadXmlNode(const xmlNodePtr& ptr, std::string supportAttr, std::string modeAttr);
};

using RoundCornerLayer = struct RoundCornerLayer {
    std::string fileName;
    int offsetX = 0;
    int offsetY = 0;
    std::string binFileName;
    int bufferSize = 0;
    int cldWidth = 0;
    int cldHeight = 0;
    uint32_t layerWidth = 0;
    uint32_t layerHeight = 0;
    Drawing::Bitmap* curBitmap = nullptr;
    bool ReadXmlNode(const xmlNodePtr& ptr, std::vector<std::string> attrArray);
};

using RoundCornerHardware = struct RoundCornerHardware {
    bool resourceChanged = false;
    RoundCornerLayer* topLayer = nullptr;
    RoundCornerLayer* bottomLayer = nullptr;
};

using RogPortrait = struct RogPortrait {
    RoundCornerLayer layerUp;
    RoundCornerLayer layerDown;
    RoundCornerLayer layerHide;
    bool ReadXmlNode(const xmlNodePtr& portraitNodePtr);
};

using RogLandscape = struct RogLandscape {
    RoundCornerLayer layerUp;
    bool ReadXmlNode(const xmlNodePtr& landNodePtr);
};

using ROGSetting = struct ROGSetting {
    int width = 0;
    int height = 0;
    std::unordered_map<std::string, RogPortrait> portraitMap;
    std::unordered_map<std::string, RogLandscape> landscapeMap;
    bool ReadXmlNode(const xmlNodePtr& rogNodePtr);
};

using SurfaceConfig = struct SurfaceConfig {
    SupportConfig topSurface;
    SupportConfig bottomSurface;
    bool ReadXmlNode(const xmlNodePtr& surfaceConfigNodePtr);
};

using SideRegionConfig = struct SideRegionConfig {
    SupportConfig sideRegion;
    bool ReadXmlNode(const xmlNodePtr& sideRegionNodePtr);
};

using HardwareComposer = struct HardwareComposer {
    bool support = false;
    bool ReadXmlNode(const xmlNodePtr& ptr, std::string supportAttr);
};

using HardwareComposerConfig = struct HardwareComposerConfig {
    HardwareComposer hardwareComposer;
    bool ReadXmlNode(const xmlNodePtr& hardwareComposerNodePtr);
};

using LCDModel = struct LCDModel {
    LCDModel() {}
    virtual ~LCDModel();
    std::string name;
    SurfaceConfig surfaceConfig;
    SideRegionConfig sideRegionConfig;
    HardwareComposerConfig hardwareConfig;
    std::vector<ROGSetting*> rogs;
    bool ReadXmlNode(const xmlNodePtr& lcdNodePtr);
    SurfaceConfig GetSurfaceConfig() const;
    SideRegionConfig GetSideRegionConfig() const;
    HardwareComposerConfig GetHardwareComposerConfig() const;
    ROGSetting* GetRog(const int w, const int h) const;
};

using RCDConfig = struct RCDConfig {
    RCDConfig() {}
    virtual ~RCDConfig();
    static void PrintLayer(std::string name, const rs_rcd::RoundCornerLayer& layer);
    static void PrintParseRog(rs_rcd::ROGSetting* rog);
    static void PrintParseLcdModel(rs_rcd::LCDModel* model);
    LCDModel* GetLcdModel(std::string name) const;
    bool Load(std::string configFile);
private:
    xmlDocPtr pDoc = nullptr;
    xmlNodePtr pRoot = nullptr;
    std::vector<LCDModel*> lcdModels;
};
} // namespace rs_rcd
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_PIPELINE_RS_RCD_CONFIG_H