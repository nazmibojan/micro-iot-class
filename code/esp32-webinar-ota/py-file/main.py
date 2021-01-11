from flask import Flask, request, abort, jsonify, send_from_directory, send_file
from werkzeug.utils import *
import random
import string
import json
import os


# BIN_DIRECTORY = ""

# if not os.path.exists(BIN_DIRECTORY):
#     os.makedirs(BIN_DIRECTORY)


api = Flask(__name__)

@api.route('/getVersion', methods=['GET'])
def getVersion() -> str:
  return jsonify({"url":"192.168.43.210","filename":"firmware.bin","version":"2.0.0","date":"11 January 2021"})

@api.route("/<image_name>", methods=['GET'])
def get_file(image_name):
    """Download a file."""
    return send_from_directory("", filename=image_name, as_attachment=True)

  
if __name__ == '__main__':
    api.run(debug=True,host='192.168.43.210', port=5000)