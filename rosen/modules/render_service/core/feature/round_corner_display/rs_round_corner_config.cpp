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

#include "rs_round_corner_config.h"
#include <algorithm>
#include <chrono>
#include <regex>

using std::chrono::high_resolution_clock;
using std::chrono::microseconds;

namespace OHOS {
namespace Rosen {
namespace rs_rcd {
const char REG_NUM[] = "^[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)$"; // regular expression of check number

bool XMLReader::RegexMatch(const std::string& str, const std::string& pattern)
{
    std::smatch result;
    std::regex r(pattern);
    return std::regex_match(str, result, r);
}

bool XMLReader::RegexMatchNum(std::string& str)
{
    // remove space from string before match
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    return RegexMatch(str, std::string(REG_NUM));
}

xmlNodePtr XMLReader::FindNode(const xmlNodePtr& src, const std::string& index)
{
    xmlNodePtr startPtr = src->children;
    while (startPtr != nullptr) {
        auto name = startPtr->name;
        if (name != nullptr && xmlStrEqual(name, BAD_CAST(index.c_str())) == 1) {
            return startPtr;
        }
        startPtr = startPtr->next;
    }
    RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] can not found node %{public}s! \n", __func__, index.c_str());
    return nullptr;
}

std::string XMLReader::ReadAttrStr(const xmlNodePtr& src, const std::string& attr)
{
    auto result = reinterpret_cast<char*>((xmlGetProp(src, BAD_CAST(attr.c_str()))));
    if (result == nullptr) {
        RS_LOGE("[%{public}s] can not found attribute %{public}s! \n", __func__, attr.c_str());
        return std::string("");
    }
    std::string str(result);
    xmlFree(result);
    return str;
}

int XMLReader::ReadAttrInt(const xmlNodePtr& src, const std::string& attr)
{
    auto result = reinterpret_cast<char*>((xmlGetProp(src, BAD_CAST(attr.c_str()))));
    if (result == nullptr) {
        RS_LOGE("[%{public}s] can not found attribute %{public}s! \n", __func__, attr.c_str());
        return 0;
    }
    std::string num(result);
    xmlFree(result);
    if (RegexMatchNum(num)) {
        return std::atoi(num.c_str());
    }
    RS_LOGE("[%{public}s] attribute %{public}s can not convert to int! \n", __func__, attr.c_str());
    return 0;
}

float XMLReader::ReadAttrFloat(const xmlNodePtr& src, const std::string& attr)
{
    auto result = reinterpret_cast<char*>((xmlGetProp(src, BAD_CAST(attr.c_str()))));
    if (result == nullptr) {
        RS_LOGE("[%{public}s] can not found attribute %{public}s! \n", __func__, attr.c_str());
        return 0.0f;
    }
    std::string num(result);
    xmlFree(result);
    if (RegexMatchNum(num)) {
        return std::atof(num.c_str());
    }
    RS_LOGE("[%{public}s] attribute %{public}s can not convert to float! \n", __func__, attr.c_str());
    return 0;
}

bool XMLReader::ReadAttrBool(const xmlNodePtr& src, const std::string& attr)
{
    auto result = reinterpret_cast<char*>((xmlGetProp(src, BAD_CAST(attr.c_str()))));
    if (result == nullptr) {
        RS_LOGE("[%{public}s] can not found attribute %{public}s! \n", __func__, attr.c_str());
        return false;
    }
    auto istrue = strcmp(result, "true") == 0 ? true : false;
    xmlFree(result);
    return istrue;
}

bool SupportConfig::ReadXmlNode(const xmlNodePtr& ptr, const std::string& supportAttr, const std::string& modeAttr)
{
    auto a1 = xmlGetProp(ptr, BAD_CAST(supportAttr.c_str()));
    if (a1 == nullptr) {
        RS_LOGE("[%{public}s] can not found supportAttr %{public}s! \n", __func__, supportAttr.c_str());
        return false;
    }
    support = XMLReader::ReadAttrBool(ptr, supportAttr);
    mode = XMLReader::ReadAttrInt(ptr, modeAttr);
    xmlFree(a1);
    return true;
}

bool RoundCornerLayer::ReadXmlNode(const xmlNodePtr& ptr, const std::vector<std::string>& attrArray)
{
    const int layerAttrSize = 7;
    if (attrArray.size() < layerAttrSize || ptr == nullptr) {
        RS_LOGE("[%{public}s] attribute size less than 7! \n", __func__);
        return false;
    }
    const int fileIndex = 0;
    const int offsetXIndex = 1;
    const int offsetYIndex = 2;
    const int binFileIndex = 3;
    const int bufferSizeIndex = 4;
    const int cldWidthIndex = 5;
    const int cldHeightIndex = 6;
    fileName = XMLReader::ReadAttrStr(ptr, attrArray[fileIndex]);
    offsetX = XMLReader::ReadAttrInt(ptr, attrArray[offsetXIndex]);
    offsetY = XMLReader::ReadAttrInt(ptr, attrArray[offsetYIndex]);
    binFileName = XMLReader::ReadAttrStr(ptr, attrArray[binFileIndex]);
    bufferSize = XMLReader::ReadAttrInt(ptr, attrArray[bufferSizeIndex]);
    cldWidth = XMLReader::ReadAttrInt(ptr, attrArray[cldWidthIndex]);
    cldHeight = XMLReader::ReadAttrInt(ptr, attrArray[cldHeightIndex]);
    return true;
}

bool RogPortrait::ReadXmlNode(const xmlNodePtr& portraitNodePtr)
{
    if (portraitNodePtr == nullptr) {
        RS_LOGE("[%{public}s] input node is null! \n", __func__);
        return false;
    }
    std::vector<std::string> attrList = { ATTR_FILENAME, ATTR_OFFSET_X, ATTR_OFFSET_Y,
        ATTR_BINFILENAME, ATTR_BUFFERSIZE, ATTR_CLDWIDTH, ATTR_CLDHEIGHT };

    auto layerUpPtr = XMLReader::FindNode(portraitNodePtr, std::string(NODE_LAYERUP));
    if (layerUpPtr == nullptr) {
        RS_LOGE("[%{public}s] layerUpPtr node is null! \n", __func__);
        return false;
    }
    layerUp.ReadXmlNode(layerUpPtr, attrList);

    auto layerDownPtr = XMLReader::FindNode(portraitNodePtr, std::string(NODE_LAYERDOWN));
    if (layerDownPtr == nullptr) {
        RS_LOGE("[%{public}s] layerDownPtr node is null! \n", __func__);
        return false;
    }
    layerDown.ReadXmlNode(layerDownPtr, attrList);

    auto layerHidePtr = XMLReader::FindNode(portraitNodePtr, std::string(NODE_LAYERHIDE));
    if (layerHidePtr == nullptr) {
        RS_LOGE("[%{public}s] layerHidePtr node is null! \n", __func__);
        return false;
    }
    layerHide.ReadXmlNode(layerHidePtr, attrList);
    return true;
}

bool RogLandscape::ReadXmlNode(const xmlNodePtr& landNodePtr)
{
    if (landNodePtr == nullptr) {
        RS_LOGE("[%{public}s] landNodePtr node is null! \n", __func__);
        return false;
    }
    std::vector<std::string> attrList = { ATTR_FILENAME, ATTR_OFFSET_X, ATTR_OFFSET_Y,
        ATTR_BINFILENAME, ATTR_BUFFERSIZE, ATTR_CLDWIDTH, ATTR_CLDHEIGHT };
    auto layerUpPtr = XMLReader::FindNode(landNodePtr, std::string(NODE_LAYERUP));
    if (layerUpPtr == nullptr) {
        RS_LOGE("[%{public}s] layerUpPtr node is null! \n", __func__);
        return false;
    }
    layerUp.ReadXmlNode(layerUpPtr, attrList);
    return true;
}

bool ROGSetting::ReadXmlNode(const xmlNodePtr& rogNodePtr)
{
    if (rogNodePtr == nullptr) {
        RS_LOGE("[%{public}s] rogNodePtr node is null! \n", __func__);
        return false;
    }
    width = XMLReader::ReadAttrInt(rogNodePtr, std::string(ATTR_WIDTH));
    height = XMLReader::ReadAttrInt(rogNodePtr, std::string(ATTR_HEIGHT));

    auto portPtr = XMLReader::FindNode(rogNodePtr, NODE_PORTRAIT);
    if (portPtr != nullptr) {
        RogPortrait p;
        p.ReadXmlNode(portPtr);
        portraitMap[NODE_PORTRAIT] = p;
    }

    auto portSidePtr = XMLReader::FindNode(rogNodePtr, NODE_PORTRAIT_SIDEREGION);
    if (portSidePtr != nullptr) {
        RogPortrait p;
        p.ReadXmlNode(portSidePtr);
        portraitMap[NODE_PORTRAIT_SIDEREGION] = p;
    }

    auto landPtr = XMLReader::FindNode(rogNodePtr, NODE_LANDSCAPE);
    if (landPtr != nullptr) {
        RogLandscape p;
        p.ReadXmlNode(landPtr);
        landscapeMap[NODE_LANDSCAPE] = p;
    }

    auto landSidePtr = XMLReader::FindNode(rogNodePtr, NODE_LANDSCAPE_SIDEREGION);
    if (landSidePtr != nullptr) {
        RogLandscape p;
        p.ReadXmlNode(landSidePtr);
        landscapeMap[NODE_LANDSCAPE_SIDEREGION] = p;
    }
    return true;
}

bool ROGSetting::HavePortrait(const std::string& name) const
{
    if (portraitMap.count(name) < 1) {
        RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] PORTRAIT layerUp %{public}s do not configured \n", __func__,
            name.c_str());
        return false;
    }
    return true;
}

bool ROGSetting::HaveLandscape(const std::string& name) const
{
    if (landscapeMap.count(name) < 1) {
        RS_LOGD_IF(DEBUG_PIPELINE, "[%{public}s] LANDSACPE layerUp %{public}s do not configured \n", __func__,
            name.c_str());
        return false;
    }
    return true;
}

std::optional<RogPortrait> ROGSetting::GetPortrait(const std::string& name) const
{
    if (!HavePortrait(name)) {
        return std::nullopt;
    }
    return portraitMap.at(name);
}

std::optional<RogLandscape> ROGSetting::GetLandscape(const std::string& name) const
{
    if (!HaveLandscape(name)) {
        return std::nullopt;
    }
    return landscapeMap.at(name);
}

bool SurfaceConfig::ReadXmlNode(const xmlNodePtr& surfaceConfigNodePtr)
{
    if (surfaceConfigNodePtr == nullptr) {
        RS_LOGE("[%{public}s] surfaceConfigNodePtr node is null! \n", __func__);
        return false;
    }

    auto topSurfacePtr = XMLReader::FindNode(surfaceConfigNodePtr, std::string(NODE_TOPSURFACE));
    if (topSurfacePtr == nullptr) {
        RS_LOGE("[%{public}s] topSurfacePtr node is null! \n", __func__);
        return false;
    }
    topSurface.ReadXmlNode(topSurfacePtr, std::string(ATTR_SUPPORT), std::string(ATTR_DISPLAYMODE));

    auto bottomSurfacePtr = XMLReader::FindNode(surfaceConfigNodePtr, std::string(NODE_BOTTOMSURFACE));
    if (bottomSurfacePtr == nullptr) {
        RS_LOGE("[%{public}s] bottomSurfacePtr node is null! \n", __func__);
        return false;
    }
    bottomSurface.ReadXmlNode(bottomSurfacePtr, std::string(ATTR_SUPPORT), std::string(ATTR_DISPLAYMODE));
    return true;
}

bool SideRegionConfig::ReadXmlNode(const xmlNodePtr& sideRegionNodePtr)
{
    if (sideRegionNodePtr == nullptr) {
        RS_LOGE("[%{public}s] sideRegionNodePtr node is null! \n", __func__);
        return false;
    }
    auto sideRegionPtr = XMLReader::FindNode(sideRegionNodePtr, std::string(NODE_SIDEREGION));
    if (sideRegionPtr == nullptr) {
        RS_LOGE("[%{public}s] sideRegionPtr node is null! \n", __func__);
        return false;
    }
    sideRegion.ReadXmlNode(sideRegionPtr, std::string(ATTR_SUPPORT), std::string(ATTR_DISPLAYMODE));
    return true;
}

bool HardwareComposer::ReadXmlNode(const xmlNodePtr& ptr, const std::string& supportAttr)
{
    auto a1 = xmlGetProp(ptr, BAD_CAST(supportAttr.c_str()));
    if (a1 == nullptr) {
        RS_LOGE("[%{public}s] supportAttr node is null! \n", __func__);
        return false;
    }
    support = XMLReader::ReadAttrBool(ptr, supportAttr);
    xmlFree(a1);
    return true;
}

bool HardwareComposerConfig::ReadXmlNode(const xmlNodePtr& hardwareComposerNodeptr)
{
    if (hardwareComposerNodeptr == nullptr) {
        RS_LOGE("[%{public}s] hardwareComposerNodeptr node is null! \n", __func__);
        return false;
    }
    auto hardwareComposerPtr = XMLReader::FindNode(hardwareComposerNodeptr, std::string(NODE_HARDWARECOMPOSER));
    if (hardwareComposerPtr == nullptr) {
        RS_LOGE("[%{public}s] hardwareComposerPtr node is null! \n", __func__);
        return false;
    }
    hardwareComposer.ReadXmlNode(hardwareComposerPtr, std::string(ATTR_SUPPORT));
    return true;
}

LCDModel::~LCDModel()
{
    for (auto& rog : rogs) {
        if (rog != nullptr) {
            delete rog;
            rog = nullptr;
        }
    }
}

bool LCDModel::ReadXmlNode(const xmlNodePtr& lcdNodePtr)
{
    if (lcdNodePtr == nullptr) {
        RS_LOGE("[%{public}s] input lcdNodePtr node is null! \n", __func__);
        return false;
    }
    name = XMLReader::ReadAttrStr(lcdNodePtr, std::string(ATTR_NAME));
    auto surfaceConfigPtr = XMLReader::FindNode(lcdNodePtr, std::string(NODE_SURFACECONFIG));
    if (surfaceConfigPtr == nullptr) {
        RS_LOGW("no surfaceConfig found \n");
    }
    surfaceConfig.ReadXmlNode(surfaceConfigPtr);

    auto sideRegionConfigPtr = XMLReader::FindNode(lcdNodePtr, std::string(NODE_SIDEREGIONCONFIG));
    if (sideRegionConfigPtr == nullptr) {
        RS_LOGW("no sideRegionConfig found \n");
    }
    sideRegionConfig.ReadXmlNode(sideRegionConfigPtr);

    auto hardwareComposerConfigPtr = XMLReader::FindNode(lcdNodePtr, std::string(NODE_HARDWARECOMPOSERCONFIG));
    if (hardwareComposerConfigPtr == nullptr) {
        RS_LOGW("no harewareComposer found \n");
    }
    hardwareConfig.ReadXmlNode(hardwareComposerConfigPtr);

    xmlNodePtr startPtr = lcdNodePtr->children;
    while (startPtr != nullptr) {
        auto name = startPtr->name;
        if (xmlStrEqual(name, BAD_CAST(NODE_ROG)) == 1) {
            ROGSetting *rog = new ROGSetting();
            if (rog->ReadXmlNode(startPtr)) {
                rogs.push_back(rog);
            } else {
                delete rog;
                rog = nullptr;
            }
        }
        startPtr = startPtr->next;
    }
    return true;
}

SurfaceConfig LCDModel::GetSurfaceConfig() const
{
    return surfaceConfig;
}

SideRegionConfig LCDModel::GetSideRegionConfig() const
{
    return sideRegionConfig;
}

HardwareComposerConfig LCDModel::GetHardwareComposerConfig() const
{
    return hardwareConfig;
}

ROGSetting* LCDModel::GetRog(const int w, const int h) const
{
    for (auto rog : rogs) {
        if (rog == nullptr) {
            continue;
        }
        if (rog->width == w && rog->height == h) {
            return rog;
        }
    }
    return nullptr;
}

void RCDConfig::PrintLayer(const std::string& name, const rs_rcd::RoundCornerLayer& layer)
{
    RS_LOGI("%{public}s->Filename: %{public}s, Offset: %{public}d, %{public}d \n",
        name.c_str(), layer.fileName.c_str(), layer.offsetX, layer.offsetY);
    RS_LOGI("BinFilename: %{public}s, BufferSize: %{public}d, cldSize:%{public}d, %{public}d \n",
        layer.binFileName.c_str(), layer.bufferSize, layer.cldWidth, layer.cldHeight);
}

void RCDConfig::PrintParseRog(rs_rcd::ROGSetting* rog)
{
    if (rog == nullptr) {
        RS_LOGW("no rog input \n");
        return;
    }
    for (auto kv : rog->portraitMap) {
        auto port = kv.second;
        RS_LOGI("rog: %{public}d, %{public}d, %{public}s: \n", rog->width, rog->height,
            kv.first.c_str());
        PrintLayer(std::string("layerUp  "), port.layerUp);
        PrintLayer(std::string("layerDown"), port.layerDown);
        PrintLayer(std::string("layerHide"), port.layerHide);
    }
    for (auto kv : rog->landscapeMap) {
        auto port = kv.second;
        RS_LOGI("rog: %{public}d, %{public}d, %{public}s: \n", rog->width, rog->height,
            kv.first.c_str());
        PrintLayer(std::string("layerUp  "), port.layerUp);
    }
}

bool RCDConfig::Load(const std::string& configFile)
{
    std::lock_guard<std::mutex> lock(xmlMut);
    if (isLoadData) {
        return true;
    }
    isLoadData = true;
    auto begin = high_resolution_clock::now();
    Clear();
    xmlKeepBlanksDefault(0);
    pDoc = xmlReadFile(configFile.c_str(), "", XML_PARSE_RECOVER);
    if (pDoc == nullptr) {
        RS_LOGE("RoundCornerDisplay read xml failed \n");
        CloseXML();
        return false;
    }
    RS_LOGI("RoundCornerDisplay read xml ok \n");
    pRoot = xmlDocGetRootElement(pDoc);
    if (pRoot == nullptr) {
        RS_LOGE("RoundCornerDisplay get xml root failed \n");
        CloseXML();
        return false;
    }
    xmlNodePtr startPtr = pRoot->children;
    while (startPtr != nullptr) {
        auto name = startPtr->name;
        if (xmlStrEqual(name, BAD_CAST(NODE_LCDMODEL)) == 1) {
            LCDModel* lcdModel = new LCDModel();
            if (lcdModel->ReadXmlNode(startPtr)) {
                lcdModels.push_back(lcdModel);
            } else {
                delete lcdModel;
                lcdModel = nullptr;
            }
        }
        startPtr = startPtr->next;
    }
    CloseXML();
    auto interval = std::chrono::duration_cast<microseconds>(high_resolution_clock::now() - begin);
    RS_LOGI("RoundCornerDisplay read xml time cost %{public}lld us \n", interval.count());
    return true;
}

bool RCDConfig::IsDataLoaded() const
{
    return isLoadData;
}

LCDModel* RCDConfig::GetLcdModel(const std::string& name) const
{
    if (name.empty()) {
        RS_LOGE("[%{public}s] name is null! \n", __func__);
        return nullptr;
    }
    for (auto model : lcdModels) {
        if (model == nullptr) {
            continue;
        }
        if (model->name.compare(name) == 0) {
            return model;
        }
    }
    RS_LOGD("[%{public}s] rcd: lcd not found in name %{public}s! \n", __func__, name.c_str());
    return nullptr;
}

void RCDConfig::CloseXML()
{
    if (pDoc != nullptr) {
        xmlFreeDoc(pDoc);
        pDoc = nullptr;
    }
}

void RCDConfig::Clear()
{
    for (auto& modelPtr : lcdModels) {
        if (modelPtr != nullptr) {
            delete modelPtr;
            modelPtr = nullptr;
        }
    }
}

RCDConfig::~RCDConfig()
{
    Clear();
}
} // namespace rs_rcd
} // namespace Rosen
} // OHOS