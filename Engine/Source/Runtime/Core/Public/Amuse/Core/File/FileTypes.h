//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>

namespace Amuse {

	//! @brief  ファイル属性
	//! @ingroup AmuseCore
	enum class FileAttribut :u32
	{
		None		= get_bit(0),	//!< なし
		Normal		= get_bit(1),	//!< 通常ファイル
		Directory	= get_bit(2),	//!< ディレクトリ
		ReadOnly	= get_bit(3),	//!< 読み取り専用
		Hidden		= get_bit(4),	//!< 隠しファイル
	};
	//! @brief  ファイル属性
	using FileAttributs = BitFlags<FileAttribut>;

	//! @brief  ファイル・オープン・モード
	//! @ingroup AmuseCore
	enum class FileOpenMode :u32
	{
		Read = get_bit(0),		//!< 読み取り
		Write = get_bit(1),		//!< 書き込み
		Append = get_bit(2),	//!< 追記
		Text = get_bit(3),		//!< テキスト
	};
	//! @brief  ファイル・オープン・モード・セット
	using FileOpenModes = BitFlags<FileOpenMode>;

	//! @brief  ファイルコピーのオプション
	//! @ingroup AmuseCore
	enum class FileCopyOption {
		None				= get_bit(0),
		SkipExisting		= get_bit(1),
		OverwriteExisting	= get_bit(2),
		UpdateExisting		= get_bit(3),
		Recursive			= get_bit(4),
		CopySymlinks		= get_bit(5),
		SkipSymLinks		= get_bit(6),
		DirectoriesOnly		= get_bit(7),
		CreateSymLinks		= get_bit(8),
		CreateHardLinks		= get_bit(9),
	};
	//! @brief  ファイルコピーのオプションセット
	using FileCopyOptions = BitFlags<FileCopyOption>;

}