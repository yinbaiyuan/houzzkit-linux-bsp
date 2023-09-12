#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QStack>
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    void testResult(QString btnTxt, bool testResult);
    QString getCNName(int groupIndex);
	QStack<QString> stack;
    ~MainWindow();

private:
    void initWindow();

public slots:
    void testClicked(QString str);
    void autoTest();
};
#endif // MAINWINDOW_H
