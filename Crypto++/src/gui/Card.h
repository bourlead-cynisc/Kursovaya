#pragma once
class QFrame;
class QVBoxLayout;

struct Card {
    QFrame*      frame = nullptr;
    QVBoxLayout* body  = nullptr;   // сюда добавляйте содержимое карточки
};
