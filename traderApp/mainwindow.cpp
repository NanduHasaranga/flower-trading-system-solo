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
#include "Utils/TimeUtils.hpp"

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

void MainWindow::on_btnViewReport_clicked()
{
    if (generatedReportPath.isEmpty() || !QFileInfo::exists(generatedReportPath))
    {
        QMessageBox::warning(this, tr("No Report"), tr("Please process an input file before viewing the report."));
        updateUiState();
        return;
    }

    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(generatedReportPath)))
    {
        QMessageBox::critical(this, tr("Open Failed"), tr("Unable to open the execution report."));
        return;
    }

    ui->statusLabel->setStyleSheet("font: 10pt \"Segoe UI\"; color: rgb(24, 128, 56);");
    ui->statusLabel->setText(tr("Opened report: %1").arg(generatedReportPath));
}

void MainWindow::on_btnDownload_clicked()
{
    if (generatedReportPath.isEmpty() || !QFileInfo::exists(generatedReportPath))
    {
        QMessageBox::warning(this, tr("No Report"), tr("Please process an input file before downloading."));
        updateUiState();
        return;
    }

    QString defaultDirectory = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (defaultDirectory.isEmpty())
    {
        defaultDirectory = QFileInfo(generatedReportPath).absolutePath();
    }

    const QString targetPath = QFileDialog::getSaveFileName(
        this,
        tr("Save Execution Report"),
        QDir(defaultDirectory).filePath("execution_report.csv"),
        tr("CSV Files (*.csv);;All Files (*.*)"));

    if (targetPath.isEmpty())
    {
        return;
    }

    const QString sourceAbsolutePath = QFileInfo(generatedReportPath).absoluteFilePath();
    const QString targetAbsolutePath = QFileInfo(targetPath).absoluteFilePath();
    if (QString::compare(sourceAbsolutePath, targetAbsolutePath, Qt::CaseInsensitive) == 0)
    {
        ui->statusLabel->setStyleSheet("font: 10pt \"Segoe UI\"; color: rgb(24, 128, 56);");
        ui->statusLabel->setText(tr("Report already available at: %1").arg(targetPath));

        QMessageBox::information(
            this,
            tr("Download Complete"),
            tr("Execution report is already saved at the selected location."));
        return;
    }

    if (QFile::exists(targetPath) && !QFile::remove(targetPath))
    {
        QMessageBox::critical(this, tr("Save Failed"), tr("Unable to overwrite the selected file."));
        return;
    }

    if (!QFile::copy(generatedReportPath, targetPath))
    {
        QMessageBox::critical(this, tr("Save Failed"), tr("Unable to save the execution report."));
        return;
    }

    ui->statusLabel->setStyleSheet("font: 10pt \"Segoe UI\"; color: rgb(24, 128, 56);");
    ui->statusLabel->setText(tr("Report downloaded to: %1").arg(targetPath));

    QMessageBox::information(this, tr("Download Complete"), tr("Execution report saved successfully."));
}

void MainWindow::updateUiState()
{
    const bool hasInputFile = !inputFilePath.isEmpty() && QFileInfo::exists(inputFilePath);
    const bool hasReportFile = !generatedReportPath.isEmpty() && QFileInfo::exists(generatedReportPath);

    ui->btnProcess->setEnabled(hasInputFile);
    ui->btnViewReport->setEnabled(hasReportFile);
    ui->btnDownload->setEnabled(hasReportFile);
}

bool MainWindow::processOrders(const std::string &inputPath, const std::string &outputPath, std::string &errorMessage) const
{
    try
    {
        std::vector<std::variant<ExecutionReport, OrderReject>> records;
        Exchange exchange;
        CsvWriter writer;
        CsvReader reader(inputPath);

        while (auto row = reader.nextRow())
        {
            const auto timestamp = utils::getCurrentTimestamp();
            auto result = OrderProcessor::processRow(*row, timestamp);

            if (auto *order = std::get_if<Order>(&result))
            {
                auto &orderBook = exchange.getOrderBook(*order);
                orderBook.processOrder(*order, records, timestamp);
            }
            else
            {
                records.emplace_back(std::get<OrderReject>(result));
            }
        }

        writer.writeExecutions(outputPath, records);
        return true;
    }
    catch (const std::exception &ex)
    {
        errorMessage = ex.what();
        return false;
    }
}