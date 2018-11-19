/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _SMARTPTRCONTAINER_HPP_
#define _SMARTPTRCONTAINER_HPP_

#include "PlayableItem.hpp"
#include <QObject>
#include <memory>

namespace DigitalRooster {

/**
 * Stupid class to wrap a smart pointer for QML since QML cant handle functions
 * that return shared_ptr, even if the return value is just passed to another
 * C++ function
 * This should be templated but this also is not supported

 template <class T> class SmartPtrContainer : public QObject {
    Q_OBJECT
 public:
    SmartPtrContainer(QObject* parent = nullptr)
        : QObject(parent){};

    SmartPtrContainer(std::shared_ptr<T> content, QObject* parent = nullptr)
        : QObject(parent)
        , ptr(content){};

    std::shared_ptr<T> get_content() {
        return ptr;
    };

 private:
    std::shared_ptr<T> ptr;
};
*/
class PlayableItemContainer : public QObject {
    Q_OBJECT
public:
    PlayableItemContainer() = default;

    PlayableItemContainer(
        std::shared_ptr<PlayableItem> content, QObject* parent = nullptr)
        : QObject(parent)
        , ptr(content){};

    std::shared_ptr<PlayableItem> get_content() {
        return ptr;
    };

private:
    std::shared_ptr<PlayableItem> ptr;
};


// using PlayableItemContainer =
//    DigitalRooster::SmartPtrContainer<DigitalRooster::PlayableItem>;

} // namespace DigitalRooster

#endif /* _SMARTPTRCONTAINER_HPP_ */
