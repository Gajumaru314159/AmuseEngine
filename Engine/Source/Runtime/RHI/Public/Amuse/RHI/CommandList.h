//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Core/Geometry/Viewport.h>
#include <Amuse/RHI/Forward.h>
#include <Amuse/RHI/GraphicObject.h>
#include <Amuse/RHI/Types/CommandListDesc.h>
#include <Amuse/RHI/Types/CommandParam.h>
#include <Amuse/RHI/Types/RenderPassDesc.h>
#include <Amuse/RHI/RenderTexture.h>
#include <Amuse/RHI/Types/Barrier.h>
#include <Amuse/RHI/Types/Copy.h>

namespace Amuse {
    //! @brief      コマンドリスト
    //! @ingroup AmuseRHI
    class CommandList :public GraphicObject {
    public:

        //! @brief インスタンスを生成する
        static Ref<CommandList> Create(const CommandListDesc& desc);

    public:

        //! @brief コマンド記録を開始する
        virtual void begin() = 0;
        //! @brief コマンド記録を終了する
        virtual void end() = 0;
        //! @brief 自動バリアの有効状態を設定する
        virtual void setEnableAutomaticBarriers(bool enable) = 0;
        //! @brief テクスチャバリアを追加する
        virtual void addTextureBarrier(const TextureBarrier& barrier) = 0;
        //! @brief バッファバリアを追加する
        virtual void addBufferBarrier(const BufferBarrier& barrier) = 0;
        //! @brief UAV バリアを追加する
        virtual void addUAVBarrier(const Ref<Texture>& texture) = 0;
        //! @brief UAV バリアを追加する
        virtual void addUAVBarrier(const Ref<Buffer>& buffer) = 0;
        //! @brief 登録済みバリアを反映する
        virtual void commitBarriers() = 0;

        //! @brief バッファをコピー
        virtual void copyBuffer(const BufferCopyRegion& region) = 0;

        //! @brief テクスチャをコピー
        virtual void copyTexture(const TextureCopyRegion& region) = 0;

        //! @brief バッファからテクスチャへコピー
        virtual void copyBufferToTexture(const BufferTextureCopyRegion& region) = 0;

        //! @brief テクスチャからバッファへコピー
        virtual void copyTextureToBuffer(const BufferTextureCopyRegion& region) = 0;

        //! @brief テクスチャへデータを書き込む
        virtual void writeTexture(const Ref<Texture>& texture, const TextureSubresource& subresource, BlobView data, u32 rowPitch, u32 depthPitch = 0) = 0;

        //! @brief カラーテクスチャをクリア
        virtual void clearColorTexture(const Ref<Texture>& texture, const TextureSubresourceRange& range, const Color& color) = 0;

        //! @brief 深度ステンシルテクスチャをクリア
        virtual void clearDepthStencilTexture(const Ref<Texture>& texture, const TextureSubresourceRange& range, bool clearDepth, f32 depth, bool clearStencil, u8 stencil) = 0;

        //! @brief バッファを32bit整数値でクリア
        virtual void clearBufferUInt(const Ref<Buffer>& buffer, u32 value) = 0;

        //! @brief MSAAテクスチャをResolve
        virtual void resolveTexture(const Ref<Texture>& source, const Ref<Texture>& target, const TextureSubresourceRange& sourceRange, const TextureSubresourceRange& targetRange) = 0;

        //! @brief レンダーパスを開始する
        virtual void beginRenderPass(const BeginPassParam& param) = 0;
        //! @brief レンダーパスを終了する
        virtual void endRenderPass() = 0;

        //! @brief スワップチェーンの表示先を適用する
        virtual void applySwapChain(const Ref<SwapChain>& swapChain, const Ref<RenderTexture>& texture) = 0;

        virtual void setViewport(const Viewport* pViewport, s32 num) = 0;  //!< ビューポートを設定
        virtual void setScissorRect(const IntRect* pRect, s32 num) = 0;    //!< シザー矩形を設定

                void setVertexBuffer(const Ref<Buffer>& buffer);         //!< 頂点バッファ設定
        virtual void setVertexBuffers(Span<Ref<Buffer>> buffers,s32 first = 0) = 0;         //!< 頂点バッファ設定
        virtual void setIndexBuffer(const Ref<Buffer>& buffer) = 0;                 //!< インデックスバッファ設定


        virtual void setPipelineState(const Ref<PipelineState>&) = 0;        //!< パイプライン設定
        virtual void setComputePipelineState(const Ref<ComputePipelineState>&) = 0;  //!< コンピュートパイプライン設定

        // setBlendFactor
        // setStencilRef

        //! @brief setDescriptorTables を実行する
        virtual void setDescriptorTables(const SetDescriptorTableParam*, s32 num) = 0;
        //! @brief setRootConstant を実行する
        virtual void setRootConstant(const SetRootConstantsParam&) = 0;


        //! @brief 描画コマンドを記録する
        virtual void draw(const DrawParam& param) = 0;
        //! @brief インデックス付き描画コマンドを記録する
        virtual void drawIndexed(const DrawIndexedParam& param) = 0;
        //! @brief コンピュートディスパッチを記録する
        virtual void dispatch(const DispatchParam& param) = 0;
        //! @brief コンピュートディスパッチを記録する
        void dispatch(u32 groupX, u32 groupY = 1, u32 groupZ = 1);
        //! @brief drawIndirect を実行する
        virtual void drawIndirect(const IndirectCommandParam& param) = 0;
        //! @brief drawIndexedIndirect を実行する
        virtual void drawIndexedIndirect(const IndirectCommandParam& param) = 0;
        //! @brief dispatchIndirect を実行する
        virtual void dispatchIndirect(const IndirectCommandParam& param) = 0;


        //! @brief pushMarker を実行する
        virtual void pushMarker(StringView name) = 0;
        //! @brief popMarker を実行する
        virtual void popMarker() = 0;
        //! @brief setEventQuery を実行する
        virtual void setEventQuery(const Ref<EventQuery>& query) = 0;
        //! @brief beginTimerQuery を実行する
        virtual void beginTimerQuery(const Ref<TimerQuery>& query) = 0;
        //! @brief endTimerQuery を実行する
        virtual void endTimerQuery(const Ref<TimerQuery>& query) = 0;

    };


    inline void CommandList::setVertexBuffer(const Ref<Buffer>& buffer) {
        Ref<Buffer> buffers[1];
        buffers[0] = buffer;
        setVertexBuffers(buffers);
    }

    inline void CommandList::dispatch(u32 groupX, u32 groupY, u32 groupZ) {
        dispatch(DispatchParam{ groupX, groupY, groupZ });
    }

}
