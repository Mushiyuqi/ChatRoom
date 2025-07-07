#include "MysqlDao.h"

#include <memory>
#include "ConfigManager.h"
#include "MysqlPool.h"

MysqlDao::MysqlDao() {
	auto& cfg = ConfigManager::GetInstance();
	const auto& host = cfg["Mysql"]["Host"];
	const auto& port = cfg["Mysql"]["Port"];
	const auto& pwd = cfg["Mysql"]["Password"];
	const auto& schema = cfg["Mysql"]["Schema"];
	const auto& user = cfg["Mysql"]["User"];
	m_pool.reset(new MySqlPool(host + ":" + port, user, pwd, schema, MysqlPoolSize));
}

MysqlDao::~MysqlDao() {
	m_pool->Close();
}

int MysqlDao::RegUser(const std::string& name, const std::string& email, const std::string& pwd) {
	auto conn = m_pool->getConnection();
	try {
		if (conn == nullptr) return false;
		// 准备调用存储过程
		std::unique_ptr<sql::PreparedStatement> stmt(conn->m_conn->prepareStatement("CALL reg_user(?,?,?,@result)"));
		// 设置输入参数
		stmt->setString(1, name);
		stmt->setString(2, email);
		stmt->setString(3, pwd);
		// 执行存储过程
		stmt->execute();
		// 由于PreparedStatement不直接支持注册输出参数，我们需要使用会话变量或其他方法来获取输出参数的值
		// 如果存储过程设置了会话变量或有其他方式获取输出参数的值，你可以在这里执行SELECT查询来获取它们
		// 例如，如果存储过程设置了一个会话变量@result来存储输出结果，可以这样获取：
		std::unique_ptr<sql::Statement> stmtResult(conn->m_conn->createStatement());
		std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));
		if (res->next()) {
			const int result = res->getInt("result");
			std::cout << "MysqlDao::RegUser Result: " << result << std::endl;
			m_pool->returnConnection(std::move(conn));
			return result;
		}
		m_pool->returnConnection(std::move(conn));
		return -1;
	}
	catch (sql::SQLException& e) {
		m_pool->returnConnection(std::move(conn));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return -1;
	}
}

bool MysqlDao::CheckEmail(const std::string& name, const std::string& email) {
	auto con = m_pool->getConnection();
	try {
		if (con == nullptr) {
			return false;
		}

		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(
			con->m_conn->prepareStatement("SELECT email FROM user WHERE name = ?"));

		// 绑定参数
		pstmt->setString(1, name);

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

		// 遍历结果集
		while (res->next()) {
			std::cout << "MysqlDao::CheckEmail : " << res->getString("email") << std::endl;
			if (email != res->getString("email")) {
				m_pool->returnConnection(std::move(con));
				return false;
			}
			m_pool->returnConnection(std::move(con));
			return true;
		}
		return true;
	}
	catch (sql::SQLException& e) {
		m_pool->returnConnection(std::move(con));
		std::cerr << "MysqlDao::CheckEmail ";
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::UpdatePwd(const std::string& name, const std::string& newpwd) {
	auto con = m_pool->getConnection();
	try {
		if (con == nullptr) {
			return false;
		}

		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(
			con->m_conn->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));

		// 绑定参数
		pstmt->setString(2, name);
		pstmt->setString(1, newpwd);

		// 执行更新
		int updateCount = pstmt->executeUpdate();

		std::cout << "MysqlDao::UpdatePwd Updated rows: " << updateCount << std::endl;
		m_pool->returnConnection(std::move(con));
		return true;
	}
	catch (sql::SQLException& e) {
		m_pool->returnConnection(std::move(con));
		std::cerr << "MysqlDao::UpdatePwd ";
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo) {
	auto con = m_pool->getConnection();
	if (con == nullptr) {
		return false;
	}

	Defer defer([this, &con]() {
		m_pool->returnConnection(std::move(con));
	});

	try {
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(
			con->m_conn->prepareStatement("SELECT * FROM user WHERE email = ?"));
		pstmt->setString(1, email); // 将username替换为你要查询的用户名

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::string origin_pwd = "";
		// 遍历结果集
		while (res->next()) {
			origin_pwd = res->getString("pwd");
			// 输出查询到的密码
			std::cout << "MysqlDao::CheckPwd OriginPassword: " << origin_pwd << std::endl;
			std::cout << "MysqlDao::CheckPwd       Password: " << pwd << std::endl;
			break;
		}

		if (pwd != origin_pwd) {
			return false;
		}
		userInfo.name = res->getString("name");
		userInfo.email = email;
		userInfo.uid = res->getInt("uid");
		userInfo.pwd = origin_pwd;
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "MysqlDao::CheckPwd SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(int uid) {
	auto con = m_pool->getConnection();
	if (con == nullptr) {
		return nullptr;
	}

	Defer defer([this, &con]() {
		m_pool->returnConnection(std::move(con));
	});

	try {
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_conn->prepareStatement("SELECT * FROM user WHERE uid = ?"));
		pstmt->setInt(1, uid); // 将uid替换为你要查询的uid

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> userPtr = nullptr;
		// 遍历结果集
		while (res->next()) {
			userPtr = std::make_shared<UserInfo>();
			userPtr->pwd = res->getString("pwd");
			userPtr->email = res->getString("email");
			userPtr->name= res->getString("name");
			userPtr->nick = res->getString("nick");
			userPtr->desc = res->getString("desc");
			userPtr->sex = res->getInt("sex");
			userPtr->icon = res->getString("icon");
			userPtr->uid = uid;
			break;
		}
		return userPtr;
	}
	catch (sql::SQLException& e) {
		std::cerr << "MysqlDao::GetUser SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return nullptr;
	}
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(const std::string& name) {
	auto con = m_pool->getConnection();
	if (con == nullptr) {
		return nullptr;
	}

	Defer defer([this, &con]() {
		m_pool->returnConnection(std::move(con));
	});

	try {
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_conn->prepareStatement("SELECT * FROM user WHERE name = ?"));
		pstmt->setString(1, name); // 将name替换为你要查询的name

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> userPtr = nullptr;
		// 遍历结果集
		while (res->next()) {
			userPtr = std::make_shared<UserInfo>();
			userPtr->pwd = res->getString("pwd");
			userPtr->email = res->getString("email");
			userPtr->name= res->getString("name");
			userPtr->nick = res->getString("nick");
			userPtr->desc = res->getString("desc");
			userPtr->sex = res->getInt("sex");
			userPtr->icon = res->getString("icon");
			userPtr->uid = res->getInt("uid");;
			break;
		}
		return userPtr;
	}
	catch (sql::SQLException& e) {
		std::cerr << "MysqlDao::GetUser SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return nullptr;
	}
}

bool MysqlDao::AddFriendApply(int uid, int touid) {
	auto con = m_pool->getConnection();
	if (con == nullptr) return false;
	Defer defer([this, &con]() {m_pool->returnConnection(std::move(con));});

	try {
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_conn->prepareStatement(
			"INSERT INTO friend_apply (from_uid, to_uid) values (?,?) "
			"ON DUPLICATE KEY UPDATE from_uid = from_uid, to_uid = to_uid "
			));
		pstmt->setInt(1, uid);
		pstmt->setInt(2, touid);

		// 执行更新
		int rowAffected = pstmt->executeUpdate();
		if(rowAffected < 0) {
			return false;
		}
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "MysqlDao::AddFriendApply SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::GetFriendApplyList(int uid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin,
	int limit) {
	auto con = m_pool->getConnection();
	if (con == nullptr) return false;
	Defer defer([this, &con]() {m_pool->returnConnection(std::move(con));});

	try {
		// 准备SQL语句, 根据起始id和限制条数返回列表
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_conn->prepareStatement(
			"select apply.from_uid, apply.status, user.name, "
			"user.nick, user.sex from friend_apply as apply join user on apply.from_uid = user.uid where apply.to_uid = ? "
			"and apply.id > ? order by apply.id ASC LIMIT ? "));

		pstmt->setInt(1, uid); // 将uid替换为你要查询的uid
		pstmt->setInt(2, begin); // 起始id
		pstmt->setInt(3, limit); //偏移量
		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		// 遍历结果集
		while (res->next()) {
			auto name = res->getString("name");
			auto uid = res->getInt("from_uid");
			auto status = res->getInt("status");
			auto nick = res->getString("nick");
			auto sex = res->getInt("sex");
			auto apply_ptr = std::make_shared<ApplyInfo>(uid, name, "", "", nick, sex, status);
			applyList.emplace_back(apply_ptr);
		}
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "MysqlDao::GetFriendApplyList SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::AuthFriendApply(int from, int to) {
	auto con = m_pool->getConnection();
	if (con == nullptr) return false;
	Defer defer([this, &con]() {m_pool->returnConnection(std::move(con));});

	try {
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_conn->prepareStatement("UPDATE friend_apply SET status = 1 "
			"WHERE from_uid = ? AND to_uid = ?"));
		//反过来的申请时from，验证时to
		pstmt->setInt(1, to); // from id
		pstmt->setInt(2, from);
		// 执行更新
		int rowAffected = pstmt->executeUpdate();
		if (rowAffected < 0) {
			return false;
		}
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "MysqlDao::AuthFriendApply SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::AddFriend(int from, int to, const std::string& back_name) {
	auto con = m_pool->getConnection();
	if (con == nullptr) return false;
	Defer defer([this, &con]() {
		con->m_conn->setAutoCommit(true);
		m_pool->returnConnection(std::move(con));
	});

	try {
		//开始事务
		con->m_conn->setAutoCommit(false);

		// 准备第一个SQL语句, 插入认证方好友数据
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_conn->prepareStatement("INSERT IGNORE INTO friend(self_id, friend_id, back) "
			"VALUES (?, ?, ?) "
			));
		//反过来的申请时from，验证时to
		pstmt->setInt(1, from); // from id
		pstmt->setInt(2, to);
		pstmt->setString(3, back_name);
		// 执行更新
		int rowAffected = pstmt->executeUpdate();
		if (rowAffected < 0) {
			con->m_conn->rollback();
			return false;
		}

		//准备第二个SQL语句，插入申请方好友数据
		std::unique_ptr<sql::PreparedStatement> pstmt2(con->m_conn->prepareStatement("INSERT IGNORE INTO friend(self_id, friend_id, back) "
			"VALUES (?, ?, ?) "
		));
		//反过来的申请时from，验证时to
		pstmt2->setInt(1, to); // from id
		pstmt2->setInt(2, from);
		pstmt2->setString(3, "");
		// 执行更新
		int rowAffected2 = pstmt2->executeUpdate();
		if (rowAffected2 < 0) {
			con->m_conn->rollback();
			return false;
		}

		// 提交事务
		con->m_conn->commit();
		std::cout << "addfriend insert friends success" << std::endl;

		return true;
	}
	catch (sql::SQLException& e) {
		// 如果发生错误，回滚事务
		if (con) {
			con->m_conn->rollback();
		}
		std::cerr << "MysqlDao::AddFriend SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

// bool MysqlDao::TestProcedure(const std::string& email, int& uid, std::string& name) {
// 	auto con = m_pool->getConnection();
// 	try {
// 		if (con == nullptr) {
// 			return false;
// 		}
//
// 		Defer defer([this, &con]() {
// 			m_pool->returnConnection(std::move(con));
// 		});
// 		// 准备调用存储过程
// 		std::unique_ptr<sql::PreparedStatement> stmt(
// 			con->m_conn->prepareStatement("CALL test_procedure(?,@userId,@userName)"));
// 		// 设置输入参数
// 		stmt->setString(1, email);
//
// 		// 由于PreparedStatement不直接支持注册输出参数，我们需要使用会话变量或其他方法来获取输出参数的值
//
// 		// 执行存储过程
// 		stmt->execute();
// 		// 如果存储过程设置了会话变量或有其他方式获取输出参数的值，你可以在这里执行SELECT查询来获取它们
// 		// 例如，如果存储过程设置了一个会话变量@result来存储输出结果，可以这样获取：
// 		std::unique_ptr<sql::Statement> stmtResult(con->m_conn->createStatement());
// 		std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @userId AS uid"));
// 		if (!(res->next())) {
// 			return false;
// 		}
//
// 		uid = res->getInt("uid");
// 		std::cout << "uid: " << uid << std::endl;
//
// 		stmtResult.reset(con->m_conn->createStatement());
// 		res.reset(stmtResult->executeQuery("SELECT @userName AS name"));
// 		if (!(res->next())) {
// 			return false;
// 		}
//
// 		name = res->getString("name");
// 		std::cout << "name: " << name << std::endl;
// 		return true;
// 	}
// 	catch (sql::SQLException& e) {
// 		std::cerr << "SQLException: " << e.what();
// 		std::cerr << " (MySQL error code: " << e.getErrorCode();
// 		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
// 		return false;
// 	}
// }
