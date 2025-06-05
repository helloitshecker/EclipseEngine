#pragma once

#include <string>
#include <format>

namespace ee {
	namespace Out {

		void RawPrint(const std::wstring& str);
		void RawPrint(const std::string& str);

		template<typename... Args>
		void Print(const std::wstring_view fmt, Args&&... args) {
			std::wstring out = std::vformat(fmt, std::make_wformat_args(args...));
			RawPrint(out);
		}
		template<typename... Args>
		void Print(const std::string_view fmt, Args&&... args) {
			std::string out = std::vformat(fmt, std::make_format_args(args...));
			RawPrint(out);
		}

		template<typename... Args>
		void Println(const std::wstring_view fmt, Args&&... args) {
			std::wstring out = std::vformat(fmt, std::make_wformat_args(args...));
			RawPrint(out);
			RawPrint("\n");
		}
		template<typename... Args>
		void Println(const std::string_view fmt, Args&&... args) {
			std::string out = std::vformat(fmt, std::make_format_args(args...));
			RawPrint(out);
			RawPrint("\n");
		}
	};
};

