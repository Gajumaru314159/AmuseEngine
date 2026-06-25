//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Reflection/TypeBuilder.h>

AMUSE_DEFINE_PRIMITIVE_INFO(Amuse::s64) {
	desc("64bit符号付き整数型");
	tag("DisplayName", "s64");
	tag("C#", "System.Int64");
}

AMUSE_DEFINE_PRIMITIVE_INFO(Amuse::u64) {
	desc("64bit符号なし整数型");
	tag("DisplayName", "u64");
	tag("C#", "System.UInt64");
}

AMUSE_DEFINE_PRIMITIVE_INFO(Amuse::s32) {
	desc("32bit符号付き整数型");
	tag("DisplayName", "s32");
	tag("C#", "System.Int32");
}

AMUSE_DEFINE_PRIMITIVE_INFO(Amuse::u32) {
	desc("32bit符号なし整数型");
	tag("DisplayName", "u32");
	tag("C#", "System.UInt32");
}

AMUSE_DEFINE_PRIMITIVE_INFO(Amuse::s16) {
	desc("16bit符号付き整数型");
	tag("DisplayName", "s16");
	tag("C#", "System.Int16");
}

AMUSE_DEFINE_PRIMITIVE_INFO(Amuse::u16) {
	desc("16bit符号なし整数型");
	tag("DisplayName", "u16");
	tag("C#", "System.UInt16");
}

AMUSE_DEFINE_PRIMITIVE_INFO(Amuse::s8) {
	desc("8bit符号付き整数型");
	tag("DisplayName", "s8");
	tag("C#", "System.SByte");
}

AMUSE_DEFINE_PRIMITIVE_INFO(Amuse::u8) {
	desc("8bit符号なし整数型");
	tag("DisplayName", "u8");
	tag("C#", "System.Byte");
}

AMUSE_DEFINE_PRIMITIVE_INFO(Amuse::f64) {
	tag("DisplayName", "f64");
	tag("C#", "System.Double");
}

AMUSE_DEFINE_PRIMITIVE_INFO(Amuse::f32) {
	tag("DisplayName", "f32");
	tag("C#", "System.Single");
}

AMUSE_DEFINE_PRIMITIVE_INFO(Amuse::Char) {
	desc("文字型");
	tag("C#", "System.Char");
}

AMUSE_DEFINE_PRIMITIVE_INFO(Amuse::Bool) {
	desc("真偽値型");
	tag("C#", "System.Boolean");
}

AMUSE_DEFINE_CLASS_INFO(Amuse::String) {
	desc("文字列型");
	tag("DisplayName", "Amuse::String");
	tag("C#", "System.String");
	constructor<const Char*>().desc("コンストラクタ");
	constructor<String>().desc("コンストラクタ");
}



AMUSE_DEFINE_CLASS_INFO(Amuse::Vec2) {
	desc("2次元ベクトル");
	tag("C#", "Common.Math.Vector2");
	constructor().desc("デフォルトコンストラクタ");
	constructor<f32>().desc("コンストラクタ(すべての要素を同じ値で初期化)");
	constructor<f32, f32>().desc("コンストラクタ(要素を指定して初期化)");
}

AMUSE_DEFINE_CLASS_INFO(Amuse::Vec3) {
	desc("3次元ベクトル");
	tag("C#", "Common.Math.Vector3");
	constructor().desc("デフォルトコンストラクタ");
	constructor<f32>().desc("コンストラクタ(すべての要素を同じ値で初期化)");
	constructor<f32, f32, f32>().desc("コンストラクタ(要素を指定して初期化)");
}

AMUSE_DEFINE_CLASS_INFO(Amuse::Vec4) {
	desc("4次元ベクトル");
	tag("C#", "Common.Math.Vector4");
	constructor().desc("デフォルトコンストラクタ");
	constructor<f32>().desc("コンストラクタ(すべての要素を同じ値で初期化)");
	constructor<f32, f32, f32, f32>().desc("コンストラクタ(要素を指定して初期化)");
}

AMUSE_DEFINE_CLASS_INFO(Amuse::IntVec2) {
	desc("2次元整数ベクトル");
	tag("C#", "Common.Math.IntVec2");
	constructor().desc("デフォルトコンストラクタ");
	constructor<s32>().desc("コンストラクタ(すべての要素を同じ値で初期化)");
	constructor<s32, s32>().desc("コンストラクタ(要素を指定して初期化)");
}

AMUSE_DEFINE_CLASS_INFO(Amuse::IntVec3) {
	desc("3次元整数ベクトル");
	tag("C#", "Common.Math.IntVec3");
	constructor().desc("デフォルトコンストラクタ");
	constructor<s32>().desc("コンストラクタ(すべての要素を同じ値で初期化)");
	constructor<s32, s32, s32>().desc("コンストラクタ(要素を指定して初期化)");
}

AMUSE_DEFINE_CLASS_INFO(Amuse::IntVec4) {
	desc("4次元整数ベクトル");
	tag("C#", "Common.Math.IntVec4");
	constructor().desc("デフォルトコンストラクタ");
	constructor<s32>().desc("コンストラクタ(すべての要素を同じ値で初期化)");
	constructor<s32, s32, s32, s32>().desc("コンストラクタ(要素を指定して初期化)");
}

AMUSE_DEFINE_CLASS_INFO(Amuse::Rot) {
	desc("回転");
	tag("C#", "Common.Math.Rotation");
	constructor().desc("デフォルトコンストラクタ");
	constructor<f32, f32, f32>().desc("コンストラクタ(要素を指定して初期化)");
}

AMUSE_DEFINE_CLASS_INFO(Amuse::Quat) {
	desc("回転");
	tag("C#", "Common.Math.Quaternion");
	constructor().desc("デフォルトコンストラクタ");
	constructor<f32, f32, f32, f32>().desc("コンストラクタ(要素を指定して初期化)");
}

AMUSE_DEFINE_CLASS_INFO(Amuse::Color) {
	desc("色");
	tag("C#", "Common.Graphics.Color");
	constructor().desc("デフォルトコンストラクタ");
	constructor<f32, f32, f32, f32>().desc("コンストラクタ(各要素を指定して初期化)");
	constructor<f32, f32>().desc("コンストラクタ(輝度とアルファ値を指定して初期化)");
}

AMUSE_DEFINE_CLASS_INFO(Amuse::IntColor) {
	desc("色");
	//tag("C#", "Common.Graphics.Color");
	constructor().desc("デフォルトコンストラクタ");
	constructor<s32, s32, s32, s32>().desc("コンストラクタ(各要素を指定して初期化)");
	constructor<s32, s32>().desc("コンストラクタ(輝度とアルファ値を指定して初期化)");
}
