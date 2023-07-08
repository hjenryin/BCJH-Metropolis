echo 修改配置、了解更多请至https://github.com/hjenryin/BCJH-Metropolis/ 以便在线运行
echo 有疑问请至https://github.com/hjenryin/BCJH-Metropolis/discussions/categories/q-a 提问
echo 有任何想法可至https://github.com/hjenryin/BCJH-Metropolis/discussions/categories/general 讨论
set "bcjhid="
set /p bcjhid=如果需要重新下载用户配置，请输入白菜菊花id；否则直接回车：

if "%bcjhid%"=="" (
    echo 未输入id，跳过下载用户配置
) 

if not "%bcjhid%"=="" (
    echo 正在下载用户配置
    powershell -command $json=$(iwr -uri "https://bcjh.xyz/api/download_data?id=%bcjhid%" ^| ConvertFrom-Json^) ^; if ($json.result -eq 'True'^) { Write-Host 用户名（中文可能乱码）：$($json.user^) -ForegroundColor:Green^; echo $json.data ^> userData.json } else { Write-Host 下载失败，请检查id是否正确 -BackgroundColor:Red -ForegroundColor:White ^}
    type userData.json > tmp
    del userData.json
    ren tmp userData.json
)
echo=
echo=查看厨具修改教程，请参考 https://github.com/hjenryin/BCJH-Metropolis#配置厨具
.\bcjh.exe
echo 按任意键关闭
pause >NUL