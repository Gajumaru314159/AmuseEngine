//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/Buffer.h>
#include <Amuse/RHI/RHI.h>
#include <Amuse/RHI/Types/CommandParam.h>

namespace Amuse {
    //! @brief  空のバッファを作成
    Ref<Buffer> Buffer::Create(const BufferDesc& desc) {
        if (auto device = Device::Get()) {
            return device->createBuffer(desc);
        }
        return nullptr;
    }


    //! @brief  初期データを指定してバッファを作成
    Ref<Buffer> Buffer::Create(const BufferDesc& desc, BlobView blob) {
        auto buffer = Create(desc);
        if (buffer) {
            buffer->update(blob.size(), blob.data());
        }
        return buffer;
    }


    //! @brief  ベースのバッファを指定して異なるビューを持つバッファを作成
    Ref<Buffer> Buffer::Create(const BufferViewDesc& desc) {
        if (auto device = Device::Get()) {
            return device->createBuffer(desc);
        }
        return nullptr;
    }


    //! @brief      drawIndirect() 用の引数を書き込む
    void Buffer::writeDrawParam(const DrawParam& param, u64 offset) {
        writeDrawParams(Span<const DrawParam>(&param, 1), offset);
    }


    //! @brief      drawIndexedIndirect() 用の引数を書き込む
    void Buffer::writeDrawIndexedParam(const DrawIndexedParam& param, u64 offset) {
        writeDrawIndexedParams(Span<const DrawIndexedParam>(&param, 1), offset);
    }


    //! @brief      dispatchIndirect() 用の引数を書き込む
    void Buffer::writeDispatchParam(const DispatchParam& param, u64 offset) {
        writeDispatchParams(Span<const DispatchParam>(&param, 1), offset);
    }

}
