
echo �޸����á��˽��������https://github.com/hjenryin/BCJH-Metropolis/ �Ա���������
echo ����������https://github.com/hjenryin/BCJH-Metropolis/discussions/categories/q-a ����
echo ���κ��뷨����https://github.com/hjenryin/BCJH-Metropolis/discussions/categories/general ����
set "bcjhid="
set /p bcjhid=�����Ҫ���������û����ã�������ײ˾ջ�id������ֱ�ӻس���

if "%bcjhid%"=="" (
    echo δ����id�����������û�����
) 
@REM iwr -uri "https://bcjh.xyz/api/download_data?id=86952" | ConvertFrom-Json | select -expand data | Out-File -Encoding utf8 userData.json
if not "%bcjhid%"=="" (
    echo ���������û�����
    powershell -command $json=$(iwr -uri "https://bcjh.xyz/api/download_data?id=%bcjhid%" ^| ConvertFrom-Json^) ^; if ($json.result -eq 'True'^) { Write-Host �û��������Ŀ������룩��$($json.user^) -ForegroundColor:Green^; echo $json.data ^> userData.json } else { Write-Host ����ʧ�ܣ�����id�Ƿ���ȷ -BackgroundColor:Red -ForegroundColor:White ^}
    type userData.json > tmp
    del userData.json
    ren tmp userData.json
)
echo=
echo=�鿴�����޸Ľ̳̣���ο� https://github.com/hjenryin/BCJH-Metropolis#���ó���
.\bcjh.exe
echo ��������ر�
pause >NUL
