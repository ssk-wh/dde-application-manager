/*
 * Copyright (C) 2021 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     weizhixiang <weizhixiang@uniontech.com>
 *
 * Maintainer: weizhixiang <weizhixiang@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "waylandmanager.h"
#include "dock.h"
#include "xcbutils.h"

#define XCB XCBUtils::instance()

WaylandManager::WaylandManager(Dock *_dock, QObject *parent)
 : QObject(parent)
 , m_dock(_dock)
 , m_mutex(QMutex(QMutex::NonRecursive))
{

}


/**
 * @brief WaylandManager::registerWindow 注册窗口
 * @param objPath
 */
void WaylandManager::registerWindow(const QString &objPath)
{
    qInfo() << "registerWindow: " << objPath;
    if (findWindowByObjPath(objPath))
        return;

    PlasmaWindow *plasmaWindow = m_dock->createPlasmaWindow(objPath);
    if (!plasmaWindow) {
        qWarning() << "registerWindowWayland: createPlasmaWindow failed";
        return;
    }

    QString appId = plasmaWindow->AppId();
    QStringList list {"dde-dock", "dde-launcher", "dde-clipboard", "dde-osd", "dde-polkit-agent", "dde-simple-egl", "dmcs"};
    if (list.indexOf(appId) >= 0)
        return;

    XWindow winId = XCB->allocId();     // XCB中未发现释放XID接口
    XWindow realId = plasmaWindow->WindowId();
    if (realId)
        winId = realId;

    WindowInfoK *winInfo = new WindowInfoK(plasmaWindow, winId);
    m_dock->listenKWindowSignals(winInfo);
    insertWindow(objPath, winInfo);
    m_dock->attachOrDetachWindow(winInfo);
    if (winId) {
        m_windowInfoMap[winId] = winInfo;
    }
}

// 取消注册窗口
void WaylandManager::unRegisterWindow(const QString &objPath)
{
    qInfo() << "unRegisterWindow: " << objPath;
    WindowInfoK *winInfo = findWindowByObjPath(objPath);
    if (!winInfo)
        return;

    m_dock->removePlasmaWindowHandler(winInfo->getPlasmaWindow());
    m_dock->detachWindow(winInfo);
    deleteWindow(objPath);
}

WindowInfoK *WaylandManager::findWindowById(uint activeWin)
{
    QMutexLocker locker(&m_mutex);
    for (auto iter = m_kWinInfos.begin(); iter != m_kWinInfos.end(); iter++) {
        if (iter.value()->getInnerId() == QString::number(activeWin)) {
            return iter.value();
        }
    }

    return nullptr;
}

WindowInfoK *WaylandManager::findWindowByXid(XWindow xid)
{
    WindowInfoK *winInfo = nullptr;
    for (auto iter = m_kWinInfos.begin(); iter != m_kWinInfos.end(); iter++) {
        if (iter.value()->getXid() == xid) {
            winInfo = iter.value();
            break;
        }
    }

    return winInfo;
}

WindowInfoK *WaylandManager::findWindowByObjPath(QString objPath)
{
    if (m_kWinInfos.find(objPath) == m_kWinInfos.end())
        return nullptr;

    return m_kWinInfos[objPath];
}

void WaylandManager::insertWindow(QString objPath, WindowInfoK *windowInfo)
{
    QMutexLocker locker(&m_mutex);
    m_kWinInfos[objPath] = windowInfo;
}

void WaylandManager::deleteWindow(QString objPath)
{
    m_kWinInfos.remove(objPath);
}

