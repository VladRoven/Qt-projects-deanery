#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "xlsxdocument.h"
#include "xlsxformat.h"

#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMap>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QDateTime>
#include <QAbstractItemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    student_editor = new StudentEditor(this);
    group_editor = new GroupEditor(this);

    connect(group_editor, &GroupEditor::updateListGroups, this, &MainWindow::setGroupList);
    connect(group_editor, &GroupEditor::changeGroup, this, &MainWindow::changeGroupInStudent);
    connect(group_editor, &GroupEditor::addCountStudentsNew, this, &MainWindow::appendCountStudentsNew);

    connect(student_editor, &StudentEditor::updateCountStudents, this, &MainWindow::changeCountStudentsOfGroup);
    connect(student_editor, &StudentEditor::addCountStudents, this, &MainWindow::addCountStudentsOfGroup);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QStringList MainWindow::getListGroups()
{
    QStringList list;
    for (int i = 0; i < model_groups->rowCount(); i++)
    {
        list.append(model_groups->item(i, 0)->text());
    }

    return list;
}

void MainWindow::changeGroupInStudent(QString old_name, QString new_name)
{
    for (int i = 0; i < model_students->rowCount(); i++)
    {
        if (model_students->item(i, 5)->text().contains(old_name))
        {
            model_students->setItem(i, 5, new QStandardItem(new_name));
        }
    }
}

void MainWindow::appendCountStudentsNew()
{
    model_groups->setData(model_groups->index(model_groups->rowCount() - 1, 1), "0");
}

void MainWindow::changeCountStudentsOfGroup(QString old_group, QString new_group)
{
    for (int i = 0; i < model_groups->rowCount(); i++)
    {
        if (model_groups->item(i, 0)->text().contains(new_group))
        {
            int count_students = model_groups->item(i, 1)->text().toInt();
            model_groups->setItem(i, 1, new QStandardItem(QString::number(++count_students)));
            ui->table_group->setModel(model_groups);
            break;
        }
    }

    for (int i = 0; i < model_groups->rowCount(); i++)
    {
        if (model_groups->item(i, 0)->text().contains(old_group))
        {
            int count_students = model_groups->item(i, 1)->text().toInt();
            model_groups->setItem(i, 1, new QStandardItem(QString::number(--count_students)));
            ui->table_group->setModel(model_groups);
            break;
        }
    }
}

void MainWindow::addCountStudentsOfGroup(QString group)
{
    for (int i = 0; i < model_groups->rowCount(); i++)
    {
        if (model_groups->item(i, 0)->text().contains(group))
        {
            int count_students = model_groups->item(i, 1)->text().toInt();
            model_groups->setItem(i, 1, new QStandardItem(QString::number(++count_students)));
            ui->table_group->setModel(model_groups);
            break;
        }
    }
}


void MainWindow::on_open_file_triggered()
{
    QString openFileName = QFileDialog::getOpenFileName(this, tr("Открыть файл"), QString(), tr("JSON (*.json)"));
    QFile jsonFile(openFileName);

    if (!jsonFile.open(QIODevice::ReadOnly))
    {
        return;
    }
    file_info = openFileName;

    model_groups->clear();
    model_students->clear();
    QByteArray saveData = jsonFile.readAll();

    QJsonDocument json_doc = QJsonDocument::fromJson(saveData);

    QStringList horizontalHeader;
    horizontalHeader.append({"Название", "Кол-во студентов"});
    model_groups->setHorizontalHeaderLabels(horizontalHeader);

    horizontalHeader.clear();
    horizontalHeader.append({"Фамилия", "Имя", "Отчество", "Дата рождения", "Форма обучения", "Группа"});
    model_students->setHorizontalHeaderLabels(horizontalHeader);

    ui->table_group->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_student->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QJsonArray json_goups = json_doc.object()["groups"].toArray();
    QJsonArray json_students = json_doc.object()["students"].toArray();

    QJsonArray temp_array;
    QList<QStandardItem *> temp_list;

    for (int i = 0; i < json_goups.count(); i++)
    {
        temp_list.clear();
        temp_array = json_goups[i].toArray();
        for (int j = 0; j < temp_array.count(); j++)
        {
             temp_list.append(new QStandardItem(temp_array[j].toString()));
        }
        model_groups->insertRow(i, temp_list);
    }

    setGroupList();

    for (int i = 0; i < json_students.count(); i++)
    {
        temp_list.clear();
        temp_array = json_students[i].toArray();
        for (int j = 0; j < temp_array.count(); j++)
        {
           temp_list.append(new QStandardItem(temp_array[j].toString()));
        }
        model_students->insertRow(i, temp_list);

        QString format = "yyyy-MM-dd";

        QString strValue = model_students->data(model_students->index(i, 3)).toString();
        QDate date = QDate :: fromString (strValue, format);
        model_students->setData(model_students->index(i, 3), date);
    }

    ui->table_group->setModel(model_groups);
    ui->table_student->setModel(model_students);
    ui->status_line->showMessage("Открыт файл: " + file_info.baseName() + " (" + file_info.absoluteFilePath() + ")");
}

void MainWindow::setGroupList()
{
    if (file_info.absoluteFilePath() != "")
    {
        QStringList list;

        list.append("Всех");
        for (int i = 0; i < model_groups->rowCount(); i++)
        {
            list.append(model_groups->item(i, 0)->text());
        }

        ui->list_group->clear();
        ui->list_group->addItems(list);
    }
}

void MainWindow::on_list_group_currentTextChanged(const QString &arg1)
{
    if (file_info.absoluteFilePath() != "")
    {
        if (arg1 != "Всех")
        {
            QSortFilterProxyModel *proxy_model = new QSortFilterProxyModel();
            proxy_model->setSourceModel(model_students);
            ui->table_student->setModel(proxy_model);
            proxy_model->setFilterKeyColumn(5);
            proxy_model->setFilterRegExp(arg1);
            ui->status_line->showMessage("Найдено записей: " + QString::number(proxy_model->rowCount()));
        }
        else
        {
            ui->table_student->setModel(model_students);
            ui->status_line->showMessage("Всего записей: " + QString::number(model_students->rowCount()));
        }
    }
}

void MainWindow::on_search_student_textChanged(const QString &arg1)
{
    if (file_info.absoluteFilePath() != "")
    {
        if (arg1.length())
        {
            QString str = arg1;
            str[ 0 ] = str[ 0 ].toUpper();
            for (int i = 1; i < str.length(); i++)
            {
                str[i] = str[i].toLower();
            }
            QSortFilterProxyModel *proxy_model = new QSortFilterProxyModel();
            proxy_model->setSourceModel(model_students);
            ui->table_student->setModel(proxy_model);
            proxy_model->setFilterKeyColumn(0);
            proxy_model->setFilterRegExp(str);
            ui->status_line->showMessage("Найдено записей: " + QString::number(proxy_model->rowCount()));
        }
        else
        {
            ui->table_student->setModel(model_students);
            ui->status_line->showMessage("Всего записей: " + QString::number(model_students->rowCount()));
        }
    }
}

void MainWindow::on_search_group_textChanged(const QString &arg1)
{
    if (file_info.absoluteFilePath() != "")
    {
        if (arg1.length())
        {
            QSortFilterProxyModel *proxy_model = new QSortFilterProxyModel();
            proxy_model->setSourceModel(model_groups);
            ui->table_group->setModel(proxy_model);
            proxy_model->setFilterKeyColumn(0);
            proxy_model->setFilterRegExp(arg1);
            ui->status_line->showMessage("Найдено записей: " + QString::number(proxy_model->rowCount()));
        }
        else
        {
            ui->table_group->setModel(model_groups);
            ui->status_line->showMessage("Всего записей: " + QString::number(model_groups->rowCount()));
        }
    }
}

void MainWindow::on_new_file_triggered()
{
    QString saveFileName = QFileDialog::getSaveFileName(this,
                                                            tr("Новый файл"),
                                                            QString(),
                                                            tr("JSON (*.json)"));
    QFileInfo fileInfo(saveFileName);
    QDir::setCurrent(fileInfo.path());
    QFile json_file(saveFileName);

    if (!json_file.open(QIODevice::WriteOnly))
    {
        return;
    }
    file_info = saveFileName;
    json_file.close();

    model_groups->clear();
    model_students->clear();

    QStringList horizontalHeader;
    horizontalHeader.append({"Название", "Кол-во студентов"});
    model_groups->setHorizontalHeaderLabels(horizontalHeader);

    horizontalHeader.clear();
    horizontalHeader.append({"Фамилия", "Имя", "Отчество", "Дата рождения", "Форма обучения", "Группа"});
    model_students->setHorizontalHeaderLabels(horizontalHeader);

    ui->table_group->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_student->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->table_group->setModel(model_groups);
    ui->table_student->setModel(model_students);
    ui->status_line->showMessage("Открыт файл: " + fileInfo.baseName() + " (" + fileInfo.absoluteFilePath() + ")");
}

void MainWindow::on_save_triggered()
{
    if (file_info.absoluteFilePath() != "")
    {
        QFile json_file(file_info.absoluteFilePath());
        QDir::setCurrent(file_info.path());

        if (!json_file.open(QIODevice::WriteOnly))
        {
            return;
        }

        QJsonObject json;
        QJsonArray data;
        for (int i = 0; i < model_groups->rowCount(); i++)
        {
            QJsonArray row;

            for (int j = 0; j < model_groups->columnCount(); j++)
            {
                row.append(QJsonValue(model_groups->item(i, j)->text()));
            }
            data.append(row);
        }

        json["groups"] = data;
        data = {};

        for (int i = 0; i < model_students->rowCount(); i++)
        {
            QJsonArray row;

            for (int j = 0; j < model_students->columnCount(); j++)
            {
                row.append(QJsonValue(model_students->item(i, j)->text()));
            }
            data.append(row);
        }
        json["students"] = data;
        data = {};

        QJsonDocument saveDoc(json);
        json_file.write(saveDoc.toJson());
        json_file.close();
        ui->status_line->showMessage("Файл \"" + file_info.baseName() + "\" сохранён! Путь: " + file_info.absoluteFilePath());
    }
    else
        ui->status_line->showMessage("Файл не открыт!");
}

void MainWindow::on_save_as_triggered()
{
    if (file_info.absoluteFilePath() != "")
    {
        QString saveFileName = QFileDialog::getSaveFileName(this,
                                                                tr("Сохранить файл как"),
                                                                QString(),
                                                                tr("JSON (*.json)"));
        QFileInfo fileInfo(saveFileName);
        QDir::setCurrent(fileInfo.path());
        QFile json_file(saveFileName);

        if (!json_file.open(QIODevice::WriteOnly))
        {
            return;
        }

        QJsonObject json;
        QJsonArray data;

        for (int i = 0; i < model_groups->rowCount(); i++)
        {
            QJsonArray row;

            for (int j = 0; j < model_groups->columnCount(); j++)
            {
                row.append(QJsonValue(model_groups->item(i, j)->text()));
            }
            data.append(row);
        }

        json["groups"] = data;
        data = {};

        for (int i = 0; i < model_students->rowCount(); i++)
        {
            QJsonArray row;

            for (int j = 0; j < model_students->columnCount(); j++)
            {
                row.append(QJsonValue(model_students->item(i, j)->text()));
            }
            data.append(row);
        }
        json["students"] = data;
        data = {};

        QJsonDocument saveDoc(json);
        json_file.write(saveDoc.toJson());
        json_file.close();
        ui->status_line->showMessage("Файл \"" + fileInfo.baseName() + "\" сохранён! Путь: " + fileInfo.absoluteFilePath());
    }
    else
        ui->status_line->showMessage("Файл не открыт!");
}

void MainWindow::on_to_excel_by_group_triggered()
{
    if (file_info.absoluteFilePath() != "")
    {
        QString group = ui->table_group->model()->index(ui->table_group->currentIndex().row(), 0).data().toString();

        if (ui->table_group->currentIndex().isValid())
        {
            QXlsx::Document excel;
            QXlsx::Format excel_format;
            excel_format.setFontBold(true);
            excel.setColumnWidth(1, 6, 23);
            excel.setRowFormat(1, excel_format);
            excel.setRowFormat(4, excel_format);
            excel.write("A1", "Название");
            excel.write("B1", "Кол-во студентов");

            excel.write("A2", ui->table_group->model()->index(ui->table_group->currentIndex().row(), 0).data().toString());
            excel.write("B2", ui->table_group->model()->index(ui->table_group->currentIndex().row(), 1).data());

            excel.write("A4", "Фамилия");
            excel.write("B4", "Имя");
            excel.write("C4", "Отчество");
            excel.write("D4", "Дата рождения");
            excel.write("E4", "Форма обучения");

            int itr = 5;
            QString date;
            QString format = "yyyy-MM-dd";
            QDate d_date;
            for (int i = 0; i < model_students->rowCount(); i++)
            {
                if (model_students->item(i, 5)->text().contains(group))
                {
                    date = model_students->item(i, 3)->text();
                    d_date = QDate :: fromString (date, format);
                    excel.write("A" + QString::number(itr), model_students->item(i, 0)->text());
                    excel.write("B" + QString::number(itr), model_students->item(i, 1)->text());
                    excel.write("C" + QString::number(itr), model_students->item(i, 2)->text());
                    excel.write("D" + QString::number(itr), d_date.toString("dd.MM.yyyy"));
                    excel.write("E" + QString::number(itr), model_students->item(i, 4)->text());
                    ++itr;
                }
            }

            QString saveFileName = QFileDialog::getSaveFileName(this,
                                                                    tr("Новый файл"),
                                                                    QString(group),
                                                                    tr("Excel (*.xlsx)"));
            excel.saveAs(saveFileName);
        }
        else
            ui->status_line->showMessage("Выберите группу!");
    }
}

void MainWindow::on_to_excel_all_group_triggered()
{

    if (file_info.absoluteFilePath() != "")
    {
        QXlsx::Document excel;
        QXlsx::Format excel_format;
        excel_format.setFontBold(true);
        excel.setColumnWidth(1, 2, 23);
        excel.setRowFormat(1, excel_format);
        excel.write("A1", "Название");
        excel.write("B1", "Кол-во студентов");

        int itr = 2;
        for (int i = 0; i < model_groups->rowCount(); i++)
        {
            excel.write("A" + QString::number(itr), model_groups->item(i, 0)->text());
            excel.write("B" + QString::number(itr), model_groups->item(i, 1)->text().toInt());
            ++itr;
        }

        QString saveFileName = QFileDialog::getSaveFileName(this,
                                                                tr("Новый файл"),
                                                                QString("Все группы"),
                                                                tr("Excel (*.xlsx)"));
        excel.saveAs(saveFileName);
    }
}

void MainWindow::on_to_excel_all_student_triggered()
{
    if (file_info.absoluteFilePath() != "")
    {
        QXlsx::Document excel;
        QXlsx::Format excel_format;
        excel_format.setFontBold(true);
        excel.setColumnWidth(1, 6, 23);
        excel.setRowFormat(1, excel_format);
        excel.write("A1", "Фамилия");
        excel.write("B1", "Имя");
        excel.write("C1", "Отчество");
        excel.write("D1", "Дата рождения");
        excel.write("E1", "Форма обучения");
        excel.write("F1", "Группа");

        int itr = 2;
        for (int i = 0; i < model_students->rowCount(); i++)
        {
            excel.write("A" + QString::number(itr), model_students->item(i, 0)->text());
            excel.write("B" + QString::number(itr), model_students->item(i, 1)->text());
            excel.write("C" + QString::number(itr), model_students->item(i, 2)->text());
            excel.write("D" + QString::number(itr), QDate::fromString(model_students->item(i, 3)->text(), "yyyy-MM-dd").toString("dd.MM.yyyy"));
            excel.write("E" + QString::number(itr), model_students->item(i, 4)->text());
            excel.write("F" + QString::number(itr), model_students->item(i, 5)->text());
            ++itr;
        }

        QString saveFileName = QFileDialog::getSaveFileName(this,
                                                                tr("Новый файл"),
                                                                QString("Все студенты"),
                                                                tr("Excel (*.xlsx)"));
        excel.saveAs(saveFileName);
    }
}

void MainWindow::on_table_group_doubleClicked(const QModelIndex &index)
{
    if (file_info.absoluteFilePath() != "")
    {
        group_editor->setModel(ui->table_group->model(), true, ui->table_group->model()->index(index.row(), 0).data().toString());
        group_editor->setListGroups(getListGroups());
        group_editor->mapper->setCurrentModelIndex(index);
        group_editor->setWindowModality(Qt::ApplicationModal);
        group_editor->show();
    }
}

void MainWindow::on_add_group_clicked()
{
    if (file_info.absoluteFilePath() != "")
    {
        group_editor->setModel(model_groups, false);
        group_editor->setListGroups(getListGroups());
        group_editor->setWindowModality(Qt::ApplicationModal);
        group_editor->show();
    }
}

void MainWindow::on_del_group_clicked()
{
    if (file_info.absoluteFilePath() != "")
    {
        QModelIndex index = ui->table_group->currentIndex();
        auto model = ui->table_group->model();

        if (index.row() >= 0)
        {
            QMessageBox *msg = new QMessageBox();
            msg->setIcon(QMessageBox::Information);
            msg->setWindowTitle("Подтверждение удаления");
            msg->setText("Вы действительно хотите удалить выбранную группу?");
            msg->setInformativeText("При удалении группы будет удалена иформация о студентах, закреплённые за ней!");
            QPushButton *btn_ok = msg->addButton("Да", QMessageBox::AcceptRole);
            msg->addButton("Отмена", QMessageBox::RejectRole);
            msg->exec();

            if (msg->clickedButton() == btn_ok)
            {
                for (int i = model_students->rowCount() - 1; i >= 0; i--)
                {
                    if (model_students->item(i, 5)->text().contains(model->data(model->index(index.row(), 0)).toString()))
                    {
                        model_students->removeRow(i);
                    }
                }

                model->removeRow(index.row());
                ui->table_group->setModel(model_groups);
                ui->table_student->setModel(model_students);
                setGroupList();
            }
        }
        else
        {
            ui->status_line->showMessage("Выберите группу!");
        }
    }
}

void MainWindow::on_table_student_doubleClicked(const QModelIndex &index)
{
    if (file_info.absoluteFilePath() != "")
    {
        student_editor->setModel(ui->table_student->model(), true, ui->table_student->model()->index(index.row(), 5).data().toString());
        student_editor->setGroupsList(getListGroups());
        student_editor->mapper->setCurrentModelIndex(index);
        student_editor->setWindowModality(Qt::ApplicationModal);
        student_editor->show();
    }
}

void MainWindow::on_add_student_clicked()
{
    if (file_info.absoluteFilePath() != "")
    {
        student_editor->setModel(model_students, false);
        student_editor->setGroupsList(getListGroups());
        student_editor->setWindowModality(Qt::ApplicationModal);
        student_editor->show();
    }
}

void MainWindow::on_del_student_clicked()
{
    if (file_info.absoluteFilePath() != "")
    {
        QModelIndex index = ui->table_student->currentIndex();
        auto model = ui->table_student->model();

        if (index.row() >= 0)
        {
            QMessageBox *msg = new QMessageBox();
            msg->setIcon(QMessageBox::Information);
            msg->setWindowTitle("Подтверждение удаления");
            msg->setText("Вы действительно хотите удалить выбранного студента?");
            QPushButton *btn_ok = msg->addButton("Да", QMessageBox::AcceptRole);
            msg->addButton("Отмена", QMessageBox::RejectRole);
            msg->exec();

            if (msg->clickedButton() == btn_ok)
            {
                for (int i = 0; i < model_groups->rowCount(); i++)
                {
                    if (model_groups->item(i, 0)->text().contains(model->index(index.row(), 5).data().toString()))
                    {
                        int count_students = model_groups->item(i, 1)->text().toInt();
                        model_groups->setItem(i, 1, new QStandardItem(QString::number(--count_students)));
                        ui->table_group->setModel(model_groups);
                        break;
                    }
                }
                model->removeRow(index.row());
                ui->table_student->setModel(model_students);
            }
        }
        else
        {
            ui->status_line->showMessage("Выберите студента!");
        }
    }
}
