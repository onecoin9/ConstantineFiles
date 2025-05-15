#include "measureDialog.h"
#include "ui_measureDialog.h"

MeasurementDialog::MeasurementDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::MeasurementDialog) {
    ui->setupUi(this);

    // ������ʽ��
    setStyleSheet(
        "QDialog {"
        "    background-color: #ffffff;"  // ��ɫ����
        "    color: #000000;"  // ��ɫ����
        "}"
        "QGroupBox {"
        "    border: 1px solid #000000;"  // ��ɫ�߿�
        "    margin-top: 10px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    padding: 0 3px;"
        "    background-color: #ffffff;"  // ��ɫ����
        "    color: #000000;"  // ��ɫ����
        "}"
        "QLabel {"
        "    color: #000000;"  // ��ɫ����
        "}"
        "QLineEdit {"
        "    border: 1px solid #000000;"  // ��ɫ�߿�
        "    background-color: #f0f0f0;"  // ǳ��ɫ����
        "    color: #000000;"  // ��ɫ����
        "}"
        "QPushButton {"
        "    background-color: #000000;"  // ��ɫ����
        "    color: #ffffff;"  // ��ɫ����
        "    border: none;"
        "    padding: 5px 10px;"
        "    border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #333333;"  // ���ɫ����
        "}"
        "QComboBox {"
        "    border: 1px solid #000000;"  // ��ɫ�߿�
        "    background-color: #f0f0f0;"  // ǳ��ɫ����
        "    color: #000000;"  // ��ɫ����
        "}"
    );
    initCheckbox();
    connect(ui->checkBoxPinContinuity, &QCheckBox::toggled, this, &MeasurementDialog::handlePinContinuityTest);
    connect(ui->checkBoxPowerOnCurrent, &QCheckBox::toggled, this, &MeasurementDialog::handlePowerOnCurrentTest);
    connect(ui->checkBoxFrequencyScreening, &QCheckBox::toggled, this, &MeasurementDialog::handleFrequencyScreening);
    connect(ui->checkBoxScaleCalibration, &QCheckBox::toggled, this, &MeasurementDialog::handleScaleCalibration);
    connect(ui->checkBoxFunctionalSafetyCalibration, &QCheckBox::toggled, this, &MeasurementDialog::handleFunctionalSafetyCalibration);
    connect(ui->checkBoxWriteOTP, &QCheckBox::toggled, this, &MeasurementDialog::handleWriteOTP);
    connect(ui->checkBoxReadPhaseInfo, &QCheckBox::toggled, this, &MeasurementDialog::handlePSI5Test);
    connect(ui->checkBoxPinContinuity, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
    connect(ui->checkBoxPowerOnCurrent, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
    connect(ui->checkBoxFrequencyScreening, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
    connect(ui->checkBoxScaleCalibration, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
    connect(ui->checkBoxFunctionalSafetyCalibration, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
    connect(ui->checkBoxWriteOTP, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
    connect(ui->checkBoxReadPhaseInfo, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
    connect(ui->checkBoxReadZeroNoise, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
    connect(ui->checkBoxReadManchesterCodeCurrent, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
    connect(ui->checkBoxMeasureBitTime, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
    connect(ui->checkBoxMeasureTDelay, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
    connect(ui->checkBoxSyncPulseVDD, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
    connect(ui->checkBoxAsynchronousMode, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
    connect(ui->checkBoxDualTimeSlotMode, &QCheckBox::toggled, this, &MeasurementDialog::updateCheckBoxStatus);
}
    // Add more connections as needed

MeasurementDialog::~MeasurementDialog() {
    delete ui;
}   

void MeasurementDialog::initCheckbox()
{
    ui->checkBoxPinContinuity->setChecked(true);
    ui->checkBoxPowerOnCurrent->setChecked(true);
    ui->checkBoxFrequencyScreening->setChecked(true);
    ui->checkBoxScaleCalibration->setChecked(true);
    ui->checkBoxFunctionalSafetyCalibration->setChecked(true);
    ui->checkBoxWriteOTP->setChecked(true);
    ui->checkBoxReadPhaseInfo->setChecked(true);
    ui->checkBoxReadZeroNoise->setChecked(true);
    ui->checkBoxReadManchesterCodeCurrent->setChecked(true);
    ui->checkBoxMeasureBitTime->setChecked(true);
    ui->checkBoxMeasureTDelay->setChecked(true);
    ui->checkBoxSyncPulseVDD->setChecked(true);
    ui->checkBoxAsynchronousMode->setChecked(true);
    ui->checkBoxDualTimeSlotMode->setChecked(true);
}

CheckBoxStatus MeasurementDialog::collectCheckBoxStatus()
{
    status.pinContinuityTest = ui->checkBoxPinContinuity->isChecked();
    status.powerOnCurrentTest = ui->checkBoxPowerOnCurrent->isChecked();
    status.frequencyScreening = ui->checkBoxFrequencyScreening->isChecked();
    status.scaleCalibration = ui->checkBoxScaleCalibration->isChecked();
    status.functionalSafetyCalibration = ui->checkBoxFunctionalSafetyCalibration->isChecked();
    status.writeOTP = ui->checkBoxWriteOTP->isChecked();
    status.readPhaseInfo = ui->checkBoxReadPhaseInfo->isChecked();
    status.readZeroNoise = ui->checkBoxReadZeroNoise->isChecked();
    status.readManchesterCodeCurrent = ui->checkBoxReadManchesterCodeCurrent->isChecked();
    status.measureBitTime = ui->checkBoxMeasureBitTime->isChecked();
    status.measureTDelay = ui->checkBoxMeasureTDelay->isChecked();
    status.syncPulseVDD = ui->checkBoxSyncPulseVDD->isChecked();
    status.asynchronousMode = ui->checkBoxAsynchronousMode->isChecked();
    status.dualTimeSlotMode = ui->checkBoxDualTimeSlotMode->isChecked();
    return status;
}


void MeasurementDialog::updateCheckBoxStatus()
{
    CheckBoxStatus status = collectCheckBoxStatus();
    // �����ﴦ����º��״̬�����緢�͸�������
    // otherClass->receiveCheckBoxStatus(status);
}

void MeasurementDialog::handlePinContinuityTest()
{
    // ����ܽ�ͨ�ϲ��Ե��߼�
}

void MeasurementDialog::handlePowerOnCurrentTest()
{
    // �����ϵ�������Ե��߼�
}

void MeasurementDialog::handleFrequencyScreening()
{
    // ����Ƶ��ɸѡ���߼�
}

void MeasurementDialog::handleScaleCalibration()
{
    // �����ȱ궨���߼�
}

void MeasurementDialog::handleFunctionalSafetyCalibration()
{
    // �����ܰ�ȫ�궨���߼�
}

void MeasurementDialog::handleWriteOTP()
{
    // ������дOTP���߼�
}

void MeasurementDialog::handlePSI5Test()
{
    // ����PSI5�²��Ե��߼�
}