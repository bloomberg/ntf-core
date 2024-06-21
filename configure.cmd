@echo off

rem Copyright 2020-2023 Bloomberg Finance L.P.
rem SPDX-License-Identifier: Apache-2.0
rem
rem Licensed under the Apache License, Version 2.0 (the "License");
rem you may not use this file except in compliance with the License.
rem You may obtain a copy of the License at
rem
rem     http://www.apache.org/licenses/LICENSE-2.0
rem
rem Unless required by applicable law or agreed to in writing, software
rem distributed under the License is distributed on an "AS IS" BASIS,
rem WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
rem See the License for the specific language governing permissions and
rem limitations under the License.

setlocal enableextensions enabledelayedexpansion

set NTF_CONFIGURE_REPOSITORY=%~dp0

rem Remove trailing slash
IF %NTF_CONFIGURE_REPOSITORY:~-1%==\ SET NTF_CONFIGURE_REPOSITORY=%NTF_CONFIGURE_REPOSITORY:~0,-1%
set "NTF_CONFIGURE_REPOSITORY=%NTF_CONFIGURE_REPOSITORY:/=\%"

set NTF_CONFIGURE_UNAME=Windows

IF NOT DEFINED NTF_CONFIGURE_REFROOT (
    set NTF_CONFIGURE_REFROOT=C:\
)

IF NOT DEFINED NTF_CONFIGURE_PREFIX (
    set NTF_CONFIGURE_PREFIX=\opt\bb
)

IF NOT DEFINED NTF_CONFIGURE_OUTPUT (
    set NTF_CONFIGURE_OUTPUT=%NTF_CONFIGURE_REPOSITORY%\build
)

IF NOT DEFINED NTF_CONFIGURE_DISTRIBUTION (
    set NTF_CONFIGURE_DISTRIBUTION=unstable
)

IF NOT DEFINED NTF_CONFIGURE_GENERATOR (
    set NTF_CONFIGURE_GENERATOR=Ninja
)

IF NOT DEFINED NTF_CONFIGURE_UFID (
    set NTF_CONFIGURE_UFID=opt_dbg_exc_mt_64
)

IF NOT DEFINED NTF_CONFIGURE_JOBS (
    set NTF_CONFIGURE_JOBS=8
)

IF NOT DEFINED NTF_CONFIGURE_PROJECT (
    set NTF_CONFIGURE_PROJECT=ntf
)

set NTF_CONFIGURE_WITH_BSL=1
set NTF_CONFIGURE_WITH_BDL=1
set NTF_CONFIGURE_WITH_BAL=1
set NTF_CONFIGURE_WITH_NTS=1
set NTF_CONFIGURE_WITH_NTC=1

IF NOT DEFINED NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV4 (
    set NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV4=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV6 (
    set NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV6=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_ADDRESS_FAMILY_LOCAL (
    set NTF_CONFIGURE_WITH_ADDRESS_FAMILY_LOCAL=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_TCP (
    set NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_TCP=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_UDP (
    set NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_UDP=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_LOCAL (
    set NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_LOCAL=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_SELECT (
    set NTF_CONFIGURE_WITH_SELECT=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_POLL (
    set NTF_CONFIGURE_WITH_POLL=1
)

set NTF_CONFIGURE_WITH_EPOLL=0
set NTF_CONFIGURE_WITH_DEVPOLL=0
set NTF_CONFIGURE_WITH_EVENTPORT=0
set NTF_CONFIGURE_WITH_POLLSET=0
set NTF_CONFIGURE_WITH_KQUEUE=0

IF NOT DEFINED NTF_CONFIGURE_WITH_IOCP (
    set NTF_CONFIGURE_WITH_IOCP=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_DYNAMIC_LOAD_BALANCING (
    set NTF_CONFIGURE_WITH_DYNAMIC_LOAD_BALANCING=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_THREAD_SCALING (
    set NTF_CONFIGURE_WITH_THREAD_SCALING=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_DEPRECATED_FEATURES (
    set NTF_CONFIGURE_WITH_DEPRECATED_FEATURES=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_LOGGING (
    set NTF_CONFIGURE_WITH_LOGGING=0
)

IF NOT DEFINED NTF_CONFIGURE_WITH_METRICS (
    set NTF_CONFIGURE_WITH_METRICS=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_INLINING_SUGGESTED (
    set NTF_CONFIGURE_WITH_INLINING_SUGGESTED=0
)

IF NOT DEFINED NTF_CONFIGURE_WITH_INLINING_FORCED (
    set NTF_CONFIGURE_WITH_INLINING_FORCED=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_INLINING_DISABLED (
    set NTF_CONFIGURE_WITH_INLINING_DISABLED=0
)

IF NOT DEFINED NTF_CONFIGURE_WITH_BRANCH_PREDICTION (
    set NTF_CONFIGURE_WITH_BRANCH_PREDICTION=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_SPIN_LOCKS (
    set NTF_CONFIGURE_WITH_SPIN_LOCKS=0
)

IF NOT DEFINED NTF_CONFIGURE_WITH_USERSPACE_MUTEXES (
    set NTF_CONFIGURE_WITH_USERSPACE_MUTEXES=0
)

IF NOT DEFINED NTF_CONFIGURE_WITH_SYSTEM_MUTEXES (
    set NTF_CONFIGURE_WITH_SYSTEM_MUTEXES=0
)

IF NOT DEFINED NTF_CONFIGURE_WITH_RECURSIVE_MUTEXES (
    set NTF_CONFIGURE_WITH_RECURSIVE_MUTEXES=0
)

IF NOT DEFINED NTF_CONFIGURE_WITH_USAGE_EXAMPLES (
    set NTF_CONFIGURE_WITH_USAGE_EXAMPLES=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_MOCKS (
    set NTF_CONFIGURE_WITH_MOCKS=0
)

IF NOT DEFINED NTF_CONFIGURE_WITH_INTEGRATION_TESTS (
    set NTF_CONFIGURE_WITH_INTEGRATION_TESTS=0
)

IF NOT DEFINED NTF_CONFIGURE_WITH_WARNINGS (
    set NTF_CONFIGURE_WITH_WARNINGS=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_WARNINGS_AS_ERRORS (
    set NTF_CONFIGURE_WITH_WARNINGS_AS_ERRORS=1
)

IF NOT DEFINED NTF_CONFIGURE_WITH_DOCUMENTATION (
    set NTF_CONFIGURE_WITH_DOCUMENTATION=0
)

IF NOT DEFINED NTF_CONFIGURE_WITH_DOCUMENTATION_INTERNAL (
    set NTF_CONFIGURE_WITH_DOCUMENTATION_INTERNAL=0
)

set NTF_CONFIGURE_VERBOSE=0
set NTF_CONFIGURE_WHAT=0
set NTF_CONFIGURE_CLEAN=1
set NTF_CONFIGURE_UPDATE=1

:PARSE_COMMAND_LINE
if not "%1"=="" (
    if "%1"=="--refroot" (
        set NTF_CONFIGURE_REFROOT=%2
        shift
    )
    if "%1"=="--prefix" (
        set NTF_CONFIGURE_PREFIX=%2
        shift
    )
    if "%1"=="--output" (
        set NTF_CONFIGURE_OUTPUT=%2
        shift
    )
    if "%1"=="--distribution" (
        set NTF_CONFIGURE_DISTRIBUTION=%2
        shift
    )
    if "%1"=="--ufid" (
        set NTF_CONFIGURE_UFID=%2
        shift
    )
    if "%1"=="--generator" (
        set NTF_CONFIGURE_GENERATOR=%2
        shift
    )
    if "%1"=="--project" (
        set NTF_CONFIGURE_PROJECT=%2
        shift
    )
    if "%1"=="--jobs" (
        set NTF_CONFIGURE_JOBS=%2
        shift
    )
    if "%1"=="--clean" (
        set NTF_CONFIGURE_CLEAN=1
    )
    if "%1"=="--keep" (
        set NTF_CONFIGURE_CLEAN=0
    )

    if "%1"=="--debug" (
        set NTF_CONFIGURE_UFID=dbg_exc_mt_64
        set NTF_CONFIGURE_WITH_LOGGING=1
    )
    if "%1"=="--release" (
        set NTF_CONFIGURE_UFID=opt_dbg_exc_mt_64
    )

    if "%1"=="--with-warnings" (
        set NTF_CONFIGURE_WITH_WARNINGS=1
    )
    if "%1"=="--with-warnings-as-errors" (
        set NTF_CONFIGURE_WITH_WARNINGS_AS_ERRORS=1
    )

    if "%1"=="--with-bsl" (
        set NTF_CONFIGURE_WITH_BSL=1
    )
    if "%1"=="--with-bdl" (
        set NTF_CONFIGURE_WITH_BDL=1
    )
    if "%1"=="--with-bal" (
        set NTF_CONFIGURE_WITH_BAL=1
    )
    if "%1"=="--with-nts" (
        set NTF_CONFIGURE_WITH_NTS=1
    )
    if "%1"=="--with-ntc" (
        set NTF_CONFIGURE_WITH_NTC=1
    )

    if "%1"=="--with-address-family-ipv4" (
        set NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV4=1
    )
    if "%1"=="--with-address-family-ipv6" (
        set NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV4=1
    )
    if "%1"=="--with-address-family-local" (
        set NTF_CONFIGURE_WITH_ADDRESS_FAMILY_LOCAL=1
        set NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_LOCAL=1
    )

    if "%1"=="--with-transport-protocol-tcp" (
        set NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_TCP=1
    )
    if "%1"=="--with-transport-protocol-udp" (
        set NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_UDP=1
    )
    if "%1"=="--with-transport-protocol-local" (
        set NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_LOCAL=1
        set NTF_CONFIGURE_WITH_ADDRESS_FAMILY_LOCAL=1
    )

    if "%1"=="--with-select" (
        set NTF_CONFIGURE_WITH_SELECT=1
    )
    if "%1"=="--with-poll" (
        set NTF_CONFIGURE_WITH_POLL=1
    )
    if "%1"=="--with-iocp" (
        set NTF_CONFIGURE_WITH_IOCP=1
    )

    if "%1"=="--with-dynamic-load-balancing" (
        set NTF_CONFIGURE_WITH_DYNAMIC_LOAD_BALANCING=1
    )
    if "%1"=="--with-thread-scaling" (
        set NTF_CONFIGURE_WITH_THREAD_SCALING=1
    )
    if "%1"=="--with-deprecated-features" (
        set NTF_CONFIGURE_WITH_DEPRECATED_FEATURES=1
    )

    if "%1"=="--with-logging" (
        set NTF_CONFIGURE_WITH_LOGGING=1
    )
    if "%1"=="--with-metrics" (
        set NTF_CONFIGURE_WITH_METRICS=1
    )

    if "%1"=="--with-inlining-suggested" (
        set NTF_CONFIGURE_WITH_INLINING_SUGGESTED=1
    )
    if "%1"=="--with-inlining-forced" (
        set NTF_CONFIGURE_WITH_INLINING_FORCED=1
    )
    if "%1"=="--with-inlining-disabled" (
        set NTF_CONFIGURE_WITH_INLINING_DISABLED=1
    )

    if "%1"=="--with-branch-prediction" (
        set NTF_CONFIGURE_WITH_BRANCH_PREDICTION=1
    )

    if "%1"=="--with-spin-locks" (
        set NTF_CONFIGURE_WITH_SPIN_LOCKS=1
    )
    if "%1"=="--with-userspace-mutexes" (
        set NTF_CONFIGURE_WITH_USERSPACE_MUTEXES=1
    )
    if "%1"=="--with-system-mutexes" (
        set NTF_CONFIGURE_WITH_SYSTEM_MUTEXES=1
    )
    if "%1"=="--with-recursive-mutexes" (
        set NTF_CONFIGURE_WITH_RECURSIVE_MUTEXES=1
    )

    if "%1"=="--with-usage-examples" (
        set NTF_CONFIGURE_WITH_USAGE_EXAMPLES=1
    )
    if "%1"=="--with-mocks" (
        set NTF_CONFIGURE_WITH_MOCKS=1
    )
    if "%1"=="--with-integration-tests" (
        set NTF_CONFIGURE_WITH_INTEGRATION_TESTS=1
    )

    if "%1"=="--with-documentation" (
        set NTF_CONFIGURE_WITH_DOCUMENTATION=1
    )
    if "%1"=="--with-documentation-internal" (
        set NTF_CONFIGURE_WITH_DOCUMENTATION_INTERNAL=1
    )

    if "%1"=="--without-warnings" (
        set NTF_CONFIGURE_WITH_WARNINGS=0
    )
    if "%1"=="--without-warnings-as-errors" (
        set NTF_CONFIGURE_WITH_WARNINGS_AS_ERRORS=0
    )

    if "%1"=="--without-bsl" (
        set NTF_CONFIGURE_WITH_BSL=0
    )
    if "%1"=="--without-bdl" (
        set NTF_CONFIGURE_WITH_BDL=0
    )
    if "%1"=="--without-bal" (
        set NTF_CONFIGURE_WITH_BAL=0
    )
    if "%1"=="--without-nts" (
        set NTF_CONFIGURE_WITH_NTS=0
    )
    if "%1"=="--without-ntc" (
        set NTF_CONFIGURE_WITH_NTC=0
    )

    if "%1"=="--without-address-family-ipv4" (
        set NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV4=0
    )
    if "%1"=="--without-address-family-ipv6" (
        set NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV4=0
    )
    if "%1"=="--without-address-family-local" (
        set NTF_CONFIGURE_WITH_ADDRESS_FAMILY_LOCAL=0
        set NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_LOCAL=0
    )

    if "%1"=="--without-transport-protocol-tcp" (
        set NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_TCP=0
    )
    if "%1"=="--without-transport-protocol-udp" (
        set NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_UDP=0
    )
    if "%1"=="--without-transport-protocol-local" (
        set NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_LOCAL=0
        set NTF_CONFIGURE_WITH_ADDRESS_FAMILY_LOCAL=0
    )

    if "%1"=="--without-select" (
        set NTF_CONFIGURE_WITH_SELECT=0
    )
    if "%1"=="--without-poll" (
        set NTF_CONFIGURE_WITH_POLL=0
    )
    if "%1"=="--without-iocp" (
        set NTF_CONFIGURE_WITH_IOCP=0
    )

    if "%1"=="--without-dynamic-load-balancing" (
        set NTF_CONFIGURE_WITH_DYNAMIC_LOAD_BALANCING=0
    )
    if "%1"=="--without-thread-scaling" (
        set NTF_CONFIGURE_WITH_THREAD_SCALING=0
    )
    if "%1"=="--without-deprecated-features" (
        set NTF_CONFIGURE_WITH_DEPRECATED_FEATURES=0
    )

    if "%1"=="--without-logging" (
        set NTF_CONFIGURE_WITH_LOGGING=0
    )
    if "%1"=="--without-metrics" (
        set NTF_CONFIGURE_WITH_METRICS=0
    )

    if "%1"=="--without-inlining-suggested" (
        set NTF_CONFIGURE_WITH_INLINING_SUGGESTED=0
    )
    if "%1"=="--without-inlining-forced" (
        set NTF_CONFIGURE_WITH_INLINING_FORCED=0
    )
    if "%1"=="--without-inlining-disabled" (
        set NTF_CONFIGURE_WITH_INLINING_DISABLED=0
    )

    if "%1"=="--without-branch-prediction" (
        set NTF_CONFIGURE_WITH_BRANCH_PREDICTION=0
    )

    if "%1"=="--without-spin-locks" (
        set NTF_CONFIGURE_WITH_SPIN_LOCKS=0
    )
    if "%1"=="--without-userspace-mutexes" (
        set NTF_CONFIGURE_WITH_USERSPACE_MUTEXES=0
    )
    if "%1"=="--without-system-mutexes" (
        set NTF_CONFIGURE_WITH_SYSTEM_MUTEXES=0
    )
    if "%1"=="--without-recursive-mutexes" (
        set NTF_CONFIGURE_WITH_RECURSIVE_MUTEXES=0
    )

    if "%1"=="--without-usage-examples" (
        set NTF_CONFIGURE_WITH_USAGE_EXAMPLES=0
    )
    if "%1"=="--without-mocks" (
        set NTF_CONFIGURE_WITH_MOCKS=0
    )
    if "%1"=="--without-integration-tests" (
        set NTF_CONFIGURE_WITH_INTEGRATION_TESTS=0
    )

    if "%1"=="--without-documentation" (
        set NTF_CONFIGURE_WITH_DOCUMENTATION=0
    )
    if "%1"=="--without-documentation-internal" (
        set NTF_CONFIGURE_WITH_DOCUMENTATION_INTERNAL=0
    )

    if "%1"=="--update" (
        set NTF_CONFIGURE_UPDATE=1
    )
    if "%1"=="--what" (
        set NTF_CONFIGURE_WHAT=1
    )
    if "%1"=="--verbose" (
        set NTF_CONFIGURE_VERBOSE=1
    )
    if "%1"=="--clion_config" (
        set NTF_GENERATE_CLION_CONFIG=%2
        shift
    )
    if "%1"=="--help" (
        call :USAGE
        exit /B 0
    )

    shift
    goto :PARSE_COMMAND_LINE
)

set NTF_CONFIGURE_UFID_BASE=%NTF_CONFIGURE_UFID%

set "NTF_CONFIGURE_UFID=%NTF_CONFIGURE_UFID%_cpp20"

set NTF_CONFIGURE_LIBDIR=lib64

set NTF_CONFIGURE_OPT_BB=%NTF_CONFIGURE_REFROOT%/%NTF_CONFIGURE_PREFIX%
call :NORMALIZEPATH %NTF_CONFIGURE_OPT_BB%
set NTF_CONFIGURE_OPT_BB=%RETVAL%
set "NTF_CONFIGURE_OPT_BB=%NTF_CONFIGURE_OPT_BB:/=\%"

set NTF_CONFIGURE_OUTPUT=%NTF_CONFIGURE_OUTPUT%/%NTF_CONFIGURE_UNAME%/%NTF_CONFIGURE_UFID_BASE%
call :NORMALIZEPATH %NTF_CONFIGURE_OUTPUT%
set NTF_CONFIGURE_OUTPUT=%RETVAL%
set "NTF_CONFIGURE_OUTPUT=%NTF_CONFIGURE_OUTPUT:/=\%"

if "%VisualStudioVersion%"=="17.0" set visualStudioDate=2022
if "%VisualStudioVersion%"=="16.0" set visualStudioDate=2019
if "%VisualStudioVersion%"=="15.0" set visualStudioDate=2017
if "%VisualStudioVersion%"=="14.0" set visualStudioDate=2015

if %NTF_CONFIGURE_CLEAN% equ 1 (
    if exist %NTF_CONFIGURE_OUTPUT% (
        rmdir /S /Q %NTF_CONFIGURE_OUTPUT%
    )
)

if %NTF_CONFIGURE_PROJECT%==bde (
    goto :BDE
) else (
    goto :NTF
)

:BDE

echo Configuring with BDE CMake

set BDE_CMAKE_UPLID=windows-windows_nt-x86_64-10.0-msvc-!visualStudioDate!
set BDE_CMAKE_UFID=%NTF_CONFIGURE_UFID%
set BDE_CMAKE_BUILD_DIR=%NTF_CONFIGURE_OUTPUT%
set CXX=cl
set CC=cl
set BDE_CMAKE_TOOLCHAIN=toolchains\win32\cl-default
set BDE_CMAKE_INSTALL_DIR=%NTF_CONFIGURE_PREFIX%

rem set DISTRIBUTION_REFROOT=%NTF_CONFIGURE_REFROOT%

set PATH=%NTF_CONFIGURE_OPT_BB%\bin;%NTF_CONFIGURE_OPT_BB%\libexec\bde-tools\bin;%PATH%

set CONFIGURE_BDE_BUILD_ENV_PY=%NTF_CONFIGURE_OPT_BB%\libexec\bde-tools\bin\bde_build_env.py
set CONFIGURE_CMAKE_BUILD_PY=%NTF_CONFIGURE_OPT_BB%\libexec\bde-tools\bin\cmake_build.py

if %NTF_CONFIGURE_VERBOSE% equ 1 (
    set NTF_CONFIGURE_VERBOSE_OPTION=-v
)

python %CONFIGURE_CMAKE_BUILD_PY% configure -G "%NTF_CONFIGURE_GENERATOR%" --refroot %NTF_CONFIGURE_REFROOT% --prefix %NTF_CONFIGURE_PREFIX% %NTF_CONFIGURE_VERBOSE_OPTION%
if %ERRORLEVEL% neq 0 (
    echo Failed to configure
    exit /B 1
)

set NTF_CONFIGURE_MAKEFILE=%NTF_CONFIGURE_REPOSITORY%\makefile
set NTF_CONFIGURE_MAKETARGET=%NTF_CONFIGURE_OUTPUT%\compile_commands.cmd

> %NTF_CONFIGURE_MAKEFILE% (
    @echo..SUFFIXES:
    @echo.
    @echo.build: .phony
    @echo.	@%NTF_CONFIGURE_MAKETARGET% build
    @echo.
    @echo.all:
    @echo.	@%NTF_CONFIGURE_MAKETARGET% all
    @echo.
    @echo.test:
    @echo.	@%NTF_CONFIGURE_MAKETARGET% test
    @echo.
    @echo.test_usage:
    @echo.	@%NTF_CONFIGURE_MAKETARGET% test_usage
    @echo.
    @echo.install:
    @echo.	@%NTF_CONFIGURE_MAKETARGET% install
    @echo.
    @echo.documentation:
    @echo.	@%NTF_CONFIGURE_MAKETARGET% documentation
    @echo.
    @echo.format:
    @echo.	@%NTF_CONFIGURE_MAKETARGET% format
    @echo.
    @echo.info:
    @echo.	@%NTF_CONFIGURE_MAKETARGET% info
    @echo.
    @echo..phony:
)

> %NTF_CONFIGURE_MAKETARGET% (
    @echo.@echo off
    @echo.setlocal
    @echo.
    @echo.set MAKEFILE_TARGET=%%1
    @echo.
    @echo.set PATH=%NTF_CONFIGURE_OUTPUT%;%NTF_CONFIGURE_OPT_BB%;%%PATH%%
    @echo.
    @echo.if "%%MAKEFILE_TARGET%%"=="build" (
    @echo.    cmake --build %NTF_CONFIGURE_OUTPUT% -j %NTF_CONFIGURE_JOBS%
    @echo.    if %%ERRORLEVEL%% neq 0 (
    @echo.        echo Failed to build
    @echo.        exit /B 1
    @echo.    ^)
    @echo.    exit /B 0
    @echo.^)
    @echo.
    @echo.if "%%MAKEFILE_TARGET%%"=="all" (
    @echo.    cmake --build %NTF_CONFIGURE_OUTPUT% -j %NTF_CONFIGURE_JOBS%
    @echo.    if %%ERRORLEVEL%% neq 0 (
    @echo.        echo Failed to build
    @echo.        exit /B 1
    @echo.    ^)
    @echo.    cmake --build %NTF_CONFIGURE_OUTPUT% -j %NTF_CONFIGURE_JOBS% --target all.t
    @echo.    if %%ERRORLEVEL%% neq 0 (
    @echo.        echo Failed to build test drivers
    @echo.        exit /B 1
    @echo.    ^)
    @echo.    exit /B 0
    @echo.^)
    @echo.
    @echo.if "%%MAKEFILE_TARGET%%"=="test" (
    @echo.    cmake --build %NTF_CONFIGURE_OUTPUT% -j %NTF_CONFIGURE_JOBS% --target all.t
    @echo.    if %%ERRORLEVEL%% neq 0 (
    @echo.        echo Failed to build test drivers
    @echo.        exit /B 1
    @echo.    ^)
    @echo.    cmake --build %NTF_CONFIGURE_OUTPUT% -j 1 --target test
    @echo.    if %%ERRORLEVEL%% neq 0 (
    @echo.        echo Failed to execute test drivers
    @echo.        exit /B 1
    @echo.    ^)
    @echo.    exit /B 0
    @echo.^)
    @echo.
    @echo.if "%%MAKEFILE_TARGET%%"=="test_usage" (
    @echo.    echo Not implemented
    @echo.    exit /B 1
    @echo.^)
    @echo.
    @echo.if "%%MAKEFILE_TARGET%%"=="install" (
    @echo.    cmake --install %NTF_CONFIGURE_OUTPUT% --prefix %NTF_CONFIGURE_OPT_BB%
    @echo.    if %ERRORLEVEL% neq 0 (
    @echo.        echo Failed to install
    @echo.        exit /B 1
    @echo.    ^)
    @echo.    exit /B 0
    @echo.^)
    @echo.
    @echo.if "%%MAKEFILE_TARGET%%"=="documentation" (
    @echo.    echo Not implemented
    @echo.    exit /B 1
    @echo.^)
    @echo.
    @echo.if "%%MAKEFILE_TARGET%%"=="format" (
    @echo.    echo Not implemented
    @echo.    exit /B 1
    @echo.^)
    @echo.
    @echo.cmake --build %NTF_CONFIGURE_OUTPUT% --target %%MAKEFILE_TARGET%%
    @echo.if %%ERRORLEVEL%% neq 0 (
    @echo.    echo Failed to build %%MAKEFILE_TARGET%%
    @echo.    exit /B 1
    @echo.^)
    @echo.exit /B 0
)

goto :DONE

:NTF

echo Configuring with NTF CMake

set CC=cl
set CXX=cl

set NTF_CONFIGURE_CMAKE_OPTION_DISTRIBUTION_REFROOT= -DDISTRIBUTION_REFROOT:STRING=%NTF_CONFIGURE_REFROOT%

set NTF_CONFIGURE_CMAKE_OPTION_INSTALL_PREFIX= -DCMAKE_INSTALL_PREFIX:STRING=%NTF_CONFIGURE_PREFIX%

if %NTF_CONFIGURE_VERBOSE% equ 1 (
    set NTF_CONFIGURE_CMAKE_OPTION_VERBOSE= -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
) else (
    set NTF_CONFIGURE_CMAKE_OPTION_VERBOSE= -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF
)

set NTF_CONFIGURE_CMAKE_OPTION_UFID= -DUFID:STRING=%NTF_CONFIGURE_UFID%

echo %NTF_CONFIGURE_UFID%|findstr /r "opt" 2>&1 > nul
if %ERRORLEVEL% equ 0 (
    set NTF_CONFIGURE_UFID_HAS_OPT=1
) else (
    set NTF_CONFIGURE_UFID_HAS_OPT=0
)

echo %NTF_CONFIGURE_UFID%|findstr /r "dbg" 2>&1 > nul
if %ERRORLEVEL% equ 0 (
    set NTF_CONFIGURE_UFID_HAS_DBG=1
) else (
    set NTF_CONFIGURE_UFID_HAS_DBG=0
)

if %NTF_CONFIGURE_UFID_HAS_OPT% equ 1 (
    if %NTF_CONFIGURE_UFID_HAS_DBG% equ 1 (
        set NTF_CONFIGURE_CMAKE_OPTION_BUILD_TYPE= -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo
    ) else (
        set NTF_CONFIGURE_CMAKE_OPTION_BUILD_TYPE= -DCMAKE_BUILD_TYPE:STRING=Release
    )
) else (
    if %NTF_CONFIGURE_UFID_HAS_DBG% equ 1 (
        set NTF_CONFIGURE_CMAKE_OPTION_BUILD_TYPE= -DCMAKE_BUILD_TYPE:STRING=Debug
    ) else (
        set NTF_CONFIGURE_CMAKE_OPTION_BUILD_TYPE= -DCMAKE_BUILD_TYPE:STRING=Unknown
    )
)

if not exist %NTF_CONFIGURE_OUTPUT% (
    mkdir %NTF_CONFIGURE_OUTPUT%
)

if defined NTF_GENERATE_CLION_CONFIG (
    call :generate_clion_cmake_xml !NTF_GENERATE_CLION_CONFIG!
    exit
)

cd %NTF_CONFIGURE_OUTPUT%

echo cmake%NTF_CONFIGURE_CMAKE_OPTION_DISTRIBUTION_REFROOT%%NTF_CONFIGURE_CMAKE_OPTION_INSTALL_PREFIX%%NTF_CONFIGURE_CMAKE_OPTION_UFID%%NTF_CONFIGURE_CMAKE_OPTION_BUILD_TYPE%%NTF_CONFIGURE_CMAKE_OPTION_VERBOSE% -DCMAKE_TOOLCHAIN_FILE:PATH="%NTF_CONFIGURE_REPOSITORY%\toolchain.cmake" -G "Ninja" -S "%NTF_CONFIGURE_REPOSITORY%"

cmake%NTF_CONFIGURE_CMAKE_OPTION_DISTRIBUTION_REFROOT%%NTF_CONFIGURE_CMAKE_OPTION_INSTALL_PREFIX%%NTF_CONFIGURE_CMAKE_OPTION_UFID%%NTF_CONFIGURE_CMAKE_OPTION_BUILD_TYPE%%NTF_CONFIGURE_CMAKE_OPTION_VERBOSE% -DCMAKE_TOOLCHAIN_FILE:PATH="%NTF_CONFIGURE_REPOSITORY%\toolchain.cmake" -G "Ninja" -S "%NTF_CONFIGURE_REPOSITORY%"
if %ERRORLEVEL% neq 0 (
    echo Failed to configure
    exit /B 1
)

> %NTF_CONFIGURE_REPOSITORY%\makefile (
    @echo..SUFFIXES:
    @echo.
    @echo.build: .phony
    @echo.	@cmake --build %NTF_CONFIGURE_OUTPUT% --parallel %NTF_CONFIGURE_JOBS%
    @echo.
    @echo.build_test:
    @echo.	@cmake --build %NTF_CONFIGURE_OUTPUT% --parallel %NTF_CONFIGURE_JOBS% --target build_test
    @echo.
    @echo.all:
    @echo.	@cmake --build %NTF_CONFIGURE_OUTPUT% --parallel %NTF_CONFIGURE_JOBS% --target all
    @echo.
    @echo.test:
    @echo.	@cmake --build %NTF_CONFIGURE_OUTPUT% --parallel %NTF_CONFIGURE_JOBS% --target build_test
    @echo.	@cmake --build %NTF_CONFIGURE_OUTPUT% --parallel 1 --target test
    @echo.
    @echo.test_usage:
    @echo.	@cmake --build %NTF_CONFIGURE_OUTPUT% --parallel %NTF_CONFIGURE_JOBS% --target build_test
    @echo.	@cmake --build %NTF_CONFIGURE_OUTPUT% --parallel 1 --target test_usage
    @echo.
    @echo.install:
    @echo.	@cmake --install %NTF_CONFIGURE_OUTPUT% --prefix %NTF_CONFIGURE_OPT_BB%
    @echo.
    @echo.clean:
    @echo.	@cmake --build %NTF_CONFIGURE_OUTPUT% --parallel %NTF_CONFIGURE_JOBS% --target clean
    @echo.
    @echo.package:
    @echo.	@cmake --build %NTF_CONFIGURE_OUTPUT% --parallel %NTF_CONFIGURE_JOBS% --target package
    @echo.
    @echo.package_source:
    @echo.	@cmake --build %NTF_CONFIGURE_OUTPUT% --parallel %NTF_CONFIGURE_JOBS% --target package_source
    @echo.
    @echo.help:
    @echo.	@cmake --build %NTF_CONFIGURE_OUTPUT% --target help
    @echo.
    @echo..phony:
)

goto :DONE

:DONE

exit /B 0

:USAGE

echo usage: configure [--clean] [--prefix ^<path^>] [--output ^<path^>] [--refroot ^<path^>] [--distribution ^<name^>] [--debug^|release] [--ufid ^<code^>] [--with-feature ^<name^>] [--without-feature ^<name^>]
echo where:
echo     --prefix       ^<path^>            Path to the refroot where the build artifacts will be installed (default: %NTF_CONFIGURE_PREFIX%)
echo     --output       ^<path^>            Path to the directory where the build artifacts will be staged (default: %NTF_CONFIGURE_OUTPUT%)
echo     --refroot      ^<path^>            Path to the refroot containing build dependencies (default: %NTF_CONFIGURE_REFROOT%)
echo     --distribution ^<name^>            Name of the distribution (default: %NTF_CONFIGURE_DISTRIBUTION%)
echo     --generator    ^<name^>            Name of the build system, either "Ninja" or "msvc" (default: %NTF_CONFIGURE_GENERATOR%)
echo     --jobs         ^<number^>          Maximum number of parallel build jobs (default: %NTF_CONFIGURE_JOBS%)
echo     --clean                          Remove the previous build output, if any, before configuring
echo     --keep                           Retain the previous build output, if any, while configuring
echo     --debug                          Build the standard debug UFID
echo     --release                        Build the standard release UFID

echo     --with-bde                       Enable and build features that depend on BDE classic
echo     --with-nts                       Enable and build features that depend on NTS
echo     --with-ntc                       Enable and build features that depend on NTC

echo     --with-select                    Enable the reactor driver implemented with 'select'
echo     --with-poll                      Enable the reactor driver implemented with 'poll'
echo     --with-iocp                      Enable the proactor driver that depends on I/O completion ports

echo     --with-dynamic-load-balancing    Enable processing I/O on any thread, rather than a single thread
echo     --with-thread-scaling            Enable automatic scaling of thread pools
echo     --with-deprecated-features       Enable deprecated features
echo     --with-logging                   Build with logging
echo     --with-metrics                   Build with metrics
echo     --with-branch-prediction         Build with branch prediction
echo     --with-spin-locks                Build with mutually-exclusive locks implemented as spin locks
echo     --with-recursive-mutexes         Build with mutually-exclusive locks implemented as recursive mutexes
echo     --with-warnings                  Build with warnings enabled
echo     --with-warnings-as-errors        Build with warnings as errors

echo     --with-documentation             Build documentation
echo     --with-documentation-internal    Build documentation of internals

echo     --with-usage-examples            Build usage examples
echo     --with-mocks                     Build mocks
echo     --with-integration-tests         Build integration tests

echo     --verbose                        Generate makefiles with verbose output

exit /B 0

:NORMALIZEPATH
    SET RETVAL=%~f1
    EXIT /B


:get_number_of_threads
    for /f %%x in ('wmic cpu get NumberOfLogicalProcessors') do (
        SET "var_check="&for /f "delims=0123456789" %%i in ("%%x") do set var_check=%%i
        if not defined var_check (
            set /a %~1=%%x
            exit /B
        )
    )
    exit /B


:copy_n_lines
    if exist tmpFile del tmpFile
    set numLinesToCopy=%1
    set /a lineCounter=0
    for /f "delims=" %%a in (%NTF_CLION_XML%) do (
        set /a lineCounter+=1
        if !lineCounter! leq %numLinesToCopy% (
            echo %%a>>tmpFile
        ) else (
            del %NTF_CLION_XML%
            move tmpFile %NTF_CLION_XML%
            exit /B
        )
    )
    exit /B


:write_config_header
    rem leading spaces are important for beautiful output
    set line1=      ^<configuration PROFILE_NAME="!NTF_CONFIGURE_UFID!" ENABLED="true"
    set line2= CONFIG_NAME="!NTF_CONFIGURE_UFID!" GENERATION_OPTIONS="
    set line3=!NTF_CONFIGURE_CMAKE_OPTION_DISTRIBUTION_REFROOT!
    set line4= !NTF_CONFIGURE_CMAKE_OPTION_INSTALL_PREFIX! !NTF_CONFIGURE_CMAKE_OPTION_VERBOSE!
    set line5= !NTF_CONFIGURE_CMAKE_OPTION_UFID! !NTF_CONFIGURE_CMAKE_OPTION_BUILD_TYPE!
    set line6= -DCMAKE_TOOLCHAIN_FILE:PATH=!NTF_CONFIGURE_REPOSITORY!\toolchain.cmake
    set line7= -G ^&quot;!NTF_CONFIGURE_GENERATOR!^&quot;"^>

    echo !line1!!line2!!line3!!line4!!line5!!line6!!line7! >> %NTF_CLION_XML%

    (
        echo.        ^<ADDITIONAL_GENERATION_ENVIRONMENT^>
        echo.          ^<envs^>
    ) >> %NTF_CLION_XML%

    exit /B

:process_ntf_configure_vars
    for /f "tokens=*" %%a in ('set') do (
        set line=%%a
        if "!line:~0,13!"=="NTF_CONFIGURE" (
            for /f "tokens=1,* delims==" %%a in ("!line!") do (
                set "key=%%a"
                set "value=%%b"
                >> %NTF_CLION_XML% (
                    echo.            ^<env name="!key!" value="!value!" /^>
                )
            )
        )
    )
    exit /B

:process_param
    >> %NTF_CLION_XML% (
        echo.            ^<env name="%1" value="!%1%!" /^>
    )
    exit /B


:generate_clion_cmake_xml
    echo Generating Clion cmake.xml...

    set /a argC=0
    for %%x in (%*) do Set /A argC+=1
    if /i "%argC%" NEQ "1" (
        echo generate_clion_cmake_xml must be called with exactly 1 argument, aborting"
        EXIT
    )

    set NTF_CLION_STORAGE=.idea
    set NTF_CLION_XML=%NTF_CLION_STORAGE%\cmake.xml
    set CLION_XML_GENERATION=%1

    if not exist %NTF_CLION_STORAGE% mkdir %NTF_CLION_STORAGE%

    call :get_number_of_threads num_threads

    echo %%CLION_XML_GENERATION%%
    if "%CLION_XML_GENERATION%"=="clean" (
        echo Performing clean generation
        if exist %NTF_CLION_XML%  (
            echo %NTF_CLION_XML% exists,it will be overwritten
        )

        (
          echo ^<?xml version="1.0" encoding="UTF-8"?^>
          echo ^<project version="4"^>
          echo.  ^<component name="CMakeSharedSettings"^>
          echo.    ^<configurations^>
        ) > %NTF_CLION_XML%
    ) else (
        if "%CLION_XML_GENERATION%"=="add" (
            echo Adding new configuration to existing cmake.xml

            if not exist %NTF_CLION_XML% (
                  echo cmake.xml does not exist, aborting
                  rem exit 1
            )

            for /f %%x in ('findstr /N "</configurations>" %NTF_CLION_XML%') do (
                if not defined num_lines (
                    set str=%%x
                    set /a num_lines=!str:~0,-1!-1
                )
            )

            if not defined num_lines (
                 echo Failed to find ^</configurations^> token, aborting...
                 exit
            )

            call :copy_n_lines !num_lines!
        ) else (
            echo ERROR
            exit
        )
    )

    call :write_config_header

    call :process_param CC
    call :process_param CXX

    call :process_ntf_configure_vars

    (
        echo.          ^</envs^>
        echo.        ^</ADDITIONAL_GENERATION_ENVIRONMENT^>

        echo.      ^</configuration^>
        echo.    ^</configurations^>
        echo.  ^</component^>
        echo ^</project^>
    ) >> %NTF_CLION_XML%

    echo Generating Clion cmake.xml done

    EXIT /B
