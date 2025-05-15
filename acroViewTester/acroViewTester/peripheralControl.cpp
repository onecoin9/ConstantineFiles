#include"peripheralControl.h"

PeripheralControl::PeripheralControl(QWidget* parent) : QWidget(parent) {
    // ���ȿ���
    fanLabel = new QLabel("���ȿ���", this);
    for (int i = 0; i < 5; ++i) {
        QPushButton* button = new QPushButton(QString::number(i), this);
        fanButtons.append(button);
        connect(button, &QPushButton::clicked, this, &PeripheralControl::onFanButtonClicked);
    }

    // ��Դ����
    powerLabel = new QLabel("��Դ���", this);
    powerInput = new QLineEdit(this);
    QStringList powerButtonNames = { "������", "�ط���", "д����", "����Դ", "�ص�Դ", "д��Դ" };
    for (const QString& name : powerButtonNames) {
        QPushButton* button = new QPushButton(name, this);
        powerButtons.append(button);
        connect(button, &QPushButton::clicked, this, &PeripheralControl::onPowerButtonClicked);
    }

    // �����ǿ���
    loadInstrumentComboBox = new QComboBox(this);
    loadInstrumentComboBox->addItems({ "Channel 1", "Channel 2", "Channel 3" });
    connect(loadInstrumentComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PeripheralControl::onLoadInstrumentChannelChanged);
    loadInstrumentLabel = new QLabel("SITE 1", this);
}

void PeripheralControl::onFanButtonClicked() {
    // ������Ȱ�ť����¼�
}

void PeripheralControl::onPowerButtonClicked() {
    // �����Դ��ť����¼�
}

void PeripheralControl::onLoadInstrumentChannelChanged(int index) {
    // ��������ͨ���ı��¼�
}