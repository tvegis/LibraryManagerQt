#pragma once
#include <QVector>
#include "../models/Reservation.h"

class ReservationRepository {
public:
    static ReservationRepository &instance();

    bool create(Reservation &r);
    bool update(const Reservation &r);
    QVector<Reservation> getByReader(int readerId);
    QVector<Reservation> getByBook(int bookId);
};


