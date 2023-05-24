#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTreeWidget>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_renameBtn_clicked();

    void on_deleteBtn_clicked();

    void on_reloadBtn_clicked();

    void on_pushButton_clicked();

    void on_comboBox_currentIndexChanged(const QString& arg1);

    void on_actionConnect_triggered();

    void on_actionDisconnect_triggered();

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem* item, int column);

private:
    Ui::MainWindow* ui;

//    void createForm();

    void clearKvTab();


};
#endif // MAINWINDOW_H
