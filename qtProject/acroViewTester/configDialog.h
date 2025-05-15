#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QWidget> // �޸�Ϊ QWidget
namespace Ui {
    class ConfigDialog;
}

class ConfigDialog : public QWidget // �޸Ļ���Ϊ QWidget
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
    void on_tableFlowCellChanged(int row, int column); // ���� tableFlow �޸�
    void on_btnExportFlow_clicked(); // ���� flow �����ļ�
    void on_btnInport_clicked(const QString& buttonId);
    void on_btnExportSpec_clicked(); 
    void on_tableSpecCellChanged(int row, int column); // ���� tableSpec �޸�
    void on_btnAddFlow_clicked();    // ����һ�е� tableFlow
    void on_btnInsertFlow_clicked(); // ����һ�е� tableFlow
    void on_btnDeleteFlow_clicked(); // ɾ��ѡ���е� item
    void on_btnMoveUpFlow_clicked();   // ����ѡ����
    void on_btnMoveDownFlow_clicked();  // ����ѡ����
private:
    void loadFlowConfig(const QJsonObject& flowConfig); 
public:
    void updateTableFlow();
    void updateTableSpec();
    void initTableFlow();
    void initTableSpec();
    void initTableBin();  // ��ʼ�� BIN ҳ��
    void initTablePin();  // ��ʼ�� PIN ҳ��
    void populateTableSpecRow(int row, const SpecItem& item); // ��� Spec �����
    void loadSpecConfig(const QJsonObject& specConfig); // ���� Spec ����
private:
    Ui::ConfigDialog* ui;
    int m_currentRow = -1; 
    QList<TestItem> testItems; 
    QList<SpecItem> specItems;
    QString tableStyleSheet;
};

#endif // CONFIGDIALOG_H