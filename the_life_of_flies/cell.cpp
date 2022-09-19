#include "cell.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

bool CCell::append_fly(CFly * fly)
{
    if(m_flies.size() < m_fly_capacity){
        m_flies.push_back(fly);
        fly->add_track_point(this->id());
        return true;
    }

    return false;
}

void CCell::remove_fly(CFly *fly)
{
    auto it = std::find(m_flies.begin(), m_flies.end(), fly);
    if(it != m_flies.end()){
        m_flies.erase(it);
    }else{
        qDebug() << Q_FUNC_INFO << "fly" << fly << "not found in" << this;
    }
}

void CCell::move_fly(int fly_id, CCell *from, CCell *to)
{
    CFly* fly = from->find(fly_id);
    if(fly){
        from->remove_fly(fly);
        to->append_fly(fly);    // add_track_point вызывается внутри нее, этого достаточно

    }
}

size_t CCell::get_free_space()
{
    return m_fly_capacity - m_flies.size();
}

int CCell::id() const
{
    return m_id;
}

size_t CCell::fly_capacity() const
{
    return m_fly_capacity;
}

size_t CCell::get_num_flies() const
{
    return m_flies.size();
}

QString CCell::get_flies_jsonString() const
{
    QJsonObject root;
    root["cell_id"] = id();
    root["fullness"] = (int)get_num_flies();
    root["capacity"] = (int)fly_capacity();

    for(size_t i = 0; i < m_flies.size(); ++i){
        QString fly_key = QString("fly_%1").arg(i);
        QJsonObject fly;
        fly["uniq_id"] = (int)m_flies.at(i)->id();
        fly["hp"] = (int)m_flies.at(i)->hp();
        fly["hp_max"] = (int)m_flies.at(i)->hp_max();
        fly["type"] = (int)m_flies.at(i)->type();
        fly["stupidity"] = (int)m_flies.at(i)->stupidity();

        if(m_flies.at(i)->type() < 0){
            qWarning() << Q_FUNC_INFO << "error type! cell:" << id();
        }

        root[fly_key] = fly;
    }

    QString out = QJsonDocument(root).toJson(QJsonDocument::Compact);
    return out;
}

CCell::CCell(uint id, size_t fly_capacity, QObject *parent)
    : QObject{parent}
{
    m_id = id;
    m_fly_capacity = fly_capacity;
}

CCell::~CCell()
{
    for (size_t i = 0; i < m_flies.size(); ++i) {
        m_flies.at(i)->stop();
    }
}

CFly *CCell::find(int fly_id)
{
    for (size_t i = 0; i < m_flies.size(); ++i) {
        if(m_flies.at(i)->id() == fly_id){
            return m_flies.at(i);
        }
    }
    return nullptr;
}
