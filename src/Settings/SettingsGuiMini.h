#ifndef SETTINGSGUIMINI_H
#define SETTINGSGUIMINI_H

#include <QObject>
#include "DeviceSettingsMini.h"

class MainWindow;

class SettingsGuiMini : public DeviceSettingsMini
{
    Q_OBJECT

public:
    explicit SettingsGuiMini(QObject* parent);
    void loadParameters() override;
    void updateParam(MPParams::Param param, int val) override;
    void createSettingUIMapping(MainWindow *mw);
    bool checkSettingsChanged();
    void getChangedSettings(QJsonObject& o, bool isNoCardInsterted);

private:
    MainWindow* m_mw = nullptr;
};

#endif // SETTINGSGUIMINI_H
