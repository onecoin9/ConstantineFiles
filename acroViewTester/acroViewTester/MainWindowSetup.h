#ifndef MAINWINDOWSETUP_H
#define MAINWINDOWSETUP_H

#include "acroViewTester.h"

class MainWindowSetup {
public:
    static void initForm(acroViewTester* tester);
    static void loadViewSettings(acroViewTester* tester);
    static void setupTableView(acroViewTester* tester);
};

#endif // MAINWINDOWSETUP_H