#Usage of Predict Client 

After the inference job (service) is created, you can download, modify and run predict client to send request to inference service. The detailed instructions are:

### Download

Run the following code to down the predict client:

```
git clone https://github.com/huawei-clouds/dls-mxserving-client.git
```



### Example

Take **Image Classification** for example（the input data is one image under the folder of dls-mxserving-client/data）, the usages of Java and Python predict client are：

#### Java Predict Client

Execute the following codes successively:

```

```

#### Python Predict Client

You will need python modules: pycurl，pandas

Directly execute the following code：

```
python dls-mxserving-client/python/image_classification_predict.py \
--server_url="http://xx.xx.xx.xx:xxxx/server_name/predict_func_name" \
--file_path="xx/dls-mxserving-client/data/kitten.jpg" \
--label_file_path="xx/dls-mxserving-client/data/synset.txt" \
--input_node_name="data"
```

#### Inference result

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

#### Parameter Interpretation (The parameters are suitable to both clients above, and should be modified according to practical situations.)

**server_url**：the url of the remote inference service, http://ip:port/server_name/predict_func_name , such as  http://127.0.0.1:8080/squeezenet_v1.1/predict。

**file_path**：the url of the input data.

**label_file_path**：the url of the file of data labels.

**input_node_name**：the input node name of the model runing on remote inference server.

