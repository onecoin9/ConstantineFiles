#pragma once
#include <QtWidgets>
class PeripheralControl : public QWidget {
    Q_OBJECT

public:
    explicit PeripheralControl(QWidget* parent = nullptr);

private slots:
    void onFanButtonClicked();
    void onPowerButtonClicked();
    void onLoadInstrumentChannelChanged(int index);

private:
    // ���ȿ���
    QList<QPushButton*> fanButtons;
    QLabel* fanLabel;

    // ��Դ����
    QLabel* powerLabel;
    QLineEdit* powerInput;
    QList<QPushButton*> powerButtons;

    // �����ǿ���
    QComboBox* loadInstrumentComboBox;
    QLabel* loadInstrumentLabel;
};