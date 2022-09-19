#include "flycontroller.h"
#include <QVariant>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

size_t CFlyController::dimention() const
{
    return m_dimention;
}

CCell *CFlyController::get_cell(size_t row, size_t column) const
{
    size_t id = m_dimention*row + column;
    if(id < m_cells.size()){
        return m_cells.at(id);
    }

    qWarning() << Q_FUNC_INFO << "Incorrect row col:" << row << column;
    return nullptr;
}

void CFlyController::start_flies()
{
    emit flies_resume();
    m_flies_started = true;
}

void CFlyController::pause_flies()
{
    emit flies_suspend();
    m_flies_started = false;
}

void CFlyController::on_fly_jump(int fly_id)
{
    // все сигналы от мух попадают в eventloop этого потока.
    // значит и обрабатываться они будут последовательно
    // и мьютексы в CCell можно не использовать
    for(auto it = m_cells.begin(); it != m_cells.end(); ++it){
       if(nullptr != (*it)->find(fly_id)){
           CCell* from = *it;
           CCell* to = get_neighboring_free_cell(from);
           if(to){
               CCell::move_fly(fly_id, from, to);
               emit cell_modified(to->id());    // обновлять, только если муха улетела туда
           }else{
               qWarning() << Q_FUNC_INFO << "no free cell!";
           }

           emit cell_modified(from->id());  // обновлять всегда. Либо муха оттуда ушла, или изменились ее параметры
           break;
       }
    }
}

CCell* CFlyController::get_random_free_cell()
{
    std::vector<CCell*> free_cells;
    for(size_t i = 0; i < m_cells.size(); ++i){
        if(m_cells.at(i)->get_free_space() > 0){
            free_cells.push_back(m_cells.at(i));
        }
    }

    if(!free_cells.empty()){
        float k = (float)generator()/(generator.max() - generator.min());
        size_t cell_id = static_cast<float>(free_cells.size()) * k;
        return free_cells.at(cell_id);
    }
    else{
        return nullptr;
    }
}

CCell* CFlyController::get_neighboring_free_cell(CCell *cell)
{
    std::vector<CCell*> candidate_cells;
    int row = cell->id() / m_dimention;
    int col = cell->id() % m_dimention;
    int from_col = col - 1;
    int to_col = col + 1;
    int from_row = row - 1;
    int to_row = row + 1;
    if(from_col < 0){ from_col = 0; }
    if(from_row < 0){ from_row = 0; }
    if(to_col >= (int)m_dimention){ to_col = m_dimention - 1; }
    if(to_row >= (int)m_dimention){ to_row = m_dimention - 1; }

    for(int c = from_col; c <= to_col; c++){
        for(int r = from_row; r <= to_row; r++){
            if(c != col || r != row){
                candidate_cells.push_back(get_cell(r, c));
            }
        }
    }

    std::vector<CCell*> free_cells;

    for(size_t i = 0; i < candidate_cells.size(); ++i){
        if(m_cells.at(i)->get_free_space() > 0){
            free_cells.push_back(candidate_cells.at(i));
        }
    }

    if(!free_cells.empty()){
        float k = (float)generator()/(generator.max() - generator.min());
        size_t cell_id = static_cast<float>(free_cells.size()) * k;
        return free_cells.at(cell_id);
    }
    else{
        return nullptr;
    }
}

CFlyController::CFlyController(QObject *parent)
    : QObject{parent}
{
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator.seed(seed);

    float a = (float)generator()/(generator.max() - generator.min());
    size_t dimention = static_cast<float>(MIN_DIMENTION) + static_cast<float>(MAX_DIMENTION-MIN_DIMENTION) * a;

    m_dimention = dimention;

    m_cells.resize(m_dimention*m_dimention);
    int full_capacity = 0;
    for(size_t i = 0; i < m_cells.size(); ++i){
        float k = (float)generator()/(generator.max() - generator.min());
        size_t fly_capacity = static_cast<float>(MIN_FLY_CAPACITY) + static_cast<float>(MAX_FLY_CAPACITY-MIN_FLY_CAPACITY) * k;
        m_cells[i] = new CCell(i, fly_capacity);
        full_capacity += fly_capacity;
    }

    qDebug() << Q_FUNC_INFO << "full_capacity:" << full_capacity;
}

CFlyController::CFlyController(size_t dimention, size_t fly_capacity, QObject *parent)
    : QObject{parent}
{
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator.seed(seed);

    m_dimention = dimention;

    m_cells.resize(m_dimention*m_dimention);
    int full_capacity = 0;
    for(size_t i = 0; i < m_cells.size(); ++i){
        m_cells[i] = new CCell(i, fly_capacity);
        full_capacity += fly_capacity;
    }

    qDebug() << Q_FUNC_INFO << "full_capacity:" << full_capacity;
}

CFlyController::~CFlyController()
{
    for(size_t i = 0; i < m_cells.size(); ++i){
        delete m_cells[i];
    }
}

void CFlyController::fly_create()
{
    CCell* cell = get_random_free_cell();
    fly_create(cell->id());
}

void CFlyController::fly_create(int cell_id)
{
    float k = (float)generator()/(generator.max() - generator.min());
    uint stup = static_cast<float>(STUPIDITY_MIN) + static_cast<float>(STUPIDITY_MAX-STUPIDITY_MIN) * k;

    fly_create(cell_id, stup);
}

void CFlyController::fly_create(int cell_id, uint stupidity)
{
    // для id сквозная нумерация на протяжении работы приложения, уникальность проверять не нужно.
    CCell* cell = nullptr;
    for (const auto &it : m_cells){
        if(it->id() == cell_id){
            cell = it;
            break;
        }
    }

    if(cell){
        QThread *thread = new QThread;
        thread->setObjectName(QString("fly_%1").arg(id_for_create));

        if(stupidity < STUPIDITY_MIN){ stupidity = STUPIDITY_MIN; }
        if(stupidity > STUPIDITY_MAX){ stupidity = STUPIDITY_MAX; }

        CFly *worker = new CFly(id_for_create, stupidity, m_dimention);

        worker->moveToThread(thread);
        connect(thread, &QThread::started, worker, &CFly::execute);
        connect(worker, &CFly::finished, thread, &QThread::quit);
        connect(worker, &CFly::finished, worker, &CFly::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        connect(worker, &CFly::jump, this, &CFlyController::on_fly_jump, Qt::QueuedConnection);
        connect(this, &CFlyController::flies_suspend, worker, &CFly::suspend, Qt::QueuedConnection);
        connect(this, &CFlyController::flies_resume, worker, &CFly::resume, Qt::QueuedConnection);

        thread->start();

        if(!cell->append_fly(worker)){
            qWarning() << "error add fly";
            thread->quit();
        }

        id_for_create++;

        // сигнал, чтобы новая муха тоже запустилась
        if(m_flies_started){
            start_flies();
        }
    }
    else{
        qWarning() << Q_FUNC_INFO << "no space!";
    }
}



void CFlyController::fly_remove(int fly_id)
{
    for(auto it = m_cells.begin(); it != m_cells.end(); ++it){
        CFly * fly = (*it)->find(fly_id);
        if(nullptr != fly){
            (*it)->remove_fly(fly);
            fly->stop();
            // эту ячейку нужно обновить
            emit cell_modified((*it)->id());
            break;
       }
    }
}

QVariantList CFlyController::fly_track(int fly_id)
{
    for(auto it = m_cells.begin(); it != m_cells.end(); ++it){
        CFly * fly = (*it)->find(fly_id);
        if(nullptr != fly){
            auto track = fly->track();
            QVariantList res;
            for (const auto &it : track){
                res << it;
            }
            return res;
       }
    }

    return QVariantList();
}

QString CFlyController::fly_info(int fly_id)
{
    for(auto it = m_cells.begin(); it != m_cells.end(); ++it){
        CFly * fly = (*it)->find(fly_id);
        if(nullptr != fly){
            QJsonObject root;
            root["fly_id"] = fly->id();
            root["stupidity"] = fly->stupidity();
            root["hp_max"] = fly->hp_max();
            root["hp"] = fly->hp();

            QJsonArray arr;
            for (const auto &it : fly->track()){
                arr << it;
            }

            root["jumps"] = arr;

            QString out = QJsonDocument(root).toJson(QJsonDocument::Compact);
            return out;
       }
    }

    return QString();
}

