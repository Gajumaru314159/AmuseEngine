//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/String/StringView.h>

namespace Amuse::Core {

    //! @brief 文字列分割ビューを表す。
    class StringSplitView {
    public:
        //! @brief Iteratorを表す。
        class Iterator {
        public:
            //! @brief Iterator を初期化する。
            Iterator(StringView str, StringView delimiter, size_t start)
                : m_str(str), m_delimiter(delimiter), m_start(start), m_end(str.find(delimiter, start)) {}

            //! @brief 参照先または演算結果を取得する。
            StringView operator*() const {
                return m_str.substr(m_start, m_end - m_start);
            }

            //! @brief 次の分割要素へ進める。
            Iterator& operator++() {
                if (m_end == StringView::npos) {
                    m_start = m_end;
                    return *this;
                }
                m_start = m_end + m_delimiter.size();
                m_end = m_str.find(m_delimiter, m_start);
                return *this;
            }

            //! @brief 非等値比較を行う。
            bool operator!=(const Iterator& other) const {
                return m_start != other.m_start;
            }
        private:
            StringView m_str;
            StringView m_delimiter;
            size_t m_start;
            size_t m_end;
        };

        //! @brief 分割対象文字列と区切り文字を指定して初期化する。
        StringSplitView(StringView str, StringView delimiter)
            : m_str(str), m_delimiter(delimiter) {}

        //! @brief 先頭イテレータを取得する。
        Iterator begin() const {
            return Iterator(m_str, m_delimiter, 0);
        }

        //! @brief 終端イテレータを取得する。
        Iterator end() const {
            return Iterator(m_str, m_delimiter, StringView::npos);
        }
    private:
        StringView m_str;
        StringView m_delimiter;
    };

    //! @brief カンマ区切り分割ビューを表す。
    class CommaSplitView : public StringSplitView {
    public:
        //! @brief CommaSplitView を初期化する。
        CommaSplitView(StringView str) : StringSplitView(str, ",") {}
    };

    //! @brief タブ区切り分割ビューを表す。
    class TabSplitView : public StringSplitView {
    public:
        //! @brief TabSplitView を初期化する。
        TabSplitView(StringView str) : StringSplitView(str, "\t") {}
    };

    //! @brief 改行区切り分割ビューを表す。
    class LineBreakSplitView : public StringSplitView {
    public:
        //! @brief LineBreakSplitView を初期化する。
        LineBreakSplitView(StringView str) : StringSplitView(str, "\n") {}
    };

}
