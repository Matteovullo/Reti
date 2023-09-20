
VBoxManage.exe snapshot debbie2023 delete Snapshot
VBoxManage.exe snapshot debbie2023 take Snapshot

VBoxManage.exe clonevm debbie2023 --snapshot Snapshot --name A --options link --register
VBoxManage.exe modifyvm A --memory 1024 --nic1 intnet --intnet1 lan01 --nic2 none --nic3 none --nic4 none

VBoxManage.exe clonevm debbie2023 --snapshot Snapshot --name B --options link --register
VBoxManage.exe modifyvm B --memory 1024 --nic1 intnet --intnet1 lan02 --nic2 none --nic3 none --nic4 none

VBoxManage.exe clonevm debbie2023 --snapshot Snapshot --name R1 --options link --register
VBoxManage.exe modifyvm R1 --memory 1024 --nic1 intnet --intnet1 lan01 --nic2 intnet --intnet2 lan02 --nic3 none --nic4 none

VBoxManage.exe startvm A
VBoxManage.exe startvm B
VBoxManage.exe startvm R1

