/****************************************************************************
**
** Copyright (C) 2016 Jochen Becher
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#ifndef QMT_MSOURCEEXPANSION_H
#define QMT_MSOURCEEXPANSION_H

#include "melement.h"

namespace qmt {

class QMT_EXPORT MSourceExpansion : public MExpansion
{
public:
    MSourceExpansion();
    MSourceExpansion(const MSourceExpansion &rhs);
    ~MSourceExpansion() override;

    MSourceExpansion &operator=(const MSourceExpansion &rhs);

    MSourceExpansion *clone(const MElement &rhs) const override;

    QString sourceId() const { return m_sourceId; }
    void setSourceId(const QString &sourceId);
    bool isTransient() const { return m_isTransient; }
    void setTransient(bool transient);

private:
    QString m_sourceId;
    bool m_isTransient;
};

} // namespace qmt

#endif // QMT_MSOURCEEXPANSION_H