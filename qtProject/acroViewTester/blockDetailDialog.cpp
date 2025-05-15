//// blockdetaildialog.cpp
//#include "blockdetaildialog.h"
//#include "ui_blockdetaildialog.h"
//
//BlockDetailDialog::BlockDetailDialog(const TestSite::BlockData& data, QWidget* parent)
//    : QDialog(parent)
//    , ui(new Ui::BlockDetailDialog)
//    , m_data(data)
//{
//    ui->setupUi(this);
//    setupUI();
//    setupCharts();
//    updateInfo();
//}
//
//BlockDetailDialog::~BlockDetailDialog()
//{
//   
//}
//
//void BlockDetailDialog::updateInfo()
//{
//    // ���»�����Ϣ��ǩ
//    ui->labelLib->setText(QString("Lib: %1").arg(m_data.libNumber));
//    ui->labelSite->setText(QString("Site: %1").arg(m_data.siteNumber));
//    ui->labelPass->setText(QString("Pass: %1").arg(m_data.pass));
//    ui->labelOSFail->setText(QString("OS Fail: %1").arg(m_data.osFail));
//    ui->labelLeakageFail->setText(QString("Leakage Fail: %1").arg(m_data.leakageFail));
//    ui->labelProgramFail->setText(QString("Program Fail: %1").arg(m_data.programFail));
//
//}
//void BlockDetailDialog::setupUI()
//{
//    setWindowTitle("Block��ϸ��Ϣ");
//
//    // ���û�����Ϣ
//    ui->labelLib->setText(QString("Lib: %1").arg(m_data.libNumber));
//    ui->labelSite->setText(QString("Site: %1").arg(m_data.siteNumber));
//    ui->labelPass->setText(QString("Pass: %1").arg(m_data.pass));
//    ui->labelOSFail->setText(QString("OS Fail: %1").arg(m_data.osFail));
//    ui->labelLeakageFail->setText(QString("Leakage Fail: %1").arg(m_data.leakageFail));
//    ui->labelProgramFail->setText(QString("Program Fail: %1").arg(m_data.programFail));
//}
//
//void BlockDetailDialog::setupCharts()
//{
//    // ����ͼ��
//    createOSChart();
//    createLeakageChart();
//
//    // ��ӵ�Tabҳ
//    ui->tabWidget->addTab(m_osChartView, "OS");
//    ui->tabWidget->addTab(m_leakageChartView, "Leakage");
//}
//
//void BlockDetailDialog::createOSChart()
//{
//    QChart* chart = new QChart();
//    QLineSeries* series = new QLineSeries();
//
//    // ������ݵ�
//    for (int i = 0; i < m_data.osData.size(); ++i) {
//        series->append(i, m_data.osData[i]);
//    }
//
//    chart->addSeries(series);
//    chart->createDefaultAxes();
//    chart->setTitle("OS����ͼ");
//
//    m_osChartView = new QChartView(chart);
//    m_osChartView->setRenderHint(QPainter::Antialiasing);
//}
//
//void BlockDetailDialog::createLeakageChart()
//{
//    QChart* chart = new QChart();
//    QLineSeries* series = new QLineSeries();
//
//    // ������ݵ�
//    for (int i = 0; i < m_data.leakageData.size(); ++i) {
//        series->append(i, m_data.leakageData[i]);
//    }
//
//    chart->addSeries(series);
//    chart->createDefaultAxes();
//    chart->setTitle("Leakage����ͼ");
//
//    m_leakageChartView = new QChartView(chart);
//    m_leakageChartView->setRenderHint(QPainter::Antialiasing);
//}