/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

export class SphereGraph {
    radius;
    widthSegments;
    heightSegments;
    phiStart;
    phiLength;
    thetaStart;
    thetaLength;
    vertices = [];
    indices = [];
    textureCoord = [];

    constructor(radius = 1, widthSegments = 32, heightSegments = 16, phiStart = 0, phiLength = Math.PI * 2, thetaStart = 0, thetaLength = Math.PI) {
        this.radius = radius;
        this.widthSegments = Math.max(3, Math.floor(widthSegments));
        this.heightSegments = Math.max(2, Math.floor(heightSegments));
        this.phiStart = phiStart;
        this.phiLength = phiLength;
        this.thetaStart = thetaStart;
        this.thetaLength = thetaLength;
        const thetaEnd = Math.min(thetaStart + thetaLength, Math.PI);
        let index = 0;
        const vertices = [];
        const textureCoord = [];
        const indices = [];
        const grid = [];
        const normals = [];
        const uvs = [];
        for (let i = 0; i <= this.heightSegments; i++) {
            const verticesRow = [];
            const v = i / heightSegments;
            let theta = i * Math.PI / this.heightSegments;
            let sinTheta = Math.sin(theta); // x
            let cosTheta = Math.cos(theta); // y
            let uOffset = 0;
            if (i === 0 && thetaStart === 0) {
                uOffset = 0.5 / widthSegments;
            }
            else if (i === heightSegments && thetaEnd === Math.PI) {
                uOffset = -0.5 / widthSegments;
            }
            for (let j = 0; j <= this.widthSegments; j++) {
                const u = j / widthSegments;
                let x = -radius * Math.cos(phiStart + u * phiLength) * Math.sin(thetaStart + v * thetaLength);
                let y = radius * Math.cos(thetaStart + v * thetaLength);
                let z = radius * Math.sin(phiStart + u * phiLength) * Math.sin(thetaStart + v * thetaLength);
                vertices.push(x, y, z);
                textureCoord.push(j / this.widthSegments, i / this.heightSegments);
                verticesRow.push(index++);
            }
            grid.push(verticesRow);
        }
        // indices
        for (let iy = 0; iy < heightSegments; iy++) {
            for (let ix = 0; ix < widthSegments; ix++) {
                const a = grid[iy][ix + 1];
                const b = grid[iy][ix];
                const c = grid[iy + 1][ix];
                const d = grid[iy + 1][ix + 1];
                if (iy !== 0 || this.thetaStart > 0)
                    indices.push(a, b, d);
                if (iy !== heightSegments - 1 || thetaEnd < Math.PI)
                    indices.push(b, c, d);
            }
        }
        this.indices = indices;
        this.vertices = vertices;
        this.textureCoord = textureCoord;
    }
}
