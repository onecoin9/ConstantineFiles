#ifndef ROLEADDDIALOG_H
#define ROLEADDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include<QDebug>
#include "ui_roleAddDialog.h"
#pragma execution_character_set("utf-8")

namespace Ui {
    class RoleAddDialog;
}

class RoleAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RoleAddDialog(QWidget* parent = nullptr);
    ~RoleAddDialog();

private:
    Ui::RoleAddDialog* ui;
};

#endif // ROLEADDDIALOG_H