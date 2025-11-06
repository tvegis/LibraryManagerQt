#include "AppConfig.h"
#include <QStandardPaths>
#include <QDir>

AppConfig &AppConfig::instance() {
    static AppConfig inst;
    return inst;
}

QString AppConfig::databaseFilePath() const {
    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    return dataDir + "/library_manager.sqlite";
}

int AppConfig::loanDaysDefault() const { return 30; }
int AppConfig::maxRenewTimes() const { return 2; }
double AppConfig::finePerOverdueDay() const { return 0.5; }


