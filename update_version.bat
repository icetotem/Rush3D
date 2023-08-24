@echo off

REM 设置Git仓库路径和版本头文件路径
set "git_repo_path=./"
set "version_header_path=./code/engine/src/version.h"


set "major_version=0"
set "minor_version=01"

REM 切换到Git仓库目录
cd %git_repo_path%

REM 获取提交次数、当前月份和当前日期作为版本号的组成部分
for /f %%I in ('git rev-list --count HEAD') do set "commit_count=000%%I"
set "commit_count=%commit_count:~-4%"


REM 构建版本号
set "version=%major_version%.%minor_version%.%commit_count%"

REM 写入版本号到头文件
(
    echo #pragma once
    echo.
    echo #define VERSION "%version%"
    echo.
) > %version_header_path%

echo 版本号已写入 %version_header_path%