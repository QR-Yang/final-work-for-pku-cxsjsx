#ifndef REMINDERDIALOG_H
#define REMINDERDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QStringList>

class ReminderDialog : public QDialog {
    Q_OBJECT

public:
    explicit ReminderDialog(QWidget *parent = nullptr);
    QStringList getReminders() const { return reminders; }
    void setReminders(const QStringList &reminders);

signals:
    void remindersUpdated();  // 添加更新信号

private slots:
    void onAddReminder();
    void onDeleteReminder();

private:
    QListWidget *reminderList;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QStringList reminders;
};

#endif // REMINDERDIALOG_H
