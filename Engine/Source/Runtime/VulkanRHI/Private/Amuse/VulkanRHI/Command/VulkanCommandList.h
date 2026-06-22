//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/CommandList.h>
#include <Amuse/RHI/Types/CommandListDesc.h>
#include <Amuse/RHI/Query.h>
#include <Amuse/Core/Utility/Swapper.h>
#include <Amuse/VulkanRHI/Command/VulkanResourceStateCache.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    class VulkanDevice;

    //@―---------------------------------------------------------------------------
    //! @brief  コマンドリスト実装(DirectX12)
    //@―---------------------------------------------------------------------------
    class VulkanCommandList:public CommandList {
    public:

        //===============================================================
        // コンストラクタ / デストラクタ
        //===============================================================

        //@―---------------------------------------------------------------------------
        //! @brief  コンストラクタ
        //@―---------------------------------------------------------------------------
        VulkanCommandList(VulkanDevice& device,const CommandListDesc& desc);

        ~VulkanCommandList() override;

        //! @brief      名前を取得
        const String& getName()const override;


        //===============================================================
        // コマンド
        //===============================================================

#pragma region Command

        //! @brief      記録開始
        void begin() override;

        //! @brief      記録終了
        void end() override;

        //! @brief      自動バリアを有効化
        void setEnableAutomaticBarriers(bool enable) override;

        //! @brief      テクスチャ遷移バリアを追加
        void addTextureBarrier(const TextureBarrier& barrier) override;

        //! @brief      バッファ遷移バリアを追加
        void addBufferBarrier(const BufferBarrier& barrier) override;

        //! @brief      テクスチャUAVバリアを追加
        void addUAVBarrier(const Ref<Texture>& texture) override;

        //! @brief      バッファUAVバリアを追加
        void addUAVBarrier(const Ref<Buffer>& buffer) override;

        //! @brief      追加済みバリアを記録
        void commitBarriers() override;

        //! @brief      バッファをコピー
        void copyBuffer(const BufferCopyRegion& region) override;

        //! @brief      テクスチャをコピー
        void copyTexture(const TextureCopyRegion& region) override;

        //! @brief      バッファからテクスチャへコピー
        void copyBufferToTexture(const BufferTextureCopyRegion& region) override;

        //! @brief      テクスチャからバッファへコピー
        void copyTextureToBuffer(const BufferTextureCopyRegion& region) override;

        //! @brief      テクスチャへデータを書き込む
        void writeTexture(const Ref<Texture>& texture, const TextureSubresource& subresource, BlobView data, u32 rowPitch, u32 depthPitch) override;

        //! @brief      カラーテクスチャをクリア
        void clearColorTexture(const Ref<Texture>& texture, const TextureSubresourceRange& range, const Color& color) override;

        //! @brief      深度ステンシルテクスチャをクリア
        void clearDepthStencilTexture(const Ref<Texture>& texture, const TextureSubresourceRange& range, bool clearDepth, f32 depth, bool clearStencil, u8 stencil) override;

        //! @brief      バッファを32bit整数値でクリア
        void clearBufferUInt(const Ref<Buffer>& buffer, u32 value) override;

        //! @brief      テクスチャをResolve
        void resolveTexture(const Ref<Texture>& source, const Ref<Texture>& target, const TextureSubresourceRange& sourceRange, const TextureSubresourceRange& targetRange) override;

        void beginRenderPass(const BeginPassParam& param) override;
        // virtual void nextSubpass();
        void endRenderPass() override;

        //! @brief      スワップチェーンにテクスチャを適用
        void applySwapChain(const Ref<SwapChain>& swapChain, const Ref<RenderTexture>& texture) override;

        //! @brief      シザー矩形を設定
        void setScissorRect(const IntRect* pRect, s32 num) override;

        //! @brief      ビューポートを設定
        void setViewport(const Viewport* pViewport, s32 num) override;

        //! @brief      頂点バッファを設定
        void setVertexBuffers(Span<Ref<Buffer>> buffers, s32 first)override;

        //! @brief      インデックスバッファを設定
        void setIndexBuffer(const Ref<Buffer>&)override;

        //! @brief      パイプラインステートを設定
        void setPipelineState(const Ref<PipelineState>&) override;

        //! @brief      コンピュートパイプラインステートを設定
        void setComputePipelineState(const Ref<ComputePipelineState>&) override;

        //! @brief      描画
        void draw(const DrawParam&)override;

        //! @brief      インデックス描画
        void drawIndexed(const DrawIndexedParam&)override;

        //! @brief      コンピュートディスパッチ
        void dispatch(const DispatchParam&) override;

        //! @brief      Indirect描画
        void drawIndirect(const IndirectCommandParam&) override;

        //! @brief      Indirectインデックス描画
        void drawIndexedIndirect(const IndirectCommandParam&) override;

        //! @brief      Indirectコンピュートディスパッチ
        void dispatchIndirect(const IndirectCommandParam&) override;


        //! @brief      デスクリプタ・テーブルを設定
        void setDescriptorTables(const Amuse::RHI::SetDescriptorTableParam*, s32 num) override;


        //! @brief      ルート定数を設定
        void setRootConstant(const SetRootConstantsParam&) override;


        //! @brief      GPUマーカーをプッシュ
        void pushMarker(StringView name) override;


        //! @brief      GPUマーカーをポップ
        void popMarker() override;

        //! @brief      GPUイベントクエリを設定
        void setEventQuery(const Ref<EventQuery>& query) override;

        //! @brief      GPUタイマークエリを開始
        void beginTimerQuery(const Ref<TimerQuery>& query) override;

        //! @brief      GPUタイマークエリを終了
        void endTimerQuery(const Ref<TimerQuery>& query) override;

#pragma endregion

    public:

		vk::CommandBuffer getNative() {
			return *m_commandBuffer;
		}
        Span<Ref<EventQuery>> getEventQueries() { return m_eventQueries; }
        void clearEventQueries() { m_eventQueries.clear(); }

    private:

        void clearRenderTargets();

		struct DepthStencilAttachmentState {
			Ref<RenderTexture> texture;
			vk::ImageLayout layout;
			vk::ImageAspectFlags aspectMask;
		};

    private:

        VulkanDevice& m_device;

        const CommandListDesc m_desc;
		
        vk::raii::CommandPool		m_commandPool = nullptr;
        vk::raii::CommandBuffer     m_commandBuffer = nullptr;

		Ref<PipelineState> m_pipeline;                          // 現在のパイプラインステート
		Ref<ComputePipelineState> m_computePipeline;             // 現在のコンピュートパイプラインステート
        RenderTargetArray m_colorTextures;                     // 現在の描画ターゲット(クリア用)
        Vector<DepthStencilAttachmentState> m_depthStencilTextures; // 現在の深度ステンシルターゲット(クリア用)

        VulkanResourceStateCache m_cache;
        bool m_enableAutomaticBarriers = true;
        Vector<Ref<Buffer>> m_temporaryBuffers;                 // コマンド実行まで保持する一時バッファ
        Vector<Ref<EventQuery>> m_eventQueries;                 // 実行完了と紐付けるイベントクエリ

    };

}// Amuse::RHI 
