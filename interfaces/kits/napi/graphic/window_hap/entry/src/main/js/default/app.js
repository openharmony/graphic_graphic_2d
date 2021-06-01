/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import window from 'libwindow.z.so'

export default {
    async onCreate() {
        console.info('1')
        console.info('onCreate begin')
        console.info('window: ' + window)
        console.info('window.getTopWindow: ' + window.getTopWindow)
        console.info('window.getTopWindow(): ' + window.getTopWindow())

        console.info('2')
        let wnd = await window.getTopWindow()
        console.info('wnd: ' + wnd)
        console.info('wnd.resetSize: ' + wnd.resetSize)

        console.info('3')
        console.info('wnd.resetSize(200, 200): ' + wnd.resetSize(200, 200))
        console.info('wnd.moveTo(200, 200): ' + wnd.moveTo(200, 200))
        console.info('wnd.setWindowType(1): ' + wnd.setWindowType(1))

        console.info('4')
        console.info('await wnd.resetSize(200, 200): ')
        let ret = await wnd.resetSize(200, 200)
        console.info(ret)

        console.info('5')
        console.info('await wnd.moveTo(200, 200): ')
        ret = await wnd.moveTo(200, 200)
        console.info(ret)

        console.info('6')
        console.info('await wnd.setWindowType(1): ')
        ret = await wnd.setWindowType(3)
        console.info(ret)

        console.info('7')
        console.info('onCreate begin')
        console.info('window: ' + window)
        console.info('window.getTopWindow: ' + window.getTopWindow)
        console.info('window.getTopWindow(): ' + window.getTopWindow())

        console.info('8')
        wnd = new window.Window()
        console.info('wnd: ' + wnd)
        console.info('wnd.resetSize: ' + wnd.resetSize)

        console.info('9')
        console.info('wnd.resetSize(200, 200): ' + wnd.resetSize(200, 200))
        console.info('wnd.moveTo(200, 200): ' + wnd.moveTo(200, 200))
        console.info('wnd.setWindowType(1): ' + wnd.setWindowType(1))

        console.info('10')
        console.info('await wnd.resetSize(200, 200): ')
        ret = await wnd.resetSize(200, 200)
        console.info(ret)

        console.info('11')
        console.info('await wnd.moveTo(200, 200): ')
        ret = await wnd.moveTo(200, 200)
        console.info(ret)

        console.info('12')
        console.info('await wnd.setWindowType(1): ')
        ret = await wnd.setWindowType(3)
        console.info(ret)

        console.info('onCreate end')
    },
    onDestroy() {
        console.info('onDestroy')
    }
}
