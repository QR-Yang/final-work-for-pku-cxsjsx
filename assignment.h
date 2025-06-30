#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <QString>
#include <QDate>
#include <QMetaType>

class Assignment {
public:
    Assignment();
    Assignment(QString subject, QString name, QDate ddl, int importance, double timeCost);

    // 获取属性
    QString getSubject() const;
    QString getName() const;
    QDate getDdl() const;
    int getImportance() const;
    double getTimeCost() const;
    int getDaysLeft() const;
    double getPriorityScore() const;

    // 设置属性
    void setSubject(const QString &subject);
    void setName(const QString &name);
    void setDdl(const QDate &ddl);
    void setImportance(int importance);
    void setTimeCost(double timeCost);

    // 文件操作
    QString toFileString() const;
    static Assignment fromFileString(const QString &str);

    // 比较运算符
    bool operator<(const Assignment &other) const;

private:
    QString m_subject;
    QString m_name;
    QDate m_ddl;
    int m_importance; // 1-5
    double m_timeCost;   // 小时
};

Q_DECLARE_METATYPE(Assignment)

#endif // ASSIGNMENT_H
