#include "assignment.h"
#include <QApplication>
#include <QDebug>
#include <cmath>

Assignment::Assignment(){}

Assignment::Assignment(QString subject, QString name, QDate ddl, int importance, double timeCost)
    : m_subject(subject), m_name(name), m_ddl(ddl),
    m_importance(importance), m_timeCost(timeCost) {}

QString Assignment::getSubject() const { return m_subject; }
QString Assignment::getName() const { return m_name; }
QDate Assignment::getDdl() const { return m_ddl; }
int Assignment::getImportance() const { return m_importance; }
double Assignment::getTimeCost() const { return m_timeCost; }

void Assignment::setSubject(const QString &subject) { m_subject = subject; }
void Assignment::setName(const QString &name) { m_name = name; }
void Assignment::setDdl(const QDate &ddl) { m_ddl = ddl; }
void Assignment::setImportance(int importance) { m_importance = importance; }
void Assignment::setTimeCost(double timeCost) { m_timeCost = timeCost; }

int Assignment::getDaysLeft() const {
    return QDate::currentDate().daysTo(m_ddl);
}

double Assignment::getPriorityScore() const {
    // 优先级评分算法
    double daysLeft = std::max(1, getDaysLeft()); // 防止除以0
    double urgency = 1.0 / daysLeft;             // 剩余天数越少，紧急度越高
    double importance = m_importance / 5.0;      // 重要性归一化
    double timeFactor = 1.0 - (std::min(m_timeCost, 8.0) / 8.0); // 耗时越短，优先级越高

    // 加权计算综合优先级 (紧急度权重最高)
    return 0.6 * urgency + 0.3 * importance + 0.1 * timeFactor;
}

// 写入文件的字符串
QString Assignment::toFileString() const {
    return QString("%1|%2|%3|%4|%5")
    .arg(m_subject)
        .arg(m_name)
        .arg(m_ddl.toString("yyyy-MM-dd"))
        .arg(m_importance)
        .arg(m_timeCost);
}

// 字符串读取任务
Assignment Assignment::fromFileString(const QString &str) {
    QStringList parts = str.split("|");
    if (parts.size() != 5) return Assignment();

    Assignment assignment;
    assignment.setSubject(parts[0]);
    assignment.setName(parts[1]);
    assignment.setDdl(QDate::fromString(parts[2], "yyyy-MM-dd"));
    assignment.setImportance(parts[3].toInt());
    assignment.setTimeCost(parts[4].toDouble());

    return assignment;
}

bool Assignment::operator<(const Assignment &other) const {
    // 按ddl排序，紧急的在前
    return m_ddl < other.m_ddl;
}
