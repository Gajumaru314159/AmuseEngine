//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/CommandList.h>
#include <Amuse/RHI/Types/CommandListDesc.h>
#include <Amuse/RHI/Query.h>
#include <Amuse/Core/Utility/Swapper.h>
#include <Amuse/DirectX12RHI/Command/ResourceStateCache.h>

namespace Amuse {
    class DirectX12RootSignature;

    //! @brief  コマンドリスト実装(DirectX12)
    //! @ingroup AmuseDirectX12RHI
    class DirectX12CommandList:public CommandList {
    public:

        //===============================================================
        // コンストラクタ / デストラクタ
        //===============================================================

        //! @brief  コンストラクタ
        DirectX12CommandList(class DirectX12Device& device, const CommandListDesc& desc);


        //! @brief  妥当な状態か
        bool isValid()const;

        
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

		 //! @brief      RenderPass開始
         void beginRenderPass(const BeginPassParam& param) override;

         //! @brief RenderPass終了
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
        void setDescriptorTables(const Amuse::SetDescriptorTableParam*, s32 num) override;


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

        ID3D12GraphicsCommandList5* getNative()const { return m_cmdList.Get(); }
        Span<Ref<EventQuery>> getEventQueries() { return m_eventQueries; }
        void clearEventQueries() { m_eventQueries.clear(); }

    private:

        void clearRenderTargets();
        void createCommandSignatures();

    private:

        class DirectX12Device& m_device;
        const CommandListDesc m_desc;

        ComPtr<ID3D12CommandAllocator> m_cmdAllocator;
        ComPtr<ID3D12GraphicsCommandList6> m_cmdList;
        ComPtr<ID3D12CommandSignature> m_drawCommandSignature;
        ComPtr<ID3D12CommandSignature> m_drawIndexedCommandSignature;
        ComPtr<ID3D12CommandSignature> m_dispatchCommandSignature;

        BeginPassParam m_currentRenderPass;

        RenderTargetArray m_colorTextures;                      // 現在の描画ターゲット(クリア用)
        Ref<RenderTexture> m_depthTexture;                      // 現在の描画ターゲット(クリア用)

        DirectX12RootSignature* m_signature = nullptr;          // 最後に設定されたPipelineStateのRootSignature
        bool m_computePipelineBound = false;
        bool m_enableAutomaticBarriers = true;

        ResourceStateCache m_cache;                             // リソースバリア用ステートキャッシュ
        Vector<ComPtr<ID3D12Resource>> m_temporaryResources;    // コマンド実行まで保持する一時リソース
        Vector<Ref<Buffer>> m_temporaryBuffers;                 // コマンド実行まで保持する一時バッファ
        Vector<Ref<EventQuery>> m_eventQueries;                 // 実行完了と紐付けるイベントクエリ

		u64                 m_frameIndex = 0;                   // フレームインデックス
        StringBase<WCHAR>   m_markerNameCache;                  // マーカー名用文字列キャッシュ

    };

}// Amuse::RHI 
