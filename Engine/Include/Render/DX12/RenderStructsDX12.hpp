#pragma once
#include "Render/RenderStructs.hpp"
#include "Tools/Tools.hpp"

namespace FS::DX12
{
    struct Heap
    {
        ID3D12Heap* BaseHeap = nullptr;
        u32 Size = 0;
        u32 Offset = 0;
    };

    struct Descriptor
    {
        D3D12_CPU_DESCRIPTOR_HANDLE Cpu{};
        D3D12_GPU_DESCRIPTOR_HANDLE Gpu{};
        u32 Index = 0;
    };

    struct DescriptorAllocator
    {
        ID3D12DescriptorHeap* Heap;
        D3D12_DESCRIPTOR_HEAP_TYPE Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        D3D12_CPU_DESCRIPTOR_HANDLE CpuBase = {};
        D3D12_GPU_DESCRIPTOR_HANDLE GpuBase = {};
        u32 Stride = {};
        u32 At = {};
        u32 Capacity = {};
        std::vector<u32> FreeIndices;

        Descriptor Allocate()
        {
            u32 index;
            if (FreeIndices.empty())
            {
                index = At++;
            }
            else
            {
                index = FreeIndices.back();
                FreeIndices.pop_back();
            }

            return {
                .Cpu = {CpuBase.ptr + Stride * index},
                .Gpu = {GpuBase.ptr + Stride * index},
                .Index = index,
            };
        }

        void Free(const Descriptor& descriptor) { FreeIndices.push_back(descriptor.Index); }

        void Free(const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, const D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) 
        {
            const int cpu_idx = static_cast<int>((cpuHandle.ptr - CpuBase.ptr) / Stride);
            const int gpu_idx = static_cast<int>((gpuHandle.ptr - GpuBase.ptr) / Stride);
            if (cpu_idx != gpu_idx)
            {
                ThrowError("Freeing an incorrect Descriptor");
            }
            FreeIndices.push_back(cpu_idx);
        }
    };

    struct Command
    {
        ID3D12CommandAllocator* CommandAllocator = nullptr;
        ID3D12GraphicsCommandList10* CommandList = nullptr;
    };

    struct Resource
    {
        ID3D12Resource2* BaseResource = nullptr;
        D3D12_RESOURCE_STATES ResourceState = D3D12_RESOURCE_STATE_COMMON;
    };

    struct Texture
    {
        Descriptor RtvDescriptor;
        Descriptor DsvDescriptor;
        Descriptor SrvDescriptor;
        ResourceHandle ResourceHandle = ResourceHandle::eNull;
    };

    struct Buffer
    {
        Descriptor Descriptor;
        ResourceHandle ResourceHandle = ResourceHandle::eNull;
        BufferType BufferType = BufferType::eStorage;
    };
} // namespace FS::DX12