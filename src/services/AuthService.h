#pragma once
#include <QString>
#include <optional>
#include "../models/Admin.h"
#include "../models/Reader.h"

enum class UserRole { Reader, Admin, SuperAdmin };

struct AuthResult {
    bool ok = false;
    QString message;
    std::optional<Admin> admin;
    std::optional<Reader> reader;
    UserRole role = UserRole::Reader;
};

class AuthService {
public:
    static AuthService &instance();
    AuthResult loginAdmin(const QString &username, const QString &passwordPlain);
    AuthResult loginReader(const QString &username, const QString &passwordPlain);
    static QString hashPassword(const QString &plain);
};


