#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QWidget> // 修改为 QWidget
namespace Ui {
    class ConfigDialog;
}

class ConfigDialog : public QWidget // 修改基类为 QWidget
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget* parent = nullptr);
    ~ConfigDialog();
    struct TestItem {
        QString id;
        QString className;
        QString alias;
        QString testMode;
        QString loopCount;
        QString passDo;
        QString failDo;
        QString comment;
    };
    struct SpecItem {
        QString id;          
        QString testItemName;
        QString aliasName;   
        QString params1Name; 
        QString params1Value;
        QString limit1Lower; 
        QString limit1Upper; 
        QString params2Name; 
        QString params2Value;
        QString limit2Lower; 
        QString limit2Upper; 
        QString params3Name;
        QString params3Value;
        QString limit3Lower; 
        QString limit3Upper;      
        QString binName;         
        QString comment;          

    };
private slots:
    void on_tableFlowTestModeChanged(int row, const QString& mode);
    void on_tableFlowCellChanged(int row, int column); // 监听 tableFlow 修改
    void on_btnExportFlow_clicked(); // 导出 flow 配置文件
    void on_btnInport_clicked(const QString& buttonId);
    void on_btnExportSpec_clicked(); 
    void on_tableSpecCellChanged(int row, int column); // 监听 tableSpec 修改
    void on_btnAddFlow_clicked();    // 新增一行到 tableFlow
    void on_btnInsertFlow_clicked(); // 插入一行到 tableFlow
    void on_btnDeleteFlow_clicked(); // 删除选定行的 item
    void on_btnMoveUpFlow_clicked();   // 上移选中行
    void on_btnMoveDownFlow_clicked();  // 下移选中行
private:
    void loadFlowConfig(const QJsonObject& flowConfig); 
public:
    void updateTableFlow();
    void updateTableSpec();
    void initTableFlow();
    void initTableSpec();
    void initTableBin();  // 初始化 BIN 页面
    void initTablePin();  // 初始化 PIN 页面
    void populateTableSpecRow(int row, const SpecItem& item); // 填充 Spec 表格行
    void loadSpecConfig(const QJsonObject& specConfig); // 加载 Spec 配置
private:
    Ui::ConfigDialog* ui;
    int m_currentRow = -1; 
    QList<TestItem> testItems; 
    QList<SpecItem> specItems;
    QString tableStyleSheet;
};

#endif // CONFIGDIALOG_H