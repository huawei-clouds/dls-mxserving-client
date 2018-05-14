import argparse
import os
import requests
import json

def predict(args):
    url = args.server_url
    files = {args.input_node_name: ('data', open(args.file_path, 'rb'), "image/jpeg")}
    response = requests.post(url, files=files)
    res = response.json()
    labels = None
    if os.path.isfile(args.label_file_path):
        labels = [line.strip() for line in open(args.label_file_path).readlines()]
    if labels is not None:
        for item in res["prediction"]:
            item['class'] = labels[int(item['class'])]
    print res

if __name__ == '__main__':
    # parse args
    parser = argparse.ArgumentParser(description="predict",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--server_url', type=str, default="http://127.0.0.1:8080/squeezenet_v1.1/predict",
                        help='The server url.')
    parser.add_argument('--file_path', type=str, default="kitten.jpg",
                        help='The pic file used to predict.')
    parser.add_argument('--label_file_path', type=str, default="synset.txt",
                        help='The label file for predict result.')
    parser.add_argument('--input_node_name', type=str, default="data",
                        help='The input node name of the model runing on predict server.')
    args, unkown = parser.parse_known_args()

    predict(args)
