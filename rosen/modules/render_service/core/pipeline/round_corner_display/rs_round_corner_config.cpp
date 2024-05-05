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

#include <chrono>
#include "rs_round_corner_config.h"

using std::chrono::high_resolution_clock;
using std::chrono::microseconds;

namespace OHOS {
namespace Rosen {
namespace rs_rcd {
xmlNodePtr XMLReader::FindNode(const xmlNodePtr& src, const std::string& index)
{
    xmlNodePtr startPtr = src->children;
    while (startPtr != nullptr) {
        auto name = startPtr->name;
        if (xmlStrEqual(name, BAD_CAST(index.c_str())) == 1) {
            return startPtr;
        }
        startPtr = startPtr->next;
    }
    RS_LOGD("[%{public}s] can not found node %{public}s! \n", __func__, index.c_str());
    return nullptr;
}

char* XMLReader::ReadAttrStr(const xmlNodePtr& src, std::string attr)
{
    return reinterpret_cast<char*>((xmlGetProp(src, BAD_CAST(attr.c_str()))));
}

int XMLReader::ReadAttrInt(const xmlNodePtr& src, std::string attr)
{
    auto result = reinterpret_cast<char*>((xmlGetProp(src, BAD_CAST(attr.c_str()))));
    if (result == nullptr) {
        RS_LOGE("[%{public}s] can not found attribute %{public}s! \n", __func__, attr.c_str());
        return 0;
    }
    return std::atoi(result);
}

float XMLReader::ReadAttrFloat(const xmlNodePtr& src, std::string attr)
{
    auto result = reinterpret_cast<char*>((xmlGetProp(src, BAD_CAST(attr.c_str()))));
    if (result == nullptr) {
        RS_LOGE("[%{public}s] can not found attribute %{public}s! \n", __func__, attr.c_str());
        return 0.0f;
    }
    return std::atof(result);
}

bool XMLReader::ReadAttrBool(const xmlNodePtr& src, std::string attr)
{
    auto result = reinterpret_cast<char*>((xmlGetProp(src, BAD_CAST(attr.c_str()))));
    if (result == nullptr) {
        RS_LOGE("[%{public}s] can not found attribute %{public}s! \n", __func__, attr.c_str());
        return false;
    }
    auto istrue = strcmp(result, "true") == 0 ? true : false;
    return istrue;
}

bool XMLReader::Init(std::string file)
{
    xmlKeepBlanksDefault(0);
    pdoc = xmlReadFile(file.c_str(), "", XML_PARSE_RECOVER);
    if (pdoc == nullptr) {
        RS_LOGE("[%{public}s] can not found the file %{public}s! \n", __func__, file.c_str());
        return false;
    }
    proot = xmlDocGetRootElement(pdoc);
    if (proot == nullptr) {
        RS_LOGE("[%{public}s] can not found root node in file %{public}s! \n", __func__, file.c_str());
        return false;
    }
    return true;
}

xmlNodePtr XMLReader::ReadNode(std::vector<std::string> attribute)
{
    xmlNodePtr ptr = proot;
    auto size = static_cast<int>(attribute.size());
    for (int i = 0; i < size; i++) {
        ptr = FindNode(ptr, attribute[i]);
        if (i == static_cast<int>(attribute.size()) - 1) {
            return ptr;
        }
        if (ptr == nullptr) {
            RS_LOGE("[%{public}s] can not found attribute %{public}s! \n", __func__, attribute[i].c_str());
            return nullptr;
        } else {
            ptr = ptr->children;
        }
    }
    return ptr;
}

xmlChar* XMLReader::Read(std::vector<std::string> attribute)
{
    const int attrSizeMin = 2;
    int size = static_cast<int>(attribute.size());
    if (size < attrSizeMin) {
        RS_LOGE("[%{public}s] attribute size less than two! \n", __func__);
        return nullptr;
    }
    std::vector<std::string> nodeIndex(attribute.begin(), attribute.end() - 1);
    auto lastNode = ReadNode(nodeIndex);
    if (lastNode != nullptr) {
        return xmlGetProp(lastNode, BAD_CAST(attribute[size - 1].c_str()));
    }
    RS_LOGE("[%{public}s] can not found attribute %{public}s! \n", __func__, attribute[size - 1].c_str());
    return nullptr;
}

bool SupportConfig::ReadXmlNode(const xmlNodePtr& ptr, std::string supportAttr, std::string modeAttr)
{
    auto a1 = xmlGetProp(ptr, BAD_CAST(supportAttr.c_str()));
    if (a1 == nullptr) {
        RS_LOGE("[%{public}s] can not found supportAttr %{public}s! \n", __func__, supportAttr.c_str());
        return false;
    }
    support = XMLReader::ReadAttrBool(ptr, supportAttr);
    mode = XMLReader::ReadAttrInt(ptr, modeAttr);
    return true;
}

bool RoundCornerLayer::ReadXmlNode(const xmlNodePtr& ptr, std::vector<std::string> attrArray)
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
    fileName = XMLReader::ReadAttrStr(ptr, attrArray[fileIndex].c_str());
    offsetX = XMLReader::ReadAttrInt(ptr, attrArray[offsetXIndex].c_str());
    offsetY = XMLReader::ReadAttrInt(ptr, attrArray[offsetYIndex].c_str());
    binFileName = XMLReader::ReadAttrStr(ptr, attrArray[binFileIndex].c_str());
    bufferSize = XMLReader::ReadAttrInt(ptr, attrArray[bufferSizeIndex].c_str());
    cldWidth = XMLReader::ReadAttrInt(ptr, attrArray[cldWidthIndex].c_str());
    cldHeight = XMLReader::ReadAttrInt(ptr, attrArray[cldHeightIndex].c_str());
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
    width = XMLReader::ReadAttrInt(rogNodePtr, ATTR_WIDTH);
    height = XMLReader::ReadAttrInt(rogNodePtr, ATTR_HEIGHT);

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
    topSurface.ReadXmlNode(topSurfacePtr, ATTR_SUPPORT, ATTR_DISPLAYMODE);

    auto bottomSurfacePtr = XMLReader::FindNode(surfaceConfigNodePtr, std::string(NODE_BOTTOMSURFACE));
    if (bottomSurfacePtr == nullptr) {
        RS_LOGE("[%{public}s] bottomSurfacePtr node is null! \n", __func__);
        return false;
    }
    bottomSurface.ReadXmlNode(bottomSurfacePtr, ATTR_SUPPORT, ATTR_DISPLAYMODE);
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
    sideRegion.ReadXmlNode(sideRegionPtr, ATTR_SUPPORT, ATTR_DISPLAYMODE);
    return true;
}

bool HardwareComposer::ReadXmlNode(const xmlNodePtr& ptr, std::string supportAttr)
{
    auto a1 = xmlGetProp(ptr, BAD_CAST(supportAttr.c_str()));
    if (a1 == nullptr) {
        RS_LOGE("[%{public}s] supportAttr node is null! \n", __func__);
        return false;
    }
    support = XMLReader::ReadAttrBool(ptr, supportAttr);
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
    hardwareComposer.ReadXmlNode(hardwareComposerPtr, ATTR_SUPPORT);
    return true;
}

LCDModel::~LCDModel()
{
    auto size = static_cast<int>(rogs.size());
    for (int i = 0; i < size; i++) {
        if (rogs[i] != nullptr) {
            delete rogs[i];
            rogs[i] = nullptr;
        }
    }
}

bool LCDModel::ReadXmlNode(const xmlNodePtr& lcdNodePtr)
{
    if (lcdNodePtr == nullptr) {
        RS_LOGE("[%{public}s] input lcdNodePtr node is null! \n", __func__);
        return false;
    }
    name = XMLReader::ReadAttrStr(lcdNodePtr, ATTR_NAME);
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

void RCDConfig::PrintLayer(std::string name, const rs_rcd::RoundCornerLayer& layer)
{
    RS_LOGD("%{public}s->Filename: %{public}s, Offset: %{public}d, %{public}d \n",
        name.c_str(), layer.fileName.c_str(), layer.offsetX, layer.offsetY);
    RS_LOGD("BinFilename: %{public}s, BufferSize: %{public}d, cldSize:%{public}d, %{public}d \n",
        layer.binFileName.c_str(), layer.bufferSize, layer.cldWidth, layer.cldHeight);
}

void RCDConfig::PrintParseRog(rs_rcd::ROGSetting* rog)
{
    if (rog == nullptr) {
        RS_LOGE("no model input \n");
        return;
    }
    for (auto kv : rog->portraitMap) {
        auto port = kv.second;
        RS_LOGD("rog: %{public}d, %{public}d, %{public}s: \n", rog->width, rog->height, kv.first.c_str());
        PrintLayer("layerUp  ", port.layerUp);
        PrintLayer("layerDown", port.layerDown);
        PrintLayer("layerHide", port.layerHide);
    }
    for (auto kv : rog->landscapeMap) {
        auto port = kv.second;
        RS_LOGD("rog: %{public}d, %{public}d, %{public}s: \n", rog->width, rog->height, kv.first.c_str());
        PrintLayer("layerUp  ", port.layerUp);
    }
}

void RCDConfig::PrintParseLcdModel(rs_rcd::LCDModel* model)
{
    if (model == nullptr) {
        RS_LOGE("no model input \n");
        return;
    }
    RS_LOGD("lcdModel: %{public}s \n", model->name.c_str());
    auto surfaceConfig = model->GetSurfaceConfig();
    RS_LOGD("surfaceConfig: \n");
    RS_LOGD("topSurface: %{public}d, %{public}d \n", surfaceConfig.topSurface.support,
        surfaceConfig.topSurface.mode);
    RS_LOGD("bottomSurface: %{public}d, %{public}d \n", surfaceConfig.bottomSurface.support,
        surfaceConfig.bottomSurface.mode);

    auto sideRegionConfig = model->GetSideRegionConfig();
    RS_LOGD("sideRegionConfig: \n");
    RS_LOGD("sideRegion: %{public}d, %{public}d \n", sideRegionConfig.sideRegion.support,
        sideRegionConfig.sideRegion.mode);

    auto hardwareConfig = model->GetHardwareComposerConfig();
    RS_LOGD("hardwareConfig: \n");
    RS_LOGD("hardwareComposer: %{public}d \n", hardwareConfig.hardwareComposer.support);
}

bool RCDConfig::Load(std::string configFile)
{
    auto begin = high_resolution_clock::now();
    xmlKeepBlanksDefault(0);
    pDoc = xmlReadFile(configFile.c_str(), "", XML_PARSE_RECOVER);
    if (pDoc == nullptr) {
        RS_LOGE("RoundCornerDisplay read xml failed \n");
        return false;
    }
    RS_LOGD("RoundCornerDisplay read xml ok \n");
    pRoot = xmlDocGetRootElement(pDoc);
    if (pRoot == nullptr) {
        RS_LOGE("RoundCornerDisplay get xml root failed \n");
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
    auto interval = std::chrono::duration_cast<microseconds>(high_resolution_clock::now() - begin);
    RS_LOGD("RoundCornerDisplay read xml time cost %{public}lld us \n", interval.count());
    return true;
}

LCDModel* RCDConfig::GetLcdModel(std::string name) const
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
    RS_LOGE("[%{public}s] lcd not found in name %{public}s! \n", __func__, name.c_str());
    return nullptr;
}

RCDConfig::~RCDConfig()
{
    if (pDoc != nullptr) {
        xmlFreeDoc(pDoc);
    }
    xmlCleanupParser();
    xmlMemoryDump();

    auto size = static_cast<int>(lcdModels.size());
    for (int i = 0; i < size; i++) {
        if (lcdModels[i] != nullptr) {
            delete lcdModels[i];
            lcdModels[i] = nullptr;
        }
    }
}
} // namespace rs_rcd
} // namespace Rosen
} // OHOS