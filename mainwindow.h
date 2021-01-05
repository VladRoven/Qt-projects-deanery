#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileInfo>
#include <QMainWindow>
#include <QStandardItemModel>
#include "groupeditor.h"
#include "studenteditor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    QStringList getListGroups();

    void changeGroupInStudent(QString old_name, QString new_name);

    void appendCountStudentsNew();

    void changeCountStudentsOfGroup(QString old_group, QString new_group);

    void addCountStudentsOfGroup(QString group);

    void on_open_file_triggered();

    void setGroupList();

    void on_list_group_currentTextChanged(const QString &arg1);

    void on_search_student_textChanged(const QString &arg1);

    void on_search_group_textChanged(const QString &arg1);

    void on_new_file_triggered();

    void on_save_triggered();

    void on_save_as_triggered();

    void on_to_excel_by_group_triggered();

    void on_to_excel_all_group_triggered();

    void on_to_excel_all_student_triggered();

    void on_table_group_doubleClicked(const QModelIndex &index);

    void on_add_group_clicked();

    void on_del_group_clicked();

    void on_table_student_doubleClicked(const QModelIndex &index);

    void on_add_student_clicked();

    void on_del_student_clicked();

private:
    Ui::MainWindow *ui;
    QFileInfo file_info;
    QStandardItemModel *model_groups = new QStandardItemModel;
    QStandardItemModel *model_students = new QStandardItemModel;
    StudentEditor *student_editor;
    GroupEditor *group_editor;
};
#endif // MAINWINDOW_H
