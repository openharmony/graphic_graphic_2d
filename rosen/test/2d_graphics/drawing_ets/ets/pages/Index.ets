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

import {TestBase} from './testbase';
import {CaseFactory} from './casefactory';
import {MyNodeController} from './myxnode';
import {Global} from './global';
import globalThis from '../utils/globalThis'
import resourceManager from '@ohos.resourceManager';
import image from '@ohos.multimedia.image';
import componentSnapshot from '@ohos.arkui.componentSnapshot';
import fs from '@ohos.file.fs'

const TAG = '[DrawingTest]';

@Entry
@Component
struct Index {
  //用来配置CanvasRenderingContext2D对象的参数，包括是否开启抗锯齿，true表明开启抗锯齿。
  private settings: RenderingContextSettings = new RenderingContextSettings(true)
  //用来创建CanvasRenderingContext2D对象，通过在canvas中调用CanvasRenderingContext2D对象来绘制。
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings)
  @State testType: string = 'function';
  @State drawingType: string = 'gpu';
  @State caseNameStr: string  = 'canvasdrawimage';
  @State information: string = '';
  @State testAll: boolean = false;
  caseNameStrBefore: string  = '';
  scroller: Scroller = new Scroller();
  caseCountStr: string = '1000';
  myNodeController = new MyNodeController();

  build() {
    Column() {
      Row() {
        if (this.drawingType == 'cpu') {
          Canvas(this.context)
            .width('100%')
            .height('100%')
            .backgroundColor('#000000')
        } else {
          NodeContainer(this.myNodeController)
            .width('100%')
            .height('100%')
            .backgroundColor('#000000')
            .onAppear(()=>{
              console.warn(TAG, 'MyNodeController onAppear');
            })
            .id('myNodeController')
        }
      }
      .height('65%')

      Row() {
        Select([
          { value: 'function' },
          { value: 'performance' },
        ])
          .id('testType')
          .selected(0)
          .value(this.testType)
          .padding({left:5})
          .border({width:1})
          .width('40%')
          .onSelect((index: number, value: string)=>{
            this.testType = value;
            console.log(TAG, "select: " + this.testType);
          })

        Select([
          { value: 'cpu' },
          { value: 'gpu' },
        ])
          .id('drawingType')
          .selected(0)
          .value(this.drawingType)
          .padding({left:5})
          .border({width:1})
          .width('40%')
          .onSelect((index: number, value: string)=>{
            this.drawingType = value;
            console.log(TAG, "select: " + this.drawingType);
          })
      }
      .height('7%')
      Row() {
        Text('TestCase')
          .fontSize(14)
          .textAlign(TextAlign.End)
          .height(38)
          .margin({right:5})
        Column() {
          TextInput({ placeholder: 'ex:drawrect', text: this.caseNameStr})
            .id('caseName')
            .enterKeyType(EnterKeyType.Send)
            .placeholderFont({size: 15})
            .height(38)
            .width('60%')
            .border({width: 1, radius:10, color: '#88000000'})
            .onChange((value) => {
              if (value != "") {
                this.caseNameStr = value
              }
            })
        }
      }
      .height('7%')
      .width('80%')
      .margin({left:5})

      if(this.testType == 'performance') {
        Row() {
          Text('TestCount')
            .fontSize(14)
            .textAlign(TextAlign.End)
            .height(38)
            .margin({right:5})
          Column() {
            TextInput({ placeholder: '10', text: this.caseCountStr})
              .id('testCount')
              .enterKeyType(EnterKeyType.Send)
              .placeholderFont({size: 15})
              .height(38)
              .width('50%')
              .border({width: 1, radius:10, color: '#88000000'})
              .onChange((value) => {
                if (value != "") {
                  this.caseCountStr = value
                }
              })
          }
          if (this.drawingType == 'gpu') {
            Button("Clear")
              .fontSize('16fp')
              .id('Clear')
              .fontWeight(500)
              .onClick(() => {
                console.log(TAG, "Clear click");
                this.myNodeController.Clear();
              })
              .width('30%')
              .shadow(ShadowStyle.OUTER_DEFAULT_LG)
          }
        }
        .width('80%')
        .height('7%')
      } else if (this.testType == 'function'){
        if (this.drawingType == 'cpu') {
          Row(){
            Button("TestAll")
              .fontSize('16fp')
              .id('testAll')
              .fontWeight(500)
              .onClick(() => {
                console.log(TAG, "TestAll click");
                this.caseNameStr = 'All';
                this.Draw();
              })
              .width('80%')
              .shadow(ShadowStyle.OUTER_DEFAULT_LG)
          }
          .height('7%')
        } else if (this.drawingType == 'gpu'){
          Row(){
            Button("SnapShort")
              .fontSize('16fp')
              .id('SnapShort')
              .fontWeight(500)
              .onClick(() => {
                const context : Context = getContext(this);
                componentSnapshot.get("myNodeController", (error:Error, pixmap: image.PixelMap) => {
                  if (error) {
                    console.log("error:", JSON.stringify(error))
                    return;
                  }
                  Global.savePixmap(context.cacheDir + '/gpu', this.caseNameStr, pixmap);
                })
              })
              .width('80%')
              .shadow(ShadowStyle.OUTER_DEFAULT_LG)
          }
          .height('7%')
        }
      }

      Row() {
        Button("draw")
          .id('draw')
          .fontSize('16fp')
          .fontWeight(500)
          .onClick(() => {
            console.log(TAG, "Draw click");
            this.Draw();
          })
          .width('80%')
          .shadow(ShadowStyle.OUTER_DEFAULT_LG)
      }
      .height('7%')
      Row() {
        Text(this.information)
          .fontSize(20)
          .textAlign(TextAlign.Center)
          .height(38)
          .id('infoText')
      }
      .height('7%')
    }
  }
  LogMsg(msg: string) {
    this.information = msg;
    console.log(TAG, msg);
  }
  async aboutToAppear() {
    const context : Context = getContext(this);
    try {
      const cpuPath : string = context.cacheDir + "/cpu";
      await fs.mkdir(cpuPath, true);
      const gpuPath = context.cacheDir + "/gpu";
      await fs.mkdir(gpuPath, true);
    } catch(e) {
      console.log("mkdir cpuPath or gpuPath failed " + JSON.stringify(e));
    }
    try {
      let resourceMgr: resourceManager.ResourceManager = getContext(this).resourceManager
      let imageResources: string[] = []
      imageResources.push("test.jpg")

      for (let i = 0; i < imageResources.length; i++) {
        let imagePath = imageResources[i]
        let fileData = await resourceMgr.getRawFileContent(imagePath)
        const buffer = fileData.buffer
        const imageSource = image.createImageSource(buffer)
        imageSource.createPixelMap((err: Error, PixelMap: image.PixelMap) => {
          globalThis.getInstance().setPixelMap(imagePath, PixelMap)
          console.log("www data 0 in createPixelMap err is " + JSON.stringify(err));
        })
      }
    } catch (e) {
    }

    this.myNodeController.SetPrintCallback((str:string)=>{
      this.LogMsg(str);
    })
  }
  async Draw():Promise<void> {
    console.log(TAG, 'Drawing test start!');
    if (this.testType == 'function') {
      if (this.drawingType == 'cpu') {
        return this.TestFunctionalCpu();
      } else {
        this.myNodeController.TestFunctionalGpuUpScreen(this.caseNameStr);
        return;
      }
    }
    if (this.testType == 'performance') {
      if (this.drawingType == 'cpu') {
        return this.TestPerformanceCpu();
      } else {
        let testCount = Number.parseInt(this.caseCountStr, 10);
        this.myNodeController.TestPerformanceGpuUpScreen(this.caseNameStr, testCount);
        return;
      }
    }
  }

  async TestFunctionalCpu() {
    console.log(TAG, 'TestFunctionalCpu start');
    const context : Context = getContext(this);
    if (this.caseNameStr == 'All') {
      CaseFactory.getFunctonCpuAllCase().forEach((func: Function, key: string) => {
        if (func) {
          let test:TestBase = func();
          test.fileName_ = key;
          test.TestFunctionCpu(context.cacheDir);
        }
      });
      this.LogMsg('TestFunctionalCpu All end');
    } else {
      let test:TestBase = CaseFactory.getFunctonCpuCase(this.caseNameStr);
      if (test == null || test == undefined) {
        this.LogMsg('Testcase name is invalid');
        return;
      }
      // 异步处理 绘制到屏幕上
      test.fileName_ = this.caseNameStr;
      await test.TestFunctionCpu(context.cacheDir).then(()=>{
        this.context.clearRect(0, 0, test.width_, test.height_);
        this.context.drawImage(test.pixelMap_, 0, 0);
      });
      this.LogMsg('TestFunctionalCpu end');
    }
  }

  async TestPerformanceCpu() {
    console.log(TAG, 'TestPerformanceCpu start');
    let test:TestBase = CaseFactory.getPerformanceCpuCase(this.caseNameStr);
    if (test == null || test == undefined) {
      this.LogMsg('Testcase name is invalid');
      return;
    }
    let testCount = Number.parseInt(this.caseCountStr, 10)
    test.SetTestCount(testCount);

    test.TestPerformanceCpu().then(()=>{
      this.context.clearRect(0, 0, test.width_, test.height_);
      this.context.drawImage(test.pixelMap_, 0, 0);
      this.LogMsg('Count: ' + testCount + ', used: ' + test.time_.toString() + 'ms');
    });
    console.log(TAG, 'TestPerformanceCpu end');
  }
}
