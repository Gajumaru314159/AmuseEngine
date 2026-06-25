//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/File/ByteOrder.h>

namespace Amuse {

	class Stream;

	//! @brief 指定したバイトオーダーでストリームへバイナリ値を書き込む。
	//! @ingroup AmuseCore
	class BinaryWriter{
	public:
		//! @brief BinaryWriter を初期化する。
		explicit BinaryWriter(Stream& stream, ByteOrder byteOrder = ByteOrder::Auto);
		~BinaryWriter();

		//! @brief 8bit 符号付き整数を書き込む。
		void writeInt8(s8 value);
		//! @brief 16bit 符号付き整数を書き込む。
		void writeInt16(s16 value);
		//! @brief 32bit 符号付き整数を書き込む。
		void writeInt32(s32 value);
		//! @brief 64bit 符号付き整数を書き込む。
		void writeInt64(s64 value);
		//! @brief 8bit 符号なし整数を書き込む。
		void writeUInt8(u8 value);
		//! @brief 16bit 符号なし整数を書き込む。
		void writeUInt16(u16 value);
		//! @brief 32bit 符号なし整数を書き込む。
		void writeUInt32(u32 value);
		//! @brief 64bit 符号なし整数を書き込む。
		void writeUInt64(u64 value);
		//! @brief 32bit 浮動小数点数を書き込む。
		void writeFloat(f32 value);
		//! @brief 64bit 浮動小数点数を書き込む。
		void writeDouble(f64 value);
		//! @brief 文字列を書き込む。
		void writeString(StringView value);

		//! @brief 指定バイト数の生データを書き込む。
		void write(const void* buffer, size_t count);
		//! @brief 書き込み位置を指定オフセットへ移動する。
		void seek(offset_t offset);
		//! @brief バッファリングされた書き込み内容をストリームへ反映する。
		void flush();

		template<class T>
		//! @brief 対応する算術型の値をバイナリ形式で書き込む。
		void write(const T& value) {
			using pure_type = remove_cvr_t<T>;
			if constexpr (std::is_same<pure_type, s8>::value)
				writeInt8(value);
			else if constexpr (std::is_same<pure_type, s16>::value)
				writeInt16(value);
			else if constexpr (std::is_same<pure_type, s32>::value)
				writeInt32(value);
			else if constexpr (std::is_same<pure_type, s64>::value)
				writeInt64(value);
			else if constexpr (std::is_same<pure_type, u8>::value)
				writeUInt8(value);
			else if constexpr (std::is_same<pure_type, u16>::value)
				writeUInt16(value);
			else if constexpr (std::is_same<pure_type, u32>::value)
				writeUInt32(value);
			else if constexpr (std::is_same<pure_type, u64>::value)
				writeUInt64(value);
			else if constexpr (std::is_same<pure_type, f32>::value)
				writeFloat(value);
			else if constexpr (std::is_same<pure_type, f64>::value)
				writeDouble(value);
			else
				static_assert(false,"not implemented");
		}

	private:
		Stream& m_stream;
		ByteOrder m_byteOrder;
	};

}
