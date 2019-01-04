from flask import Flask, redirect, url_for, render_template, send_from_directory
import time
import json
import os

app = Flask(__name__)

wf = "%s/%s" % (os.environ.get('PWD'), "weather.json")
if(os.environ.get('WEATHERFILE')):
        wf = os.environ.get('WEATHERFILE');

def epoch2string(e):
    return time.strftime("%a, %d %b %Y %H:%M:%S %Z", time.localtime(e))

@app.route("/", methods=["GET"])
def index():
    with open(wf) as w:
        d = json.load(w)
        w.close()
    return render_template("home.html", x=d, u=epoch2string(d["timestamp"]))

@app.route("/json", methods=["GET"])
def j():
    with open(wf) as w:
        d = json.load(w)
        w.close()
    return json.dumps(d)

@app.route('/rrd/<path:path>')
def send_static(path):
        return send_from_directory('static', path, cache_timeout=5)

if(__name__ == '__main__'):
    app.run(host='0.0.0.0', port=8000)
