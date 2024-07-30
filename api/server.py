from flask import Flask, jsonify, abort, make_response

app = Flask(__name__)

lsensors = [
    {
        'id': 1,
        'description': u'first',
        'val': -1,
        'alarm': -1
    },
    {
        'id': 2,
        'description': u'second _ sensor',
        'val': -1,
        'alarm': -1
    }
]

@app.route('/alyarma/api/v1.0/sensors', methods=['GET'])
def get_sensors():
    return jsonify({'sensors': lsensors})

@app.route('/alyarma/api/v1.0/sensors/<int:sensor_id>', methods=['GET'])
def get_senor(sensor_id):
    sensor = list(filter(lambda s: s['id'] == sensor_id, lsensors))
    if len(sensor) == 0:
        abort(404)
    return jsonify({'sensor': sensor[0]})

@app.errorhandler(404)
def not_found(error):
    return make_response(jsonify({'error': 'Not found'}), 404)

if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0")