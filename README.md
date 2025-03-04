# mmt-parking-map-render
MMT泊车地图渲染

使用方式：
```shell
git clone https://github.com/dengfeng2/mmt-parking-map-render.git --recursive
cd mmt-parking-map-render
mkdir build
cd build
cmake ..
make
```

然后在相应目录下找到对应的可执行文件。

1. bin/offline_hmi_map
使用方式：`./offline_hmi_map <path_of_map_file(json)>` 或 `./offline_hmi_map <path_of_db> <partition_id>`
2. bin/offline_navi_map
使用方式：`./offline_navi_map <path_of_db> <partition_id>`
