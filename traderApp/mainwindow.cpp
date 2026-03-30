#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setObjectName("MyMainWindow");
    this->setStyleSheet(
        "#MyMainWindow {"
        "   border-image: url(:/resources/images/background1.png) 0 0 0 0 stretch stretch;"
        "}"
    );

    connect(ui->btnOk, SIGNAL(clicked()),this,SLOT(on_btnOk_Clicked()));
    connect(ui->btnCancel, SIGNAL(clicked()),this,SLOT(on_btnCancel_Clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnOk_Clicked()
{
    qDebug() << "Ok button clicked";
}

void MainWindow::on_btnCancel_Clicked()
{
    qDebug() << "Cancel button clicked";
}
