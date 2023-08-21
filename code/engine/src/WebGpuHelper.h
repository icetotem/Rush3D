#pragma once

#include <webgpu/webgpu.h>
#include "Platform.h"

namespace webgpu
{

    WGPUDevice create(rush::WindowHandle window, WGPUBackendType type = WGPUBackendType_Force32);

    WGPUSwapChain createSwapChain(WGPUDevice device, int width, int height);

    WGPUTextureFormat getSwapChainFormat(WGPUDevice device);

}
