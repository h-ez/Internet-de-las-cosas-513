import firebase_admin
from firebase_admin import firestore
from firebase_admin import credentials
import time
import random
from datetime import datetime

cred = credentials.Certificate('credentials.json')
app = firebase_admin.initialize_app(cred)
db = firestore.client()

while(True):
    current_date = datetime.now()
    date = current_date.strftime('%Y-%m-%d')
    hour = current_date.strftime('%H')
    print(date)
    print(hour)

    collec_name = u'sensor_1_{0}'.format(date)
    sensor1 = db.collection(collec_name).document(hour)

    encendido = bool(random.getrandbits(1))

    doc = sensor1.get()
    data = doc.to_dict()

    total = db.collection(collec_name).document('totals')
    tdoc = total.get()
    tdata = total.to_dict()

    if tdata == None:
        total.set({
            u'total_mins': 1 if encendido else 0,
        })
    else:
        if encendido:
            total.update({
            u'total_mins': tdata[u'total_mins'] + 1
        })

        

    if data == None:
        sensor1.set({
        u'mins_encendido' : 1 if encendido else 0,
        u'mins_apagado' : 0 if not encendido else 0})
    else:
        if encendido:
            sensor1.update({
                u'mins_encendido': data[u'mins_encendido'] + 1
            })
        else:

            sensor1.update({
                u'mins_apagado': data[u'mins_apagado'] + 1
            })


    time.sleep(60)