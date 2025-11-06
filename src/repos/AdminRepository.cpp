#include "AdminRepository.h"
#include <QSqlQuery>
#include <QVariant>
#include "../db/DatabaseManager.h"

AdminRepository &AdminRepository::instance() {
    static AdminRepository inst;
    return inst;
}

std::optional<Admin> AdminRepository::getByUsername(const QString &username) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT * FROM admins WHERE username=:u");
    q.bindValue(":u", username);
    if (q.exec() && q.next()) {
        Admin a;
        a.id = q.value("id").toInt();
        a.username = q.value("username").toString();
        a.passwordHash = q.value("password_hash").toString();
        a.role = q.value("role").toInt();
        return a;
    }
    return std::nullopt;
}


