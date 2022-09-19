#ifndef CFLYCONTROLLER_H
#define CFLYCONTROLLER_H

#include <QObject>
#include "fly.h"
#include "cell.h"
#include <QThread>

class CFlyController : public QObject
{
    Q_OBJECT

    size_t m_dimention = 0;
    std::vector<CCell*> m_cells;

    std::mt19937 generator;

    size_t id_for_create = 0;
    bool m_flies_started = false;

    CCell* get_random_free_cell();
    CCell *get_neighboring_free_cell(CCell *cell);

public:
    static constexpr size_t MAX_FLY_CAPACITY{3*3};
    static constexpr size_t MIN_FLY_CAPACITY{2};

    static constexpr size_t MAX_DIMENTION{7};
    static constexpr size_t MIN_DIMENTION{2};

    // чтобы было поинтереснее и в конце мухи дохли не одновременно
    static constexpr uint STUPIDITY_MIN{4000};
    static constexpr uint STUPIDITY_MAX{8000};

    CFlyController(QObject *parent = nullptr);
    CFlyController(size_t dimention, size_t fly_capacity, QObject *parent = nullptr);
    ~CFlyController();

    void fly_create();
    void fly_create(int cell_id);
    void fly_create(int cell_id, uint stupidity);
    void fly_remove(int fly_id);
    QVariantList fly_track(int fly_id);
    QString fly_info(int fly_id);

    size_t dimention() const;
    CCell *get_cell(size_t row, size_t column) const;


    void start_flies();
    void pause_flies();

public slots:
    void on_fly_jump(int fly_id);

signals:
    void cell_modified(int cell_id);
    void flies_suspend();
    void flies_resume();
};

#endif // CFLYCONTROLLER_H
