import argparse
import os
import pycurl
from pandas.compat import StringIO
import json
import numpy as np
from PIL import Image
import base64

def getpallete(num_cls):
    """
    this function is to get the colormap for visualizing the segmentation mask
    """
    pallete = [0] * (num_cls * 3)
    for j in xrange(0, num_cls):
        lab = j
        pallete[j*3+0] = 0
        pallete[j*3+1] = 0
        pallete[j*3+2] = 0
        i = 0
        while lab > 0:
            pallete[j*3+0] |= (((lab >> 0) & 1) << (7-i))
            pallete[j*3+1] |= (((lab >> 1) & 1) << (7-i))
            pallete[j*3+2] |= (((lab >> 2) & 1) << (7-i))
            i = i + 1
            lab >>= 3
    return pallete

def predict(args):
    assert os.path.exists(args.file_root_path), 'file_root_path is not exists'
    if not os.path.exists(args.res_save_path):
        os.mkdir(args.res_save_path)
    file_list = os.listdir(args.file_root_path)
    for item in file_list:
        predict_curl = pycurl.Curl()
        storage = StringIO()
        values = [(args.input_node_name, (pycurl.FORM_FILE, os.path.join(args.file_root_path, item)))]
        predict_curl.setopt(predict_curl.URL, args.server_url)
        predict_curl.setopt(predict_curl.WRITEFUNCTION, storage.write)
        predict_curl.setopt(predict_curl.HTTPPOST, values)
        predict_curl.perform()
        predict_curl.close()
        content = storage.getvalue()
        content = content.replace('\n', '')
        res = json.loads(content)
        d64 = base64.decodestring(res["prediction"])
        rec = np.frombuffer(d64, dtype=np.float32)
        out_img = Image.fromarray(rec.reshape(360, 480).astype(np.uint8))
        pallete = getpallete(256)
        out_img.putpalette(pallete)
        out_img.save(args.res_save_path + item.split('.')[0] + '_res.png')

if __name__ == '__main__':
    # parse args
    parser = argparse.ArgumentParser(description="predict",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--server_url', type=str, default="http://127.0.0.1:8080/segnet_vgg_pre/predict",
                        help='The server url.')
    parser.add_argument('--file_root_path', type=str, default="../../0001TP_007440.jpg",
                        help='The pic file path used to predict.')
    parser.add_argument('--res_save_path', type=str, default="./res_pic/",
                        help='The path used to save result pic.')
    parser.add_argument('--input_node_name', type=str, default="data",
                        help='The input node name of the model runing on predict server.')
    parser.add_argument('--output_data_shape', type=str, default="360, 480",
                        help='The data shape of server returns.')
    args, unkown = parser.parse_known_args()

    predict(args)
