#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QMessageBox>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hiredis.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createForm();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createForm()
{
    auto layout = new QVBoxLayout(ui->centralwidget);

    auto connectBtn = new QPushButton("Connect");
    layout->addWidget(connectBtn);

    auto disconnectBtn = new QPushButton("Disconnect");
    disconnectBtn->setEnabled(false);
    layout->addWidget(disconnectBtn);

    auto statusLabel = new QLabel("Disconnected");
    layout->addWidget(statusLabel);

    auto treeWidget = new QTreeWidget();
    treeWidget->setHeaderLabels({"Key", "Type", "Size"});
    layout->addWidget(treeWidget);

    auto keyLineEdit = new QLineEdit();
    auto valueLineEdit = new QLineEdit();
    auto addBtn = new QPushButton("Add");
    auto keyLabel = new QLabel();
    keyLabel->setText("key:");
    auto valueLabel = new QLabel();

    valueLabel->setText("value:");
    addBtn->setEnabled(false);

    auto deleteBtn = new QPushButton("Delete");
    deleteBtn->setEnabled(false);

    auto hLayout = new QHBoxLayout();
    hLayout->addWidget(keyLabel);
    hLayout->addWidget(keyLineEdit);
    hLayout->addWidget(valueLabel);
    hLayout->addWidget(valueLineEdit);
    hLayout->addWidget(addBtn);
    hLayout->addWidget(deleteBtn);
    layout->addLayout((hLayout));


    QObject::connect(connectBtn, &QPushButton::clicked, [statusLabel, connectBtn, disconnectBtn, treeWidget, addBtn, deleteBtn]() {
        statusLabel->setText("Connecting...");
        connectBtn->setEnabled(false);

        auto rContext = redisConnect("localhost", 6379);
        if (rContext == nullptr || rContext->err) {
            const char *errMsg = rContext == nullptr ? "Connection error" : rContext->errstr;
            qDebug() << errMsg;
            statusLabel->setText(errMsg);
        } else {
            qDebug() << "Connected";
            statusLabel->setText("Connected");
            connectBtn->setEnabled(false);
            disconnectBtn->setEnabled(true);
            addBtn->setEnabled(true);
            deleteBtn->setEnabled(true);

            redisReply *reply = (redisReply *)redisCommand(rContext, "keys *");

            if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY) {
                qDebug() << "Failed to get keys list";
            } else {
                for (int i = 0; i < reply->elements; ++i) {
                    auto keyName = reply->element[i]->str;
                    auto keyTypeReply = (redisReply *)redisCommand(rContext, "type %s", keyName);

                    if (keyTypeReply == nullptr || keyTypeReply->type != REDIS_REPLY_STATUS) {
                        qDebug() << "Failed to get key type for: " << keyName;
                        continue;
                    }

                    auto keyType = QString(keyTypeReply->str).toLower();
                    QTreeWidgetItem *item = new QTreeWidgetItem({keyName, keyType, ""});
                    if (keyType == "string") {
                        redisReply *keyValueReply = (redisReply *)redisCommand(rContext, "get %s", keyName);
                        if (keyValueReply != nullptr && keyValueReply->type == REDIS_REPLY_STRING) {
                            item->setText(2, QString::number(keyValueReply->len));
                        }
                        freeReplyObject(keyValueReply);
                    } else if (keyType == "list") {
                        redisReply *keyLengthReply = (redisReply *)redisCommand(rContext, "llen %s", keyName);
                        if (keyLengthReply != nullptr && keyLengthReply->type == REDIS_REPLY_INTEGER) {
                            item->setText(2, QString::number(keyLengthReply->integer));
                        }
                        freeReplyObject(keyLengthReply);
                    } else if (keyType == "set") {
                        redisReply *keyLengthReply = (redisReply *)redisCommand(rContext, "scard %s", keyName);
                        if (keyLengthReply != nullptr && keyLengthReply->type == REDIS_REPLY_INTEGER) {
                            item->setText(2, QString::number(keyLengthReply->integer));
                        }
                        freeReplyObject(keyLengthReply);
                    } else if (keyType == "zset") {
                        redisReply *keyLengthReply = (redisReply *)redisCommand(rContext, "zcard %s", keyName);
                        if (keyLengthReply != nullptr && keyLengthReply->type == REDIS_REPLY_INTEGER) {
                            item->setText(2, QString::number(keyLengthReply->integer));
                        }
                        freeReplyObject(keyLengthReply);
                    } else if (keyType == "hash") {
                        redisReply *keyLengthReply = (redisReply *)redisCommand(rContext, "hlen %s", keyName);
                        if (keyLengthReply != nullptr && keyLengthReply->type == REDIS_REPLY_INTEGER) {
                            item->setText(2, QString::number(keyLengthReply->integer));
                        }
                        freeReplyObject(keyLengthReply);
                    } else {
                        qDebug() << "Unknown key type: " << keyType;
                    }

                    treeWidget->addTopLevelItem(item);
                    freeReplyObject(keyTypeReply);
                }
            }

            freeReplyObject(reply);
            redisFree(rContext);
        }
    });

    QObject::connect(disconnectBtn, &QPushButton::clicked, [statusLabel, connectBtn, disconnectBtn, treeWidget, addBtn, keyLineEdit, valueLineEdit]() {
        statusLabel->setText("Disconnected");
        connectBtn->setEnabled(true);
        disconnectBtn->setEnabled(false);
        addBtn->setEnabled(false);
        treeWidget->clear();
        keyLineEdit->setText("");
        valueLineEdit->setText("");
    });

    QObject::connect(addBtn, &QPushButton::clicked, [statusLabel, treeWidget, keyLineEdit, valueLineEdit]() {
        auto keyName = keyLineEdit->text();
        auto keyValue = valueLineEdit->text();
        if (keyName.isEmpty() || keyValue.isEmpty()) {
            QMessageBox::warning(nullptr, "Warning", "Please input both Key and Value.");
            return;
        }

        auto rContext = redisConnect("localhost", 6379);
        if (rContext == nullptr || rContext->err) {
            const char *errMsg = rContext == nullptr ? "Connection error" : rContext->errstr;
            qDebug() << errMsg;
            statusLabel->setText(errMsg);
            redisFree(rContext);
            return;
        }

        auto reply = (redisReply *)redisCommand(rContext, "SET %s %s", keyName.toLatin1().constData(), keyValue.toLatin1().constData());
        if (reply == nullptr || reply->type != REDIS_REPLY_STATUS) {
            qDebug() << "Failed to set key-value pair.";
            redisFree(rContext);
            return;
        }

        qDebug() << "Key-Value pair added: " << keyName << "-" << keyValue;
        statusLabel->setText("Key-Value pair added.");
        QTreeWidgetItem *item = new QTreeWidgetItem({keyName, "string", QString::number(keyValue.size())});
        treeWidget->addTopLevelItem(item);

        freeReplyObject(reply);
        redisFree(rContext);

        keyLineEdit->setText("");
        valueLineEdit->setText("");
    });

    QObject::connect(deleteBtn, &QPushButton::clicked, [statusLabel, treeWidget, keyLineEdit]() {
        auto keyName = keyLineEdit->text();
        if (keyName.isEmpty()) {
            QMessageBox::warning(nullptr, "Warning", "Please input Key to delete.");
            return;
        }

        auto rContext = redisConnect("localhost", 6379);
        if (rContext == nullptr || rContext->err) {
            const char *errMsg = rContext == nullptr ? "Connection error" : rContext->errstr;
            qDebug() << errMsg;
            statusLabel->setText(errMsg);
            redisFree(rContext);
            return;
        }

        auto reply = (redisReply *)redisCommand(rContext, "DEL %s", keyName.toLatin1().constData());
        if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
            qDebug() << "Failed to delete key: " << keyName;
            redisFree(rContext);
            return;
        }

        qDebug() << "Key deleted: " << keyName;
        statusLabel->setText("Key deleted.");
        auto items = treeWidget->findItems(keyName, Qt::MatchExactly);
        for (auto item : items) {
            int index = treeWidget->indexOfTopLevelItem(item);
            treeWidget->takeTopLevelItem(index);
            delete item;
        }

        freeReplyObject(reply);
        redisFree(rContext);

        keyLineEdit->setText("");
    });



}

