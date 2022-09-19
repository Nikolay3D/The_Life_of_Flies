#ifndef CFLY_H
#define CFLY_H

#include <QObject>
#include <random>
#include <QMutexLocker>

class CFly : public QObject
{
    Q_OBJECT
public:
    typedef enum {
        FT_1 = 1,
        FT_2,
        FT_3,
        FT_4,
        FT_5,
        FT_6,
        FT_7,
        FT_NUM
    }TFlyType;

    typedef enum {
        FS_LIVE = 1,
        FS_SUSPEND,
        FS_DEAD,
    }TFlyState;

private:
    int m_id = -1;
    uint m_stupidity = 0;
    int m_hp = 0;   // health point, уменьшаются в процессе
    int m_hp_max = 0;
    TFlyType m_type = FT_1;
    TFlyType get_random_type();
    TFlyState m_state = FS_LIVE;

    bool m_allow_destroy = false;

    bool m_suspended = true;

    QMutex m_track_mutex;
    std::vector<int> m_track;

    std::mt19937 generator;

    void freeze();
    void wait_suspend();

public:


    explicit CFly(int id, uint stupidity, int hp, QObject *parent = nullptr);
    ~CFly();

    int id() const;
    int hp() const;

    TFlyType type() const;

    int hp_max() const;

    QString toString() const;

    bool suspended() const;

    TFlyState state() const;

    void add_track_point(int cell_id);

    int stupidity() const;

    const std::vector<int> &track() const;

public slots:
    void execute();
    void suspend();
    void resume();
    void stop();

signals:
    void finished();
    void jump(int fly_id);

};

#endif // CFLY_H
