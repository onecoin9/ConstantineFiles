#include "acroViewTester.h"
void acroViewTester::loadStyle(const QString& qssFile)
{
	QFile file(qssFile);
	if (file.open(QFile::ReadOnly)) {
		QString qss = QLatin1String(file.readAll());
		setStyleSheet(qss);
		file.close();
	}
}

QString acroViewTester::getLocalIPAddress()
{
	QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
	//for (const QNetworkInterface& iface : interfaces) 
	//{
	//    if (iface.flags() & QNetworkInterface::IsUp &&
	//        !(iface.flags() & QNetworkInterface::IsLoopBack)) {

	//        QList<QNetworkAddressEntry> entries = iface.allAddresses();
	//        for (const QNetworkAddressEntry& entry : entries) {
	//            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
	//                return entry.ip().toString();
	//            }
	//        }
	//    }
	//}
	return "N/A";
}

QString acroViewTester::getCurrentVersion()
{
	int major = APP_VERSION_MAJOR;
	int minor = APP_VERSION_MINOR;
	int patch = APP_VERSION_PATCH;
	QString versionInfo = QString("%1.%2.%3")
		.arg(major)
		.arg(minor)
		.arg(patch);
	qDebug() << "��ǰ�汾��" << versionInfo;

	//��ʾ���²���
	//bool isVersionNewer(int major, int minor, int patch) {
	//    return (major > APP_VERSION_MAJOR) ||
	//        (major == APP_VERSION_MAJOR && minor > APP_VERSION_MINOR) ||
	//        (major == APP_VERSION_MAJOR && minor == APP_VERSION_MINOR && patch >= APP_VERSION_PATCH);
	//}
	//if (isVersionNewer(1, 0, 2)) {
	//    // ��ʾ����
	//}
	return versionInfo;

}

void TestNullPointer() {
	int* p = nullptr;
	*p = 100;  // ��������������dump
}

void TestArrayOverflow() {
	int arr[5] = { 1, 2, 3, 4, 5 };
	for (int i = 0; i <= 5; i++) {
		arr[i] = 0;  // ��i=5ʱԽ�����
	}
}
void TestDivideByZero() {
	int a = 100;
	int b = 0;
	int c = a / b;  // �������
}

void TestStackOverflow() {
	TestStackOverflow();  // ���޵ݹ鵼��ջ���
}

void acroViewTester::setupTestButtons()
{
	
	connect(ui.btnNull, &QPushButton::clicked, this, &acroViewTester::onTestNullPointer);
	connect(ui.btnArray, &QPushButton::clicked, this, &acroViewTester::onTestArrayOverflow);
	connect(ui.btnDiv, &QPushButton::clicked, this, &acroViewTester::onTestDivideByZero);
	connect(ui.btnStack, &QPushButton::clicked, this, &acroViewTester::onTestStackOverflow);

}
void acroViewTester::onTestNullPointer()
{
	TestNullPointer();
}

void acroViewTester::onTestArrayOverflow()
{
	TestArrayOverflow();
}

void acroViewTester::onTestDivideByZero()
{
	TestDivideByZero();
}

void acroViewTester::onTestStackOverflow()
{
	TestStackOverflow();
}