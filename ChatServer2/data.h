#pragma once
#include <string>
#include <utility>

// 屏蔽外部作用域
struct UserInfo {
	UserInfo(): name(""), pwd(""), uid(0), email(""), nick(""), desc(""), sex(0), icon(""), back("") {
	}

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
		: uid(uid), name(std::move(name)), desc(std::move(desc)),
		  icon(std::move(icon)), nick(std::move(nick)), sex(sex), status(status) {
	}

	int uid;
	std::string name;
	std::string desc;
	std::string icon;
	std::string nick;
	int sex;
	int status;
};
