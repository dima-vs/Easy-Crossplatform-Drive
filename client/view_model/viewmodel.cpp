#include "viewmodel.h"

viewmodel::viewmodel() {}

void viewmodel::on_saveToken(QString accessToken)
{
    QSettings settings("config.cfg", QSettings::IniFormat);
    settings.beginGroup("User");
    settings.setValue("accessToken", accessToken);
    settings.endGroup();
    settings.sync();
}
