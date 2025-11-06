#pragma once
#include <optional>
#include "../models/Admin.h"

class AdminRepository {
public:
    static AdminRepository &instance();
    std::optional<Admin> getByUsername(const QString &username);
};


