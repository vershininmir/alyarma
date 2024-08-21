from flask import Flask, jsonify, abort, make_response, request
import time
from apscheduler.schedulers.background import BackgroundScheduler
import urllib
import json
app = Flask(__name__)
scheduler = BackgroundScheduler()

def tg(tgmsg):
    urllib.request.urlopen(f"https://api.telegram.org/bot5348701174:AAEfRST-YfqqY5BkkqEZlt9RWVloyd-tt1A/sendMessage?chat_id=115850485&text={tgmsg}")

lsensors = [
    {
        'id': 1,
        'description': u'это будет датчика дыма',
        'val': -1,
        'alarm': -1,
        'timestamp': time.time()
    },
    {
        'id': 2,
        'description': u'это будет ик сенсор огня',
        'val': -1,
        'alarm': -1,
        'timestamp': time.time()
    }
]
#curl -i http://localhost:5000/alyarma/api/v1.0/sensors
@app.route('/alyarma/api/v1.0/sensors', methods=['GET'])
def get_sensors():
    return jsonify({'sensors': lsensors})

@app.route('/alyarma/api/v1.0/sensors/<int:sensor_id>', methods=['GET'])
def get_senor(sensor_id):
    sensor = list(filter(lambda s: s['id'] == sensor_id, lsensors))
    if len(sensor) == 0:
        abort(404)
    return jsonify({'sensor': sensor[0]})

#curl -i -H "Content-Type: application/json" -X POST -d "{"""description""":"""DATCHANIN3""","""alarm""":"""-1"""}" http://localhost:5000/alyarma/api/v1.0/sensors

@app.route('/alyarma/api/v1.0/sensors', methods=['POST'])
def create_sensor():
    if not request.json or not 'alarm' in request.json:
        abort(400)
    sensor = {
        'id': lsensors[-1]['id'] + 1,
        'description': request.json.get('description', ""),
        'val': -1,
        'alarm': -1,
        'timestamp': time.time()
    }
    lsensors.append(sensor)
    return jsonify({'sensor': sensor}), 201

# curl -i -H "Content-Type: application/json" -X PUT -d "{"""val""":2}" http://localhost:5000/alyarma/api/v1.0/sensors/2

@app.route('/alyarma/api/v1.0/sensors/<int:sensor_id>', methods=['PUT'])
def update_sensor(sensor_id):
    sensor = list(filter(lambda t: t['id'] == sensor_id, lsensors))
    if len(sensor) == 0:
        abort(404)
    if not request.json:
        abort(400)
    if 'description' in request.json and type(request.json['description']) is not unicode:
        abort(400)
    if 'val' in request.json and type(request.json['val']) is not int:
        abort(400)
    if 'alarm' in request.json and type(request.json['alarm']) is not int:
        abort(400)
    sensor[0]['description'] = request.json.get('description', sensor[0]['description'])
    sensor[0]['val'] = request.json.get('val', sensor[0]['val'])
    sensor[0]['alarm'] = request.json.get('alarm', sensor[0]['alarm'])
    sensor[0]['timestamp'] = time.time()
    return jsonify({'sensor': sensor[0]})

@app.route('/alyarma/api/v1.0/sensors/<int:sensor_id>', methods=['DELETE'])
def delete_sensor(sensor_id):
    sensor = list(filter(lambda t: t['id'] == sensor_id, lsensors))
    if len(sensor) == 0:
        abort(404)
    lsensors.remove(sensor[0])
    return jsonify({'result': True})

@app.errorhandler(404)
def not_found(error):
    return make_response(jsonify({'error': 'Not found'}), 404)

nocon = False
def task():
    global nocon
    jsensors = list(lsensors)
    for sen in jsensors:
        if sen['alarm'] == 1:
            msg = urllib.parse.quote_plus("ПОЖАР! '" + str(sen['description']) + "' прислал оповещение")
            tg(msg)
        timeshrimp = time.time() - sen['timestamp']
        if sen['alarm'] == 0:
            msg = urllib.parse.quote_plus("Нет данных с '" + str(sen['description']) + "' на Arduino")
            tg(msg)
        if timeshrimp >= 30:
            msg = urllib.parse.quote_plus("'" + str(sen['description']) + "' не присылает данных " + str(int(timeshrimp)) + " секунд")
            tg(msg)
            nocon = True
        if nocon == True and timeshrimp < 30:
            nocon = False
            msg = urllib.parse.quote_plus("Связь с устройством восстановлена")
            tg(msg)





scheduler.add_job(task, 'interval', seconds=10)
scheduler.start()



if __name__ == '__main__':
    app.run(host="0.0.0.0", use_reloader=False)