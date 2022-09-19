#ifndef CCELL_H
#define CCELL_H

#include <QObject>
#include "fly.h"

class CCell : public QObject
{
    Q_OBJECT
    size_t m_fly_capacity;
    int m_id;

    std::vector<CFly*> m_flies;
public:
    explicit CCell(uint id, size_t fly_capacity, QObject *parent = nullptr);
    ~CCell();

    CFly* find(int fly_id);
    bool append_fly(CFly *fly);
    void remove_fly(CFly *fly);
    static void move_fly(int fly_id, CCell* from, CCell* to);
    size_t get_free_space();

    int id() const;

    size_t fly_capacity() const;
    size_t get_num_flies() const;
    QString get_flies_jsonString() const;

signals:

};

#endif // CCELL_H
