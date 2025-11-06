#include "AuthService.h"
#include "../repos/AdminRepository.h"
#include "../repos/ReaderRepository.h"
#include <QCryptographicHash>

AuthService &AuthService::instance() {
    static AuthService inst;
    return inst;
}

QString AuthService::hashPassword(const QString &plain) {
    // 简化：演示使用明文或MD5；生产不建议
    QByteArray md5 = QCryptographicHash::hash(plain.toUtf8(), QCryptographicHash::Md5).toHex();
    return QString::fromUtf8(md5);
}

AuthResult AuthService::loginAdmin(const QString &username, const QString &passwordPlain) {
    AuthResult r; r.ok = false;
    auto adminOpt = AdminRepository::instance().getByUsername(username);
    if (!adminOpt) { r.message = QObject::tr("管理员不存在"); return r; }
    const auto &admin = *adminOpt;
    if (admin.passwordHash == passwordPlain || admin.passwordHash == hashPassword(passwordPlain)) {
        r.ok = true; r.admin = admin; r.role = admin.role >= 9 ? UserRole::SuperAdmin : UserRole::Admin; return r;
    }
    r.message = QObject::tr("密码错误");
    return r;
}

AuthResult AuthService::loginReader(const QString &username, const QString &passwordPlain) {
    AuthResult r; r.ok = false; r.role = UserRole::Reader;
    auto readerOpt = ReaderRepository::instance().getByUsername(username);
    if (!readerOpt) { r.message = QObject::tr("读者不存在"); return r; }
    const auto &reader = *readerOpt;
    if (reader.status == 0) { r.message = QObject::tr("账号已注销"); return r; }
    if (reader.passwordHash == passwordPlain || reader.passwordHash == hashPassword(passwordPlain)) {
        r.ok = true; r.reader = reader; return r;
    }
    r.message = QObject::tr("密码错误");
    return r;
}


