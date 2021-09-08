import sys
sys.path.append('../ML-lib/')

import datalib as data
import drawlib as draw
import matlib as mat
import supplib as supp

if __name__ == '__main__':
    frame = data.collect_csv_file('heart_failure_clinical_records_dataset.csv').drop(columns=['anaemia', 'diabetes', 'high_blood_pressure', 'sex', 'smoking', 'time', 'DEATH_EVENT'])

    print(frame)
