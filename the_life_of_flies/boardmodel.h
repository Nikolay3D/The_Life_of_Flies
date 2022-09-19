#pragma once

#include <QAbstractTableModel>
#include <QtQml>

#include "flycontroller.h"

class CBoardModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT
public:
    static constexpr size_t DEFAULT_DIMENTION{4};
    CBoardModel(QObject *parent = nullptr);
    virtual ~CBoardModel();

    enum BoardRoles {
        Flies = Qt::UserRole+1,
    };

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int /* section */, Qt::Orientation /* orientation */,
                        int role) const override;
    Q_INVOKABLE int dimention() const;

    Q_INVOKABLE void start();
    Q_INVOKABLE void pause();

    Q_INVOKABLE void init();
    Q_INVOKABLE void init(int dimention, int fly_capacity);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void add_fly();
    Q_INVOKABLE void add_fly(int cell_id);
    Q_INVOKABLE void add_fly(int cell_id, int stupidity);
    Q_INVOKABLE void remove_fly(int fly_id);

    Q_INVOKABLE QVariantList fly_track(int fly_id);
    Q_INVOKABLE QString fly_info(int fly_id);
private:
    CFlyController *m_fly_controller = nullptr;

private slots:
    void on_modified_cell(int cell_id);
};

