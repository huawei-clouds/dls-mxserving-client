import argparse
import os
import pycurl
from pandas.compat import StringIO
import json
import numpy as np
import cv2
import random

THRESH = 0.1

def draw_detection(im_array, detections, class_names):
    color_white = (255, 255, 255)
    for det in detections:
        score = det[1]
        if score < THRESH:
            continue
        bbox = det[-4:]
        class_index = det[0]
        bbox = map(int, bbox)
        color = (random.randint(0, 256), random.randint(0, 256), random.randint(0, 256))
        cv2.rectangle(im_array, (bbox[0], bbox[1]), (bbox[2], bbox[3]), color=color, thickness=2)
        cv2.putText(im_array, '%s %.3f' % (class_names[class_index], score), (bbox[0], bbox[1] + 10),
                    color=color_white, fontFace=cv2.FONT_HERSHEY_COMPLEX, fontScale=0.5)
    return im_array

def predict(args):
    assert os.path.exists(args.file_root_path), 'file_root_path is not exists'
    if not os.path.exists(args.res_save_path):
        os.mkdir(args.res_save_path)
    file_list = os.listdir(args.file_root_path)
    for item in file_list:
        file_name = os.path.join(args.file_root_path, item)
        predict_curl = pycurl.Curl()
        storage = StringIO()
        values = [(args.input_node_name, (pycurl.FORM_FILE, file_name))]
        predict_curl.setopt(predict_curl.URL, args.server_url)
        predict_curl.setopt(predict_curl.WRITEFUNCTION, storage.write)
        predict_curl.setopt(predict_curl.HTTPPOST, values)
        predict_curl.perform()
        predict_curl.close()
        content = storage.getvalue()
        content = content.replace('\n', '')
        res = json.loads(content)
        im_array = cv2.imread(file_name)
        if os.path.isfile(args.label_file_path):
            labels = [line.strip() for line in open(args.label_file_path).readlines()]
        else:
            labels = ['object'] * 21
        im_array = draw_detection(im_array, res['prediction'], labels)
        cv2.imwrite(args.res_save_path + item.split('.')[0] + '_res.jpg', im_array)


if __name__ == '__main__':
    # parse args
    parser = argparse.ArgumentParser(description="predict",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--server_url', type=str, default="http://127.0.0.1:8080/rcnn_eval/predict",
                        help='The server url.')
    parser.add_argument('--file_root_path', type=str, default="../data/rcnn_pic/",
                        help='The pic file path used to predict.')
    parser.add_argument('--res_save_path', type=str, default="./res_pic/",
                        help='The path used to save result pic.')
    parser.add_argument('--input_node_name', type=str, default="data",
                        help='The input node name of the model runing on predict server.')
    parser.add_argument('--label_file_path', type=str, default="rcnn_classes.txt",
                        help='The label file for predict result.')
    args, unkown = parser.parse_known_args()

    predict(args)
