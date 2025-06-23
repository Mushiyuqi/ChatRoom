#pragma once
#include <string>
#include <utility>

// 屏蔽外部作用域
namespace Data {
	struct UserInfo {
		UserInfo(): name(""), pwd(""), uid(0), email(""), nick(""), desc(""), sex(0), icon(""), back("") {}

		std::string name;
		std::string pwd;
		int uid;
		std::string email;
		std::string nick;
		std::string desc;
		int sex;
		std::string icon;
		std::string back;
	};

	struct ApplyInfo {
		ApplyInfo(const int uid, std::string name, std::string desc,
				  std::string icon, std::string nick, const int sex, const int status)
			: _uid(uid), _name(std::move(name)), _desc(std::move(desc)),
			  _icon(std::move(icon)), _nick(std::move(nick)), _sex(sex), _status(status) {
		}

		int _uid;
		std::string _name;
		std::string _desc;
		std::string _icon;
		std::string _nick;
		int _sex;
		int _status;
	};
}
