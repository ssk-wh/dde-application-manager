// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QVariantMap>
#include "constant.h"

struct LaunchOption
{
    explicit LaunchOption(QVariant v)
        : m_val(std::move(v))
    {
    }
    virtual ~LaunchOption() = default;
    [[nodiscard]] virtual QStringList generateCommandLine() const noexcept = 0;
    [[nodiscard]] static const QString &key() noexcept
    {
        static QString none;
        return none;
    };
    [[nodiscard]] virtual const QString &type() const noexcept = 0;

    uint32_t m_priority{0};
    QVariant m_val;

protected:
    LaunchOption() = default;
};

struct setUserLaunchOption : public LaunchOption
{
    using LaunchOption::LaunchOption;
    [[nodiscard]] const QString &type() const noexcept override
    {
        static QString tp{AppExecOption};
        return tp;
    }
    [[nodiscard]] static const QString &key() noexcept
    {
        static QString uid{"uid"};
        return uid;
    }
    [[nodiscard]] QStringList generateCommandLine() const noexcept override;
};

struct setEnvLaunchOption : public LaunchOption
{
    using LaunchOption::LaunchOption;
    [[nodiscard]] const QString &type() const noexcept override
    {
        static QString tp{systemdOption};
        return tp;
    }
    [[nodiscard]] static const QString &key() noexcept
    {
        static QString env{"env"};
        return env;
    }
    [[nodiscard]] QStringList generateCommandLine() const noexcept override;
};

struct splitLaunchOption : public LaunchOption
{
    splitLaunchOption()
        : LaunchOption(QString{"--"})
    {
    }

    [[nodiscard]] const QString &type() const noexcept override
    {
        static QString tp{splitOption};
        return tp;
    }
    [[nodiscard]] QStringList generateCommandLine() const noexcept override;
};

struct hookLaunchOption : public LaunchOption
{
    explicit hookLaunchOption(QVariant v)
        : LaunchOption(std::move(v))
    {
        m_priority = 1;
    }
    [[nodiscard]] const QString &type() const noexcept override
    {
        static QString tp{AppExecOption};
        return tp;
    }
    [[nodiscard]] static const QString &key() noexcept
    {
        static QString hook{"_hooks"};
        return hook;
    }
    [[nodiscard]] QStringList generateCommandLine() const noexcept override { return m_val.toStringList(); };
};

struct setWorkingPathLaunchOption : public LaunchOption
{
    using LaunchOption::LaunchOption;
    [[nodiscard]] const QString &type() const noexcept override
    {
        static QString tp{systemdOption};
        return tp;
    }
    [[nodiscard]] static const QString &key() noexcept
    {
        static QString path{"workingPath"};
        return path;
    }
    [[nodiscard]] QStringList generateCommandLine() const noexcept override;
};

struct builtInSearchExecOption : public LaunchOption
{
    using LaunchOption::LaunchOption;
    [[nodiscard]] const QString &type() const noexcept override
    {
        static QString tp{systemdOption};
        return tp;
    }
    [[nodiscard]] static const QString &key() noexcept
    {
        static QString key{"_builtIn_searchExec"};
        return key;
    }
    [[nodiscard]] QStringList generateCommandLine() const noexcept override;
};

QStringList generateCommand(const QVariantMap &props) noexcept;
