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

import { Matrix4 } from "./Matrix.js";
export class Shape {
    initFinish = false;
    vertexCode = `
        attribute vec4 a_Position;
        attribute vec4 a_Color;
        attribute vec2 a_TexCoord;
        attribute vec4 a_Normal;
        uniform mat4 u_MvpMatrix;
        uniform mat4 u_ModelMatrix;
        uniform mat4 u_NormalMatrix;
        uniform vec4 u_Eye;//for fog
        varying float v_Dist;//for fog
        varying vec4 v_Color;
        varying vec2 v_TexCoord;
        varying vec3 v_Normal;
        varying vec3 v_Position;
        void main() {
            gl_Position = u_MvpMatrix * u_ModelMatrix * a_Position;
            v_Position = vec3(u_ModelMatrix * a_Position);
            v_Normal = normalize(vec3( u_NormalMatrix * a_Normal));
            gl_PointSize = 1.0;
            v_Color = a_Color;
            v_TexCoord = a_TexCoord;
            v_Dist = distance(u_ModelMatrix * a_Position, u_Eye);//for fog
        }
    `;
    fragmentCode = `
        precision mediump float;
        uniform sampler2D u_Sampler;
        varying vec2 v_TexCoord;
        uniform int v_UseTexture;
        uniform float alpha;
        uniform vec3 u_FogColor;//for fog color
        uniform vec2 u_FogDist;//for fog start and end point
        varying float v_Dist;//for fog
        varying vec3 v_Normal;
        varying vec3 v_Position;
        varying vec4 v_Color;
        
        uniform vec3 lightPosition;
        uniform vec3 lightDirection; // (normalized)
        uniform float lightInnerCutoff; // (cosine angle)
        uniform float lightOuterCutoff; // (cosine angle)
        
        void main() {
            vec3 offset = lightPosition - v_Position;
            vec3 surfToLight = normalize(offset);
            vec3 lightToSurf = -surfToLight;
        
            float diffuse = max( 0.0 , dot(surfToLight, normalize(v_Normal)));
            float angleToSurface = dot(lightDirection , lightToSurf);
            float spot = smoothstep(lightOuterCutoff,lightInnerCutoff,angleToSurface);
            float brightness = diffuse * spot;
            vec3 ambient = vec3(0.5,0.5,0.5) ;
            
            //fog
            float fogFactory = clamp((u_FogDist.y - v_Dist) / (u_FogDist.y - u_FogDist.x), 0.0, 1.0);
            
            if(v_UseTexture > 0){
                vec4 color = texture2D(u_Sampler,v_TexCoord);
                vec4 color1 = vec4(  brightness * diffuse +ambient ,alpha) ;
                vec3 color2 = mix(u_FogColor, vec3(color1), fogFactory); //fog
                gl_FragColor = vec4(color2,alpha) * color; //fog
            }else {
                gl_FragColor = vec4(diffuse + ambient, alpha) * v_Color;
            }
        }
    `;
    program;
    vertexShader;
    fragmentShader;
    vertices = [];
    indices = [];
    color = [];
    uvs = [];
    normal = [];
    static normalMatrix = new Matrix4();
    static modelMatrix = new Matrix4();
    static fogColor = new Float32Array([0.00, 0.00, 0.0]);
    static fog = new Float32Array([16, 19]);
    static lightInner = Math.cos(Math.PI / 12);
    static lightOuter = Math.cos(Math.PI / 6);
    static lightDirection = new Float32Array([-0.0, -1, -0.0]);
    constructor() {
    }
    createProgram(gl,attribute = [], uniform = []) {
        let initShader = (gl, type, source) => {
            let shader = gl.createShader(type);
            if (shader === null) {
                gl.deleteShader(shader);
                return null;
            }
            gl.shaderSource(shader, source);
            gl.compileShader(shader);
            if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
                console.log(gl.getShaderInfoLog(shader));
                gl.deleteShader(shader);
                return null;
            }
            return shader;
        };
        let vShader = initShader(gl, gl.VERTEX_SHADER, this.vertexCode);
        let fShader = initShader(gl, gl.FRAGMENT_SHADER, this.fragmentCode);
        if (vShader === null || fShader === null) {
            return null;
        }
        let program = gl.createProgram();
        if (program === null) {
            gl.deleteProgram(program);
            return null;
        }
        gl.attachShader(program, vShader);
        gl.attachShader(program, fShader);
        gl.linkProgram(program);
        if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
            console.log(gl.getProgramInfoLog(program));
            gl.deleteProgram(program);
            return null;
        }
        this.program = program;
        this.vertexShader = vShader;
        this.fragmentShader = fShader;
        if (attribute.length > 0) {
            attribute.forEach((item, index) => {
                this.program[item] = gl.getAttribLocation(program, item);
            });
        }
        if (uniform.length > 0) {
            uniform.forEach((item, index) => {
                this.program[item] = gl.getUniformLocation(program, item);
            });
        }
    }
    initAttributeUniform(gl,attribute = [], uniform = []){
        if (attribute.length > 0) {
            attribute.forEach((item, index) => {
                this.program[item] = gl.getAttribLocation(this.program, item);
            });
        }
        if (uniform.length > 0) {
            uniform.forEach((item, index) => {
                this.program[item] = gl.getUniformLocation(this.program, item);
            });
        }
    }
    render(mode, gl) {
        gl.drawElements(mode, this.indices.length, gl.UNSIGNED_BYTE, 0);
    }
    renderPoint(gl) {
        gl.drawArrays(gl.POINTS, 0, this.vertices.length / 3);
    }
    init(gl, camera) {
    }
}
