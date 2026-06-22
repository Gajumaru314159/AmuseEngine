//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Core/Misc/BlobView.h>
#include <Amuse/Core/Utility/Ref.h>
#include <Amuse/RHI/Config.h>
#include <Amuse/RHI/Forward.h>
#include <Amuse/RHI/Buffer.h>
#include <Amuse/RHI/GraphicFile.h>
#include <Amuse/RHI/Types/BufferDesc.h>
#include <Amuse/RHI/Types/CommandListDesc.h>
#include <Amuse/RHI/Types/SwapChainDesc.h>
#include <Amuse/RHI/Types/DescriptorLayoutDesc.h>
#include <Amuse/RHI/Types/DescriptorTableDesc.h>
#include <Amuse/RHI/Types/PipelineStateDesc.h>
#include <Amuse/RHI/Types/ComputePipelineStateDesc.h>
#include <Amuse/RHI/Types/RenderPassDesc.h>
#include <Amuse/RHI/Types/RenderTextureDesc.h>
#include <Amuse/RHI/Types/RootSignatureDesc.h>
#include <Amuse/RHI/Types/ShaderStage.h>
#include <Amuse/RHI/Types/TextureDesc.h>
#include <Amuse/RHI/Types/VideoCard.h>
#include <Amuse/RHI/Types/Barrier.h>
#include <Amuse/RHI/Types/Capability.h>
#include <Amuse/RHI/Types/Copy.h>
#include <Amuse/RHI/Query.h>
#include <Amuse/RHI/Fence.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief      Rendering Hardware Interface (RHI)
	//! @details    DirectX, Vulkan, MetalなどのグラフィックAPIを抽象化したインターフェース。
	class Device:public Singleton<Device> {
	public:

        //! @brief      コンストラクタ
		Device(GraphicObjectManager&, const RHIConfig*);

        //! @brief      デストラクタ
        ~Device() override;

		//! @brief デバイス内部の遅延処理を更新する
        virtual void update();


        //! @brief  妥当な状態か
        virtual bool isValid()const { return true; };

        //! @brief 使用中のグラフィック API 名を取得する
        virtual String getAPIName()const { return {}; };

        //===============================================================
        // 更新
        //===============================================================
        //! @brief コマンドリストを登録する
        virtual void entryCommandList(const Ref<CommandList>&) {}
        //! @brief フェンスを生成する
        virtual Ref<Fence> createFence(const FenceDesc& desc) { return nullptr; }
        //! @brief コマンドリストを実行する
        virtual u64 executeCommandLists(Span<Ref<CommandList>> commandLists, CommandQueueType queue = CommandQueueType::Graphics) { return 0; }
        //! @brief キューのアイドル状態を待機する
        virtual void waitForIdle(CommandQueueType queue = CommandQueueType::Graphics) {}
        //! @brief キュー間のフェンス待機を設定する
        virtual void queueWait(CommandQueueType waitQueue, CommandQueueType executeQueue, u64 fenceValue) {}

        //! @brief DrawIndirect 用引数のバイトサイズを取得する
        virtual u32 getDrawIndirectArgumentSize() const { return sizeof(u32) * 4; }
        //! @brief DrawIndexedIndirect 用引数のバイトサイズを取得する
        virtual u32 getDrawIndexedIndirectArgumentSize() const { return sizeof(u32) * 5; }
        //! @brief DispatchIndirect 用引数のバイトサイズを取得する
        virtual u32 getDispatchIndirectArgumentSize() const { return sizeof(u32) * 3; }

        //! @brief CPUからバッファをマップ
        virtual void* mapBuffer(const Ref<Buffer>& buffer, CpuAccess access) { return buffer ? buffer->map(access) : nullptr; }

        //! @brief CPUからのバッファマップを解除
        virtual void unmapBuffer(const Ref<Buffer>& buffer) { if (buffer) buffer->unmap(); }

		//! @brief 利用可能なビデオカード一覧を取得する
		virtual Vector<VideoCard>           getVideoCards()const { return {}; }

        //! @brief スワップチェーンを生成する
        virtual Ref<SwapChain>              createSwapChain(const SwapChainDesc& desc) { return nullptr; }

        //! @brief コマンドリストを生成する
        virtual Ref<CommandList>            createCommandList(const CommandListDesc& desc) { return nullptr; }

        //! @brief ルートシグネチャを生成する
        virtual Ref<RootSignature>          createRootSignature(const RootSignatureDesc& desc) { return nullptr; }
        //! @brief グラフィックスパイプラインステートを生成する
        virtual Ref<PipelineState>          createPipelineState(const PipelineStateDesc& desc) { return nullptr; }
        //! @brief コンピュートパイプラインステートを生成する
        virtual Ref<ComputePipelineState>   createComputePipelineState(const ComputePipelineStateDesc& desc) { return nullptr; }

        //! @brief バッファを生成する
        virtual Ref<Buffer>                 createBuffer(const BufferDesc& desc) { return nullptr; }
        //! @brief バッファを生成する
        virtual Ref<Buffer>                 createBuffer(const BufferViewDesc& desc) { return nullptr; }

        //! @brief テクスチャを生成する
        virtual Ref<Texture>                createTexture(const TextureDesc& desc) { return nullptr; }
        //! @brief テクスチャを生成する
        virtual Ref<Texture>                createTexture(StringView name,BlobView blob) { return nullptr; }
        //! @brief テクスチャを生成する
        virtual Ref<Texture>                createTexture(StringView name, TextureType type, Size size, Span<const IntColor> colors) { return nullptr; }
        //! @brief テクスチャを生成する
        virtual Ref<Texture>                createTexture(const TextureViewDesc& desc) { return nullptr; }
        //! @brief レンダーターゲットを生成する
        virtual Ref<RenderTexture>          createRenderTexture(const RenderTextureDesc& desc) { return nullptr; }

        //! @brief サンプラを生成する
        virtual Ref<Sampler>                createSampler(const SamplerDesc& desc) { return nullptr; }

        //! @brief シェーダをコンパイルする
        virtual Ref<Shader>                 compileShader(const ShaderCompileDesc& desc) { return nullptr; }
        //! @brief シェーダバイナリを読み込む
        virtual Ref<Shader>                 loadShader(BlobView binary, ShaderStage stage) { return nullptr; }

        //! @brief ディスクリプタレイアウトを生成する
        virtual Ref<DescriptorLayout>       createDescriptorLayout(const DescriptorLayoutDesc& desc) { return nullptr; }
        //! @brief ディスクリプタテーブルを生成する
        virtual Ref<DescriptorTable>        createDescriptorTable(const DescriptorTableDesc& desc) { return nullptr; }

        //! @brief イベントクエリを生成する
        virtual Ref<EventQuery>             createEventQuery() { return nullptr; }
        //! @brief タイマークエリを生成する
        virtual Ref<TimerQuery>             createTimerQuery() { return nullptr; }
        //! @brief イベントクエリの完了状態を確認する
        virtual bool                        pollEventQuery(const Ref<EventQuery>&) { return false; }
        //! @brief イベントクエリの完了を待機する
        virtual void                        waitEventQuery(const Ref<EventQuery>&) {}
        //! @brief タイマークエリの完了状態を確認する
        virtual bool                        pollTimerQuery(const Ref<TimerQuery>&) { return false; }
        //! @brief タイマークエリの計測時間を取得する
        virtual f32                         getTimerQueryTime(const Ref<TimerQuery>&) { return 0.0f; }

        //! @brief グラフィックファイルハンドルを生成する
        virtual Ref<GraphicFileHandle>      createGraphicFileHandle(StringView path) { return nullptr; }
        //! @brief グラフィックファイルイベントを生成する
        virtual Ref<GraphicFileEvent>       createGraphicFileEvent() { return nullptr; }
        //! @brief グラフィックファイルキューを生成する
        virtual Ref<GraphicFileQueue>       createGraphicFileQueue(const GraphicFileQueueDesc&) { return nullptr; }

        //! @brief グラフィックファイルを生成する
        virtual bool                        generateGraphicFile(StringView input, StringView output, s32 compressionLevel) { return false; }
        //! @brief グラフィックファイルを読み込み準備する
        virtual Vector<GraphicFileMipInfo>  prepareGraphicFile(StringView path) { return {}; }

		//! @brief RHI 設定を取得する
		const RHIConfig& getConfig()const { return m_config; }

    public:

        //! @brief 機能対応状況を確認する
        virtual bool supports(TextureFormat format, TextureType type)const { return false; }
        //! @brief レンダーターゲット対応状況を確認する
        virtual bool supportsForRenderTexture(TextureFormat format)const { return false; }
        //! @brief 機能対応状況を確認する
        virtual bool supports(ShaderStage stage)const { return false; }
        //! @brief 対応機能一覧を取得する
        virtual RHIFeatures getFeatures()const { return {}; }
        //! @brief 機能対応状況を確認する
        virtual bool supports(TextureFormat format, FormatSupport usage)const { return false; }

    protected:

        //! @brief      終了処理
        //! @details    GraphicObjectの解放が遅延する影響で派生クラスのデストラクタの最初で呼び出す必要があります。
        void finalize();

    private:
        RHIConfig m_config;
        GraphicObjectManager& m_objectManager;
	};

}
