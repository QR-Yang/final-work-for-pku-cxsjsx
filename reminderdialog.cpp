#include "reminderdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

ReminderDialog::ReminderDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("提醒事项管理");
    resize(500, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 创建提醒列表
    reminderList = new QListWidget(this);
    reminderList->setStyleSheet(
        "QListWidget {"
        "   background-color: white;"
        "   border: 1px solid #ced4da;"
        "   border-radius: 5px;"
        "}"
        "QListWidget::item {"
        "   padding: 10px;"
        "   border-bottom: 1px solid #e9ecef;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #e2e6ea;"
        "}"
        );
    mainLayout->addWidget(reminderList);

    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    addButton = new QPushButton("添加提醒", this);
    addButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #28a745;"
        "   color: white;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #218838;"
        "}"
        );

    deleteButton = new QPushButton("删除提醒", this);
    deleteButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #dc3545;"
        "   color: white;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #c82333;"
        "}"
        );

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    // 连接信号槽
    connect(addButton, &QPushButton::clicked, this, &ReminderDialog::onAddReminder);
    connect(deleteButton, &QPushButton::clicked, this, &ReminderDialog::onDeleteReminder);
}

void ReminderDialog::setReminders(const QStringList &reminders) {
    this->reminders = reminders;
    reminderList->clear();
    reminderList->addItems(reminders);
}


void ReminderDialog::onAddReminder() {
    bool ok;
    QString text = QInputDialog::getText(this, "添加提醒",
                                         "请输入提醒内容:",
                                         QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.trimmed().isEmpty()) {
        reminders.append(text);
        reminderList->addItem(text);
        emit remindersUpdated();  // 通知外部提醒已更新
    }
}

void ReminderDialog::onDeleteReminder() {
    QListWidgetItem *item = reminderList->currentItem();
    if (item) {
        int row = reminderList->row(item);
        reminders.removeAt(row);
        delete item;
        emit remindersUpdated();  // 通知外部提醒已更新
    }
}
