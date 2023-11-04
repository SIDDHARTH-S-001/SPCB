import csv

with open('./cs_data_pink_discharging.tsv', 'r') as file:
    csv_reader = csv.reader(file, delimiter='\t')
    writer = csv.writer(open('parsed_discharging_1C.csv', 'w'), lineterminator='\n')
    writer.writerow(['V', 'C', 'Ah'])
    for row in csv_reader:
        x,y = (float(row[0]) * 0.6)/23 , 2.40 + (float(row[1]) * 0.1)/18
        writer.writerow([y,1,x])
        
    print('Done')