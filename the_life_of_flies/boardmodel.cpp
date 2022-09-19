#include "boardmodel.h"
#include <QDebug>

void CBoardModel::init()
{
    clear();

    beginResetModel();

    m_fly_controller = new CFlyController();

    for(size_t i = 0; i < m_fly_controller->dimention()*3; ++i){
        m_fly_controller->fly_create();
    }

    connect(m_fly_controller, &CFlyController::cell_modified, this, &CBoardModel::on_modified_cell);

    endResetModel();
}

void CBoardModel::init(int dimention, int fly_capacity){
    clear();

    beginResetModel();

    m_fly_controller = new CFlyController(dimention, fly_capacity);

    connect(m_fly_controller, &CFlyController::cell_modified, this, &CBoardModel::on_modified_cell);

    endResetModel();
}

void CBoardModel::clear()
{
    beginResetModel();

    if(m_fly_controller){
        delete m_fly_controller;
        m_fly_controller = nullptr;
    }

    endResetModel();
}

CBoardModel::CBoardModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

CBoardModel::~CBoardModel()
{
    clear();
}

// формирование имен, которые будут использоваться при обращении из QML
QHash<int, QByteArray> CBoardModel::roleNames() const
{
    return {
//        {BoardRoles::CellId, "cell_id"},
//        {BoardRoles::Capacity, "capacity"},
//        {BoardRoles::Fullness, "fullness"},
        {BoardRoles::Flies, "flies"},
    };
    // оказалось удобнее передать всю информацию пачкой через json
}

int CBoardModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if(!m_fly_controller){ return 0; }
    return m_fly_controller->dimention();
}

int CBoardModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if(!m_fly_controller){ return 0; }
    return m_fly_controller->dimention();
}

QVariant CBoardModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() > rowCount(index) || index.column() > columnCount(index)){
        return QVariant();
    }

    switch (role) {
//        case Qt::DisplayRole:
//            return QString("%1, %2").arg(index.column()).arg(index.row());

//        case BoardRoles::CellId:
//            return QString("%1").arg(m_fly_controller->get_cell(index.row(), index.column())->id());
//        break;

//        case BoardRoles::Capacity:
//            return QString("%1").arg(m_fly_controller->get_cell(index.row(), index.column())->fly_capacity());
//        break;

//        case BoardRoles::Fullness:
//            return QString("%1").arg(m_fly_controller->get_cell(index.row(), index.column())->get_num_flies());
//        break;

        case BoardRoles::Flies:
            return m_fly_controller->get_cell(index.row(), index.column())->get_flies_jsonString();
        break;

        default:
            break;
    }

    return QVariant();
}

QVariant CBoardModel::headerData(int, Qt::Orientation, int role) const
{
    Q_UNUSED(role);
    return QVariant();
}

int CBoardModel::dimention() const
{
    if(!m_fly_controller){ return 0; }
    return m_fly_controller->dimention();
}

void CBoardModel::start()
{
    if(!m_fly_controller){ return; }
    m_fly_controller->start_flies();
}

void CBoardModel::pause()
{
    if(!m_fly_controller){ return; }
    m_fly_controller->pause_flies();
    // TODO дождаться остановки или остановка происходит мгновенно?
}

void CBoardModel::add_fly()
{
    if(!m_fly_controller){ return; }

    beginResetModel();
    m_fly_controller->fly_create();
    endResetModel();
}

void CBoardModel::add_fly(int cell_id)
{
    if(!m_fly_controller){ return; }

    beginResetModel();
    m_fly_controller->fly_create(cell_id);
    endResetModel();
}

void CBoardModel::add_fly(int cell_id, int stupidity)
{
    if(!m_fly_controller){ return; }

    beginResetModel();
    m_fly_controller->fly_create(cell_id, stupidity);
    endResetModel();
}

void CBoardModel::remove_fly(int fly_id)
{
    if(!m_fly_controller){ return; }
    m_fly_controller->fly_remove(fly_id);
}

QVariantList CBoardModel::fly_track(int fly_id)
{
    if(!m_fly_controller){ return QVariantList(); }
    return m_fly_controller->fly_track(fly_id);
}

QString CBoardModel::fly_info(int fly_id)
{
    if(!m_fly_controller){ return QString(); }
    return m_fly_controller->fly_info(fly_id);
}



void CBoardModel::on_modified_cell(int cell_id)
{
    if(dimention()){
        int row = cell_id / dimention();
        int col = cell_id % dimention();
        emit dataChanged(index(row, col), index(row, col));
    }
}
