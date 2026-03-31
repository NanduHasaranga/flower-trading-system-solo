#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QUrl>

#include <exception>
#include <variant>
#include <vector>

#include "Core/ExecutionReport.hpp"
#include "Core/Order.hpp"
#include "Core/OrderReject.hpp"
#include "Engine/Exchange.hpp"
#include "IO/CsvReader.hpp"
#include "IO/CsvWriter.hpp"
#include "IO/OrderProcessor.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    updateUiState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnUpload_clicked()
{
    const QString defaultDirectory = inputFilePath.isEmpty()
                                         ? QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                                         : QFileInfo(inputFilePath).absolutePath();

    const QString selectedFile = QFileDialog::getOpenFileName(
        this,
        tr("Select Order CSV"),
        defaultDirectory,
        tr("CSV Files (*.csv);;All Files (*.*)"));

    if (selectedFile.isEmpty())
    {
        return;
    }

    inputFilePath = selectedFile;
    generatedReportPath.clear();

    ui->inputPathEdit->setText(inputFilePath);
    ui->outputPathEdit->clear();
    ui->statusLabel->setStyleSheet("font: 10pt \"Segoe UI\"; color: rgb(80, 80, 80);");
    ui->statusLabel->setText(
        tr("Input file selected: %1. Click Process to generate a new execution report.")
            .arg(QFileInfo(inputFilePath).fileName()));

    updateUiState();
}
