Pause
echo "�޸����á��˽��������https://github.com/hjenryin/BCJH-Metropolis/ �Ա���������"
echo "����������https://github.com/hjenryin/BCJH-Metropolis/discussions/categories/q-a ����"
echo "���κ��뷨����https://github.com/hjenryin/BCJH-Metropolis/discussions/categories/general ����"
$bcjhid = Read-Host "�����Ҫ���������û����ã�������ײ˾ջ�id������ֱ�ӻس�"

if ($bcjhid -eq "") {
    echo "δ����id�����������û�����"
}
else {
    echo ���������û�����
    $json = $(iwr -uri "https://bcjh.xyz/api/download_data?id=$bcjhid" | ConvertFrom-Json)
    echo $json
    if ($json.result -eq 'True') { 
        Write-Host �û��������Ŀ������룩��$($json.user) -ForegroundColor:Green
        Out-File -userData.json -Encoding utf8 $json.data 
    }
    else { Write-Host ����ʧ�ܣ�����id�Ƿ���ȷ -BackgroundColor:Red -ForegroundColor:White }
}

echo ""
echo "�鿴�����޸Ľ̳̣���ο� https://github.com/hjenryin/BCJH-Metropolis#���ó���"
.\bcjh.exe

