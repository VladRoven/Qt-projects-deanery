#include "groupeditor.h"
#include "ui_groupeditor.h"

#include <QMessageBox>

GroupEditor::GroupEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GroupEditor)
{
    ui->setupUi(this);
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
}

GroupEditor::~GroupEditor()
{
    delete ui;
}

void GroupEditor::setListGroups(QStringList groups)
{
    this->groups = groups;
}

bool GroupEditor::checkGroup(QStringList groups, QString group)
{
    for (QString val : groups)
    {
        if (val.contains(group))
            return true;
    }
    return false;
}

void GroupEditor::setModel(QAbstractItemModel *model, bool isEdit, QString old_name)
{
    ui->name->clear();
    this->old_name = old_name;
    this->isEdit = isEdit;
    mapper->clearMapping();
    mapper->setModel(model);
    mapper->addMapping(ui->name, 0);
}

void GroupEditor::setModel(QStandardItemModel *model, bool isEdit)
{
    ui->name->clear();
    this->isEdit = isEdit;
    mapper->clearMapping();
    mapper->setModel(model);
}

void GroupEditor::on_btn_accept_clicked()
{
    QString error = "";
    QRegExp client_reg("([А-Яа-яё]+-[0-9]{2}-[0-9]{1,2})");
    if (!client_reg.exactMatch(ui->name->text()))
        error += "Корректно введите название!\n";

    if (checkGroup(groups, ui->name->text()))
        error += "Данная группа существует!\n";

    if (error.length())
    {
        QMessageBox *msg = new QMessageBox();
        msg->setIcon(QMessageBox::Warning);
        msg->setWindowTitle("Ошибка");
        msg->setInformativeText(error);
        msg->addButton("Понял", QMessageBox::AcceptRole);
        msg->setAttribute(Qt::WA_QuitOnClose, false);
        msg->exec();
    }
    else
    {
        if (isEdit)
        {
            mapper->submit();
            emit updateListGroups();
            emit changeGroup(old_name, ui->name->text());
            close();
        }
        else
        {
            mapper->model()->insertRow(mapper->model()->rowCount());
            mapper->setCurrentModelIndex(mapper->model()->index(mapper->model()->rowCount() - 1, 0));

            mapper->addMapping(ui->name, 0);

            mapper->submit();
            emit updateListGroups();
            emit addCountStudentsNew();

            close();
        }
    }
}

void GroupEditor::on_btn_cancle_clicked()
{
    close();
}
