@REM *********************
@REM ** 这一部分无需修改 **
@REM *********************

@REM When adding lines, remember to update the README file! Two additional lines need to be added.
@REM This file should be encoded in GB2312.
echo 修改配置、了解更多请至https://github.com/hjenryin/BCJH-Metropolis/ 以便在线运行
echo 有疑问请至https://github.com/hjenryin/BCJH-Metropolis/discussions/categories/q-a 提问
echo 有任何想法可至https://github.com/hjenryin/BCJH-Metropolis/discussions/categories/general 讨论
echo 现已更新汤圆、乌龙等奇奇怪怪的技能，如果发现有问题，欢迎到 https://github.com/hjenryin/BCJH-Metropolis/issues/new 提出~

powershell -command "$file = 'ruleData.json'; $halfHourAgo = (Get-Date).AddMinutes(-30); $modifiedTime = (Get-Item $file).LastWriteTime; $modifiedTime -lt $halfHourAgo" | findstr "True" >NUL && call:download_data || echo 半小时内已更新过规则，跳过下载

set "bcjhid="
set /p bcjhid=如果需要重新下载用户配置，请输入白菜菊花id；否则直接回车：

if "%bcjhid%"=="" (
    echo 未输入id，跳过下载用户配置
) 

if not "%bcjhid%"=="" (
    echo 正在下载用户配置
    powershell -command curl -o "userData.json" "https://bcjh.xyz/api/download_data?id=%bcjhid%"
    powershell -command $json=$(iwr -uri "https://bcjh.xyz/api/download_data?id=%bcjhid%" ^| ConvertFrom-Json^) ^; if ($json.result -eq 'True'^) { Write-Host 下载成功 -ForegroundColor:Green } else { Write-Host 下载失败，请检查id是否正确 -BackgroundColor:Red -ForegroundColor:White ^}
    win-iconv\iconv.exe -f utf-8 -t gbk userData.json > tmp 2>NUL
    if %ERRORLEVEL% EQU 0 (
        del userData.json
        ren tmp userData.json
    )
)
echo=
powershell -command "Write-Host '查看厨具修改教程，请参考' -NoNewline; Write-Host 'https://github.com/hjenryin/BCJH-Metropolis#配置厨具' -ForegroundColor:Blue -NoNewline; Write-Host '。'"

@REM ******************************************************
@REM ** 如有需要，可以在此修改-C，-R后的数字来更改迭代次数。 **
@REM ******************************************************

.\bcjh.exe -C 5000 -R 1000
@REM -h 帮助


@REM *********************
@REM ** 这一部分无需修改 **
@REM *********************
echo 按任意键关闭
pause >NUL
goto :eof


:download_data
powershell -command "Write-Host '正在获取宴会规则... ' -NoNewline"
powershell -command curl -o "ruleData.json" "https://bcjh.xyz/api/get_banquet_rule"
win-iconv\iconv.exe -f utf-8 -t gbk ruleData.json > tmp 
if %ERRORLEVEL% EQU 0 (
    del ruleData.json
    ren tmp ruleData.json
    echo 宴会规则获取完毕
) else (
    echo 宴会规则获取失败，使用已缓存的宴会规则。如果宴会规则不是最新，请检查网络并重试。
)

powershell -command "Write-Host '正在获取最新厨师菜谱... ' -NoNewline"
powershell -command curl -o "data.min.json" "https://yuwenxifan.github.io/bcjhMobile/data/data.min.json"
win-iconv\iconv.exe -f utf-8 -t gbk data.min.json > tmp 
if %ERRORLEVEL% EQU 0 (
    del data.min.json
    ren tmp data.min.json
    echo 厨师菜谱获取完毕
) else (
    echo 厨师菜谱获取失败，使用已缓存的池子数据。如果数据不是最新，请检查网络并重试。
)
goto :eof
