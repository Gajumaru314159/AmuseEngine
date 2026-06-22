//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/String/StringView.h>
#include <Amuse/RHI/Forward.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief グラフィックファイル読み込みキュー設定
	struct GraphicFileQueueDesc {
		s32 capacity = 0; //!< 容量
		String name; //!< 名前
	};

	//! @brief		非同期ファイル定義
	//! @details	* uncompressedSizeが0以外の場合はプラットフォームに応じた解凍処理が行われます。
	//!				* destが指定されている場合は指定先にファイルの内容が展開されます。
	//!				* destが指定されていない場合は内部的にバッファを確保し、そのバッファに展開されます。
	struct GraphicFileRequest {
		Ref<GraphicFileHandle> handle;			//!< ファイルハンドル
		size_t			offset = 0;				//!< 読み込み開始位置
		size_t			size = 0;				//!< 読み込みサイズ
		size_t			uncompressedSize = 0;	//!< 解凍後サイズ
		
		//! @brief バッファへの転送先情報
		struct BufferDesc {
			Ref<Amuse::RHI::Buffer> buffer; //!< 転送先バッファ
			size_t offset = 0; //!< オフセット
		};
		//! @brief テクスチャ生成情報
		struct TextureDesc {
			Ref<Amuse::RHI::Texture> texture; //!< 転送先テクスチャ
			s32 subresourceIndex = 0; //!< サブリソースインデックス
		};
		//! @brief 連続テクスチャ転送先情報
		struct TextureSequenceDesc {
			Ref<Texture> texture; //!< テクスチャ
			s32 firstSubresourceIndex = 0; //!< 先頭サブリソースインデックス
		};

		Variant<BufferDesc, TextureDesc, TextureSequenceDesc> dest; //!< 転送先

	};

	//! @brief 非同期ファイルハンドル
	class GraphicFileHandle : public RefObject {
	public:
		//! @brief インスタンスを生成する
		static Ref<GraphicFileHandle> Create(StringView path);
	public:
		//! @brief ファイルパスを取得する
		virtual const String& path()const = 0;
		//! @brief 空かどうかを判定する
		virtual bool empty()const = 0;
		//! @brief サイズを取得する
		virtual size_t size()const = 0;
	};

	//! @brief 非同期ファイル読み込みイベント
	class GraphicFileEvent : public RefObject {
	public:
		//! @brief インスタンスを生成する
		static Ref<GraphicFileEvent> Create();
	public:
		//! @brief 完了状態を確認する
		virtual bool poll() const = 0;
		//! @brief 完了を待機する
		virtual void wait() const = 0;
	};

	//! @brief 非同期ファイル読み込みキュー
	//! @details このクラスは非同期ファイル読み込みを管理するキューです。
	class GraphicFileQueue : public RefObject {
	public:
		//! @brief インスタンスを生成する
		static Ref<GraphicFileQueue> Create(const GraphicFileQueueDesc& desc = {});
	public:
		//! @brief 読み込み要求を追加する
		virtual void add(const GraphicFileRequest& desc) = 0;
		//! @brief 読み込み要求を追加する
		virtual void add(Ref<GraphicFileEvent>&) = 0;
		//! @brief 完了を待機する
		virtual void wait() = 0;
		//! @brief キューに積んだ要求を投入する
		virtual void submit() = 0;
		//! @brief キュー状態を検証する
		virtual void validate() = 0;
	};

	//! @brief グラフィックファイルの Mip 情報
	struct GraphicFileMipInfo {
		u32 offset = 0; //!< オフセット
		u32 size = 0; //!< サイズ
		u32 uncompressedSize = 0; //!< 展開後サイズ
		u32 width = 0; //!< 幅
		u32 height = 0; //!< 高さ
	};

	//! @brief グラフィックファイル
	class GraphicFile {
	public:
		//! @brief 入力テクスチャをプラットフォームごとの直接アップロード可能な形式に変換してファイルに保存します。
		//! @param input 入力テクスチャのファイルパス
		//! @param output 出力バイナリのファイルパス
		//! @param compressionLevel 圧縮レベル
		//! @return 変換に成功したか
		//! @details compressionLevelは0～10の範囲で指定します。0が無圧縮、10が最高圧縮です。
		static bool Generate(StringView input, StringView output,s32 compressionLevel = 10);
		//! @brief グラフィックファイルを変換準備する
		static Vector<GraphicFileMipInfo> Prepare(StringView file);
	};

}
