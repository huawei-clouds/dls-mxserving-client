## 预测作业客户端使用指南

预测作业创建成功后，可以下载和运行客户端发起预测请求，具体使用说明如下：

### 下载

执行如下命令下载预测作业客户端：

```
git clone https://github.com/huawei-clouds/dls-mxserving-client.git
```

### 例子

以**图像分类**为例（样例数据为dls-mxserving-client/data目录下的某个图片），Java和Python客户端使用方式为：

#### Java客户端

依次执行如下命令即可实现一次预测：

```

```

#### Python客户端

直接运行如下命令即可实现一次预测：

```
python dls-mxserving-client/python/image_classification_predict.py \
--server_url="http://xx.xx.xx.xx:xxxx/server_name/predict_func_name" \
--file_path="xx/dls-mxserving-client/data/kitten.jpg" \
--label_file_path="xx/dls-mxserving-client/data/synset.txt" \
--input_node-name="data"
```

#### 返回预测结果

```
{
  "prediction": [
    [
      {
        "class": "n02124075 Egyptian cat",
        "probability": 0.9408261179924011
      },
      {
        "class": "n02127052 lynx, catamount",
        "probability": 0.055966004729270935
      },
      {
        "class": "n02123045 tabby, tabby cat",
        "probability": 0.0025502564385533333
      },
      {
        "class": "n02123159 tiger cat",
        "probability": 0.00034320182749070227
      },
      {
        "class": "n02123394 Persian cat",
        "probability": 0.00026897044153884053
      }
    ]
  ]
}
```

#### 参数说明（适用于上述所有客户端，根据实际值修改）

**server_url**：远程预测服务的url，http://ip:port/服务名称/预测函数名，例如 http://127.0.0.1:8080/squeezenet_v1.1/predict。

**file_path**：输入数据所在的路径。

**label_file_path**：数据集标签名所在的路径。

**input_node_name**：创建预测作业时模型的输入节点名。