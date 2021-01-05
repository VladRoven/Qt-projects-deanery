#ifndef GROUPEDITOR_H
#define GROUPEDITOR_H

#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class GroupEditor;
}

class GroupEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit GroupEditor(QWidget *parent = nullptr);
    ~GroupEditor();

    void setListGroups(QStringList groups);
    bool checkGroup(QStringList groups, QString group);
    void setModel(QAbstractItemModel *model, bool isEdit, QString old_name);
    void setModel(QStandardItemModel *model, bool isEdit);
    QDataWidgetMapper *mapper;

signals:
    void updateListGroups();
    void changeGroup(QString old_name, QString new_name);
    void addCountStudentsNew();

private slots:
    void on_btn_accept_clicked();

    void on_btn_cancle_clicked();

private:
    Ui::GroupEditor *ui;
    bool isEdit;
    QString old_name;
    QStringList groups;
};

#endif // GROUPEDITOR_H
