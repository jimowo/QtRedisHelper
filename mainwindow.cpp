#include <QComboBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QMessageBox>
#include <QGroupBox>
#include <QPlainTextEdit>
#include <QSizePolicy>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hiredis.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    createForm();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::createForm()
//{
//    auto layout = new QGridLayout(ui->centralwidget);

//    auto connectBtn = new QPushButton("Connect");
//    layout->addWidget(connectBtn);

//    auto disconnectBtn = new QPushButton("Disconnect");
//    disconnectBtn->setEnabled(false);
//    layout->addWidget(disconnectBtn);

//    auto statusLabel = new QLabel("Disconnected");
//    layout->addWidget(statusLabel);

//    auto treeWidget = new QTreeWidget();
//    treeWidget->setHeaderLabels({"Key", "Type", "Size"});
//    layout->addWidget(treeWidget);

//    // kvDisplayGroup
//    auto kvDispGroup = new QGroupBox();
//    auto keyTypeLabel = new QLabel();
//    auto keyDispLineEdit = new QLineEdit();
//    keyDispLineEdit->setReadOnly(true);
//    auto ttlLabel = new QLabel();
//    ttlLabel->setText("TTL: -1");   // if not set ttl, diaplay -1
//    auto renameBtn = new QPushButton();
//    auto deleteCurKeyBtn = new QPushButton();
//    auto reloadBtn = new QPushButton();
//    auto setTtlBtn = new QPushButton();
//    auto valueDispLabel = new QLabel();
//    valueDispLabel->setText("Value: ");
//    auto hSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
//    auto valueViewTypeLable = new QLabel();
//    valueViewTypeLable->setText("View as: ");
//    auto valueViewTypeComboBox = new QComboBox();
//    valueViewTypeComboBox->addItems({"Plain Text", "Json"});
//    auto valueViewType = new QHBoxLayout();
//    valueViewType->addWidget(valueViewTypeLable, Qt::AlignRight | Qt::AlignVCenter);
//    valueViewType->addWidget(valueViewTypeComboBox);
//    auto valuePlainText = new QPlainTextEdit();
//    auto kvDispGroupLayout = new QGridLayout();
//    kvDispGroupLayout->addWidget(keyTypeLabel, 0, 0, 1, 1);
//    kvDispGroupLayout->addWidget(keyDispLineEdit, 0, 1, 1, 3);
//    kvDispGroupLayout->addWidget(ttlLabel, 0, 4, 1, 1);
//    kvDispGroupLayout->addWidget(renameBtn, 0, 5, 1, 1);
//    kvDispGroupLayout->addWidget(deleteCurKeyBtn, 0, 6, 1, 1);
//    kvDispGroupLayout->addWidget(reloadBtn, 0, 7, 1, 1);
//    kvDispGroupLayout->addWidget(setTtlBtn, 0, 8, 1, 1);
//    kvDispGroupLayout->addWidget(valueDispLabel, 1, 0, 1, 2);
//    kvDispGroupLayout->addWidget(valueViewTypeLable, 1, 2, 1, 1);
//    kvDispGroupLayout->addItem(hSpacer, 1, 3, 1, 4);
//    kvDispGroupLayout->addLayout(valueViewType, 1, 7, 1, 2, Qt::AlignRight | Qt::AlignVCenter);
//    kvDispGroupLayout->addWidget(valuePlainText, 2, 0, 5, 9);

//    kvDispGroup->setLayout(kvDispGroupLayout);
//    layout->addWidget(kvDispGroup);


//    auto keyLineEdit = new QLineEdit();
//    auto valueLineEdit = new QLineEdit();
//    auto addBtn = new QPushButton("Add");
//    auto keyLabel = new QLabel();
//    keyLabel->setText("key:");
//    auto valueLabel = new QLabel();
//    valueLabel->setText("value:");
//    addBtn->setEnabled(false);
//    auto deleteBtn = new QPushButton("Delete");
//    deleteBtn->setEnabled(false);

//    auto hLayout = new QHBoxLayout();
//    hLayout->addWidget(keyLabel);
//    hLayout->addWidget(keyLineEdit);
//    hLayout->addWidget(valueLabel);
//    hLayout->addWidget(valueLineEdit);
//    hLayout->addWidget(addBtn);
//    hLayout->addWidget(deleteBtn);






//    QObject::connect(addBtn, &QPushButton::clicked, [statusLabel, treeWidget, keyLineEdit, valueLineEdit]()
//    {
//        auto keyName = keyLineEdit->text();
//        auto keyValue = valueLineEdit->text();
//        if (keyName.isEmpty() || keyValue.isEmpty())
//        {
//            QMessageBox::warning(nullptr, "Warning", "Please input both Key and Value.");
//            return;
//        }

//        auto rContext = redisConnect("localhost", 6379);
//        if (rContext == nullptr || rContext->err)
//        {
//            const char* errMsg = rContext == nullptr ? "Connection error" : rContext->errstr;
//            qDebug() << errMsg;
//            statusLabel->setText(errMsg);
//            redisFree(rContext);
//            return;
//        }

//        auto reply = (redisReply*)redisCommand(rContext, "SET %s %s", keyName.toLatin1().constData(), keyValue.toLatin1().constData());
//        if (reply == nullptr || reply->type != REDIS_REPLY_STATUS)
//        {
//            qDebug() << "Failed to set key-value pair.";
//            redisFree(rContext);
//            return;
//        }

//        qDebug() << "Key-Value pair added: " << keyName << "-" << keyValue;
//        statusLabel->setText("Key-Value pair added.");
//        QTreeWidgetItem* item = new QTreeWidgetItem({keyName, "string", QString::number(keyValue.size())});
//        treeWidget->addTopLevelItem(item);

//        freeReplyObject(reply);
//        redisFree(rContext);

//        keyLineEdit->setText("");
//        valueLineEdit->setText("");
//    });





//}

void MainWindow::clearKvTab()
{
    ui->renameBtn->setEnabled(false);
    ui->deleteBtn->setEnabled(false);
    ui->reloadBtn->setEnabled(false);
    ui->setTtlBtn->setEnabled(false);
    ui->keyLineEdit->setText("");
    ui->textEdit->setText("");
    ui->typeLabel->setText("Null");
    ui->ttlLabel->setText("TTL: ");
    ui->sizeLabel->setText("Size: ");

}

void MainWindow::on_renameBtn_clicked()
{

}

void MainWindow::on_deleteBtn_clicked()
{
    auto keyName = ui->keyLineEdit->text();
    if (keyName.isEmpty())
    {
        QMessageBox::warning(nullptr, "Warning", "Please input Key to delete.");
        return;
    }

    auto rContext = redisConnect("localhost", 6379);
    if (rContext == nullptr || rContext->err)
    {
        const char* errMsg = rContext == nullptr ? "Connection error" : rContext->errstr;
        qDebug() << errMsg;
        ui->statusbar->showMessage(errMsg);
        redisFree(rContext);
        return;
    }

    auto reply = (redisReply*)redisCommand(rContext, "DEL %s", keyName.toLatin1().constData());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER)
    {
        qDebug() << "Failed to delete key: " << keyName;
        redisFree(rContext);
        return;
    }

    qDebug() << "Key deleted: " << keyName;
    ui->statusbar->showMessage("Key deleted.");
    auto items = ui->treeWidget->findItems(keyName, Qt::MatchExactly);
    for (auto item : items)
    {
        int index = ui->treeWidget->indexOfTopLevelItem(item);
        ui->treeWidget->takeTopLevelItem(index);
        delete item;
    }

    freeReplyObject(reply);
    redisFree(rContext);

    clearKvTab();

}

void MainWindow::on_reloadBtn_clicked()
{

}

void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::on_comboBox_currentIndexChanged(const QString& arg1)
{

}

void MainWindow::on_actionConnect_triggered()
{
    ui->statusbar->showMessage("Connecting...");
    ui->actionConnect->setEnabled(false);

    auto rContext = redisConnect("localhost", 6379);
    if (rContext == nullptr || rContext->err)
    {
        const char* errMsg = rContext == nullptr ? "Connection error" : rContext->errstr;
        qDebug() << errMsg;
        ui->statusbar->showMessage(errMsg);
        ui->actionConnect->setEnabled(true);
    }
    else
    {
        qDebug() << "Connected";
        ui->statusbar->showMessage("Connected");
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->deleteBtn->setEnabled(true);

        // get all K-V
        redisReply* reply = (redisReply*)redisCommand(rContext, "keys *");

        if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY)
        {
            qDebug() << "Failed to get keys list";
        }
        else
        {
            for (int i = 0; i < reply->elements; ++i)
            {
                auto keyName = reply->element[i]->str;
                auto keyTypeReply = (redisReply*)redisCommand(rContext, "type %s", keyName);

                if (keyTypeReply == nullptr || keyTypeReply->type != REDIS_REPLY_STATUS)
                {
                    qDebug() << "Failed to get key type for: " << keyName;
                    continue;
                }

                auto keyType = QString(keyTypeReply->str).toLower();
                QTreeWidgetItem* item = new QTreeWidgetItem({keyName, keyType, ""});
                if (keyType == "string")
                {
                    redisReply* keyValueReply = (redisReply*)redisCommand(rContext, "get %s", keyName);
                    if (keyValueReply != nullptr && keyValueReply->type == REDIS_REPLY_STRING)
                    {
                        item->setText(2, QString::number(keyValueReply->len));
                    }
                    freeReplyObject(keyValueReply);
                }
                else if (keyType == "list")
                {
                    redisReply* keyLengthReply = (redisReply*)redisCommand(rContext, "llen %s", keyName);
                    if (keyLengthReply != nullptr && keyLengthReply->type == REDIS_REPLY_INTEGER)
                    {
                        item->setText(2, QString::number(keyLengthReply->integer));
                    }
                    freeReplyObject(keyLengthReply);
                }
                else if (keyType == "set")
                {
                    redisReply* keyLengthReply = (redisReply*)redisCommand(rContext, "scard %s", keyName);
                    if (keyLengthReply != nullptr && keyLengthReply->type == REDIS_REPLY_INTEGER)
                    {
                        item->setText(2, QString::number(keyLengthReply->integer));
                    }
                    freeReplyObject(keyLengthReply);
                }
                else if (keyType == "zset")
                {
                    redisReply* keyLengthReply = (redisReply*)redisCommand(rContext, "zcard %s", keyName);
                    if (keyLengthReply != nullptr && keyLengthReply->type == REDIS_REPLY_INTEGER)
                    {
                        item->setText(2, QString::number(keyLengthReply->integer));
                    }
                    freeReplyObject(keyLengthReply);
                }
                else if (keyType == "hash")
                {
                    redisReply* keyLengthReply = (redisReply*)redisCommand(rContext, "hlen %s", keyName);
                    if (keyLengthReply != nullptr && keyLengthReply->type == REDIS_REPLY_INTEGER)
                    {
                        item->setText(2, QString::number(keyLengthReply->integer));
                    }
                    freeReplyObject(keyLengthReply);
                }
                else
                {
                    qDebug() << "Unknown key type: " << keyType;
                }

                ui->treeWidget->addTopLevelItem(item);
                freeReplyObject(keyTypeReply);
            }
        }

        freeReplyObject(reply);
        redisFree(rContext);
    }
}

void MainWindow::on_actionDisconnect_triggered()
{
    ui->statusbar->showMessage("Disconnected");
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->treeWidget->clear();

    clearKvTab();
}
