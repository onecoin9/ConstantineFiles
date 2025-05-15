#include"peripheralControl.h"

PeripheralControl::PeripheralControl(QWidget* parent) : QWidget(parent) {
    // 风扇控制
    fanLabel = new QLabel("风扇控制", this);
    for (int i = 0; i < 5; ++i) {
        QPushButton* button = new QPushButton(QString::number(i), this);
        fanButtons.append(button);
        connect(button, &QPushButton::clicked, this, &PeripheralControl::onFanButtonClicked);
    }

    // 电源控制
    powerLabel = new QLabel("电源序号", this);
    powerInput = new QLineEdit(this);
    QStringList powerButtonNames = { "开风扇", "关风扇", "写风扇", "开电源", "关电源", "写电源" };
    for (const QString& name : powerButtonNames) {
        QPushButton* button = new QPushButton(name, this);
        powerButtons.append(button);
        connect(button, &QPushButton::clicked, this, &PeripheralControl::onPowerButtonClicked);
    }

    // 负载仪控制
    loadInstrumentComboBox = new QComboBox(this);
    loadInstrumentComboBox->addItems({ "Channel 1", "Channel 2", "Channel 3" });
    connect(loadInstrumentComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PeripheralControl::onLoadInstrumentChannelChanged);
    loadInstrumentLabel = new QLabel("SITE 1", this);
}

void PeripheralControl::onFanButtonClicked() {
    // 处理风扇按钮点击事件
}

void PeripheralControl::onPowerButtonClicked() {
    // 处理电源按钮点击事件
}

void PeripheralControl::onLoadInstrumentChannelChanged(int index) {
    // 处理负载仪通道改变事件
}