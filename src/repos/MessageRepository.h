#pragma once
#include <QVector>
#include "../models/Message.h"

class MessageRepository {
public:
    static MessageRepository &instance();

    bool create(Message &m);
    bool markRead(int id);
    QVector<Message> getByReader(int readerId);
};


