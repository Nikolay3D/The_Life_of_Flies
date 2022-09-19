#include "fly.h"
#include <QThread>
#include <QDebug>
#include <QCoreApplication>


int CFly::id() const
{
    return m_id;
}

int CFly::hp() const
{
    return m_hp;
}

CFly::TFlyType CFly::type() const
{
    return m_type;
}

int CFly::stupidity() const
{
    return m_stupidity;
}

CFly::TFlyType CFly::get_random_type()
{
    float k = (float)generator()/(generator.max() - generator.min());
    float type = static_cast<float>(FT_1) + static_cast<float>(FT_NUM-FT_1) * k;

    if(type < static_cast<float>(FT_1)){
        return FT_1;
    }

    if(type > static_cast<float>(FT_NUM-1)){
        return static_cast<TFlyType>(FT_NUM-1);
    }

    return static_cast<TFlyType>(static_cast<int>(type));
}

const std::vector<int> &CFly::track() const
{
    return m_track;
}

int CFly::hp_max() const
{
    return m_hp_max;
}

QString CFly::toString() const
{
    return QString("%1: 0x%2, id: %3, type: %4, hp_max: %5, hp: %6, stupidity: %7")
            .arg(this->metaObject()->className())
            .arg((quintptr)this, QT_POINTER_SIZE * 2, 16, QChar('0'))
            .arg(id())
            .arg(type())
            .arg(hp_max())
            .arg(hp())
            .arg(stupidity())
            ;
}

bool CFly::suspended() const
{
    return m_suspended;
}

CFly::TFlyState CFly::state() const
{
    return m_state;
}

void CFly::add_track_point(int cell_id)
{
    QMutexLocker locker(&m_track_mutex);

    m_track.push_back(cell_id);
}

void CFly::freeze()
{
    wait_suspend();

    float min_freeze = 1000;
    float k = (float)generator()/(generator.max() - generator.min());
    int ms = min_freeze + static_cast<float>(m_stupidity-min_freeze) * k;
    if(ms > m_hp){ ms = m_hp; }
    // ожидание может быть несколько секунд,
    // чтобы быстрее отреагировать на внешние сигналы, оно разбито на промежутки по 100 мс
    constexpr int WAIT_STEP_MS{100};
    while(ms){
        wait_suspend();
        int _ms = ms < WAIT_STEP_MS ? ms : WAIT_STEP_MS;
        QThread::msleep(_ms);
        QCoreApplication::processEvents(); // Принудительно заставляем работать EventLoop потока
        ms -= _ms;
        m_hp -= _ms; // вся жизнь мухи - тупить...
    }

}

void CFly::wait_suspend()
{
    if(m_state == FS_SUSPEND){
        qWarning() << Q_FUNC_INFO << "Incorrect call function. Fly is already suspend!";
    }

    TFlyState prev_sate = m_state;

    while(suspended()){
        m_state = FS_SUSPEND;
        QThread::msleep(10);    // без этой строчки загрузка CPU для каждого потока мухи становится 100%
        QCoreApplication::processEvents(); // Принудительно заставляем работать EventLoop потока
    }

    m_state = prev_sate;
}

CFly::CFly(int id, uint stupidity, int hp, QObject *parent)
    : QObject{parent}
{
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator.seed(seed);

    m_id = id;
    m_stupidity = stupidity;
    m_hp = hp*stupidity;
    m_hp_max = m_hp;
    m_type = get_random_type();

    qDebug() << Q_FUNC_INFO << m_id;
}

CFly::~CFly()
{
    qDebug() << Q_FUNC_INFO << m_id;
}

void CFly::execute()
{
    wait_suspend();

    while(m_hp > 0){
        freeze();
        emit jump(id());
    }

    // если не выполнять этого ожидания, то поток завершается и уничтожается объект мухи.
    // а к нему продолжают выполняться запросы
    m_state = FS_DEAD;
    while(!m_allow_destroy){
        //wait kill
        QThread::msleep(10);    // без этой строчки загрузка CPU для каждого потока мухи становится 100%
        QCoreApplication::processEvents(); // Принудительно заставляем работать EventLoop потока
    }

    emit finished();
}

void CFly::suspend()
{
    // т.к. работа через сигналы и eventloop , мьютекс не нужен
    m_suspended = true;
}

void CFly::resume()
{
    // т.к. работа через сигналы и eventloop , мьютекс не нужен
    m_suspended = false;
}

void CFly::stop()
{
    m_hp = 0;
    m_allow_destroy = true;
    resume();
}
