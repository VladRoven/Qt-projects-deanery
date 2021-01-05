#ifndef STUDENTEDITOR_H
#define STUDENTEDITOR_H

#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class StudentEditor;
}

class StudentEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit StudentEditor(QWidget *parent = nullptr);
    ~StudentEditor();

    void setGroupsList(QStringList groups);

    void setModel(QAbstractItemModel *model, bool isEdit, QString old_group);
    void setModel(QStandardItemModel *model, bool isEdit);
    QDataWidgetMapper *mapper;

signals:
    void updateCountStudents(QString old_group, QString new_group);
    void addCountStudents(QString group);

private slots:
    void on_btn_accept_clicked();

    void on_btn_cancle_clicked();

    void clear();

private:
    Ui::StudentEditor *ui;
    bool isEdit;
    QString old_group;
};

#endif // STUDENTEDITOR_H
