#include "studenteditor.h"
#include "ui_studenteditor.h"

#include <QMessageBox>

StudentEditor::StudentEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StudentEditor)
{
    ui->setupUi(this);
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
}

StudentEditor::~StudentEditor()
{
    delete ui;
}

void StudentEditor::setGroupsList(QStringList groups)
{
    groups.push_front("Не выбрано");
    ui->group->addItems(groups);
}

void StudentEditor::setModel(QAbstractItemModel *model, bool isEdit, QString old_group)
{
    clear();
    this->old_group = old_group;
    this->isEdit = isEdit;
    mapper->clearMapping();
    mapper->setModel(model);
    mapper->addMapping(ui->surname, 0);
    mapper->addMapping(ui->name, 1);
    mapper->addMapping(ui->patronymic, 2);
    mapper->addMapping(ui->date, 3);
    mapper->addMapping(ui->type, 4);
    mapper->addMapping(ui->group, 5);
}

void StudentEditor::setModel(QStandardItemModel *model, bool isEdit)
{
    clear();
    this->isEdit = isEdit;
    mapper->clearMapping();
    mapper->setModel(model);
}

void StudentEditor::on_btn_accept_clicked()
{
    QString error = "";
    QRegExp client_reg("([А-Я][а-яё]+)");
    if (!client_reg.exactMatch(ui->surname->text()))
        error += "Корректно введите фамилию!\n";

    if (!client_reg.exactMatch(ui->name->text()))
        error += "Корректно введите имя!\n";

    if (!client_reg.exactMatch(ui->patronymic->text()))
        error += "Корректно введите отчество!\n";

    if (ui->type->currentText().contains("Не выбрано"))
        error += "Выберите форму обучения!\n";

    if (ui->group->currentText().contains("Не выбрано"))
        error += "Выберите группу!\n";

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
            emit updateCountStudents(old_group, ui->group->currentText());
            close();
        }
        else
        {
            mapper->model()->insertRow(mapper->model()->rowCount());
            mapper->setCurrentModelIndex(mapper->model()->index(mapper->model()->rowCount() - 1, 0));

            mapper->addMapping(ui->surname, 0);
            mapper->addMapping(ui->name, 1);
            mapper->addMapping(ui->patronymic, 2);
            mapper->addMapping(ui->date, 3);
            mapper->addMapping(ui->type, 4);
            mapper->addMapping(ui->group, 5);

            mapper->submit();
            emit addCountStudents(ui->group->currentText());

            close();
        }
    }
}

void StudentEditor::on_btn_cancle_clicked()
{
    close();
}

void StudentEditor::clear()
{
    QString str_date = "2000-01-01";
    QString format = "yyyy-MM-dd";
    QDate date = QDate :: fromString (str_date, format);

    ui->surname->clear();
    ui->name->clear();
    ui->patronymic->clear();
    ui->date->setDate(date);
    ui->type->setCurrentText("Не выбрано");
    ui->group->clear();
    mapper->clearMapping();
}
