#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnUpload_clicked();
    void on_btnProcess_clicked();
    void on_btnViewReport_clicked();
    void on_btnDownload_clicked();

private:
    Ui::MainWindow *ui;
    QString inputFilePath;
    QString generatedReportPath;

    void updateUiState();
    bool processOrders(const std::string &inputPath, const std::string &outputPath, std::string &errorMessage) const;
};
#endif // MAINWINDOW_H