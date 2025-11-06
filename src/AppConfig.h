#pragma once
#include <QString>

class AppConfig {
public:
    static AppConfig &instance();

    QString databaseFilePath() const; // 默认放置在可写目录
    int loanDaysDefault() const;      // 默认借阅天数
    int maxRenewTimes() const;        // 最大续借次数
    double finePerOverdueDay() const; // 超期每天罚金

private:
    AppConfig() = default;
};


