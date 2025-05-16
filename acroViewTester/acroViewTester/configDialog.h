#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QWidget> // Include QWidget base class
namespace Ui {
    class ConfigDialog;
}

class ConfigDialog : public QWidget // Inherit from QWidget
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
    QString params;     
    QString value;      
    QString binName;
    QString comment;
    QString lowerLimit; // 添加下限
    QString upperLimit; // 添加上限
};

private slots:
    void on_tableFlowTestModeChanged(int row, const QString& mode);
    void on_tableFlowCellChanged(int row, int column); // Handle tableFlow cell changes
    void on_btnExportFlow_clicked(); // Export flow configuration
    void on_btnInport_clicked(const QString& buttonId);
    void on_btnExportSpec_clicked(); 
    void on_tableSpecCellChanged(int row, int column); // Handle tableSpec cell changes
    void on_btnAddFlow_clicked();    // Add new row to tableFlow
    void on_btnInsertFlow_clicked(); // Insert row to tableFlow
    void on_btnDeleteFlow_clicked(); // Delete selected item
    void on_btnMoveUpFlow_clicked();   // Move selected row up
    void on_btnMoveDownFlow_clicked(); // Move selected row down

private:
    void loadFlowConfig(const QJsonObject& flowConfig); 
    void updateSpecFromFlow(const TestItem& flowItem);
    QStringList parseCommentParams(const QString& comment);
    void generateSpecItems(const TestItem& flowItem, const QStringList& params);
    QString generateSpecItemID(const QString& baseID, int index);
    void syncSpecWithFlow();
public:
    void updateTableFlow();
    void updateTableSpec();
    void initTableFlow();
    void initTableSpec();
    void initTableBin();  // Initialize BIN table
    void initTablePin();  // Initialize PIN table
    void populateTableSpecRow(int row, const SpecItem& item); // Populate Spec table row
    void loadSpecConfig(const QJsonObject& specConfig); // Load Spec configuration

private:
    Ui::ConfigDialog* ui;
    int m_currentRow = -1; 
    QList<TestItem> testItems; 
    QList<SpecItem> specItems;
    QString tableStyleSheet;
};

#endif // CONFIGDIALOG_H