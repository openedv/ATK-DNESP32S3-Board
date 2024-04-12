/*
 * Copyright [2021] Mauro Riva <info@lemariva.com>
 * 
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

#ifndef MICROPY_INCLUDED_ESP32_MODCAMERA_H
#define MICROPY_INCLUDED_ESP32_MODCAMERA_H

enum { OV2640, OV7725};

#define TAG "camera"

/*
#define CAM_PIN_PWDN    -1
#define CAM_PIN_RESET   -1
#define CAM_PIN_XCLK    10
#define CAM_PIN_SIOD    21
#define CAM_PIN_SIOC    14

#define CAM_PIN_D7      11
#define CAM_PIN_D6      9
#define CAM_PIN_D5      8
#define CAM_PIN_D4      16
#define CAM_PIN_D3      6
#define CAM_PIN_D2      4
#define CAM_PIN_D1      5
#define CAM_PIN_D0      17
#define CAM_PIN_VSYNC   13
#define CAM_PIN_HREF    12
#define CAM_PIN_PCLK    18
*/

//
#define CAM_PIN_PWDN    -1
#define CAM_PIN_RESET   -1
#define CAM_PIN_XCLK    -1
#define CAM_PIN_SIOD    39
#define CAM_PIN_SIOC    38

#define CAM_PIN_D7      18
#define CAM_PIN_D6      17
#define CAM_PIN_D5      16
#define CAM_PIN_D4      15
#define CAM_PIN_D3      7
#define CAM_PIN_D2      6
#define CAM_PIN_D1      5
#define CAM_PIN_D0      4
#define CAM_PIN_VSYNC   47
#define CAM_PIN_HREF    48
#define CAM_PIN_PCLK    45
//

#define XCLK_FREQ_10MHz    10000000
#define XCLK_FREQ_20MHz    20000000
#define XCLK_FREQ_24MHz    24000000

//White Balance
#define WB_NONE     0
#define WB_SUNNY    1
#define WB_CLOUDY   2
#define WB_OFFICE   3
#define WB_HOME     4

//Special Effect  
#define EFFECT_NONE    0
#define EFFECT_NEG     1
#define EFFECT_BW      2
#define EFFECT_RED     3
#define EFFECT_GREEN   4
#define EFFECT_BLUE    5
#define EFFECT_RETRO   6

#define MODULE_CAMERA_ENABLED   1

#endif
