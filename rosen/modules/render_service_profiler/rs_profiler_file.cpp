/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rs_profiler_file.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#ifndef REPLAY_TOOL_CLIENT
#include "platform/common/rs_log.h"
#else
#include <fmt/printf.h>
#include <spdlog/spdlog.h>
#define ROSEN_LOGD(...) spdlog::debug(fmt::sprintf(__VA_ARGS__))
#endif

namespace OHOS::Rosen {

RSFile::RSFile() = default;

const std::string& RSFile::GetDefaultPath()
{
    static const std::string PATH("/data/rsrecord.bin");
    return PATH;
}

void RSFile::Create(const std::string& fname)
{
    if (f_ != NULL) {
        Close();
    }
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    f_ = fopen(fname.c_str(), "wbe");
    if (f_ == NULL) {
        return;
    }

    uint32_t headerId = 'RPLY';
    fwrite(&headerId, sizeof(headerId), 1, f_);

    headerOff_ = 0; // TEMP VALUE
    fwrite(&headerOff_, sizeof(headerOff_), 1, f_);

    constexpr int OFFSET8 = 8;
    writeDataOff_ = OFFSET8;

    wasChanged_ = true;
}

bool RSFile::Open(const std::string& fname)
{
    if (f_ != NULL) {
        Close();
    }

    f_ = fopen(fname.c_str(), "rbe");
    if (f_ == NULL) {
        return false;
    }

    uint32_t headerId = 'RPLY';
    fread(&headerId, sizeof(headerId), 1, f_);
    if (headerId != 'RPLY') {
        fclose(f_);
        f_ = NULL;
        return false;
    }

    fread(&headerOff_, sizeof(headerOff_), 1, f_);

    fseek(f_, 0, SEEK_END);
    writeDataOff_ = ftell(f_);

    fseek(f_, headerOff_, SEEK_SET);
    ReadHeaders();

    wasChanged_ = false;

    return true;
}

bool RSFile::IsOpen()
{
    return f_ != nullptr;
}

void RSFile::SetWriteTime(double time)
{
    writeStartTime_ = time;
}

double RSFile::GetWriteTime()
{
    return writeStartTime_;
}

// ***********************************
// *** GLOBAL HEADER

void RSFile::AddHeaderPID(pid_t pid)
{
    if (std::find(std::begin(headerPidList_), std::end(headerPidList_), pid) != std::end(headerPidList_)) {
        return;
    }
    headerPidList_.push_back(pid);

    wasChanged_ = true;
}

void RSFile::SetImageMapPtr(std::map<uint64_t, ReplayImageCacheRecordFile>* imageMapPtr)
{
    imageMapPtr_ = imageMapPtr;
}

std::vector<pid_t>& RSFile::GetHeaderPIDList()
{
    return headerPidList_;
}

void RSFile::WriteHeader()
{
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    if (f_ == NULL) {
        return;
    }

    headerOff_ = writeDataOff_;
    fseek(f_, writeDataOff_, SEEK_SET);

    // WRITE TIME START
    fwrite(&writeStartTime_, 1, sizeof(writeStartTime_), f_);

    // SAVE PID LIST
    uint32_t recordSize = headerPidList_.size();
    fwrite(&recordSize, sizeof(recordSize), 1, f_);
    fwrite(&headerPidList_[0], headerPidList_.size(), sizeof(pid_t), f_);

    // ALL TEXTURES
    if (imageMapPtr_ == nullptr) {
        recordSize = 0;
        fwrite(&recordSize, sizeof(recordSize), 1, f_);
    } else {
        recordSize = imageMapPtr_->size();
        fwrite(&recordSize, sizeof(recordSize), 1, f_);
        for (auto item : *imageMapPtr_) {
            fwrite(&item.first, sizeof(item.first), 1, f_);                       // unique_id
            fwrite(&item.second.skipBytes, sizeof(item.second.skipBytes), 1, f_); // skip bytes
            fwrite(&item.second.imageSize, sizeof(item.second.imageSize), 1, f_); // image_size
            fwrite(item.second.image.get(), item.second.imageSize, 1, f_);        // image_data
        }
    }

    // SAVE LAYERS OFFSETS
    recordSize = layerData_.size();
    fwrite(&recordSize, sizeof(recordSize), 1, f_);
    for (auto& i : layerData_) {
        fwrite(&i.layerHeader, sizeof(i.layerHeader), 1, f_);
    }

    constexpr int OFFSET4 = 4;
    fseek(f_, OFFSET4, SEEK_SET);
    fwrite(&headerOff_, sizeof(headerOff_), 1, f_);
}

void RSFile::ReadHeader()
{
    if (f_ == NULL) {
        return;
    }

    fseek(f_, headerOff_, SEEK_SET);

    // READ what was write start time
    fread(&writeStartTime_, 1, sizeof(writeStartTime_), f_);

    // READ PID LIST
    uint32_t recordSize;
    fread(&recordSize, sizeof(recordSize), 1, f_);
    headerPidList_.resize(recordSize);
    fread(&headerPidList_[0], headerPidList_.size(), sizeof(pid_t), f_);

    // ALL TEXTURES
    if (imageMapPtr_ != nullptr) {
        imageMapPtr_->clear();
    }
    fread(&recordSize, sizeof(recordSize), 1, f_);
    for (size_t i = 0; i < recordSize; i++) {
        uint64_t key;
        uint32_t dataSize;
        uint32_t skipBytes;
        uint8_t* data;
        fread(&key, sizeof(key), 1, f_);
        fread(&skipBytes, sizeof(skipBytes), 1, f_);
        fread(&dataSize, sizeof(dataSize), 1, f_);
        data = new uint8_t[dataSize];
        fread(data, dataSize, 1, f_);
        const std::shared_ptr<void> ptr(data);
        if (imageMapPtr_ != nullptr) {
            ReplayImageCacheRecordFile record;
            record.image = ptr;
            record.imageSize = dataSize;
            record.skipBytes = skipBytes;
            imageMapPtr_->insert({ key, record });
        }
    }

    // READ LAYERS OFFSETS
    fread(&recordSize, sizeof(recordSize), 1, f_);
    layerData_.resize(recordSize);
    for (auto& i : layerData_) {
        fread(&i.layerHeader, sizeof(i.layerHeader), 1, f_);
        i.readindexRsData = 0;
    }
}

// ***********************************
// *** LAYER HEADER

uint32_t RSFile::AddLayer()
{
    const uint32_t newId = layerData_.size();
    const RSFileLayer data;
    layerData_.push_back(data);

    wasChanged_ = true;

    return newId;
}

void RSFile::LayerAddHeaderProperty(uint32_t layer, const std::string& name, const std::string& value)
{
    if (layer >= layerData_.size()) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];
    layerData.property.SetProperty(name, value);

    wasChanged_ = true;
}

void RSFile::LayerWriteHeader(uint32_t layer)
{
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    if (f_ == NULL) {
        return;
    }

    if (layer >= layerData_.size()) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];

    const uint32_t layerHeaderOff = writeDataOff_; // position of layer table
    fseek(f_, writeDataOff_, SEEK_SET);

    std::vector<char> propertyData;
    layerData.property.Serialize(propertyData);

    // SAVE LAYER PROPERTY
    uint32_t recordSize = propertyData.size();
    fwrite(&recordSize, sizeof(recordSize), 1, f_);
    ROSEN_LOGD(
        "RSMainThread::MainLoop Server REPLAY WRITE Layer prop_data_size=%d foff=%d", (int)recordSize, (int)ftell(f_));
    fwrite(propertyData.data(), propertyData.size(), 1, f_);

    // SAVE LAYER RSDATA
    recordSize = layerData.rsData.size();
    fwrite(&recordSize, sizeof(recordSize), 1, f_);
    ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY WRITE RSData count=%d", (int)recordSize);
    for (auto item : layerData.rsData) {
        fwrite(&item.first, sizeof(item.first), 1, f_);
        fwrite(&item.second, sizeof(item.second), 1, f_);
        ROSEN_LOGD(
            "RSMainThread::MainLoop Server REPLAY WRITE RSData offset=%d len=%d", (int)item.first, (int)item.second);
    }

    // SAVE LAYER OGLDATA
    recordSize = layerData.oglData.size();
    fwrite(&recordSize, sizeof(recordSize), 1, f_);
    for (auto item : layerData.oglData) {
        fwrite(&item.first, sizeof(item.first), 1, f_);
        fwrite(&item.second, sizeof(item.second), 1, f_);
    }

    // SAVE LAYER RSMetrics
    recordSize = layerData.rsMetrics.size();
    fwrite(&recordSize, sizeof(recordSize), 1, f_);
    for (auto item : layerData.rsMetrics) {
        fwrite(&item.first, sizeof(item.first), 1, f_);
        fwrite(&item.second, sizeof(item.second), 1, f_);
    }

    // SAVE LAYER OGLMetrics
    recordSize = layerData.oglMetrics.size();
    fwrite(&recordSize, sizeof(recordSize), 1, f_);
    for (auto item : layerData.oglMetrics) {
        fwrite(&item.first, sizeof(item.first), 1, f_);
        fwrite(&item.second, sizeof(item.second), 1, f_);
    }

    // SAVE LAYER GFXMetrics
    recordSize = layerData.gfxMetrics.size();
    fwrite(&recordSize, sizeof(recordSize), 1, f_);
    for (auto item : layerData.gfxMetrics) {
        fwrite(&item.first, sizeof(item.first), 1, f_);
        fwrite(&item.second, sizeof(item.second), 1, f_);
    }

    ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY WRITE Layer offset=%d len=%d", (int)layerHeaderOff,
        (int)(ftell(f_) - layerHeaderOff));
    layerData.layerHeader = { layerHeaderOff, ftell(f_) - layerHeaderOff }; // position of layer table

    writeDataOff_ = ftell(f_);
}

void RSFile::LayerReadHeader(uint32_t layer)
{
    if (f_ == NULL) {
        return;
    }

    if (layer >= layerData_.size()) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];

    fseek(f_, layerData.layerHeader.first, SEEK_SET);
    ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY READ Layer offset=%d", (int)layerData.layerHeader.first);

    // READ LAYER PROPERTY
    uint32_t recordSize;
    fread(&recordSize, sizeof(recordSize), 1, f_);
    std::vector<char> propertyData;
    propertyData.resize(recordSize);
    fread(propertyData.data(), recordSize, 1, f_);
    layerData.property.Deserialize(propertyData);
    ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY READ Layer prop_data_size=%d", (int)recordSize);

    // READ LAYER RSDATA
    fread(&recordSize, sizeof(recordSize), 1, f_);
    // ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY READ RSData count=%d", (int)record_size);
    layerData.rsData.resize(recordSize);
    for (size_t i = 0; i < recordSize; i++) {
        fread(&layerData.rsData[i].first, sizeof(layerData.rsData[i].first), 1, f_);
        fread(&layerData.rsData[i].second, sizeof(layerData.rsData[i].second), 1, f_);
        // ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY READ RSData offset=%d len=%d",
        // (int)layer_data.RSData[i].first, (int)layer_data.RSData[i].second);
    }

    // READ LAYER OGLDATA
    fread(&recordSize, sizeof(recordSize), 1, f_);
    layerData.oglData.resize(recordSize);
    for (size_t i = 0; i < recordSize; i++) {
        fread(&layerData.oglData[i].first, sizeof(layerData.oglData[i].first), 1, f_);
        fread(&layerData.oglData[i].second, sizeof(layerData.oglData[i].second), 1, f_);
    }

    // READ LAYER RSMetrics
    fread(&recordSize, sizeof(recordSize), 1, f_);
    layerData.rsMetrics.resize(recordSize);
    for (size_t i = 0; i < recordSize; i++) {
        fread(&layerData.rsMetrics[i].first, sizeof(layerData.rsMetrics[i].first), 1, f_);
        fread(&layerData.rsMetrics[i].second, sizeof(layerData.rsMetrics[i].second), 1, f_);
    }

    // READ LAYER OGLMetrics
    fread(&recordSize, sizeof(recordSize), 1, f_);
    layerData.oglMetrics.resize(recordSize);
    for (size_t i = 0; i < recordSize; i++) {
        fread(&layerData.oglMetrics[i].first, sizeof(layerData.oglMetrics[i].first), 1, f_);
        fread(&layerData.oglMetrics[i].second, sizeof(layerData.oglMetrics[i].second), 1, f_);
    }

    // READ LAYER GFXMetrics
    fread(&recordSize, sizeof(recordSize), 1, f_);
    layerData.gfxMetrics.resize(recordSize);
    for (size_t i = 0; i < recordSize; i++) {
        fread(&layerData.gfxMetrics[i].first, sizeof(layerData.gfxMetrics[i].first), 1, f_);
        fread(&layerData.gfxMetrics[i].second, sizeof(layerData.gfxMetrics[i].second), 1, f_);
    }
}

// ***********************************
// *** LAYER DATA - WRITE

// XXX: static std::map<uint64_t, std::pair<std::shared_ptr<void>, int>> ImageMap;

void RSFile::WriteRSData(double time, void* data, int dataLen)
{
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    if (f_ == NULL) {
        return;
    }
    if (layerData_.empty()) {
        return;
    }

    RSFileLayer& layerData = layerData_[0];
    layerData.rsData.emplace_back(writeDataOff_, dataLen + sizeof(time));

    fseek(f_, writeDataOff_, SEEK_SET);
    fwrite(&time, sizeof(time), 1, f_);
    fwrite(data, dataLen, 1, f_);
    writeDataOff_ = ftell(f_);
}

void RSFile::WriteOGLData(uint32_t layer, double time, void* data, int dataLen)
{
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    if (f_ == NULL) {
        return;
    }
    if (layer >= layerData_.size()) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];
    layerData.oglData.emplace_back(writeDataOff_, dataLen + sizeof(time));

    fseek(f_, writeDataOff_, SEEK_SET);
    fwrite(&time, sizeof(time), 1, f_);
    fwrite(data, dataLen, 1, f_);
    writeDataOff_ = ftell(f_);
}

void RSFile::WriteRSMetrics(uint32_t layer, double time, void* data, int dataLen)
{
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    if (f_ == NULL) {
        return;
    }
    if (layer >= layerData_.size()) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];
    layerData.rsMetrics.emplace_back(writeDataOff_, dataLen + sizeof(time));

    fseek(f_, writeDataOff_, SEEK_SET);
    fwrite(&time, sizeof(time), 1, f_);
    fwrite(data, dataLen, 1, f_);
    writeDataOff_ = ftell(f_);
}

void RSFile::WriteOGLMetrics(uint32_t layer, double time, uint32_t /*frame*/, void* data, int dataLen)
{
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    if (f_ == NULL) {
        return;
    }
    if (layer >= layerData_.size()) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];
    layerData.oglMetrics.emplace_back(writeDataOff_, dataLen + sizeof(time));

    fseek(f_, writeDataOff_, SEEK_SET);
    fwrite(&time, sizeof(time), 1, f_);
    fwrite(data, dataLen, 1, f_);
    writeDataOff_ = ftell(f_);
}

void RSFile::WriteGFXMetrics(uint32_t layer, double time, uint32_t /*frame*/, void* data, int dataLen)
{
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    if (f_ == NULL) {
        return;
    }
    if (layer >= layerData_.size()) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];
    layerData.gfxMetrics.emplace_back(writeDataOff_, dataLen + sizeof(time));

    fseek(f_, writeDataOff_, SEEK_SET);
    fwrite(&time, sizeof(time), 1, f_);
    fwrite(data, dataLen, 1, f_);
    writeDataOff_ = ftell(f_);
}

// ***********************************
// *** LAYER DATA - READ

void RSFile::ReadRSDataRestart()
{
    if (layerData_.empty()) {
        return;
    }

    RSFileLayer& layerData = layerData_[0];
    layerData.readindexRsData = 0;
}

void RSFile::ReadOGLDataRestart(uint32_t layer)
{
    if (layer >= layerData_.size()) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];
    layerData.readindexOglData = 0;
}

void RSFile::ReadRSMetricsRestart(uint32_t layer)
{
    if (layer >= layerData_.size()) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];
    layerData.readindexRsMetrics = 0;
}

void RSFile::ReadOGLMetricsRestart(uint32_t layer)
{
    if (layer >= layerData_.size()) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];
    layerData.readindexOglMetrics = 0;
}

void RSFile::ReadGFXMetricsRestart(uint32_t layer)
{
    if (layer >= layerData_.size()) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];
    layerData.readindexGfxMetrics = 0;
}

bool RSFile::RSDataEOF()
{
    if (f_ == NULL) {
        return true;
    }
    if (layerData_.empty()) {
        return true;
    }

    const RSFileLayer& layerData = layerData_[0];
    // ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY readindex_RSData=%d layer_data.RSData.size()=%d",
    // (int)layer_data.readindex_RSData, (int)layer_data.RSData.size());
    return layerData.readindexRsData >= layerData.rsData.size();
}

bool RSFile::OGLDataEOF(uint32_t layer)
{
    if (f_ == NULL) {
        return true;
    }
    if (layerData_.empty()) {
        return true;
    }
    if (layer >= layerData_.size()) {
        return true;
    }

    const RSFileLayer& layerData = layerData_[layer];
    return layerData.readindexOglData >= layerData.oglData.size();
}

bool RSFile::RSMetricsEOF(uint32_t layer)
{
    if (f_ == NULL) {
        return true;
    }
    if (layerData_.empty()) {
        return true;
    }
    if (layer >= layerData_.size()) {
        return true;
    }

    const RSFileLayer& layerData = layerData_[layer];
    return layerData.readindexRsMetrics >= layerData.rsMetrics.size();
}

bool RSFile::OGLMetricsEOF(uint32_t layer)
{
    if (f_ == NULL) {
        return true;
    }
    if (layerData_.empty()) {
        return true;
    }
    if (layer >= layerData_.size()) {
        return true;
    }

    const RSFileLayer& layerData = layerData_[layer];
    return layerData.readindexOglMetrics >= layerData.oglMetrics.size();
}

bool RSFile::GFXMetricsEOF(uint32_t layer)
{
    if (f_ == NULL) {
        return true;
    }
    if (layerData_.empty()) {
        return true;
    }
    if (layer >= layerData_.size()) {
        return true;
    }

    const RSFileLayer& layerData = layerData_[layer];
    return layerData.readindexGfxMetrics >= layerData.gfxMetrics.size();
}

bool RSFile::ReadRSData(double untilTime, std::vector<uint8_t>& data, double& readTime)
{
    if (f_ == NULL) {
        return false;
    }
    if (layerData_.empty()) {
        return false;
    }

    RSFileLayer& layerData = layerData_[0];

    if (layerData.readindexRsData >= layerData.rsData.size()) {
        return false;
    }

    fseek(f_, layerData.rsData[layerData.readindexRsData].first, SEEK_SET);

    fread(&readTime, sizeof(readTime), 1, f_);
    constexpr double EPSILON = 1e-9;
    if (readTime >= untilTime + EPSILON) {
        return false;
    }

    ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY READ RSData offset=%d len=%d read_time=%lf",
        (int)layerData.rsData[layerData.readindexRsData].first, (int)layerData.rsData[layerData.readindexRsData].second,
        readTime);

    const uint32_t dataLen = layerData.rsData[layerData.readindexRsData].second - sizeof(readTime);
    data.resize(dataLen);
    fread(data.data(), dataLen, 1, f_);

    layerData.readindexRsData++;
    return true;
}

bool RSFile::ReadOGLData(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime)
{
    if (f_ == NULL) {
        return false;
    }
    if (layerData_.empty()) {
        return false;
    }
    if (layer >= layerData_.size()) {
        return false;
    }

    RSFileLayer& layerData = layerData_[layer];

    if (layerData.readindexOglData >= layerData.oglData.size()) {
        return false;
    }

    fseek(f_, layerData.oglData[layerData.readindexOglData].first, SEEK_SET);
    fread(&readTime, sizeof(readTime), 1, f_);
    if (readTime > untilTime) {
        return false;
    }

    const uint32_t dataLen = layerData.oglData[layerData.readindexOglData].second - 8;
    data.resize(dataLen);
    fread(data.data(), dataLen, 1, f_);

    layerData.readindexOglData++;
    return true;
}

bool RSFile::ReadRSMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime)
{
    if (f_ == NULL) {
        return false;
    }
    if (layerData_.empty()) {
        return false;
    }
    if (layer >= layerData_.size()) {
        return false;
    }

    RSFileLayer& layerData = layerData_[layer];

    if (layerData.readindexRsMetrics >= layerData.rsMetrics.size()) {
        return false;
    }

    fseek(f_, layerData.rsMetrics[layerData.readindexRsMetrics].first, SEEK_SET);
    fread(&readTime, sizeof(readTime), 1, f_);
    if (readTime > untilTime) {
        return false;
    }

    const uint32_t dataLen = layerData.rsMetrics[layerData.readindexRsMetrics].second - 8;
    data.resize(dataLen);
    fread(data.data(), dataLen, 1, f_);

    layerData.readindexRsMetrics++;
    return true;
}

bool RSFile::ReadOGLMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime)
{
    if (f_ == NULL) {
        return false;
    }
    if (layerData_.empty()) {
        return false;
    }
    if (layer >= layerData_.size()) {
        return false;
    }

    RSFileLayer& layerData = layerData_[layer];

    if (layerData.readindexOglMetrics >= layerData.oglMetrics.size()) {
        return false;
    }

    fseek(f_, layerData.oglMetrics[layerData.readindexOglMetrics].first, SEEK_SET);
    fread(&readTime, sizeof(readTime), 1, f_);
    if (readTime > untilTime) {
        return false;
    }

    const uint32_t dataLen = layerData.oglMetrics[layerData.readindexOglMetrics].second - 8;
    data.resize(dataLen);
    fread(data.data(), dataLen, 1, f_);

    layerData.readindexOglMetrics++;
    return true;
}

bool RSFile::ReadGFXMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime)
{
    if (f_ == NULL) {
        return false;
    }
    if (layerData_.empty()) {
        return false;
    }
    if (layer >= layerData_.size()) {
        return false;
    }

    RSFileLayer& layerData = layerData_[layer];

    if (layerData.readindexGfxMetrics >= layerData.gfxMetrics.size()) {
        return false;
    }

    fseek(f_, layerData.gfxMetrics[layerData.readindexGfxMetrics].first, SEEK_SET);
    fread(&readTime, sizeof(readTime), 1, f_);
    if (readTime > untilTime) {
        return false;
    }

    const uint32_t dataLen = layerData.gfxMetrics[layerData.readindexGfxMetrics].second - 8;
    data.resize(dataLen);
    fread(data.data(), dataLen, 1, f_);

    layerData.readindexGfxMetrics++;
    return true;
}

// ***********************************
// *** READ/SAVE HEADERS

void RSFile::WriteHeaders()
{
    for (size_t i = 0; i < layerData_.size(); i++) {
        LayerWriteHeader(i);
    }
    WriteHeader();
}

void RSFile::ReadHeaders()
{
    ReadHeader();
    for (size_t i = 0; i < layerData_.size(); i++) {
        LayerReadHeader(i);
    }
}

void RSFile::Close()
{
    if (f_ == NULL) {
        return;
    }

    if (wasChanged_) {
        WriteHeaders();
        wasChanged_ = false;
    }

    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    fclose(f_);
    f_ = NULL;

    headerOff_ = 0;
    headerPidList_.clear();
    layerData_.clear();

    writeDataOff_ = 0;
    wasChanged_ = false;
}

} // namespace OHOS::Rosen