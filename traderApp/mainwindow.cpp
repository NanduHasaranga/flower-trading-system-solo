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

void MainWindow::on_btnProcess_clicked()
{
    if (inputFilePath.isEmpty())
    {
        QMessageBox::warning(this, tr("Missing Input"), tr("Please upload an order CSV file first."));
        return;
    }

    const QFileInfo inputInfo(inputFilePath);
    const QString outputPath = QDir(inputInfo.absolutePath()).filePath("execution_report.csv");

    std::string errorMessage;
    const bool success = processOrders(inputFilePath.toStdString(), outputPath.toStdString(), errorMessage);
    if (!success)
    {
        generatedReportPath.clear();
        ui->outputPathEdit->clear();
        ui->statusLabel->setStyleSheet("font: 10pt \"Segoe UI\"; color: rgb(180, 40, 40);");
        ui->statusLabel->setText(tr("Processing failed. Please check the input file."));
        updateUiState();

        QMessageBox::critical(
            this,
            tr("Processing Failed"),
            tr("Could not process orders from the selected file.\n\nDetails: %1")
                .arg(QString::fromStdString(errorMessage)));
        return;
    }

    if (!QFileInfo::exists(outputPath))
    {
        generatedReportPath.clear();
        ui->outputPathEdit->clear();
        ui->statusLabel->setStyleSheet("font: 10pt \"Segoe UI\"; color: rgb(180, 40, 40);");
        ui->statusLabel->setText(tr("Processing failed. Report file was not generated."));
        updateUiState();

        QMessageBox::critical(
            this,
            tr("Processing Failed"),
            tr("Processing completed, but execution_report.csv could not be created at:\n%1")
                .arg(outputPath));
        return;
    }

    generatedReportPath = outputPath;
    ui->outputPathEdit->setText(generatedReportPath);
    ui->statusLabel->setStyleSheet("font: 10pt \"Segoe UI\"; color: rgb(24, 128, 56);");
    ui->statusLabel->setText(tr("Processing complete. Report ready to download."));
    updateUiState();

    QMessageBox::information(this, tr("Processing Complete"), tr("Execution report generated successfully."));
}

